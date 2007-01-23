/********************************************************
File: .cc
Abstract: implementation for Mesh_Features

See header (Mesh_Features.hh) for documentation

Last Revision ($Revision: 1.4 $) 
*********************************************************/

#include <fstream>
#include <cstdio>

#include "Mesh_Features.hh"
#include "curvature.hh"
#include "constant.h"
#undef NDEBUG
//#define MESSED_NORM_DIR_IN_OOGL			
//#define OOGL_IN_VOXEL_UNIT
#define TOL = 0.00001

Mesh_Features::MeshVertexSet::MeshVertexSet(): IMTK::RBTree<MeshVertexElement>()
{
	bounds[0] = 0;
	bounds[1] = 0;
	bounds[2] = 0;
	voxelSize[0] = 1.0;
	voxelSize[1] = 1.0;
	voxelSize[2] = 1.0;

	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;

	numverts = 0;

	shared = true;
}

Mesh_Features::MeshVertexSet::MeshVertexSet(bool shareflg){
	bounds[0] = 0;
	bounds[1] = 0;
	bounds[2] = 0;

	voxelSize[0] = 1.0;
	voxelSize[1] = 1.0;
	voxelSize[2] = 1.0;

	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;

	numverts = 0;

	shared = shareflg;
}

bool Mesh_Features::MeshVertexSet::isShared()
{
	return shared;
}


Mesh_Features::MeshVertexSet::MeshVertexSet(const MeshVertexSet & MVS){

	// not implemented
	assert(0);
}

Mesh_Features::MeshVertexSet::~MeshVertexSet(){

	MeshVertexElement e;
	std::slist<Vertex_with_Features *>::iterator sli;

	while( !IMTK::RBTree<MeshVertexElement>::isEmpty() ){
		e = IMTK::RBTree<MeshVertexElement>::getRoot();

		// delete each vertex from pointer list
		sli = e.verts.begin();
		while(sli != e.verts.end()){
			delete *sli;
			sli++;
		}
		IMTK::RBTree<MeshVertexElement>::deleteNode(e.key);
	}
}

Vertex_with_Features * Mesh_Features::MeshVertexSet::insert(Vertex_with_Features
v){

	double key;
	IMTK::RBTree<MeshVertexElement>::Node * n;
	MeshVertexElement *e;
	Vertex_with_Features * newv;
	int x, y, z, found;
	bool inlist;

	switch(shared){

	case true:

		x = (int)(v.getPX() + 0.5);
		y = (int)(v.getPY() + 0.5);
		z = (int)(v.getPZ() + 0.5);
		
		key = bounds[0]*bounds[1]*z + bounds[1]*x + y;

		n = IMTK::RBTree<MeshVertexElement>::binTreeSearch(key, &found);
		
		// if found search the node vertex sublist for a match
		if(found){
			inlist = false;
			e = n->getData();
			std::slist<Vertex_with_Features *>::iterator sli = e->verts.begin();
			while(sli != e->verts.end()){
				if(**sli == v){
					inlist = true;
					break;
				}
				sli++;
			}
			
			if(!inlist){
				newv = new Vertex_with_Features(v);
				newv->label(numverts++);
				e->verts.push_front(newv);
				linearList.push_front(newv);
				return newv;
			}
			else{
				return *sli;
			}
		}
		else{ // if not found add the correct node
			e = new MeshVertexElement;
			newv = new Vertex_with_Features(v);
			newv->label(numverts++);

			e->key = key;
			e->verts.push_front(newv);
			linearList.push_front(newv);
			
			IMTK::RBTree<MeshVertexElement>::insertNode(*e, key);
			delete e;
						
			return newv;
		}

	case false:

		newv = new Vertex_with_Features(v);
		newv->label(numverts++);
		linearList.push_front(newv);
		return newv;
	}
	
	return NULL;
}


//Only used with unshared
Vertex_with_Features * Mesh_Features::MeshVertexSet::insert(Vertex_with_Features * v)
{
	v->label(numverts++);
	linearList.push_front(v);
	return v;
}


void Mesh_Features::clear(bool shareflg)
{

	// delete the verticies
	delete verticies;
	verticies = new MeshVertexSet(shareflg);

	// clear the triangle list
	pttriangles.clear();
}

Mesh_Features::MeshVertexSet::iterator Mesh_Features::MeshVertexSet::begin(){

	return linearList.begin();
}

Mesh_Features::MeshVertexSet::iterator Mesh_Features::MeshVertexSet::end(){

	return linearList.end();
}

Mesh_Features::Mesh_Features(){

	verticies = new MeshVertexSet();
	convexityDepth = 1;
	
}

Mesh_Features::Mesh_Features(bool shareflg){

	verticies = new MeshVertexSet(shareflg);
	convexityDepth = 1;

}

Mesh_Features::~Mesh_Features(){

	// for efficiency reasons, the vertices are deleted when
	// the MeshVertexSet is deleted.
}

void Mesh_Features::setBounds(int xdim, int ydim, int zdim){

	verticies->bounds[0] = xdim;
	verticies->bounds[1] = ydim;
	verticies->bounds[2] = zdim;
}

void Mesh_Features::addTriangle(Triangle & tri){

	Vertex_with_Features *v1, *v2, *v3;
	Triangle *newt = new Triangle();

	v1 = verticies->insert(*(tri.getV1()));
	v2 = verticies->insert(*(tri.getV2()));
	v3 = verticies->insert(*(tri.getV3()));

	newt->set(v1, v2, v3);
	newt->label(tri.label());
	newt->setIndex(0);  //set the temp index to 0 for further use
	v1->addNeighborTriangleLabel(newt->label());		
	v2->addNeighborTriangleLabel(newt->label());		
	v3->addNeighborTriangleLabel(newt->label());

	v1->addNeighborTriangle((void *)newt);
	v2->addNeighborTriangle((void *)newt);
	v3->addNeighborTriangle((void *)newt);
			

	pttriangles.push_front(newt);
}


void Mesh_Features::addTriangle(Vertex_with_Features *v1, Vertex_with_Features *v2, Vertex_with_Features *v3,
int label){

	Triangle *newt = new Triangle();
	assert(newt != NULL);

	newt->set(v1, v2, v3);
	newt->label(label);
	newt->setIndex(0);  //set the temp index to 0 for further use
	v1->addNeighborTriangleLabel(newt->label());		
	v2->addNeighborTriangleLabel(newt->label());		
	v3->addNeighborTriangleLabel(newt->label());

	v1->addNeighborTriangle((void *)newt);
	v2->addNeighborTriangle((void *)newt);
	v3->addNeighborTriangle((void *)newt);
			

	pttriangles.push_front(newt);
}



int Mesh_Features::getNverticies(){

	return verticies->numverts;
}

int Mesh_Features::getNtriangles(){

	return pttriangles.size();
}

Mesh_Features::vertexIterator Mesh_Features::vertexBegin(){

	return verticies->begin();
}

Mesh_Features::vertexIterator Mesh_Features::vertexEnd(){

	return verticies->end();
}

Mesh_Features::triangleIterator Mesh_Features::triangleBegin(){

	return pttriangles.begin();
}

Mesh_Features::triangleIterator Mesh_Features::triangleEnd(){

	return pttriangles.end();
}

bool Mesh_Features::Calc_Thickness(Vertex_with_Features *ptVert,
Volume_ext<unsigned short> & volume){

float xorig, yorig, zorig;
//float dx,dy,dz;

float profile[100], avg[100], dev[100], dev2[100];
mvVec3f s = mvVec3f(ptVert->getPX(),ptVert->getPY(),ptVert->getPZ());
volume.getOrigin(xorig,yorig,zorig);

//mvVec3f orig = mvVec3f(xorig,yorig,zorig);
//s = s+orig;
mvVec3f norm = ptVert->getNormDir();
s = s-norm*0.2*25;

int jj;

for (jj = 0; jj<76;jj++)
{
	s += norm*0.2; 
	profile[jj] = volume.getValue(s.x,s.y,s.z);
	avg[jj] = dev[jj] = dev2[jj] = 0;
}				 
// 10 points average on the profile
for (int ii= -2 ; ii<2;ii++)
	for (jj = 5 ; jj < 71; jj++)
		avg[jj] = avg[jj]+ profile[jj+ii];
float devmax = -1000.0;
int index = 0; 
for (jj = 5; jj<71; jj++)
	{
	avg[jj]=avg[jj]/10;
	dev[jj] = avg[jj] - avg[jj-1];
	dev2[jj] = -dev[jj]+dev[jj-1];
	if (jj > 6 && dev2[jj] > devmax)
		{
		devmax = dev2[jj];
		index = jj;
		} 
	}

jj = index;	
//From the top the 2nd derivative go left
while(dev2[jj] > 0 && jj > 5) jj--;
int start = jj;



jj = index;	
//From the top of the 2nd derivative go right
while(dev2[jj] > 0 && jj < 71) jj++;
int end = jj;

// for debug 
/*ofstream out;
out.open("prof.dat");
for (int i = 0; i < 75; i ++)
	out << avg[i] << " "<<dev[i]<<" "<<dev2[i]<<endl;
out.close();
	*/


//cout << index<<" "<<start<<" "<<end<<" "<<endl;

ptVert->setThickness((end-start)*0.2);

return true;
}
		

double Mesh_Features::Calc_Convexity_and_Curvature(Vertex_with_Features *ptVert,
int level, int DB, Volume_ext<unsigned short> & volume_ext)
/* Return the condition number of the matrix */
{
std::slist<Vertex_with_Features *> nblist;
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;


std::slist<void *> ptTriangles = ptVert->getNeighborTriangles();
std::slist<void *>::iterator ptiter = ptTriangles.begin();
int totalTriangles, labels[MAX_NEIGHBORS]; 

Triangle * tri;
nblist.push_front(ptVert);
float area = 0;
float tarea = 0;
bool found = false;

totalTriangles = 0;
mvVec3f normal(0,0,0);
mvVec3f temp; //this temp is to solve the problem caused by the messy order for triangle sequences
for (i =0;i<level;i++)
	{
	nbiter = nblist.begin();
	while(nbiter != nblist.end())
		{
		ptTriangles = (*nbiter)->getNeighborTriangles();
		ptiter = ptTriangles.begin();
		
		while(ptiter != ptTriangles.end())
			{
			tri = (Triangle *)*ptiter;
			nblist.push_front(tri->getV1());
			nblist.push_front(tri->getV2());
			nblist.push_front(tri->getV3());
			ptiter++;
			if(i>=1) 
				{
				found = false;
				for( int j=0;j<totalTriangles;j++)
					if(tri->label() == labels[j]) found =
					true;
				if (!found) 
					{
					tarea = tri->getArea();
					area += tarea; 
					temp = tri->getNormDir();
#ifdef MESSED_NORM_DIR_IN_OOGL				
					if(temp.innerProduct(normal) >= 0)
						normal += temp*tarea;
					else 
						normal -= temp*tarea;
#else	
					normal += temp*tarea;
#endif				
				 	labels[totalTriangles] = tri->label();
					totalTriangles++;
					}
				}
			else
				{
				tarea = tri->getArea();
				area += tarea; 
				temp = tri->getNormDir();				
#ifdef MESSED_NORM_DIR_IN_OOGL				
				if(temp.innerProduct(normal) >= 0)
					normal += temp*tarea;
				else 
					normal -= temp*tarea;
#else
				normal += temp*tarea;
#endif				
			 	labels[totalTriangles] = tri->label();
				totalTriangles++;
				}			
			}
		nbiter++;	
		}
//Remove duplicate points
	nblist.sort();
	nblist.unique();
	}

if(DB ==1)
{
	ofstream outfile;
	outfile.open( "mesh1.off" );
	outfile<<"OFF"<<endl;
	outfile<<nblist.size()<<" "<<totalTriangles<<" 0"<<endl;
	outfile.close();


	char sout[80];
	sprintf(sout, "%f %f %f ",
	ptVert->getPX(),ptVert->getPY(),ptVert->getPZ());

	cout<<sout<<endl;	
}
normal.normalize();
		
nbiter = nblist.begin();
float shapeVariance = 0; 
ptVert->setNormDir(normal);
mvVec3f *thisv = new mvVec3f(ptVert->getPX(),ptVert->getPY(),ptVert->getPZ());
while(nbiter != nblist.end())
	{
	mvVec3f *temp = new mvVec3f((*nbiter)->getPX(),(*nbiter)->getPY(),
		(*nbiter)->getPZ());
	nbiter++;		
	*temp = *thisv -*temp;
	shapeVariance += normal.innerProduct((*temp));	
delete temp;		
	}
		
delete thisv;
ptVert->setConvexity(shapeVariance/area);
float result;
double cond = 0;
float gc, mc;
#ifdef NDEBUG
  result = curvature(*ptVert, nblist, cond, DB, volume_ext, gc, mc );
#else
  result = curvature(*ptVert, nblist,  gc, mc );
#endif 
if(gc > 1) gc = 1;
if(gc < -1) gc = -1;
 
ptVert->setGaussCurvature(gc);


if(mc > 1) mc = 1;
if(mc < -1) mc = -1;

ptVert->setMeanCurvature(mc);
return cond;
}
	
double Mesh_Features::Calc_Convexity_and_Curvature(Vertex_with_Features *ptVert, int level )
/* Return the condition number of the matrix */
{
std::slist<Vertex_with_Features *> nblist;
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;


std::slist<void *> ptTriangles = ptVert->getNeighborTriangles();
std::slist<void *>::iterator ptiter = ptTriangles.begin();
int totalTriangles, labels[MAX_NEIGHBORS]; 

Triangle * tri;
nblist.push_front(ptVert);
float area = 0;
float tarea = 0;
bool found = false;
totalTriangles = 0;
mvVec3f normal(0,0,0);
mvVec3f temp; //this temp is to solve the problem caused by the messy order for triangle sequences
for (i =0;i<level;i++)
	{
	nbiter = nblist.begin();
	while(nbiter != nblist.end())
		{
		ptTriangles = (*nbiter)->getNeighborTriangles();
		ptiter = ptTriangles.begin();
		
		while(ptiter != ptTriangles.end())
			{
			tri = (Triangle *)*ptiter;
			nblist.push_front(tri->getV1());
			nblist.push_front(tri->getV2());
			nblist.push_front(tri->getV3());
			ptiter++;
			if(i>=1) 
				{
				found = false;
				for( int j=0;j<totalTriangles;j++)
					if(tri->label() == labels[j]) found = true;
				if (!found) 
					{
					tarea = tri->getArea();
					area += tarea; 
					temp = tri->getNormDir();
					normal += temp*tarea;
				 	labels[totalTriangles] = tri->label();
					totalTriangles++;
					}
				}
			else
				{
				tarea = tri->getArea();
				area += tarea; 
				temp = tri->getNormDir();				
				normal += temp*tarea;
			 	labels[totalTriangles] = tri->label();
				totalTriangles++;
				}			
			}
		nbiter++;	
		}
//Remove duplicate points
	nblist.sort();
	nblist.unique();
	}

normal.normalize();
		
nbiter = nblist.begin();
float shapeVariance = 0; 
ptVert->setNormDir(normal);
/*
mvVec3f *thisv = new mvVec3f(ptVert->getPX(),ptVert->getPY(),ptVert->getPZ());
while(nbiter != nblist.end())
	{
	mvVec3f *temp = new mvVec3f((*nbiter)->getPX(),(*nbiter)->getPY(),
		(*nbiter)->getPZ());
	nbiter++;		
	*temp = *thisv -*temp;
	shapeVariance += normal.innerProduct((*temp));	
delete temp;		
	}
		
delete thisv;
ptVert->setConvexity(shapeVariance/area);
*/
float result;
double cond = 0;
float gc, mc;
result = curvature(*ptVert, nblist,  gc, mc );
if(gc > 1) gc = 1;
if(gc < -1) gc = -1;
 
ptVert->setGaussCurvature(gc);
//printf("gc %f", gc);

if(mc > 1) mc = 1;
if(mc < -1) mc = -1;

ptVert->setMeanCurvature(mc);
//printf("mc %f \n", mc);
return cond;
}
	

//get area covered by vertices  
double Mesh_Features::getAreaFromVertexList(std::slist<Vertex_with_Features *> vlist, double & isoPlanicity)
{

std::slist<void *> ptTriangles;
std::slist<void *>::iterator ptiter;
int totalTriangles, labels[MAX_VERTEX_LESION*3]; 

bool found = false;
bool found1 = false;
bool found2 = false;
bool found3 = false;
bool counted = false;
double area = 0;
double tarea = 0;
Triangle * tri;

std::slist<Vertex_with_Features *>::iterator  nbiter, tempiter;

totalTriangles = 0;
nbiter = vlist.begin();
isoPlanicity = 0;
double A,B,C,D,E,F;
mvVec3f n(0,0,0);
mvVec3f nlist[MAX_VERTEX_LESION*3];
float arealist[MAX_VERTEX_LESION*3];
A = B = C = D = E = F =0;

while(nbiter != vlist.end())
	{
	ptTriangles = (*nbiter)->getNeighborTriangles();
	ptiter = ptTriangles.begin();	
	while(ptiter != ptTriangles.end())
		{
		tri = (Triangle *)*ptiter;
		tempiter = vlist.begin();
		while(tempiter != vlist.end())
			{
			if((*tempiter)->label()==tri->getV1()->label()) found1 = true;
			if((*tempiter)->label()==tri->getV2()->label()) found2 = true;
			if((*tempiter)->label()==tri->getV3()->label()) found3 = true;
			if(found1&&found2&&found3) 
				{
				found = true;
				break;
				}			
			tempiter++;	
			}
		if(found)
			{		
			counted = false;
			for( int j=0;j<totalTriangles;j++)
				{
				if(tri->label() == labels[j]) 
					{
					counted = true;
					break;
					}
				}							
			if (!counted) 
				{
				tarea = tri->getArea();
				area += tarea;
				nlist[totalTriangles] =  tri->getNormDir();
				arealist[totalTriangles] = tarea;
				A += tarea * nlist[totalTriangles].x * nlist[totalTriangles].x;
				B += tarea * nlist[totalTriangles].x * nlist[totalTriangles].y;
				C += tarea * nlist[totalTriangles].x * nlist[totalTriangles].z;
				D += tarea * nlist[totalTriangles].y * nlist[totalTriangles].y;
				E += tarea * nlist[totalTriangles].y * nlist[totalTriangles].z;
				
			 	labels[totalTriangles] = tri->label();
				totalTriangles++;
				//cout << totalTriangles++ <<endl;
				}
			}
		counted = false;
		found = false;
		found1 = false;
		found2 = false;
		found3 = false;					
		ptiter++;	
		}
	nbiter++;	
	}

n.y = (E*A-B*C)/(B*B-A*D);
n.x = (D*C-B*E)/(B*B-A*D);
n.z = 1;
n.normalize();
//cout << "norm of plane "<< n.x <<" "<< n.y<<" "<<n.z << "Num of Tri :"<<totalTriangles<<endl;

//calculate residue error
for(int i = 0; i < totalTriangles; i ++)
	isoPlanicity += pow(n.innerProduct(nlist[i]), 2) * arealist[i];
isoPlanicity /= area;	
return area;
}



//wrtie triangles to oFF file covered by vertices  
int Mesh_Features::write_OFF_fromVertexList(std::slist<Vertex_with_Features *> vlist, char * filename)
{

std::slist<void *> ptTriangles, triList;
std::slist<void *>::iterator ptiter;
int totalTriangles, labels[MAX_VERTEX_LESION*3]; 

std::slist<Vertex_with_Features *>::iterator  nbiter, tempiter;

char filename2[80];

strcpy(filename2, filename);
strcat(filename2, ".dat");

Triangle * tri;
ofstream  outfile, outfile2;
char sout[80];

bool found = false;
bool found1 = false;
bool found2 = false;
bool found3 = false;
bool counted = false;
double area = 0;
double tarea = 0;


if(vlist.size() <= 1) return 0;

totalTriangles = 0;
nbiter = vlist.begin();
while(nbiter != vlist.end())
	{
	ptTriangles = (*nbiter)->getNeighborTriangles();
	ptiter = ptTriangles.begin();	
	while(ptiter != ptTriangles.end())
		{
		tri = (Triangle *)*ptiter;
		tempiter = vlist.begin();
		while(tempiter != vlist.end())
			{
			if((*tempiter)->label()==tri->getV1()->label()) found1 = true;
			if((*tempiter)->label()==tri->getV2()->label()) found2 = true;
			if((*tempiter)->label()==tri->getV3()->label()) found3 = true;
			if(found1&&found2&&found3) 
				{
				found = true;
				break;
				}			
			tempiter++;	
			}
		if(found)
			{		
			counted = false;
			for( int j=0;j<totalTriangles;j++)
				{
				if(tri->label() == labels[j]) 
					{
					counted = true;
					break;
					}
				}							
			if (!counted) 
				{
				tarea = tri->getArea();
				area += tarea;
			 	labels[totalTriangles] = tri->label();
				triList.push_front(tri);
				totalTriangles++;
				}
			}
		counted = false;
		found = false;
		found1 = false;
		found2 = false;
		found3 = false;					
		ptiter++;	
		}
	nbiter++;	
	}

outfile.open( filename );
outfile<<"OFF"<<endl;
outfile<<vlist.size()<<" "<<totalTriangles<<" 0"<<endl;

outfile2.open( filename2 );


nbiter = vlist.begin();
int j = 0; 

while(nbiter != vlist.end())
	{
	sprintf(sout, "%f %f %f ",(*nbiter)->getPX(),(*nbiter)->getPY(),(*nbiter)->getPZ());
	outfile<<sout<<endl;
	sprintf(sout, "%d %f %f\n",(*nbiter)->label(), (*nbiter)->getMeanCurvature(),(*nbiter)->getGaussCurvature());
	outfile2<<sout<<endl;
	
	(*nbiter)->setIndex(j++);
	nbiter++;
	}


ptiter = triList.begin();		
while(ptiter != triList.end())
	{
		tri = (Triangle *)*ptiter;
		outfile << "3 "<<(tri->getV1())->getIndex()<<" ";
		outfile << (tri->getV2())->getIndex()<<" ";
		outfile << (tri->getV3())->getIndex()<<" "<<"1 0 0 1"<<endl;
		ptiter++;				
	}
return 1;	
}


//wrtie vertex to VECTs file covered by vertices  
int Mesh_Features::write_VECT_fromVertexList(std::slist<Vertex_with_Features *> vlist, Vertex_with_Features *vCenter, mvVec3f normal, char * filename)
{

std::slist<void *> ptTriangles, triList;
std::slist<void *>::iterator ptiter;
int totalTriangles; 

std::slist<Vertex_with_Features *>::iterator  nbiter, tempiter;
int i;

Triangle * tri;

ofstream  outfile, tempfile;
char sout[80];
int nvector = 0;
char emptyLine[40] = {"                                   "};

bool found = false;
bool found1 = false;
bool found2 = false;
bool found3 = false;

if(vlist.size() <= 1) return 0;
if(vlist.size() > 3000) {
	cout << "lesion too big " << endl;
	return 0;
	}

outfile.open( filename );
outfile<<"VECT"<<endl;

long pos1 = outfile.tellp();

outfile <<emptyLine<<endl;

long pos2 = outfile.tellp();

for(i=0;i<3000;i++)
  outfile<<"   ";
outfile<<endl;

long pos3 = outfile.tellp();

outfile<<" 1 ";
for(i=1;i<3000;i++)
  outfile<<"   ";
outfile<<endl;

totalTriangles = 0;
nbiter = vlist.begin();
while(nbiter != vlist.end())
	{
//	cout<<"i id"<<(*nbiter)->label()<<endl;
	ptTriangles = (*nbiter)->getNeighborTriangles();
	ptiter = ptTriangles.begin();	
	while(ptiter != ptTriangles.end())
		{
		tri = (Triangle *)*ptiter;
		tempiter = nbiter;
		tempiter ++;
		while(tempiter != vlist.end())
			{
			if((*tempiter)->label()==tri->getV1()->label()) 
				{
				found1=true;
				sprintf(sout, "%f %f %f   %f %f %f ",tri->getV1()->getPX(), tri->getV1()->getPY(),  tri->getV1()->getPZ(), \
					(*nbiter)->getPX(),(*nbiter)->getPY(),(*nbiter)->getPZ());
				outfile <<sout <<endl;
				nvector++;
				//cout<<sout<<endl;
				//write, nvetor++;
				}
			if((*tempiter)->label()==tri->getV2()->label()) 
				{
				found2=true;
				sprintf(sout, "%f %f %f   %f %f %f",tri->getV2()->getPX(), tri->getV2()->getPY(),  tri->getV2()->getPZ(), \
					(*nbiter)->getPX(),(*nbiter)->getPY(),(*nbiter)->getPZ());
				outfile <<sout <<endl;
				nvector++;
				//write, nvetor++;
//				cout<<sout<<endl;
				}
			if((*tempiter)->label()==tri->getV3()->label()) 
				{
				found3=true;
				sprintf(sout, "%f %f %f   %f %f %f", tri->getV3()->getPX(), tri->getV3()->getPY(),  tri->getV3()->getPZ(), \
					(*nbiter)->getPX(),(*nbiter)->getPY(),(*nbiter)->getPZ());
				outfile <<sout <<endl;
				nvector++;
//				cout<<sout<<endl;
				//write, nvetor++;
				}
			tempiter++;	
			}

		found = false;
		found1 = false;
		found2 = false;
		found3 = false;					
		ptiter++;	
		}
	nbiter++;	
	}

//display noremal and center
sprintf(sout, "%f %f %f   %f %f %f", vCenter->getPX(), vCenter->getPY(),  vCenter->getPZ(), \
	vCenter->getPX()+5*normal.x, vCenter->getPY()+5*normal.y,  vCenter->getPZ()+5*normal.z);
outfile <<sout <<endl;


//green is the color
outfile <<" 0 1 0 1"<<endl;
outfile <<" 1 0 0 1"<<endl;

outfile.seekp(pos1);
sprintf(sout,"%d %d 2",nvector+1, 2*nvector + 2); 
outfile <<sout;

outfile.seekp(pos2);
for(i=0;i<nvector + 1;i++)
  outfile<<" 2 ";

outfile.seekp(pos3);
outfile<<" 1 ";
for(i=1;i<nvector;i++)
  outfile<<" 0 ";
outfile<<" 1 ";


outfile.close();
return 1;	
}


#ifdef NEIGHBORDEBUG
bool Mesh_Features::neighborhood_oogl(char * filename, Vertex_with_Features *ptVert, int level){

/* write all oogl files for a neighboirhood observation */

std::slist<Vertex_with_Features *> nblist;
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;

std::slist<void *> ptTriangles = ptVert->getNeighborTriangles();
std::slist<void *>::iterator ptiter = ptTriangles.begin();
int totalTriangles, labels[MAX_NEIGHBORS]; 

Triangle * tri;
ofstream  outfile, outfile2, outfile3;

outfile.open( "neighborhood.list" );

outfile << "LIST\n";
outfile<<"<"<<filename<<endl;
outfile<<"<norm.vect"<<endl;
outfile <<"<mesh.off"<<endl;
outfile.close();
cout<<"Finish Writing neighborhood.list"<<endl;	
nblist.push_front(ptVert);
float area = 0;
float tarea;

bool found = false;
mvVec3f normal(0,0,0);
mvVec3f temp; //this temp is to solve the problem caused by the messy order for triangle sequences
totalTriangles = 0;
for (i =0;i<level;i++)
	{
	nbiter = nblist.begin();
	while(nbiter != nblist.end())
		{
		ptTriangles = (*nbiter)->getNeighborTriangles();
		ptiter = ptTriangles.begin();
		
		while(ptiter != ptTriangles.end())
			{
			tri = (Triangle *)*ptiter;
			nblist.push_front(tri->getV1());
			nblist.push_front(tri->getV2());
			nblist.push_front(tri->getV3());
			ptiter++;
			if(i>=1) 
				{
				found = false;
				for( int j=0;j<totalTriangles;j++)
					if(tri->label() == labels[j]) found =
					true;
				if (!found) 
					{
					tarea = tri->getArea();
					tri->setIndex(1);
					temp = tri->getNormDir();					
#ifdef MESSED_NORM_DIR_IN_OOGL				
				if(temp.innerProduct(normal) >= 0)
					normal += temp*tarea;
				else 
					normal -= temp*tarea;
#else
				normal += temp*tarea;
#endif				
					area += tarea;
				 	labels[totalTriangles] = tri->label();
					totalTriangles++;
					}
				}
			else
				{
				tarea = tri->getArea();
				tri->setIndex(1);
				temp = tri->getNormDir();
#ifdef MESSED_NORM_DIR_IN_OOGL				
				if(temp.innerProduct(normal) >= 0)
					normal += temp*tarea;
				else 
					normal -= temp*tarea;
#else
				normal += temp*tarea;
#endif				
					
				area += tarea;
				
			 	labels[totalTriangles] = tri->label();
				totalTriangles++;
				}			
			}
		nbiter++;	
		}
//Remove duplicate points
	nblist.sort();
	nblist.unique();
	}

//normal.normalize();
//ptVert->setNormDir(normal);


outfile2.open( "mesh.off" );
outfile2<<"OFF"<<endl;
outfile2<<nblist.size()<<" "<<totalTriangles<<" 0"<<endl;

//cout <<nblist.size()<<" "<<totalTriangles<<" 0"<<endl;
outfile3.open ("mesh1.off", ofstream::out | ofstream::app);

outfile.open( "norm.vect" );
outfile << "VECT\n";
outfile <<"1 2 1\n";
outfile <<"2"<<endl;
outfile <<"1  ";
outfile <<endl;



char sout[80];
sprintf(sout, "%f %f %f ",
ptVert->getPX(),ptVert->getPY(),ptVert->getPZ());

outfile<<sout;	
outfile<<ptVert->getPX()+(ptVert->getNormDir()).x*8<<" "
	<<ptVert->getPY()+(ptVert->getNormDir()).y*8<<" "
	<<ptVert->getPZ()+(ptVert->getNormDir()).z*8<<endl;	


nbiter = nblist.begin();
int j = 0; 

while(nbiter != nblist.end())
	{
//	sprintf(sout, "%f %f %f ",ptVert->getPX(),ptVert->getPY(),ptVert->getPZ());
//	outfile<<sout;	
	sprintf(sout, "%f %f %f ",(*nbiter)->getPX(),(*nbiter)->getPY(),(*nbiter)->getPZ());
//	outfile<<sout<<endl;
	outfile2<<sout<<endl;

	(*nbiter)->setIndex(j++);

	nbiter++;
	}


nbiter = nblist.begin();
while(nbiter != nblist.end())
	{
	ptTriangles = (*nbiter)->getNeighborTriangles();
	ptiter = ptTriangles.begin();		
	while(ptiter != ptTriangles.end())
	{
		tri = (Triangle *)*ptiter;
		if(tri->getIndex()==1)
			{
			outfile2 << "3 "<<(tri->getV1())->getIndex()<<" ";
			outfile2 << (tri->getV2())->getIndex()<<" ";
			outfile2 << (tri->getV3())->getIndex()<<" "<<"1 0 0 1"<<endl;
			outfile2 << "#"<<(tri->getNormDir()).x<<" "
				<<(tri->getNormDir()).y<<" "
				<<(tri->getNormDir()).z<<endl;	
			
			outfile3 << "3 "<<(tri->getV1())->getIndex()<<" ";
			outfile3 << (tri->getV2())->getIndex()<<" ";
			outfile3 << (tri->getV3())->getIndex()<<" "<<"1 0 0 1"<<endl;

			tri->setIndex(0);
			}
		ptiter++;				
	}
	nbiter++;
	}
	


outfile <<"1 0 1 1"<<endl; //red normal
outfile.close();
//cout<<"Finish Writing norm.vect"<<endl;	

outfile2 << "#"<<(ptVert->getNormDir()).x<<" "
	<<(ptVert->getNormDir()).y<<" "
	<<(ptVert->getNormDir()).z<<endl;	

outfile2.close();
outfile3.close();
//cout<<"Finish Writing mesh.off"<<endl;	
//cout<<"Finish Writing mesh1.off"<<endl;	
return true;
}
	
#endif
	

std::slist<Vertex_with_Features *> 
Mesh_Features::getNeighborVertices(Vertex_with_Features *ptVert, int level)
{
std::slist<Vertex_with_Features *> nblist;
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;

std::slist<void *> ptTriangles = ptVert->getNeighborTriangles();
std::slist<void *>::iterator ptiter = ptTriangles.begin();

Triangle * tri;
nblist.push_front(ptVert);
for (i =0;i<level;i++)
	{
	nbiter = nblist.begin();
	while(nbiter != nblist.end())
		{
		ptTriangles = (*nbiter)->getNeighborTriangles();
		ptiter = ptTriangles.begin();
		
		while(ptiter != ptTriangles.end())
			{
			tri = (Triangle *)*ptiter;
			nblist.push_front(tri->getV1());
			nblist.push_front(tri->getV2());
			nblist.push_front(tri->getV3());
			ptiter++;
			}
		nbiter++;	
		}
	}

//Remove duplicate points
nblist.sort();
nblist.unique();
return nblist;		 
}
 

/* Added in April, 30 , 2003 for another neibourhood based on distance from the certex */

std::slist<Vertex_with_Features *> 
Mesh_Features::getNeighborVertices(Vertex_with_Features *ptVert, float distance)
{
std::slist<Vertex_with_Features *> nblist;
std::slist<Vertex_with_Features *>::iterator  nbiter;
std::slist<Vertex_with_Features *> visitlist;
std::slist<Vertex_with_Features *>::iterator  visititer;
int i;

std::slist<void *> ptTriangles = ptVert->getNeighborTriangles();
std::slist<void *>::iterator ptiter = ptTriangles.begin();

Triangle * tri;
nblist.push_front(ptVert);
ptVert->setflag(true);
nbiter = nblist.begin();
Vertex_with_Features * curV;
while(nbiter != nblist.end())
	{
	ptTriangles = (*nbiter)->getNeighborTriangles();
	ptiter = ptTriangles.begin();
	
	while(ptiter != ptTriangles.end())
		{
		tri = (Triangle *)*ptiter;
		curV = tri->getV1(); 
		if(!curV->getflag())
			{
			visitlist.push_front(curV);
			curV->setflag(true);
			if(curV->distance(*ptVert) < distance)	nblist.push_front(curV);
			}
		curV = tri->getV2(); 
		if(!curV->getflag())
			{
			visitlist.push_front(curV);
			curV->setflag(true);
			if(curV->distance(*ptVert) < distance)	nblist.push_front(curV);
			}
		curV = tri->getV3(); 
		if(!curV->getflag())
			{
			visitlist.push_front(curV);
			curV->setflag(true);
			if(curV->distance(*ptVert) < distance)	nblist.push_front(curV);
			}
		ptiter++;
		}
	nbiter++;	
	}

visititer = visitlist.begin();
while(visititer != visitlist.end())
	(*visititer)->setflag(false);

return nblist;		 
}
 

/* Added in April, 30 , 2003 detect a verterx is isolated or not */
#include <stack>

bool Mesh_Features::isIsolate(Vertex_with_Features *ptVert, float range)
{
std::stack<Vertex_with_Features *> open;
std::stack<Vertex_with_Features *> close;
int i;

std::slist<void *> ptTriangles = ptVert->getNeighborTriangles();
std::slist<void *>::iterator ptiter = ptTriangles.begin();

Triangle * tri;
open.push(ptVert);
ptVert->setflag(true);
close.push(ptVert);
Vertex_with_Features * curV;
float max_dist = 0;
float dist;
while(!open.empty())
{
curV = open.top();
open.pop();
ptTriangles = curV->getNeighborTriangles();
ptiter = ptTriangles.begin();
	
while(ptiter != ptTriangles.end())
	{
	tri = (Triangle *)*ptiter;
	curV = tri->getV1(); 
	if(!curV->getflag())
		{
		curV->setflag(true);
		close.push(curV);
		if((dist = curV->distance(*ptVert)) < range)	
			{
			open.push(curV);
			if (dist > max_dist) max_dist = dist;
			}
		else	{
			while(!close.empty())
				{
				curV = close.top();
				close.pop();
				curV->setflag(false);
				}
			return false;
			}
		}
	curV = tri->getV2(); 
	if(!curV->getflag())
		{
		curV->setflag(true);
		close.push(curV);
		if((dist = curV->distance(*ptVert)) < range)
			{
			open.push(curV);
			if (dist > max_dist) max_dist = dist;
			}
		else	{
			while(!close.empty())
				{
				curV = close.top();
				close.pop();
				curV->setflag(false);
				}
			return false;
			}
		}
	curV = tri->getV3(); 
	if(!curV->getflag())
		{
		curV->setflag(true);
		close.push(curV);
		if((dist = curV->distance(*ptVert)) < range)	
			{
			open.push(curV);
			if (dist > max_dist) max_dist = dist;
			}
		else	{
			while(!close.empty())
				{
				curV = close.top();
				close.pop();
				curV->setflag(false);
				}
			return false;
			}
		}	
		ptiter++;
	}
}

while(!close.empty())
{
curV = close.top();
close.pop();
curV->setflag(false);
}

cout << "max_distance is" << max_dist <<endl;
return true;		 
}
 




std::slist<Vertex_with_Features *> 
Mesh_Features::getNeighborVertices(Vertex_with_Features *ptVert, int level, int
flag)

//get Neighbors with flag opration
// flag = 0, get Neighbors with flag = false;
// flag = 1, get Neighbors with flag = true;
// flag = 2, get Neighbors with flag = false, set flag to true;
// flag = 3, get Neighbors with flag = true, set flag to false;

{
std::slist<Vertex_with_Features *> nblist;
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;

std::slist<void *> ptTriangles = ptVert->getNeighborTriangles();
std::slist<void *>::iterator ptiter = ptTriangles.begin();

bool vflag;
Triangle * tri;
vflag = ptVert->getflag();

/*if(vflag == 0 && (flag == 0 || flag ==2))
	nblist.push_front(ptVert);
if(vflag == 1 && (flag == 1 || flag ==3))
*/
	nblist.push_front(ptVert);

for (i =0;i<level;i++)
	{
	nbiter = nblist.begin();
	while(nbiter != nblist.end())
		{
		ptTriangles = (*nbiter)->getNeighborTriangles();
		ptiter = ptTriangles.begin();
		
		while(ptiter != ptTriangles.end())
			{
			tri = (Triangle *)*ptiter;
			vflag = tri->getV1()->getflag();
			if(vflag == 0 && (flag == 0 || flag ==2))
				nblist.push_front(tri->getV1());
			if(vflag == 1 && (flag == 1 || flag ==3))
				nblist.push_front(tri->getV1());

			vflag = tri->getV2()->getflag();
			if(vflag == 0 && (flag == 0 || flag ==2))
				nblist.push_front(tri->getV2());
			if(vflag == 1 && (flag == 1 || flag ==3))
				nblist.push_front(tri->getV2());

			vflag = tri->getV3()->getflag();
			if(vflag == 0 && (flag == 0 || flag ==2))
				nblist.push_front(tri->getV3());
			if(vflag == 1 && (flag == 1 || flag ==3))
				nblist.push_front(tri->getV3());

			ptiter++;
			}
		nbiter++;	
		}
	}

//Remove duplicate points
nblist.sort();
nblist.unique();

if(flag==2)
{
	nbiter = nblist.begin();
	while(nbiter != nblist.end())
	{
		(*nbiter)->setflag(true);
		nbiter++;	
	}
}
		
if(flag==3)
{
	nbiter = nblist.begin();
	while(nbiter != nblist.end())
	{
		(*nbiter)->setflag(false);
		nbiter++;	
	}
}

return nblist;		 
}
 

void ooglwrite(Mesh_Features &mesh, int type)
{
        ofstream  outfile, outfile0, outfile1, outfile2;
	char entry[100];
	int i;
	Vertex_with_Features *v1, *v2, *v3;
	outfile.open( "Gcurvature.off_" );
	outfile0.open( "Mcurvature.off_" );

	outfile1.open( "convexity.off_" );
	outfile2.open( "thickness.off_" );
	
	outfile <<mesh.getNtriangles()<<endl;
	outfile0 <<mesh.getNtriangles()<<endl;
	outfile1 <<mesh.getNtriangles()<<endl;
	outfile2 <<mesh.getNtriangles()<<endl;
	
	Mesh_Features::vertexIterator mvi = mesh.vertexBegin();
	i = 0;
	float value;

	Mesh_Features::triangleIterator mti = mesh.triangleBegin();

	while( mti !=  mesh.triangleEnd() ){
		v1 = (*mti)->getV1();
		v2 = (*mti)->getV2();
		v3 = (*mti)->getV3();
		value = (v1->getGaussCurvature() + v2->getGaussCurvature()+v3->getGaussCurvature())/3;
		outfile << value<<" ";
		value = (v1->getMeanCurvature() + v2->getMeanCurvature()+v3->getMeanCurvature())/3;
		outfile0 << value<<" ";

		value = (v1->getConvexity() + v2->getConvexity()+v3->getConvexity())/3;
		outfile1 << value<<" ";
		
		value = (v1->getThickness() + v2->getThickness()+v3->getThickness())/3;
		outfile2 << value<<" ";		
		mti++;
	}
	outfile << endl;
	outfile0 << endl;
	outfile1 << endl;
	outfile2 << endl;
	
	sprintf(entry, "%i %i 0\n\n", mesh.getNverticies(), mesh.getNtriangles());
	outfile << entry;
	outfile0 << entry;
	outfile1 << entry;
	outfile2 << entry;

	while( mvi !=  mesh.vertexEnd() ){
//		sprintf(entry, "%f %f %f\n", (*mvi)->getPX(), (*mvi)->getPY(), 1.5*(*mvi)->getPZ());
		sprintf(entry, "%f %f %f\n", (*mvi)->getPX(), (*mvi)->getPY(), (*mvi)->getPZ());
		outfile << entry;
		outfile0 << entry;
		outfile1 << entry;
		outfile2 << entry;
		// label vertex 		
//		(*mvi)->label(i);
		i++;
		mvi++;
	}
	
	outfile << "##"<<endl;
	outfile0 << "##"<<endl;
	outfile1 << "##"<<endl;
	outfile2 << "##"<<endl;
	mti = mesh.triangleBegin();
	while( mti !=  mesh.triangleEnd() ){
		v1 = (*mti)->getV1();
		v2 = (*mti)->getV2();
		v3 = (*mti)->getV3();
		value = (v1->getGaussCurvature() + v2->getGaussCurvature()+v3->getGaussCurvature())/3;
		sprintf(entry, "3\t%i %i %i\n", v1->label(), v2->label(), v3->label());
		outfile << entry;
		value = (v1->getMeanCurvature() + v2->getMeanCurvature()+v3->getMeanCurvature())/3;
		sprintf(entry, "3\t%i %i %i\n", v1->label(), v2->label(), v3->label());
		outfile0 << entry;

		value = (v1->getConvexity() + v2->getConvexity()+v3->getConvexity())/3;
		sprintf(entry, "3\t%i %i %i\n", v1->label(), v2->label(), v3->label());
		outfile1 << entry;

		value = (v1->getThickness() + v2->getThickness()+v3->getThickness())/3;
		sprintf(entry, "3\t%i %i %i\n", v1->label(), v2->label(), v3->label());
		outfile2<< entry;

		mti++;
	}

	outfile << "#end";
	outfile0 << "#end";
	outfile1 << "#end";
	outfile2 << "#end";

	outfile.close();
	outfile0.close();
	outfile1.close();
	outfile2.close();

};

void ooglread(Mesh_Features &mesh, char * name, float & dx, float & dy, float & dz, float & xorig, float & yorig, float & zorig)
{
        ifstream  infile;
	char entry[100];
	int i, nv, nt, temp, idx1, idx2, idx3;
	float *vx, *vy, *vz;
	Vertex_with_Features v1, v2, v3;
	Triangle tri;

	infile.open( name );
	if(!infile){
		cerr << "Error in ooglread(...): cannot open file" << endl;
		exit(-1);
	}
	
	infile.getline(entry, 100);
	infile.getline(entry, 100);

	infile >> nv >> nt >> temp;	
	clog << nv << " vertices, " << nt << " triangles" << endl;

	infile.getline(entry, 100);
	
	vx = new float[nv];
	vy = new float[nv];
	vz = new float[nv];
	
	//read in vertices
	for(i=0; i < nv; i++){
		infile >> vx[i] >> vy[i] >> vz[i];
#ifdef OOGL_IN_VOXEL_UNIT
		vx[i] = vx[i]*dx +xorig;
		vy[i] = vy[i]*dy +yorig;
		vz[i] = vz[i]*dz +zorig;			
#endif		
	}	
	
#ifdef NDEBUG
	int num = 0;
	int found = 0;
	for (i =0;i<nv; i++)
		{
		found = 0;
		for(int j = i+1; j < nv; j++)
			{
			if(vx[i] == vx[j] && vy[i] ==vy[j] && vz[i]==vz[j]) 
				{
				found = 1;
				break;
				}
			}	
		if(found ==0) num ++;
		}		
	cout << "non duplicated vertices num"<< num<<endl;
#endif	 
	//read in triangles and add to mesh
	for(i=0; i < nt; i++){

		infile >> temp >> idx1 >> idx2 >> idx3;
		assert(temp == 3);
		v1.setPoint(vx[idx1], vy[idx1], vz[idx1]);
		v2.setPoint(vx[idx2], vy[idx2], vz[idx2]);
		v3.setPoint(vx[idx3], vy[idx3], vz[idx3]);
		tri.set(&v1, &v2, &v3);
		tri.label(i);
		mesh.addTriangle(tri);			
	}
	
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
};




float * Mesh_Features::getPoints(int * nP)
{

	float * points;
	int i, l;
	std::slist<Triangle *>::iterator sli;
	Vertex_with_Features * v;

	l = pttriangles.size();
	*nP = 9*l;

	points = new float[9*l];

	if(verticies->isShared()){
		float xo, yo, zo, dx, dy, dz;
		
		xo = verticies->origin[0];
		yo = verticies->origin[1];
		zo = verticies->origin[2];

		dx = verticies->voxelSize[0];
		dy = verticies->voxelSize[1];
		dz = verticies->voxelSize[2];

		sli = pttriangles.begin();
		i = 0;
		while(sli != pttriangles.end()){
			v = (*sli)->getV1();
			points[i] = xo + dx*(v->getPX());
			points[i + 1] = yo + dy*(v->getPY());
			points[i + 2] = zo + dz*(v->getPZ());
			
			v = (*sli)->getV2();
			i+= 3;
			points[i] =  xo + dx*(v->getPX());
			points[i + 1] = yo + dy*(v->getPY());
			points[i + 2] = zo + dz*(v->getPZ());	
			
			v = (*sli)->getV3();
			i+= 3;
			points[i] =  xo + dx*(v->getPX());
			points[i + 1] = yo + dy*(v->getPY());
			points[i + 2] = zo + dz*(v->getPZ());
			
			i+= 3;

			sli++;
		}
	}
	else{

		sli = pttriangles.begin();
		i = 0;
		while(sli != pttriangles.end()){
			v = (*sli)->getV1();
			points[i] = v->getPX();
			points[i + 1] = v->getPY();
			points[i + 2] = v->getPZ();
			
			v = (*sli)->getV2();
			i+= 3;
			points[i] = v->getPX();
			points[i + 1] = v->getPY();
			points[i + 2] = v->getPZ();	
			
			v = (*sli)->getV3();
			i+= 3;
			points[i] = v->getPX();
			points[i + 1] = v->getPY();
			points[i + 2] = v->getPZ();
			
			i+= 3;

			sli++;
		}
	}
	
	return points;
}

float * Mesh_Features::getNormals(int * nP)
{

	float * normals;
	int i, l;
	std::slist<Triangle *>::iterator sli;
	Vertex_with_Features * v;
	mvVec3f norm;

	l = pttriangles.size();
	*nP = 9*l;

	normals = new float[9*l];

	if(verticies->isShared()){
		float xo, yo, zo, dx, dy, dz;
		
		xo = verticies->origin[0];
		yo = verticies->origin[1];
		zo = verticies->origin[2];

		dx = verticies->voxelSize[0];
		dy = verticies->voxelSize[1];
		dz = verticies->voxelSize[2];
		

		sli = pttriangles.begin();
		i = 0;
		while(sli != pttriangles.end()){
			v = (*sli)->getV1();
			//Modified by Hong
			if(v->isNormValid()){
			  norm = v->getNormDir();
			  normals[i] = norm.x;
			  normals[i + 1] = norm.y;
			  normals[i + 2] = norm.z;
				}
			else {
			  normals[i] = xo + dx*(v->getPX());
			  normals[i + 1] = yo + dy*(v->getPY());
			  normals[i + 2] = zo + dz*(v->getPZ());
			     }
			v = (*sli)->getV2();
			i+= 3;
			//Modified by Hong
			if(v->isNormValid()){
			  norm = v->getNormDir();
			  normals[i] = norm.x;
			  normals[i + 1] = norm.y;
			  normals[i + 2] = norm.z;
				}
			else {
			  normals[i] = xo + dx*(v->getPX());
			  normals[i + 1] = yo + dy*(v->getPY());
			  normals[i + 2] = zo + dz*(v->getPZ());
			     }
			
			v = (*sli)->getV3();
			i+= 3;
			//Modified by Hong
			if(v->isNormValid()){
			  norm = v->getNormDir();
			  normals[i] = norm.x;
			  normals[i + 1] = norm.y;
			  normals[i + 2] = norm.z;
				}
			else {
			  normals[i] = xo + dx*(v->getPX());
			  normals[i + 1] = yo + dy*(v->getPY());
			  normals[i + 2] = zo + dz*(v->getPZ());
			     }
			
			i+= 3;

			sli++;
		}
	}
	else{

		sli = pttriangles.begin();
		i = 0;
		while(sli != pttriangles.end()){
			v = (*sli)->getV1();
			//Modified by Hong
			if(v->isNormValid()){
			  norm = v->getNormDir();
			  normals[i] = norm.x;
			  normals[i + 1] = norm.y;
			  normals[i + 2] = norm.z;
				}
			else {
			  normals[i] = (v->getPX());
			  normals[i + 1] =(v->getPY());
			  normals[i + 2] = (v->getPZ());
			     }
			
			v = (*sli)->getV2();
			i+= 3;
			//Modified by Hong
			if(v->isNormValid()){
			  norm = v->getNormDir();
			  normals[i] = norm.x;
			  normals[i + 1] = norm.y;
			  normals[i + 2] = norm.z;
				}
			else {
			  normals[i] =(v->getPX());
			  normals[i + 1] = (v->getPY());
			  normals[i + 2] = (v->getPZ());
			     }
			
			v = (*sli)->getV3();
			i+= 3;
			//Modified by Hong
			if(v->isNormValid()){
			  norm = v->getNormDir();
			  normals[i] = norm.x;
			  normals[i + 1] = norm.y;
			  normals[i + 2] = norm.z;
				}
			else {
			  normals[i] = (v->getPX());
			  normals[i + 1] = (v->getPY());
			  normals[i + 2] = (v->getPZ());
			     }
			
			i+= 3;

			sli++;
		}
	}
	
	return normals;
}


bool Mesh_Features::read(ifstream & theInStream)
{
	if( theInStream.bad() ){ //bad stream
		return false;
	}
	
	// temporary holders
	unsigned long tempul;
	float tempf;
	
	// counters
	int i;
	
	// bounds
	theInStream.read((char *) &tempul, sizeof tempul);
	if( theInStream.bad() ) return false;
	verticies->bounds[0] = tempul;
	theInStream.read((char *) &tempul, sizeof tempul);
	if( theInStream.bad() ) return false;
	verticies->bounds[1] = tempul;
	theInStream.read((char *) &tempul, sizeof tempul);
	if( theInStream.bad() ) return false;
	verticies->bounds[2] = tempul;
	
	// voxel size
	theInStream.read((char *) &tempf, sizeof tempf);
	if( theInStream.bad() ) return false;
	verticies->voxelSize[0] = tempf;
	theInStream.read((char *) &tempf, sizeof tempf);
	if( theInStream.bad() ) return false;
	verticies->voxelSize[1] = tempf;
	theInStream.read((char *) &tempf, sizeof tempf);
	if( theInStream.bad() ) return false;
	verticies->voxelSize[2] = tempf;
	
	// origin
	theInStream.read((char *) &tempf, sizeof tempf);
	if( theInStream.bad() ) return false;
	verticies->origin[0] = tempf;
	theInStream.read((char *) &tempf, sizeof tempf);
	if( theInStream.bad() ) return false;
	verticies->origin[1] = tempf;
	theInStream.read((char *) &tempf, sizeof tempf);
	if( theInStream.bad() ) return false;
	verticies->origin[2] = tempf;

	// temp vertex holders
	std::vector<float> vx, vy, vz, nx, ny, nz;
	int nv;

	// number of vertices
	theInStream.read((char *) &tempul, sizeof tempul);
	if( theInStream.bad() ) return false;
	nv = tempul;

	// pre-allocate for efficiency
	vx.reserve(nv);
	vy.reserve(nv);
	vz.reserve(nv);
	nx.reserve(nv);
	ny.reserve(nv);
	nz.reserve(nv);

	// vertices: position (3 floats) normal (3 floats)
	//read in vertices
	for(i=0; i < nv; i++){
		// vertex position
		theInStream.read((char *) &tempf, sizeof tempf);
		if( theInStream.bad() ) return false;
		vx[i] = tempf;
		
		theInStream.read((char *) &tempf, sizeof tempf);
		if( theInStream.bad() ) return false;
		vy[i] = tempf;

		theInStream.read((char *) &tempf, sizeof tempf);
		if( theInStream.bad() ) return false;
		vz[i] = tempf;
		
		// normal direction
		theInStream.read((char *) &tempf, sizeof tempf);
		if( theInStream.bad() ) return false;
		nx[i] = tempf;
		
		theInStream.read((char *) &tempf, sizeof tempf);
		if( theInStream.bad() ) return false;
		ny[i] = tempf;
		
		theInStream.read((char *) &tempf, sizeof tempf);
		if( theInStream.bad() ) return false;
		nz[i] = tempf;
	}
	
	// number of triangles
	int nt;
	theInStream.read((char *) &tempul, sizeof tempul);
	if( theInStream.bad() ) return false;
	nt = tempul;
	
	// read in and insert triangles
	Vertex_with_Features v1, v2, v3;
	Triangle tri;
	int idx1, idx2, idx3;
	for(i=0; i < nt; i++){
		// first vertex
		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		idx1 = tempul;

		// second vertex
		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		idx2 = tempul;

		// third vertex
		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		idx3 = tempul;

		v1.setPoint(vx[idx1], vy[idx1], vz[idx1]);
		v1.setNormal(nx[idx1], ny[idx1], nz[idx1]);
		v2.setPoint(vx[idx2], vy[idx2], vz[idx2]);
		v2.setNormal(nx[idx2], ny[idx2], nz[idx2]);
		v3.setPoint(vx[idx3], vy[idx3], vz[idx3]);
		v3.setNormal(nx[idx3], ny[idx3], nz[idx3]);
		tri.set(&v1, &v2, &v3);
		addTriangle(tri);	

		//fprintf(stderr, "triangle %i/%i\r", i, nt);
	}


	return true;
}

// FIX THIS TO USE std::vector INSTEAD OF ARRAYS
bool Mesh_Features::ooglread(const char * name)
{
        ifstream  infile;
	char entry[100];
	int i, nv, nt, temp, idx1, idx2, idx3;
	float *vx, *vy, *vz, *nx, *ny, *nz;
	Vertex_with_Features v1, v2, v3;
	Triangle tri;

	infile.open( name );
	if(!infile){
		cerr << "Error in IMTK::ooglread(...): cannot open file" << endl;
		return false;
	}

	// clear the old data, no sharing of vertices 
	clear(false);
	// allocate vertices, cannot share
	//verticies = new MeshVertexSet(false);
	
	infile.getline(entry, 100);
	infile.getline(entry, 100);

	infile >> nv >> nt >> temp;	
	clog << nv << " vertices, " << nt << " triangles" << endl;

	infile.getline(entry, 100);
	
	vx = new float[nv];
	vy = new float[nv];
	vz = new float[nv];
	nx = new float[nv];
	ny = new float[nv];
	nz = new float[nv];
	
	Vertex_with_Features** vlist = (Vertex_with_Features**)malloc(sizeof(Vertex_with_Features *) * nv); 
	//read in vertices
	for(i=0; i < nv; i++){
		infile >> vx[i] >> vy[i] >> vz[i]; // >> nx[i] >> ny[i] >> nz[i];	
		v1.setPoint(vx[i], vy[i], vz[i]); 
		vlist[i] = verticies->insert(v1);
	}

	clog << nv << " vertices loaded.\n" << endl;

	//read in triangles and add to mesh
	for(i=0; i < nt; i++){
		infile >> temp >> idx1 >> idx2 >> idx3;
		assert(temp == 3);
		/*v1.setPoint(vx[idx1], vy[idx1], vz[idx1]);
		v2.setPoint(vx[idx2], vy[idx2], vz[idx2]);
		v3.setPoint(vx[idx3], vy[idx3], vz[idx3]);
		tri.set(&v1, &v2, &v3);
		tri.label(i);
		addTriangle(tri); */
		addTriangle(vlist[idx1], vlist[idx2], vlist[idx3], i);
		
		fprintf(stderr, "triangle %i/%i\r", i, nt);		
	}
	free(vlist);
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
	delete [] nx;
	delete [] ny;
	delete [] nz;
	
	return true;
};

void Mesh_Features::setVerticesflag(bool f)
	{
	int i =0;
	vertexIterator v = vertexBegin();
	while (v !=  vertexEnd()) 
		(*v++)->setflag(f);
	}

int Mesh_Features::CheckVerticesflag(bool f)
	{
	int i =0;
	vertexIterator v = vertexBegin();
	while (v !=  vertexEnd()) 
		if((*v++)->getflag() != f) i++;
		
	return i;
	}
