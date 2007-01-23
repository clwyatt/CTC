/*
 * File:	mvLesion.cc
 *
 * Project:	MedVis Library
 *
 * Author:	David K. Ahn
 *		Virtual Endoscopy Center
 *		Wake Forest University School of Medicine
 *
 * Description:	Lesion groups in Computer Assisted Polyp Detection (CAPD)
 *
 * $Id: mvLesion.cc,v 1.4 2003/10/02 19:44:22 hongli Exp $
 *
 */
#include "mvLesion.hh"
#include "curvature.hh"
#include "nrmatrix.hh"
#include "nrfree_matrix.hh"
#include "nrfree_vector.hh"
#include "hotelling.hh"


void mvLesion::write_oogl(float red, float green, float blue)
{
char filename[80];
ofstream file;
sprintf(filename, "lesion%d.off",id);
colon->plumen->write_OFF_fromVertexList( vertexList, filename);
///attach surface fitting and trans formation matrix
file.open(filename, ofstream::out | ofstream::app); 
file << "# "<<a << " "<<b<<" "<<c<< " "<<Ay<<" "<<Az<<endl;
file << "# "<<TranX[0] << " "<< TranX[4]<<" "<<TranX[8]<<" "<<TranX[12]<< endl;
file << "# "<<TranX[1] << " "<< TranX[5]<<" "<<TranX[9]<<" "<<TranX[13]<< endl;
file << "# "<<TranX[2] << " "<< TranX[6]<<" "<<TranX[10]<<" "<<TranX[14]<< endl;
file << "# "<<TranX[3] << " "<< TranX[7]<<" "<<TranX[11]<<" "<<TranX[15]<< endl;
file.close();

sprintf(filename, "lesion%d.vect",id);
colon->plumen->write_VECT_fromVertexList( vertexList, vCenter, normal, filename);
///attach surface fitting and trans formation matrix
file.open(filename, ofstream::out | ofstream::app); 
file << "# "<<a << " "<<b<<" "<<c<< " "<<Ay<<" "<<Az<<endl;
file << "# "<<TranX[0] << " "<< TranX[4]<<" "<<TranX[8]<<" "<<TranX[12]<< endl;
file << "# "<<TranX[1] << " "<< TranX[5]<<" "<<TranX[9]<<" "<<TranX[13]<< endl;
file << "# "<<TranX[2] << " "<< TranX[6]<<" "<<TranX[10]<<" "<<TranX[14]<< endl;
file << "# "<<TranX[3] << " "<< TranX[7]<<" "<<TranX[11]<<" "<<TranX[15]<< endl;
file.close();
}

void mvLesion::setNormal()
{
std::slist<Vertex_with_Features *>::iterator nbiter =vertexList.begin(); 

  /* Calculate group normal.
   */
  normal = mvVec3f(0.0f, 0.0f, 0.0f);
 
  while(nbiter != vertexList.end()) 
 {
    normal += (*nbiter)->getNormDir();
    nbiter++;
  }
  normal.normalize();
  return;
}

void mvLesion::setBLength()
{
std::slist<Vertex_with_Features *>::iterator nbiter =vertexList.begin(); 
std::slist<void *> ptTriangles ;
std::slist<void *>::iterator ptiter ;
bool vflag1, vflag2;
Triangle * tri;

if(colon->plumen->CheckVerticesflag(false) > 0) 
	cout << "Problem ! "<< colon->plumen->CheckVerticesflag(false) << endl;

 //Mark all vertices in the list first 
  while(nbiter != vertexList.end()) 
 {
    (*nbiter)->setflag(true);
    nbiter++;
  }
  
nbiter =vertexList.begin();   
  while(nbiter != vertexList.end()) 
 {
	ptTriangles = (*nbiter)->getNeighborTriangles();
	ptiter = ptTriangles.begin();	
	while(ptiter != ptTriangles.end())
	{
		tri = (Triangle *)*ptiter;
		vflag1 = tri->getV2()->getflag();
		vflag2 = tri->getV3()->getflag();
		
		if(!vflag1 && vflag2)
			blength += (*nbiter)->distance( *(tri->getV3()));
		if(vflag1 && !vflag2)
			blength += (*nbiter)->distance(*(tri->getV2()));
		ptiter++;	
	}	
    nbiter++;
  }

blength = blength/2;
cout << " Blength 3= "<<blength <<endl;
 //unMark all vertices in the list first 
nbiter =vertexList.begin();   
  while(nbiter != vertexList.end()) 
 {
    (*nbiter)->setflag(false);
    nbiter++;
  }
return;
}

  
void mvLesion::setCenter()
{
  std::slist<Vertex_with_Features *>::iterator nbiter = vertexList.begin(); 

  mvVec3f v;
  double maxProject = -10000.0;
  double project; 
  float xsum, ysum, zsum;
  xsum = ysum = zsum = 0;
  while(nbiter != vertexList.end()) 
 {
    	 
    v.x = (*nbiter)->getPX();
    v.y = (*nbiter)->getPY();
    v.z = (*nbiter)->getPZ();
    xsum += v.x;
    ysum += v.y;
    zsum += v.z;
    
    project = v.innerProduct(normal);
    if(project > maxProject) 
    	{
	maxProject = project;
	vCenter = *nbiter;
	}
    nbiter++;
  }  
  
  center.x = xsum/vertexList.size();
  center.y = ysum/vertexList.size();
  center.z = zsum/vertexList.size();
} 

void mvLesion::setVolumeSize()
{
float maxx, maxy, maxz;
float minx, miny, minz;
float x,y,z;
maxx = maxy=maxz = -1000;
minx = miny=minz = 1000;

  std::slist<Vertex_with_Features *>::iterator nbiter = vertexList.begin(); 

 while(nbiter != vertexList.end()) 
 {
    	 
    x = (*nbiter)->getPX();
    if( x > maxx) maxx = x;
    if( x < minx) minx = x;
    	
    y = (*nbiter)->getPY();
    if( y > maxy) maxy = y;
    if( y < miny) miny = y;

    z = (*nbiter)->getPZ();
    if( z > maxz) maxz = z;
    if( z < minz) minz = z;
    nbiter++;
  }
  volumeSize = abs((maxx-minx)*(maxy-miny)*(maxz-minz));  
} 


void mvLesion::setRadius()
{
float g, m;
float maxPrinciple, minPrinciple;
::curvature(*vCenter, vertexList, normal, g,m, maxPrinciple, minPrinciple,
a,b,c, Ay, Az, TranX);
maxRadius = 1.0/minPrinciple;
minRadius = 1.0/maxPrinciple;
 return;
}
 
 
 
 /* 
 * Author: Yaorong Ge
 * Date:   Nov. 20, 1997
 * Purpose: This routine checks each group that is generated for
 *          potential lesions to see if it forms a very elongated
 *          patch. I use a simple hotelling transform to set up 
 *          the local coordinate system. I find the local axis
 *          that is most aligned with the surface normal. Then, I 
 *          compare the eigenvalues that correspond to the other
 *          two axes. If the ratio is larger than a threshold then
 *          I declare that this group is a narrow strip which is
 *          not likely to be a polyp. This approach works well if
 *          the surface patch is a relative flat one. If the patch
 *          circles around into a ring type, then this algorihtm
 *          becomes insensitive at some point.
 */


int  mvLesion::elongation() {

  int i, n;
  mvVec3f normal;

  mvVec3f *vList;
  std::slist<Vertex_with_Features *>::iterator nbiter = vertexList.begin(); 

  /* Calculate group normal.
   */
  normal = mvVec3f(0.0f, 0.0f, 0.0f);
  n = vertexList.size();
  i = 0;
  vList = new mvVec3f[n];
 
  while(nbiter != vertexList.end()) 
 {
    normal += (*nbiter)->getNormDir();
    vList[i].x = (*nbiter)->getPX();
    vList[i].y = (*nbiter)->getPY();
    vList[i].z = (*nbiter)->getPZ();
    nbiter++;
    i++;
  }
  normal.normalize();

  /* call hotelliing algorithm to get the eigenvectors and eigenvalues
   */

  float **eigenvectors, *eigenvalues;

  eigenvalues = nrvector<float>(1,3);
  eigenvectors = matrix<float>(1,3,1,3);

  hotelling(vList, n, eigenvalues, eigenvectors);

  delete [] vList;

  /* find the eigenvector that is most aligned with the group normal
   * using inner product (I guess I could use vector operation for this).
   */

  float ip1, ip2, ip3;
  int idx1, idx2;

  ip1 = fabs(normal.x * eigenvectors[1][1] +
             normal.y * eigenvectors[2][1] +
             normal.z * eigenvectors[3][1]);

  ip2 = fabs(normal.x * eigenvectors[1][2] +
             normal.y * eigenvectors[2][2] +
             normal.z * eigenvectors[3][2]);

  ip3 = fabs(normal.x * eigenvectors[1][3] +
             normal.y * eigenvectors[2][3] +
             normal.z * eigenvectors[3][3]);

  if (ip1 > ip2) {
    if (ip1 > ip3) {

      /* the largest is 1
       */

      idx1 = 2;
      idx2 = 3;

    } else {

      /* the largest is 3
       */

      idx1 = 1;
      idx2 = 2;

    }
  } else {
    if (ip2 < ip3) {

      /* the largest is 3
       */

      idx1 = 1;
      idx2 = 2;

    } else {

      /* the largest is 2
       */

      idx1 = 1;
      idx2 = 3;

    }
  }
 
//  cout << "Elong: all eigenvalues: " << eigenvalues[1] << " " <<
//            eigenvalues[2] << " " << eigenvalues[3]<<endl;

  /* compute the ratio of the eigenvalues associated with the 
   * other two axes.  
   */

  for (i=1; i<=3; i++) {
    eigenvalues[i] = fabs(eigenvalues[i]);
  }

  if (eigenvalues[idx1] < eigenvalues[idx2]) {
    if (eigenvalues[idx2] == 0) {
//      MV_DEBUG3("Elong: the larger eigenvalue is 0, do nothing.");

      free_matrix(eigenvectors, 1,3,1,3);
      free_vector(eigenvalues, 1,3);

      return 0; 
    }

    ratio = eigenvalues[idx1] / eigenvalues[idx2];

  } else {
    if (eigenvalues[idx1] == 0) {
//      MV_DEBUG3("Elong: the larger eigenvalue is 0, do nothing.");

      free_matrix(eigenvectors, 1,3,1,3);
      free_vector(eigenvalues, 1,3);

      return 0; 
    }

    ratio = eigenvalues[idx2] / eigenvalues[idx1];
  }

//  MV_DEBUG2("Elong: non-normal eigenvalues: " << eigenvalues[idx1] << ", " <<
//            eigenvalues[idx2] << "  Ratio: " << ratio);

  free_matrix(eigenvectors, 1,3,1,3);
  free_vector(eigenvalues, 1,3);
 
//  cout <<ratio<<endl;
  if (ratio < min_elongation) return 1;
  else return 0;

}

 
