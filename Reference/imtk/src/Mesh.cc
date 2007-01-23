/********************************************************
File: Mesh.cc
Abstract: implementation for Mesh

See header (Mesh.hh) for documentation

Last Revision ($Revision: 1.8 $) 
by $Author: clwyatt $ 
on $Date: 2002/05/16 19:11:22 $
*********************************************************/

#include <fstream>
#include <cstdio>
#include <list>
#include <iostream>


#include "Mesh.hh"

using namespace IMTK;
using namespace std;

Mesh::MeshVertexSet::MeshVertexSet() : RBTree<MeshVertexElement>()
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

	shared = false;
}

Mesh::MeshVertexSet::MeshVertexSet(bool shareflg) : RBTree<MeshVertexElement>()
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

	shared = shareflg;
}

Mesh::MeshVertexSet::MeshVertexSet(const MeshVertexSet & MVS)
{

	// not implemented
	assert(0);
}

Mesh::MeshVertexSet::~MeshVertexSet()
{

	MeshVertexElement e;
	std::list<Vertex *>::iterator sli;

	if(shared){
		while( !RBTree<MeshVertexElement>::isEmpty() ){
			e = RBTree<MeshVertexElement>::getRoot();
			
			// delete each vertex from pointer list
			sli = e.verts.begin();
			while(sli != e.verts.end()){
				delete *sli;
				sli++;
			}
			RBTree<MeshVertexElement>::deleteNode(e.key);
		}
	}
	else{
		std::list<Vertex *>::iterator sli = linearList.begin();
		while(sli != linearList.end()){
			delete *sli;
			sli++;
		}
	}
}

Vertex * Mesh::MeshVertexSet::insert(Vertex v)
{

	double key;
	RBTree<MeshVertexElement>::Node * n;
	MeshVertexElement *e;
	Vertex * newv;
	int x, y, z, found;
	bool inlist;

	switch(shared){

	case true:

		x = (int)(v.getPX() + 0.5);
		y = (int)(v.getPY() + 0.5);
		z = (int)(v.getPZ() + 0.5);
		
		key = bounds[0]*bounds[1]*z + bounds[1]*x + y;

		n = RBTree<MeshVertexElement>::binTreeSearch(key, &found);
		
		// if found search the node vertex sublist for a match
		if(found){
			inlist = false;
			e = n->getData();
			std::list<Vertex *>::iterator sli = e->verts.begin();
			while(sli != e->verts.end()){
				if(**sli == v){
					inlist = true;
					break;
				}
				sli++;
			}
			
			if(!inlist){
				newv = new Vertex(v);
				e->verts.push_front(newv);
				linearList.push_front(newv);
				numverts++;
				return newv;
			}
			else{
				return *sli;
			}
		}
		else{ // if not found add the correct node
			e = new MeshVertexElement;
			newv = new Vertex(v);
			e->key = key;
			e->verts.push_front(newv);
			linearList.push_front(newv);
			
			RBTree<MeshVertexElement>::insertNode(*e, key);
			delete e;
			
			numverts++;
			
			return newv;
		}

	case false:

		newv = new Vertex(v);
		linearList.push_front(newv);
		numverts++;
		return newv;
	}
	
	return NULL;
}

Vertex * Mesh::MeshVertexSet::get(Vertex &v, bool &found)
{
	// this only works for shared vertex sets at the moment
	if( ! shared ){
		found = false;
		return NULL;
	}

	double key;
	RBTree<MeshVertexElement>::Node * n;
	MeshVertexElement *e;
	int x, y, z, foundVoxel;
	bool inlist;

	x = (int)(v.getPX() + 0.5);
	y = (int)(v.getPY() + 0.5);
	z = (int)(v.getPZ() + 0.5);
	
	key = bounds[0]*bounds[1]*z + bounds[1]*x + y;

	n = RBTree<MeshVertexElement>::binTreeSearch(key, &foundVoxel);
		
	if(!foundVoxel){
		found = false;
		return NULL;
	}

	// if voxel found search the node vertex sublist for a match
	inlist = false;
	e = n->getData();
	std::list<Vertex *>::iterator sli = e->verts.begin();
	while(sli != e->verts.end()){
		if(**sli == v){
			inlist = true;
			break;
		}
		sli++;
	}
	
	if(!inlist){
		found = false;
		return NULL;
	}

	// if we get here, everything is ok
	found = true;
	return *sli;

}

Mesh::MeshVertexSet::iterator Mesh::MeshVertexSet::begin()
{

	return linearList.begin();
}

Mesh::MeshVertexSet::iterator Mesh::MeshVertexSet::end()
{

	return linearList.end();
}

bool Mesh::MeshVertexSet::isShared()
{
	return shared;
}

Mesh::Mesh()
{

	verticies = new MeshVertexSet();
	finalized = false;
}

Mesh::Mesh(bool shareflg)
{

	verticies = new MeshVertexSet(shareflg);
	finalized = false;
}

Mesh::~Mesh()
{

	// delete the verticies
	delete verticies;
}

void Mesh::clear(bool shareflg)
{

	// delete the verticies
	delete verticies;
	verticies = new MeshVertexSet(shareflg);

	// clear the triangle list
	triangles.clear();
}

bool Mesh::lineIntersection(Vertex &theVertex, Vertex &theIntersection, Vertex &closestMeshVertex)
{

	// find the end points of the point/normal line constrained to lie
	// within the bounding box
	// EASIER to just parameterize the line and choose an
	// upper bound on the parameter (the length of the bounding
	// box diagonal in this case)
	double t; //line parameter
	double p1x, p1y, p1z, p2x, p2y, p2z;
	t = sqrt(float((verticies->bounds[0])*(verticies->bounds[0]) + 
		 (verticies->bounds[1])*(verticies->bounds[1]) + 
		 (verticies->bounds[2])*(verticies->bounds[2])));
	p1x = theVertex.getPX() - t*theVertex.getNX();
	p2x = theVertex.getPX() + t*theVertex.getNX();
	p1y = theVertex.getPY() - t*theVertex.getNY();
	p2y = theVertex.getPY() + t*theVertex.getNY();
	p1z = theVertex.getPZ() - t*theVertex.getNZ();
	p2z = theVertex.getPZ() + t*theVertex.getNZ();

	cerr << "Point  = " << theVertex.getPX() << ", " << theVertex.getPY() << ", " << theVertex.getPZ() << "\n";
	cerr << "Normal = " << theVertex.getNX() << ", " << theVertex.getNY() << ", " << theVertex.getNZ() << endl;

	// compute the plucker coordinates for the line
	// Each Plucker coordinate is the determinant of a 2x2 minor of the matrix: 
	// p1x p1y p1z 1
	// p2x p2y p2z 1
	//
	double pluckerP1P2[6];
	pluckerP1P2[0] = p1x*p2y - p2x*p1y;
	pluckerP1P2[1] = p1x*p2z - p2x*p1z;
	pluckerP1P2[2] = p1x - p2x;
	pluckerP1P2[3] = p1y*p2z - p2y*p1z;;
	pluckerP1P2[4] = p1z - p2z;
	pluckerP1P2[5] = p2y - p1y;

	// for each triangle, compute the 'side operator' of the line
	// and each of the 3 edges. Using sign test determine the intersection
	// type if any. store intersection and triangle if closest
	std::list<Triangle>::iterator sli;
	sli = triangles.begin();
	Vertex * tempv;
	// temp holders for triangles vertices
	double t1x, t2x, t3x, t1y, t2y, t3y, t1z, t2z, t3z; 
	double pluckerEdge[6], sideOp1, sideOp2, sideOp3;
	bool intersectionExists = false;
	unsigned int numTrueIntersections = 0;
	unsigned int numEdgeIntersections = 0;
	unsigned int numVertexIntersections = 0;
	bool firstIntersection = true;
	double distance, lastDistance = 0;
	while(sli != triangles.end()){
		// vertex 1
		tempv = (*sli).getV1();
		t1x = tempv->getPX();
		t1y = tempv->getPY();
		t1z = tempv->getPZ();
		// vertex 2
		tempv = (*sli).getV2();
		t2x = tempv->getPX();
		t2y = tempv->getPY();
		t2z = tempv->getPZ();
		// vertex 3
		tempv = (*sli).getV3();
		t3x = tempv->getPX();
		t3y = tempv->getPY();
		t3z = tempv->getPZ();
		
		// plucker coordinates for edge t1-t2
		pluckerEdge[0] = t1x*t2y - t2x*t1y;
		pluckerEdge[1] = t1x*t2z - t2x*t1z;
		pluckerEdge[2] = t1x - t2x;
		pluckerEdge[3] = t1y*t2z - t2y*t1z;;
		pluckerEdge[4] = t1z - t2z;
		pluckerEdge[5] = t2y - t1y;

		// compute side operator for line and edge t1-t2
		sideOp1 = pluckerP1P2[0]*pluckerEdge[4] + 
			pluckerP1P2[1]*pluckerEdge[5] + 
			pluckerP1P2[2]*pluckerEdge[3] + 
			pluckerP1P2[3]*pluckerEdge[2] + 
			pluckerP1P2[4]*pluckerEdge[0] + 
			pluckerP1P2[5]*pluckerEdge[1];

		// plucker coordinates for edge t2-t3
		pluckerEdge[0] = t2x*t3y - t3x*t2y;
		pluckerEdge[1] = t2x*t3z - t3x*t2z;
		pluckerEdge[2] = t2x - t3x;
		pluckerEdge[3] = t2y*t3z - t3y*t2z;;
		pluckerEdge[4] = t2z - t3z;
		pluckerEdge[5] = t3y - t2y;

		// compute side operator for line and edge t2-t3
		sideOp2 = pluckerP1P2[0]*pluckerEdge[4] + 
			pluckerP1P2[1]*pluckerEdge[5] + 
			pluckerP1P2[2]*pluckerEdge[3] + 
			pluckerP1P2[3]*pluckerEdge[2] + 
			pluckerP1P2[4]*pluckerEdge[0] + 
			pluckerP1P2[5]*pluckerEdge[1];

		// plucker coordinates for edge t3-t1
		pluckerEdge[0] = t3x*t1y - t1x*t3y;
		pluckerEdge[1] = t3x*t1z - t1x*t3z;
		pluckerEdge[2] = t3x - t1x;
		pluckerEdge[3] = t3y*t1z - t1y*t3z;;
		pluckerEdge[4] = t3z - t1z;
		pluckerEdge[5] = t1y - t3y;

		// compute side operator for line and edge t2-t3
		sideOp3 = pluckerP1P2[0]*pluckerEdge[4] + 
			pluckerP1P2[1]*pluckerEdge[5] + 
			pluckerP1P2[2]*pluckerEdge[3] + 
			pluckerP1P2[3]*pluckerEdge[2] + 
			pluckerP1P2[4]*pluckerEdge[0] + 
			pluckerP1P2[5]*pluckerEdge[1];

		// comparison of side operators to get intersection
		// the edge and vertex intersections are highly unlikely
		// and may be removed for performance/approximation reasons
		// they are included now for completeness sake.

		// assume there is no intersection
		intersectionExists = false;

		if(sideOp1==0 && sideOp2==0 && sideOp3==0){  // line and triangle are coplanar, ignore
		}
		// line passes through the triangle
		else if(( sideOp1>0 && sideOp2>0 && sideOp3>0)||(sideOp1<0 && sideOp2<0 && sideOp3<0)){
			cerr << "Side operators = " << sideOp1 << " " << sideOp2 << " " << sideOp3 << endl;
			intersectionExists = true;
			numTrueIntersections += 1;
		}
		// line passes through the edge
		else if(sideOp1==0 && sideOp2*sideOp3>0){ 
			cerr << "Side operators = " << sideOp1 << " " << sideOp2 << " " << sideOp3 << endl;
			intersectionExists = true;
			numEdgeIntersections += 1;
		}
		// line passes through the edge
		else if(sideOp2==0 && sideOp1*sideOp3>0){ 
			cerr << "Side operators = " << sideOp1 << " " << sideOp2 << " " << sideOp3 << endl;
			intersectionExists = true;
			numEdgeIntersections += 1;
		}
		// line passes through the edge
		else if(sideOp3==0 && sideOp1*sideOp2>0){ 
			cerr << "Side operators = " << sideOp1 << " " << sideOp2 << " " << sideOp3 << endl;
			intersectionExists = true;
			numEdgeIntersections += 1;
		}
		// line passes through the vertex
		else if(sideOp1==0 && sideOp2==0){ 
			cerr << "Side operators = " << sideOp1 << " " << sideOp2 << " " << sideOp3 << endl;
			intersectionExists = true;
			numVertexIntersections += 1;
		}
		// line passes through the vertex
		else if(sideOp1==0 && sideOp3==0){ 
			cerr << "Side operators = " << sideOp1 << " " << sideOp2 << " " << sideOp3 << endl;
			intersectionExists = true;
			numVertexIntersections += 1;
		}
		// line passes through the vertex
		else if(sideOp2==0 && sideOp3==0){ 
			cerr << "Side operators = " << sideOp1 << " " << sideOp2 << " " << sideOp3 << endl;
			intersectionExists = true;
			numVertexIntersections += 1;
		}
		
		// if intersection, compute distance, storing if first or smaller
		if(intersectionExists){
			double x0, y0, z0; // original vertex location
			double dx, dy, dz; // original vertex normal
			double t; // line parameter
			double x1, y1, z1; // point on the triangle plane
			double nx, ny, nz; // normal for the plane
			double exact[3]; //exact intersection

			// original point and normal
			x0 = theVertex.getPX();
			y0 = theVertex.getPY();
			z0 = theVertex.getPZ();
			dx = theVertex.getNX();
			dy = theVertex.getNY();
			dz = theVertex.getNZ();

			// choose second vertex to define the plane
			tempv = (*sli).getV2();
			x1 = tempv->getPX();
			y1 = tempv->getPY();
			z1 = tempv->getPZ();

			// compute cross product (v2-v3 X v2-v1) to get the normal
			double u1, u2, u3, v1, v2, v3;
			tempv = (*sli).getV3();
			u1 = tempv->getPX() - x1;
			u2 = tempv->getPY() - y1;
			u3 = tempv->getPZ() - z1;
			tempv = (*sli).getV1();
			v1 = tempv->getPX() - x1;
			v2 = tempv->getPY() - y1;
			v3 = tempv->getPZ() - z1;
			nx = u2*v3 - u3*v2;
			ny = u3*v1 - u1*v3;
			nz = u1*v2 - u2*v1;

			// compute parameter at the intersection
			// well defined as long as the denominator
			// ( the dot product) is non-zero, as it should
			// be since, to get to this state, the line and the
			// plane containing this triangle cannot be coplanar.
			t = (nx*(x1-x0) + ny*(y1-y0) + nz*(z1-z0))/(nx*dx + ny*dy + nz*dz);
			
			// evaluate the intersection as the exact intersection
			exact[0] = x0 + dx*t;
			exact[1] = y0 + dy*t;
			exact[2] = z0 + dz*t;
			distance = sqrt(dx*dx*t*t + dy*dy*t*t + dz*dz*t*t);

			//cerr << "Exact interserction = " << exact[0] << ", " << exact[1] << ", " << exact[2] << endl;

			// if first intersection set, else see if smaller
			if(firstIntersection){
				theIntersection.setPoint(exact[0], exact[1], exact[2]);
				closestMeshVertex.setPoint(x1, y1, z1);
				lastDistance = distance;
				firstIntersection = false;
			}
			else{
				if(distance < lastDistance){
					theIntersection.setPoint(exact[0], exact[1], exact[2]);
					closestMeshVertex.setPoint(x1, y1, z1);
					lastDistance = distance;
				}
			}
		}
		
		// next triangle
		sli++;
	}
	
	cerr << numTrueIntersections << " true intersections\n";
	cerr << numEdgeIntersections << " edge intersections\n";
	cerr << numVertexIntersections << " vertex intersections\n" << endl;

	return intersectionExists;
}

void Mesh::setBounds(int xdim, int ydim, int zdim)
{

	verticies->bounds[0] = xdim;
	verticies->bounds[1] = ydim;
	verticies->bounds[2] = zdim;
}

void Mesh::setVoxelSize(float dx, float dy, float dz)
{

	verticies->voxelSize[0] = dx;
	verticies->voxelSize[1] = dy;
	verticies->voxelSize[2] = dz;	
}

void Mesh::setOrigin(float xo, float yo, float zo)
{

	verticies->origin[0] = xo;
	verticies->origin[1] = yo;
	verticies->origin[2] = zo;
}

void Mesh::addTriangle(Triangle tri)
{

	Vertex *v1, *v2, *v3;
	Triangle newt;

	v1 = verticies->insert(*(tri.getV1()));
	v2 = verticies->insert(*(tri.getV2()));
	v3 = verticies->insert(*(tri.getV3()));

	// add this triangle index into the vertex neighborhood
	if(verticies->isShared()){
		int numt = triangles.size();
		v1->addNeighbor(numt);
		v2->addNeighbor(numt);
		v3->addNeighbor(numt);
	}

	// insert triangle into the list
	// depending on finalize
	newt.set(v1, v2, v3);
	if(!finalized){
		triangles.push_front(newt);
	}
	else{
		triangles.push_front(newt);
		finalTlist.push_back(newt);
	}
		
}

void Mesh::finalize()
{
	// cinvert vertex list to vector
	// convert triangle list to vector
	finalTlist.resize(triangles.size());

	std::list<Triangle>::iterator trit = triangles.begin();
	size_t idx = 0;
	while( trit != triangles.end() ){
		finalTlist[idx] = *trit;
		idx++;
		trit++;
	}

	finalized = true;
}

int Mesh::getNverticies()
{

	return verticies->numverts;
}

int Mesh::getNtriangles()
{

	return triangles.size();
}

bool Mesh::getSubMesh(Vertex & center, unsigned int depth, Mesh & subMesh)
{
	assert(depth > 0);
	assert(finalized);

	center.printInfo();

	// make sure vertex is in the mesh
	Vertex * centerp;
	bool found;
	centerp = verticies->get(center, found);
	if(!found) return false;

	//centerp->printInfo();

	// add nearest neighboring triangles
	std::vector<unsigned int> triNeighborIndices; // indicies from END!!! of the triangle array
	unsigned int numT = finalTlist.size();
	centerp->getNeighbors(triNeighborIndices);
	unsigned int i;
	Vertex * tempV1, * tempV2, * tempV3;
	Triangle tempT;
	std::list<Vertex*> vertexFIFO; // first in first out queue for BFS
	for(i = 0; i < triNeighborIndices.size(); i++){
		tempV1 = finalTlist[ numT - triNeighborIndices[i] - 1].getV1();
		if(tempV1 != centerp){ //push onto searchable stack
			vertexFIFO.push_front(tempV1);
		} 
		//tempV1->printInfo();
		// same for V2 and V3
		tempV2 = finalTlist[ numT - triNeighborIndices[i] - 1].getV2();
		if(tempV2 != centerp){ //push onto searchable stack
			vertexFIFO.push_front(tempV2);
		} 
		//tempV2->printInfo();
		tempV3 = finalTlist[ numT - triNeighborIndices[i] - 1].getV3();
		if(tempV3 != centerp){ //push onto searchable stack
			vertexFIFO.push_front(tempV3);
		} 
		//tempV3->printInfo();
		// add triangle to submesh
		tempT.set(tempV1, tempV2, tempV3);
		subMesh.addTriangle(tempT);
	}

	unsigned int currentDepth = 1;

	// continue breadth-first-search until depth reached
	while(currentDepth < depth){
		centerp = *(vertexFIFO.rbegin());
		vertexFIFO.pop_back();

		//centerp->printInfo();

		centerp->getNeighbors(triNeighborIndices);

		// add neighbors to FIFO
		for(i = 0; i < triNeighborIndices.size(); i++){
			tempV1 = finalTlist[ numT - triNeighborIndices[i] - 1].getV1();
			if(tempV1 != centerp){ //push onto searchable stack
				vertexFIFO.push_front(tempV1);
			} 
			// same for V2 and V3
			tempV2 = finalTlist[ numT - triNeighborIndices[i] - 1].getV2();
			if(tempV2 != centerp){ //push onto searchable stack
				vertexFIFO.push_front(tempV2);
			} 
			tempV3 = finalTlist[ numT - triNeighborIndices[i] - 1].getV3();
			if(tempV3 != centerp){ //push onto searchable stack
				vertexFIFO.push_front(tempV3);
			} 
			
			// add triangle to submesh
			tempT.set(tempV1, tempV2, tempV3);
			subMesh.addTriangle(tempT);
		}

		currentDepth += 1;
	}

	return true;

}

void Mesh::getVertices(std::vector<Vertex> &theVertArray)
{

	theVertArray.reserve(verticies->numverts);

	Mesh::vertexIterator mvi = vertexBegin();
	int i = 0;
	while( mvi !=  vertexEnd() ){
		theVertArray[i] = **mvi;
// 		if( (theVertArray[i].getNX() == 0) &&
// 		    (theVertArray[i].getNY() == 0) &&
// 		    (theVertArray[i].getNZ() == 0) ){
// 			(**mvi).printInfo();
// 			cerr << "degenerate vertex inside get: " << i << endl;
// 		}
		mvi++;
		i++;
	}
}

float * Mesh::getPoints(int * nP)
{

	float * points;
	int i, l;
	std::list<Triangle>::iterator sli;
	Vertex * v;

	l = triangles.size();
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

		sli = triangles.begin();
		i = 0;
		while(sli != triangles.end()){
			v = (*sli).getV1();
			points[i] = xo + dx*(v->getPX());
			points[i + 1] = yo + dy*(v->getPY());
			points[i + 2] = zo + dz*(v->getPZ());
			
			v = (*sli).getV2();
			i+= 3;
			points[i] =  xo + dx*(v->getPX());
			points[i + 1] = yo + dy*(v->getPY());
			points[i + 2] = zo + dz*(v->getPZ());	
			
			v = (*sli).getV3();
			i+= 3;
			points[i] =  xo + dx*(v->getPX());
			points[i + 1] = yo + dy*(v->getPY());
			points[i + 2] = zo + dz*(v->getPZ());
			
			i+= 3;

			sli++;
		}
	}
	else{

		sli = triangles.begin();
		i = 0;
		while(sli != triangles.end()){
			v = (*sli).getV1();
			points[i] = v->getPX();
			points[i + 1] = v->getPY();
			points[i + 2] = v->getPZ();
			
			v = (*sli).getV2();
			i+= 3;
			points[i] = v->getPX();
			points[i + 1] = v->getPY();
			points[i + 2] = v->getPZ();	
			
			v = (*sli).getV3();
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

float * Mesh::getNormals(int * nP)
{

	float * normals;
	int i, l;
	std::list<Triangle>::iterator sli;
	Vertex * v;

	l = triangles.size();
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

		sli = triangles.begin();
		i = 0;
		while(sli != triangles.end()){
			v = (*sli).getV1();
			normals[i] = xo + dx*(v->getPX());
			normals[i + 1] = yo + dy*(v->getPY());
			normals[i + 2] = zo + dz*(v->getPZ());
			
			v = (*sli).getV2();
			i+= 3;
			normals[i] =  xo + dx*(v->getPX());
			normals[i + 1] = yo + dy*(v->getPY());
			normals[i + 2] = zo + dz*(v->getPZ());	
			
			v = (*sli).getV3();
			i+= 3;
			normals[i] =  xo + dx*(v->getPX());
			normals[i + 1] = yo + dy*(v->getPY());
			normals[i + 2] = zo + dz*(v->getPZ());
			
			i+= 3;

			sli++;
		}
	}
	else{

		sli = triangles.begin();
		i = 0;
		while(sli != triangles.end()){
			v = (*sli).getV1();
			normals[i] = v->getPX();
			normals[i + 1] = v->getPY();
			normals[i + 2] = v->getPZ();
			
			v = (*sli).getV2();
			i+= 3;
			normals[i] = v->getPX();
			normals[i + 1] = v->getPY();
			normals[i + 2] = v->getPZ();	
			
			v = (*sli).getV3();
			i+= 3;
			normals[i] = v->getPX();
			normals[i + 1] = v->getPY();
			normals[i + 2] = v->getPZ();
			
			i+= 3;

			sli++;
		}
	}
	
	return normals;
}

Mesh::vertexIterator Mesh::vertexBegin()
{

	return verticies->begin();
}

Mesh::vertexIterator Mesh::vertexEnd()
{

	return verticies->end();
}

Mesh::triangleIterator Mesh::triangleBegin()
{

	return triangles.begin();
}

Mesh::triangleIterator Mesh::triangleEnd()
{

	return triangles.end();
}

bool Mesh::write(ofstream & theOutStream)
{

	if( theOutStream.bad() ){ //bad stream
		return false;
	}
	
	// temporary holders
	unsigned long tempul;
	float tempf;
	
	// counters
	int i;
	
	printInfo();

	// bounds
	tempul = verticies->bounds[0];
	theOutStream.write((char *) &tempul, sizeof tempul);
	if( theOutStream.bad() ) return false;
	tempul = verticies->bounds[1];
	theOutStream.write((char *) &tempul, sizeof tempul);
	if( theOutStream.bad() ) return false;
	tempul = verticies->bounds[2];
	theOutStream.write((char *) &tempul, sizeof tempul);
	if( theOutStream.bad() ) return false;
	
	// voxel size
	tempf = verticies->voxelSize[0];
	theOutStream.write((char *) &tempf, sizeof tempf);
	if( theOutStream.bad() ) return false;
	tempf = verticies->voxelSize[1];
	theOutStream.write((char *) &tempf, sizeof tempf);
	if( theOutStream.bad() ) return false;
	tempf = verticies->voxelSize[2];
	theOutStream.write((char *) &tempf, sizeof tempf);
	if( theOutStream.bad() ) return false;
	
	// origin
	tempf = verticies->origin[0];
	theOutStream.write((char *) &tempf, sizeof tempf);
	if( theOutStream.bad() ) return false;
	tempf = verticies->origin[1];
	theOutStream.write((char *) &tempf, sizeof tempf);
	if( theOutStream.bad() ) return false;
	tempf = verticies->origin[2];
	theOutStream.write((char *) &tempf, sizeof tempf);
	if( theOutStream.bad() ) return false;
	
	// number of vertices
	tempul = verticies->numverts;
	theOutStream.write((char *) &tempul, sizeof tempul);
	if( theOutStream.bad() ) return false;
	
	// vertices: position (3 floats) normal (3 floats)
	Mesh::vertexIterator mvi = vertexBegin();
	i = 0;
	while( mvi !=  vertexEnd() ){
		// vertex position
		tempf = (*mvi)->getPX();
		theOutStream.write((char *) &tempf, sizeof tempf);
		if( theOutStream.bad() ) return false;
		
		tempf = (*mvi)->getPY();
		theOutStream.write((char *) &tempf, sizeof tempf);
		if( theOutStream.bad() ) return false;
		
		tempf = (*mvi)->getPZ();
		theOutStream.write((char *) &tempf, sizeof tempf);
		if( theOutStream.bad() ) return false;
		
		// normal direction
		tempf = (*mvi)->getNX();
		theOutStream.write((char *) &tempf, sizeof tempf);
		if( theOutStream.bad() ) return false;
		
		tempf = (*mvi)->getNY();
		theOutStream.write((char *) &tempf, sizeof tempf);
		if( theOutStream.bad() ) return false;
		
		tempf = (*mvi)->getNZ();
		theOutStream.write((char *) &tempf, sizeof tempf);
		if( theOutStream.bad() ) return false;
		
		// label vertex 
		(*mvi)->label(i);
		
		i++;
		mvi++;
	}
	
	// number of triangles
	tempul = triangles.size();
	theOutStream.write((char *) &tempul, sizeof tempul);
	if( theOutStream.bad() ) return false;
	
	Mesh::triangleIterator mti = triangleBegin();
	while( mti !=  triangleEnd() ){
		// first vertex
		tempul = (*mti).getV1()->label();
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;
		
		// second vertex
		tempul = (*mti).getV2()->label();
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;
		
		// third vertex
		tempul = (*mti).getV3()->label();
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;
		
		mti++;
	}
	
	return true;
}

bool Mesh::read(ifstream & theInStream)
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
	Vertex v1, v2, v3;
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

	printInfo();

	return true;
}

void Mesh::ooglwrite(const char * name)
{
        ofstream  outfile;
	char entry[100];
	int i;
	Vertex *v1, *v2, *v3;
	    
	float xo, yo, zo, dx, dy, dz;
		
	xo = verticies->origin[0];
	yo = verticies->origin[1];
	zo = verticies->origin[2];

	dx = verticies->voxelSize[0];
	dy = verticies->voxelSize[1];
	dz = verticies->voxelSize[2];
		

	outfile.open( name );

	outfile << "appearance { shading flat }\n";
	outfile << "NOFF\n";

	sprintf(entry, "%i %i 0\n\n", getNverticies(), getNtriangles());
	outfile << entry;

	i = 0;
	Mesh::vertexIterator mvi = vertexBegin();

	while( mvi !=  vertexEnd() ){
		sprintf(entry, "%f %f %f %f %f %f\n", 
			xo + dx*((*mvi)->getPX()), 
			yo + dy*((*mvi)->getPY()), 
			zo + dz*((*mvi)->getPZ()), 
			(*mvi)->getNX(), (*mvi)->getNY(), (*mvi)->getNZ());
		outfile << entry;
		// label vertex 
		(*mvi)->label(i);
		i++;
		mvi++;
	}

	outfile << "\n";

	Mesh::triangleIterator mti = triangleBegin();
	while( mti !=  triangleEnd() ){
		v1 = (*mti).getV1();
		v2 = (*mti).getV2();
		v3 = (*mti).getV3();
		sprintf(entry, "3\t%i %i %i\n", v1->label(), v2->label(), v3->label());
		outfile << entry;
		mti++;
	}

	outfile.close();

};

void Mesh::vtkwrite(const char * name)
{
        ofstream  outfile;
	char entry[100];
	int i;
	Vertex *v1, *v2, *v3;

	outfile.open( name );

	outfile << "# vtk DataFile Version 2.0\n";
	outfile << "Export from matlab\n";
	outfile << "ASCII\n";
	outfile << "DATASET POLYDATA\n";
	outfile << "POINTS " << getNverticies() << " float\n";

	Mesh::vertexIterator mvi = vertexBegin();
	i = 0;
	while( mvi !=  vertexEnd() ){
		sprintf(entry, "%f %f %f\n", (*mvi)->getPX(), (*mvi)->getPY(), (*mvi)->getPZ());
		outfile << entry;
		// label vertex 
		(*mvi)->label(i);
		i++;
		mvi++;
	}

	outfile << "POLYGONS " << getNtriangles() << " " << 4*getNtriangles() << "\n";

	Mesh::triangleIterator mti = triangleBegin();
	while( mti !=  triangleEnd() ){
		v1 = (*mti).getV1();
		v2 = (*mti).getV2();
		v3 = (*mti).getV3();
		sprintf(entry, "3 %i %i %i\n", v1->label(), v2->label(), v3->label());
		outfile << entry;
		mti++;
	}

	outfile.close();

};

// FIX THIS TO USE std::vector INSTEAD OF ARRAYS
void Mesh::ooglread(const char * name)
{
        ifstream  infile;
	char entry[100];
	int i, nv, nt, temp, idx1, idx2, idx3;
	float *vx, *vy, *vz, *nx, *ny, *nz;
	Vertex v1, v2, v3;
	Triangle tri;

	infile.open( name );
	if(!infile){
		cerr << "Error in IMTK::ooglread(...): cannot open file" << endl;
		exit(-1);
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
	
	//read in vertices
	for(i=0; i < nv; i++){
		infile >> vx[i] >> vy[i] >> vz[i] >> nx[i] >> ny[i] >> nz[i];	
	}

	clog << nv << " vertices loaded.\n" << endl;

	//read in triangles and add to mesh
	for(i=0; i < nt; i++){
		infile >> temp >> idx1 >> idx2 >> idx3;
		assert(temp == 3);
		v1.setPoint(vx[idx1], vy[idx1], vz[idx1]);
		v2.setPoint(vx[idx2], vy[idx2], vz[idx2]);
		v3.setPoint(vx[idx3], vy[idx3], vz[idx3]);
		tri.set(&v1, &v2, &v3);
		addTriangle(tri);
		fprintf(stderr, "triangle %i/%i\r", i, nt);		
	}
	
	infile.close();
	
	delete [] vx;
	delete [] vy;
	delete [] vz;
	delete [] nx;
	delete [] ny;
	delete [] nz;
};

void Mesh::printInfo()
{

	cerr << "Mesh Info :\n";
	cerr << "          : Bounds     " << verticies->bounds[0] << " " << verticies->bounds[1] << " " << verticies->bounds[2] << "\n";
	cerr << "          : Voxel Size " << verticies->voxelSize[0] << " " << verticies->voxelSize[1] << " " << verticies->voxelSize[2] << "\n";
	cerr << "          : Origin     " << verticies->origin[0] << " " << verticies->origin[1] << " " << verticies->origin[2] << "\n";
	cerr << endl;

}
