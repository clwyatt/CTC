// File: curvature.cc
// Abstract: determine curvature of surface points on binary volume
//
// ref. Tracing Surfaces for Surfacing Traces
//      Sander & Zucker
//
// ref. Surface Parameterization and Curvature Measurement
//      of Arbitrary 3-D Objects: Five Pratical Methods
//      Stokely and Wu, PAMI vol. 14, 1992
//
// Created: 02/24/98 by: Chris L. Wyatt
// Modified: 10/09/2001 by: Hong Li
//
//                
#include "curvature.hh"
#include <fstream>
#include <cstdio>
#ifndef NDEBUG
#define NDEBUG
#endif 
#include "constant.h"

//Before using the function, check the normal vector validity
int curvature(Vertex_with_Features & vert, std::slist<Vertex_with_Features *>
nblist, double &cond, int DB, Volume_ext<unsigned short>  & volume_ext, float
&g, float & m)
{
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;
double Ay, Az, dx, dy, dz;
double sum11, sum12, sum13, sum22, sum23, sum33;
double a, b, c;
mvVec3f ip; 
mvVec3f neighbors[MAX_NEIGHBORS];
double **ATA, **ATAInv;
int numn = 0;
mvVec3f norm =  vert.getNormDir();

    Az = gzangle(norm);
    norm = (norm.zRotate(-Az)); 		
    Ay = gyangle(norm);
    norm = (norm.yRotate(-Ay)); 		
    nbiter = nblist.begin();
    while(nbiter != nblist.end())		
		{
		ip.x = -vert.getPX()+ (*nbiter)->getPX();
		ip.y = -vert.getPY()+ (*nbiter)->getPY();
		ip.z = -vert.getPZ()+ (*nbiter)->getPZ();
		ip = (ip.zRotate(-Az));
		ip = (ip.yRotate(-Ay));
		neighbors[numn++] = ip;
		nbiter++;
		}
		
    ATA = matrix<double>(1, 3, 1, 3);
    ATAInv = matrix<double>(1, 3, 1, 3);

    sum11 = 0; sum12 = 0; sum13 = 0;
    sum22 = 0; sum23 = 0; sum33 = 0;
    for (i=0; i<numn; i++){
      dx = (double)neighbors[i].x;	
      dy = (double)neighbors[i].y;	
      sum11 += dx*dx*dx*dx;
      sum12 += dx*dx*2*dx*dy;
      sum13 += dx*dx*dy*dy;
      sum22 += 2*dx*dy*2*dx*dy;
      sum23 += 2*dx*dy*dy*dy;
      sum33 += dy*dy*dy*dy;
    }

    ATA[1][1] = sum11;
    ATA[1][2] = sum12;
    ATA[1][3] = sum13;
    ATA[2][1] = sum12;
    ATA[2][2] = sum22;
    ATA[2][3] = sum23;
    ATA[3][1] = sum13;
    ATA[3][2] = sum23;
    ATA[3][3] = sum33;
        
    invertMatrixSVD(ATA, ATAInv, 3, cond);

    sum11 = sum12 = sum13 = 0;
    for (i=0; i<numn; i++){
      dx = (double)neighbors[i].x;	
      dy = (double)neighbors[i].y;	
      dz = (double)neighbors[i].z;	
      sum11 += dx*dx*dz;	
      sum12 += 2*dx*dy*dz;	
      sum13 += dy*dy*dz;	
    }

    a = ATAInv[1][1]*sum11 + ATAInv[1][2]*sum12 + ATAInv[1][3]*sum13; 
    b = ATAInv[2][1]*sum11 + ATAInv[2][2]*sum12 + ATAInv[2][3]*sum13; 
    c = ATAInv[3][1]*sum11 + ATAInv[3][2]*sum12 + ATAInv[3][3]*sum13; 

#ifdef NDEBUG
if (DB ==1)
{
	ofstream  outfile, outfile1, outfile2,outfile3, outfile4;
	char sout[80];
	outfile1.open ("mesh1.off", ofstream::out | ofstream::app);

	cout<<numn<<endl;
		
    	for (i=0; i<numn; i++){
		sprintf(sout, "%f %f %f ",neighbors[i].x,
		neighbors[i].y,neighbors[i].z);
		outfile1<<sout<<endl;		
		}	
	
	cout << " a = "<< a <<" b = "<< b << " c = "<< c <<endl;
	float dy = 0.25; //mm 
	float dz = 0.25; //mm

	outfile2.open ("mesh2.mesh"); //transforemed mesh
	outfile2 <<"CMESH" <<endl;
	outfile2 <<"200 200" <<endl;

	outfile3.open ("mesh3.mesh"); //untransformed mesh
	outfile3 <<"CMESH" <<endl;
	outfile3 <<"200 200" <<endl;
	
	float x,y,z,v;
//	float xorig, yorig, zorig, xs,ys,zs;
//	volume_ext.getOrigin(xorig,yorig,zorig);
//	volume_ext.getVoxelSpacing(xs,ys,zs);
//	cout << "xo= "<<xorig<<" yo= "<<yorig<<" zo= "<<zorig<<
//		"dx = "<<xs<<" dy= "<<ys<<" dz = "<<zs<<endl;
//	xorig = yorig=xorig = 0; //for start in voxel
	mvVec3f temp;
	for (i = -100; i< 100; i++)
	{
		for (int j=-100;j<100;j++)
		{
			outfile2<< "0 "<< j*dy << " " <<i*dz<<" 1 0 0 1 ";
			temp = mvVec3f(0, j*dy, i*dz);
			temp = temp.yRotate(Ay);
			temp = temp.zRotate(Az);
			x = vert.getPX()+temp.x;
			y = vert.getPY()+temp.y;
			z = vert.getPZ()+temp.z;
			v = volume_ext.getValue(x,y,z);
			//cout <<"v = "<<v<<endl;
			
			if (v>=1500) v = 1;
			else v = v/1500;
			outfile3<< x << " "<<y << " " <<z<<" "<<v<<" "<<v<<" "<<v<<" 1 ";
		}
		outfile2<<endl;
		outfile3<<endl;
	}

	temp = temp.crossProduct( vert.getNormDir());
	mvVec3f parallel = (vert.getNormDir()).crossProduct(temp);
	parallel.normalize();
	
	outfile4.open ("stop.vect"); //untransformed mesh
	outfile4 <<"VECT" <<endl;
	outfile4 <<"1 2 1"<<endl;
	outfile4 <<"2"<<endl;
	outfile4 <<"1"<<endl;
	x = vert.getPX();
	y = vert.getPY();
	z = vert.getPZ();
	mvVec3f curv = mvVec3f(x,y,z);
	temp  = vert.getNormDir();
	curv = curv + temp * vert.getThickness();
	/*float tempv = volume_ext.getValue(curv.x,curv.y,curv.z);
	int turn = 0;
	for (i = 0; i < 100; i++)
	{
		curv += temp * 0.25;
		v = volume_ext.getValue(curv.x,curv.y,curv.z); 
		if ( v - 1024 > 60) 
		{
			cout << v <<" Larger than 60 at "<< i*0.25 <<" mm" <<endl;
			break;
		}
		if ( turn ==1 && v-1024 < -425) 		
		{
			cout << v <<" Lower than -425 at "<< i*0.25 <<" mm"<<endl;
			break;
		}

		if ( turn == 1 && v > tempv) 		
		{
			cout << "turning at "<< i*0.25 <<" mm" <<endl;
			break;
		}

		if ( v < tempv) turn = 1;
		tempv = v;
	}*/
	
	mvVec3f from  = curv - parallel * 2;
	mvVec3f to  = curv + parallel * 2;
	outfile4 <<from.x <<" "<<from.y<<" "<<from.z<<" "<<
		to.x<<" "<<to.y<<" "<<to.z<<endl;
	outfile4 <<"1 1 0 1"<<endl;
	 
	outfile1.close();
	outfile2.close();		
	outfile3.close();		
	outfile4.close();		
}
#endif

    free_matrix(ATA, 1, 3, 1, 3);
    free_matrix(ATAInv, 1, 3, 1, 3);
    g= 4*(a*c - b*b);
    m= a + c;

}


//Before using the function, check the normal vector validity
int curvature(Vertex_with_Features & vert, std::slist<Vertex_with_Features *> nblist, float &g, float & m)
{
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;
double Ay, Az, dx, dy, dz;
double sum11, sum12, sum13, sum22, sum23, sum33;
double a, b, c;
mvVec3f ip; 
mvVec3f neighbors[MAX_NEIGHBORS];
double **ATA, **ATAInv;
double cond;
int numn = 0;
mvVec3f norm =  vert.getNormDir();

    Az = gzangle(norm);
    norm = (norm.zRotate(-Az)); 		
    Ay = gyangle(norm);
    norm = (norm.yRotate(-Ay)); 		
    nbiter = nblist.begin();
    while(nbiter != nblist.end())		
		{
		ip.x = -vert.getPX()+ (*nbiter)->getPX();
		ip.y = -vert.getPY()+ (*nbiter)->getPY();
		ip.z = -vert.getPZ()+ (*nbiter)->getPZ();
		ip = (ip.zRotate(-Az));
		ip = (ip.yRotate(-Ay));
		neighbors[numn++] = ip;
		nbiter++;
		}
		
    ATA = matrix<double>(1, 3, 1, 3);
    ATAInv = matrix<double>(1, 3, 1, 3);

    sum11 = 0; sum12 = 0; sum13 = 0;
    sum22 = 0; sum23 = 0; sum33 = 0;
    for (i=0; i<numn; i++){
      dx = (double)neighbors[i].x;	
      dy = (double)neighbors[i].y;	
      sum11 += dx*dx*dx*dx;
      sum12 += dx*dx*2*dx*dy;
      sum13 += dx*dx*dy*dy;
      sum22 += 2*dx*dy*2*dx*dy;
      sum23 += 2*dx*dy*dy*dy;
      sum33 += dy*dy*dy*dy;
    }

    ATA[1][1] = sum11;
    ATA[1][2] = sum12;
    ATA[1][3] = sum13;
    ATA[2][1] = sum12;
    ATA[2][2] = sum22;
    ATA[2][3] = sum23;
    ATA[3][1] = sum13;
    ATA[3][2] = sum23;
    ATA[3][3] = sum33;
        
    invertMatrixSVD(ATA, ATAInv, 3, cond);

    sum11 = sum12 = sum13 = 0;
    for (i=0; i<numn; i++){
      dx = (double)neighbors[i].x;	
      dy = (double)neighbors[i].y;	
      dz = (double)neighbors[i].z;	
      sum11 += dx*dx*dz;	
      sum12 += 2*dx*dy*dz;	
      sum13 += dy*dy*dz;	
    }

    a = ATAInv[1][1]*sum11 + ATAInv[1][2]*sum12 + ATAInv[1][3]*sum13; 
    b = ATAInv[2][1]*sum11 + ATAInv[2][2]*sum12 + ATAInv[2][3]*sum13; 
    c = ATAInv[3][1]*sum11 + ATAInv[3][2]*sum12 + ATAInv[3][3]*sum13; 


    free_matrix(ATA, 1, 3, 1, 3);
    free_matrix(ATAInv, 1, 3, 1, 3);
    g= 4*(a*c - b*b);
    m= a + c;

}


//Before using the function, check the normal vector validity
//This form of curvature is calculated based on vertex list, a given center, and
// a given normal diection, and the output is gaussian, mean, and two principle
//curvatures
//

int curvature(Vertex_with_Features & vert, std::slist<Vertex_with_Features *>
nblist, mvVec3f norm, float &g, float & m, float & maxPrinciple, float
 & minPrinciple, double & a, double & b, double & c, double & Ay, double & Az, double * trans)
{
std::slist<Vertex_with_Features *>::iterator  nbiter;
int i;
double   dx, dy, dz;
double sum11, sum12, sum13, sum22, sum23, sum33;
mvVec3f ip; 
mvVec3f neighbors[MAX_NEIGHBORS];
double **ATA, **ATAInv;
double cond;
int numn = 0;
double R1[4][4];
double R2[4][4];
double T[4][4], Temp[4][4];

    Az = gzangle(norm);
    norm = (norm.zRotate(-Az)); 		
    Ay = gyangle(norm);
	
    norm = (norm.yRotate(-Ay)); 		


   if( trans !=NULL) 
   {
   /* Tansform Matrix is defined as T*X = X'; T = R(2) * R(1) * Tanslation; */
   /* R(1) rotate about y
   	[ 	cos(Ay) 	0 	sin(Ay) 	0
		0		1	0		0	
	  	-sin(Ay) 	0 	cos(Ay) 	0
		0		0	0		1]
	*/
	R1[0][0] = cos(-Ay),	R1[0][1]=0, 	R1[0][2] = sin(-Ay), 	R1[0][3] = 0;
	R1[1][0] = 0, 		R1[1][1]=1, 	R1[1][2] = 0, 		R1[1][3] = 0;
	R1[2][0] = -sin(-Ay), 	R1[2][1]=0, 	R1[2][2] = cos(-Ay), 	R1[2][3] = 0;
	R1[3][0] = 0, 		R1[3][1]=0, 	R1[3][2] = 0, 		R1[3][3] = 1;

   /* R(2) rotate about 
   	[ 	cos(Az) 	-sin(Az)	0 	0
		sin(Az)		cos(Az)		0	0	
	  	0		0 		1 	0
		0		0		0	1]
	*/
	R2[0][0] = cos(-Az),	R2[0][1]=-sin(-Az), 	R2[0][2] = 0, 	R2[0][3] = 0;
	R2[1][0] = sin(-Az), 	R2[1][1]=cos(-Az), 	R2[1][2] = 0, 	R2[1][3] = 0;
	R2[2][0] = 0, 		R2[2][1]=0, 	R2[2][2] = 1, 		R2[2][3] = 0;
	R2[3][0] = 0, 		R2[3][1]=0, 	R2[3][2] = 0, 		R2[3][3] = 1;
	

  /* Translation
   	[ 	1 	0 	0 	xs
		0	1	0	ys	
	  	0 	0 	1 	zs
		0	0	0	1] */
		
	T[0][0] = 1, 		T[0][1]=0, 	T[0][2] = 0, 		T[0][3] = -vert.getPX();
	T[1][0] = 0, 		T[1][1]=1, 	T[1][2] = 0, 		T[1][3] = -vert.getPY();
	T[2][0] = 0, 		T[2][1]=0, 	T[2][2] = 1, 		T[2][3] = -vert.getPZ();
	T[3][0] = 0, 		T[3][1]=0, 	T[3][2] = 0, 		T[3][3] = 1;

	
  /* calculate Tranform matrix */
  	for (int i =0; i < 4 ; i++)
  		{
		for (int j = 0; j < 4; j++)
		{
			Temp[i][j] = 0;
			for (int k = 0; k < 4; k++)
				Temp[i][j] += R1[i][k]*R2[k][j] ;
  		}
		}
	double * result;
	result = trans;
  	for (int i =0; i < 4 ; i++)
  		{
		for (int j = 0; j < 4; j++)
		{
			*result = 0;
			for (int k = 0; k < 4; k++)
				* result += Temp[i][k]*T[k][j] ;
			result ++;	
  		}
		}		
	}


    nbiter = nblist.begin();
    while(nbiter != nblist.end())		
		{
		ip.x = -vert.getPX()+ (*nbiter)->getPX();
		ip.y = -vert.getPY()+ (*nbiter)->getPY();
		ip.z = -vert.getPZ()+ (*nbiter)->getPZ();
		ip = (ip.zRotate(-Az));
		ip = (ip.yRotate(-Ay));
		neighbors[numn++] = ip;
		nbiter++;
		}
		
    ATA = matrix<double>(1, 3, 1, 3);
    ATAInv = matrix<double>(1, 3, 1, 3);

    sum11 = 0; sum12 = 0; sum13 = 0;
    sum22 = 0; sum23 = 0; sum33 = 0;
    for (i=0; i<numn; i++){
      dx = (double)neighbors[i].x;	
      dy = (double)neighbors[i].y;	
      sum11 += dx*dx*dx*dx;
      sum12 += dx*dx*2*dx*dy;
      sum13 += dx*dx*dy*dy;
      sum22 += 2*dx*dy*2*dx*dy;
      sum23 += 2*dx*dy*dy*dy;
      sum33 += dy*dy*dy*dy;
    }

    ATA[1][1] = sum11;
    ATA[1][2] = sum12;
    ATA[1][3] = sum13;
    ATA[2][1] = sum12;
    ATA[2][2] = sum22;
    ATA[2][3] = sum23;
    ATA[3][1] = sum13;
    ATA[3][2] = sum23;
    ATA[3][3] = sum33;
        
    invertMatrixSVD(ATA, ATAInv, 3, cond);

    sum11 = sum12 = sum13 = 0;
    for (i=0; i<numn; i++){
      dx = (double)neighbors[i].x;	
      dy = (double)neighbors[i].y;	
      dz = (double)neighbors[i].z;	
      sum11 += dx*dx*dz;	
      sum12 += 2*dx*dy*dz;	
      sum13 += dy*dy*dz;	
    }

    a = ATAInv[1][1]*sum11 + ATAInv[1][2]*sum12 + ATAInv[1][3]*sum13; 
    b = ATAInv[2][1]*sum11 + ATAInv[2][2]*sum12 + ATAInv[2][3]*sum13; 
    c = ATAInv[3][1]*sum11 + ATAInv[3][2]*sum12 + ATAInv[3][3]*sum13; 


    free_matrix(ATA, 1, 3, 1, 3);
    free_matrix(ATAInv, 1, 3, 1, 3);
    g= 4*(a*c - b*b);
    m= a + c;
    float k1 = m+sqrt(m*m-g);
    float k2 = m-sqrt(m*m-g);
    maxPrinciple = max(k1,k2);
    minPrinciple = min(k1,k2);

}


double gxangle(mvVec3f p)
{
  double Ax;

  Ax = atan2(p.z, p.y);

  return Ax;
}

double gyangle(mvVec3f p)
{
  double Ay;

  Ay = atan2(p.x, p.z);

  return Ay;
}

double gzangle(mvVec3f p)
{
  double Az;

  Az = atan2(p.y, p.x);

  return Az;
}
