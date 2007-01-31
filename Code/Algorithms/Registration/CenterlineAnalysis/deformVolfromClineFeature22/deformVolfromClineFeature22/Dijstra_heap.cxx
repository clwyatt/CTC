//#include "mex.h"
#include "tnt/tnt_array3d.h"
#include <string.h>
//#include <iostream.h>
#include <queue> // Defines both queue and priority_queue
using namespace std;

#include "MyHeader.h"

/* distancehook2.cpp  */
extern void projectionZ(TNT::Array3D<int> &M, double *slice, int sizeX, int sizeY, int sizeZ);
extern void projectionY(TNT::Array3D<int> &M, double *slice, int sizeX, int sizeY, int sizeZ);
extern void projectionX(TNT::Array3D<int> &M, double *slice, int sizeX, int sizeY, int sizeZ);

// Global Heap
priority_queue<VOXEL, vector<VOXEL>, ElementLess > pq;
queue<COORD<int> > qCLine;

unsigned long int markCnt ;

inline void arrayFill(TNT::Array3D<VOXEL> &M, double *p, int dimx, int dimy, int dimz)
{
	//mexPrintf ("dimx=%d, dimy=%d, dimz=%d\n",dimx,dimy,dimz);
	double temp=0.0;
	for(int i=0; i < dimx; i++){
		for(int j=0; j < dimy; j++){
			for(int k=0; k < dimz; k++){ 	
				M[i][j][k].dfb = (int) *(p + k*dimx*dimy + j*dimx + i); 
				M[i][j][k].mark = 0;
				M[i][j][k].dfs = 0;
				M[i][j][k].current.x =0;   M[i][j][k].previous.x =0;
				M[i][j][k].current.y =0;   M[i][j][k].previous.y =0;
				M[i][j][k].current.z =0;   M[i][j][k].previous.z =0;
				
				//temp =  *(p + k*dimx*dimy + j*dimx + i);
				//if(temp >=100)
				//clog << "p="<<temp <<" M="<< M[i][j][k].dfb<<endl;
				
			}
		}
	}
}

void fill_VOXEL_from_intArray(TNT::Array3D<VOXEL> &M, TNT::Array3D<int> bVoxel, int dimx, int dimy, int dimz)
{
	markCnt =0;
	//mexPrintf ("dimx=%d, dimy=%d, dimz=%d\n",dimx,dimy,dimz);
	double temp=0.0;
	for(int i=0; i < dimx; i++){
		for(int j=0; j < dimy; j++){
			for(int k=0; k < dimz; k++){ 	
				M[i][j][k].dfb = bVoxel[i][j][k];   if(bVoxel[i][j][k] !=0) markCnt+=1;
				M[i][j][k].mark = 0;
				M[i][j][k].dfs = 0;
				M[i][j][k].current.x =0;   M[i][j][k].previous.x =0;
				M[i][j][k].current.y =0;   M[i][j][k].previous.y =0;
				M[i][j][k].current.z =0;   M[i][j][k].previous.z =0;
				
				//temp =  *(p + k*dimx*dimy + j*dimx + i);
				//if(temp >=100)
				//clog << "p="<<temp <<" M="<< M[i][j][k].dfb<<endl;
				
			}
		}
	}
	
	cout <<"&&& markCnt ="<<markCnt<<" &&&"<<endl;
}

int temp_cnt =0;
int gnLongestDfs=0, gnLongestDfs_x=0, gnLongestDfs_y=0, gnLongestDfs_z=0;

// Modified Dijstra algorithm search for the maximum cost instead of the minimum 
// cost of the original Dijstra algorithm    by wook
void node_distance(TNT::Array3D<VOXEL> &v, int nodex, int nodey, int nodez, PathCost<int> BEST, int dimx, int dimy, int dimz)
{
	int x,y,z;
	int relative_dist345=0, num0=0;
	int heap_size;
	
	//	mexPrintf("## %d current node (%d, %d, %d)\n",temp_cnt, nodex, nodey, nodez); temp_cnt	++;
	for (z = -1; z < 2; z++) {
		for (y = -1; y < 2; y++) {
			for (x = -1; x < 2; x++) {
				
				heap_size = pq.size();
				/* skip center voxel */
				if(((nodex+x)<0)||((nodey+y)<0)||((nodez+z)<0)) continue;
				if(((nodex+x)>=dimx)||((nodey+y)>=dimy)||((nodez+z)>=dimz)) continue;
				if(v[nodex+x][nodey+y][nodez+z].mark == MARK_ON)      continue;
				if(v[nodex+x][nodey+y][nodez+z].dfb == 0)      continue;
				
				
				if (!((x == 0) && (y == 0) && (z == 0))) {
					
					v[nodex+x][nodey+y][nodez+z].mark = MARK_ON;   
					
					if(x==0) num0 += 1;
					if(y==0) num0 += 1;
					if(z==0) num0 += 1;
					
					if(num0==0) relative_dist345=5;
					else if(num0==1) relative_dist345=4;
					else if(num0==2) relative_dist345=3;
					else {
						relative_dist345=0;
						//mexPrintf ("**** relative distance error ! ****  x=%d, y=%d, z=%d num0=%d\n",x,y,z,num0);
					}
					
					v[nodex+x][nodey+y][nodez+z].dfs = v[nodex][nodey][nodez].dfs + relative_dist345;
					
					v[nodex+x][nodey+y][nodez+z].current.x = nodex+x;
					v[nodex+x][nodey+y][nodez+z].current.y = nodey+y;
					v[nodex+x][nodey+y][nodez+z].current.z = nodez+z;
					
					v[nodex+x][nodey+y][nodez+z].previous.x = nodex;
					v[nodex+x][nodey+y][nodez+z].previous.y = nodey;
					v[nodex+x][nodey+y][nodez+z].previous.z = nodez;
					
					pq.push(v[nodex+x][nodey+y][nodez+z]);   // push into the heap
					if(gnLongestDfs < v[nodex+x][nodey+y][nodez+z].dfs) {
						
						gnLongestDfs = v[nodex+x][nodey+y][nodez+z].dfs;
						gnLongestDfs_x = nodex+x;
						gnLongestDfs_y = nodey+y;
						gnLongestDfs_z = nodez+z;        
					}
					
				}  // if
				
				num0=0;
			}  // x
		}  // y
	}  // z
	
}


void select_node(TNT::Array3D<VOXEL> &v, COORD<int> &start, int dimx, int dimy, int dimz)
{
	int nodex, nodey, nodez;
	
	//clog << "in the select_node() " << endl;
	
	PathCost<int> BEST;
	VOXEL result;
	
	nodex=start.x; nodey=start.y; nodez=start.z;  
	
	v[nodex][nodey][nodez].mark = MARK_ON;    
	v[nodex][nodey][nodez].dfs = 0;   
	
	//mexPrintf ("value at starting point : v(%d, %d, %d)=%d \n",nodex,nodey,nodez,v[nodex][nodey][nodez]);
	node_distance(v, nodex, nodey, nodez, BEST, dimx, dimy, dimz);
	
	while (!pq.empty()) {
		
		//cout <<" &&&&&&&&&&& Is the pq.empty() ?  &&&&&&"<<endl;
		result = pq.top(); ////mexPrintf ("%d The priority_queue size is now %d\n",result.dfb, pq.size());
		pq.pop(); 
		
		nodex=result.current.x; nodey=result.current.y; nodez=result.current.z;
		v[nodex][nodey][nodez].mark = MARK_ON;   
		node_distance(v, nodex, nodey, nodez, BEST, dimx, dimy, dimz);
		
	}
	//cout <<" &&& Length="<<gnLongestDfs <<"at ("<<gnLongestDfs_x<<","<< gnLongestDfs_y<<","<< gnLongestDfs_z<<endl;
	//mexPrintf ("Length = %d  at the longest point (%d, %d, %d)\n", gnLongestDfs, gnLongestDfs_x, gnLongestDfs_y, gnLongestDfs_z);   
	
}
/*
void buildCenterLine(TNT::Array3D<VOXEL> &v, COORD start)
{
	int xx, yy, zz, x1,y1,z1;
	xx = gnLongestDfs_x; yy = gnLongestDfs_y; zz = gnLongestDfs_z;
	
	while(!((xx==start.x)&&(yy==start.y)&&(zz==start.z))) {
	
		mexPrintf("(%d, %d, %d)\n",xx,yy,zz);
		x1 = v[xx][yy][zz].previous.x;
		y1 = v[xx][yy][zz].previous.y;
		z1 = v[xx][yy][zz].previous.z;
		
		xx=x1; yy=y1; zz=z1;
	}
}
*/

void buildCenterLine(TNT::Array3D<VOXEL> &v, COORD<int> start, int num)
{
	int tempx, tempy, tempz;
	COORD<int> t;
	
	//clog << "begin in buildCenterLine()" << endl;
	t.x = gnLongestDfs_x; t.y = gnLongestDfs_y; t.z = gnLongestDfs_z;
	
	qCLine.push(t);    // clog << "after qCLine.push(t)" << endl;
	
	while(!((t.x==start.x)&&(t.y==start.y)&&(t.z==start.z))) {
		
		//if(num>0)	cout<<"centerline location:"<<t.x<<", "<<t.y<<", "<<t.z<<endl;
		tempx = v[t.x][t.y][t.z].previous.x;
		tempy = v[t.x][t.y][t.z].previous.y;
		tempz = v[t.x][t.y][t.z].previous.z;
		
		t.x = tempx;    t.y = tempy;    t.z = tempz;   
		qCLine.push(t); 
	}
	//clog << "the size of queue =" << qCLine.size() << endl;
	//mexPrintf (" the size of queue = %d\n",qCLine.size());
	
}

#if 0
void expressTheResult(double *out, unsigned int size, TNT::Array3D<int> bVoxel, double *sliceX, double *sliceY, double *sliceZ, int sizeX, int sizeY, int sizeZ)
{
	COORD r;
	int i=0;
	
	projectionZ(bVoxel, sliceZ, sizeX, sizeY, sizeZ);
	projectionY(bVoxel, sliceY, sizeX, sizeY, sizeZ);
	projectionX(bVoxel, sliceX, sizeX, sizeY, sizeZ);
	
	//mexPrintf ("size of centerline queue =%d\n",size);
	
	while(!qCLine.empty()){
		r = qCLine.front();     
		//	mexPrintf("i=%d: %d, %d, %d\n",i, r.x, r.y, r.z);
		
		*(out + i) = r.x;
		*(out + size + i) = r.y;
		*(out + size*2 + i) = r.z;
		i++;
		
		/* add centerline on the projections */
		*(sliceZ + r.x*sizeX + r.y) = 3.5 ;
		*(sliceY + r.z*sizeX + r.x) = 3.5;
		*(sliceX + r.z*sizeZ + r.y) = 3.5;
		
		qCLine.pop();
	}
}

void FlipMoveClineAndExpressTheResult(double *out, unsigned int size, TNT::Array3D<int> bVoxel, double *sliceX, double *sliceY, double *sliceZ, int sizeX, int sizeY, int sizeZ, int Mx, int My, int Mz)
{
	COORD<int> r;
	int i=0;
	int cx, cy, cz, dx, dy;
	int centerX, centerY, centerZ;
	
	centerX = sizeX>>1;   centerY = sizeY>>1;  centerZ = sizeZ>>1;
	clog << "centerX="<<centerX<<" , centerY="<<centerY<<" , centerZ="<<centerZ<<endl;
	
	projectionZ(bVoxel, sliceZ, sizeX, sizeY, sizeZ);
	projectionY(bVoxel, sliceY, sizeX, sizeY, sizeZ);
	projectionX(bVoxel, sliceX, sizeX, sizeY, sizeZ);
	
	//mexPrintf ("size of centerline queue =%d\n",size);
	
	while(!qCLine.empty()){
		r = qCLine.front();     
		//	mexPrintf("i=%d: %d, %d, %d\n",i, r.x, r.y, r.z);
		
		// move the centerline in order to align starting points of the two
		// centerline.
		//cx = r.x + Mx;
		//cy = r.y + My;
		cz = r.z + Mz;
		
		// flip right to left
		if(r.x >= centerX){
			dx = r.x - centerX;
			cx = centerX - dx + Mx;
		}
		else{ 
			dx = centerX - r.x;
			cx = centerX + dx + Mx;
		}
		
		if(r.y >= centerY){
			dy = r.y - centerY;
			cy = centerY - dy + My;
		}
		else{ 
			dy = centerY - r.y;
			cy = centerY + dy + My;
		}
		
		*(out + i) = cx;
		*(out + size + i) = cy;
		*(out + size*2 + i) = cz;
		i++;
		
		/* add centerline on the projections */
		*(sliceZ + r.x*sizeX + cy) = 3.5 ;
		*(sliceY + r.z*sizeX + cx) = 3.5;
		*(sliceX + r.z*sizeZ + cy) = 3.5;
		
		qCLine.pop();
	}
}

#endif

unsigned int Dijkstra_main2(TNT::Array3D<int> bVoxel, int dimx, int dimy, int dimz, int num)
{
	COORD<int> start;
	unsigned int size=0;
	//clog << "Dijkstra_main()" << endl;
	
	TNT::Array3D<VOXEL> vin(dimx, dimy, dimz);      
	
	
	//clog << "after setting VOXEL vin " << endl;
	fill_VOXEL_from_intArray(vin, bVoxel, dimx, dimy, dimz); 
	//	arrayFill(vin, in_ptr, dimx, dimy, dimz);   
	
	//clog << "after arrayFill()" << endl;
	
	
	//start.x=121; start.y=110; start.z=35; // 443074_2146_104.rvf
	start.x=76; start.y=112; start.z=90; // 443074_2146_102.rvf
	//start.x=127; start.y=151; start.z=17; //anus hole of  443074_2146_102.rvf
	//start.x=126; start.y=126; start.z=20; //anus hole of  443074_2146_104tube.rvf
	
	//clog << "*** starting point of c-line at ("<< start.x <<", "<< start.y<<", "<<start.z<<")" <<endl;
	
	//clog << "before selec_node()" << endl;
	select_node(vin, start, dimx, dimy, dimz);
	
	//clog << "before buildCenterLine()" << endl;
	buildCenterLine(vin, start, num);
	
	//clog << "before qCLine.size()" << endl;
	size = qCLine.size();
	
	return size;
	
	
}

void storeCenterline(TNT::Array3D<int> &Cline, int sizeX, int sizeY, int sizeZ, int CentValue, int dilate, ClineVoxel &Cvoxel)
{
	COORD<int> r;
	int ddx, ddy, ddz, dxx, dyy, dzz;
	
	while(!qCLine.empty()){
		r = qCLine.front();
		//	mexPrintf("i=%d: %d, %d, %d\n",i, r.x, r.y, r.z);
		
		Cline[r.x][r.y][r.z] = CentValue;
		
		////////////   added by wook  ////////////////////////
		Cvoxel.SetValue(r);
		//////////////////////////////////////////////////////////////	
		
		if(dilate>0){
			
			for(ddz=-dilate; ddz<dilate+1; ddz++){
				for(ddy=-dilate; ddy<dilate+1; ddy++){
					for(ddx=-dilate; ddx<dilate+1; ddx++){
						
						dxx = ddx + r.x;
						dyy = ddy + r.y;
						dzz = ddz + r.z;
						
						if((dxx<0)||(dyy<0)||(dzz<0)) continue;
						if((dxx>=sizeX)||(dyy>=sizeY)||(dzz>=sizeZ)) continue;
						
						Cline[dxx][dyy][dzz] = CentValue;
						
					} // ddx
				} // ddy
			} // ddz
			
		} // if(dilate>0)
		
		
		
		qCLine.pop();
	}
	
}

void storeCenterline1(TNT::Array3D<int> &Cline, int sizeX, int sizeY, int sizeZ, int CentValue, int dilate, ClineVoxel &Cvoxel)
{
	COORD<int> r;
	int ddx, ddy, ddz, dxx, dyy, dzz;
	int i=0;
	FILE *fp;
	if((fp=fopen("c1.txt","wt"))==NULL){
		
		cout << "cline file open error!"<<endl;
	}
	
	while(!qCLine.empty()){
		r = qCLine.front();
			fprintf(fp,"i=%d: %d, %d, %d\n",i, r.x, r.y, r.z);
		
		Cline[r.x][r.y][r.z] = CentValue;
		
		////////////   added by wook  ////////////////////////
		Cvoxel.SetValue(r);
		//////////////////////////////////////////////////////////////	
		
		if(dilate>0){
			
			for(ddz=-dilate; ddz<dilate+1; ddz++){
				for(ddy=-dilate; ddy<dilate+1; ddy++){
					for(ddx=-dilate; ddx<dilate+1; ddx++){
						
						dxx = ddx + r.x;
						dyy = ddy + r.y;
						dzz = ddz + r.z;
						
						if((dxx<0)||(dyy<0)||(dzz<0)) continue;
						if((dxx>=sizeX)||(dyy>=sizeY)||(dzz>=sizeZ)) continue;
						
						Cline[dxx][dyy][dzz] = CentValue;
						
					} // ddx
				} // ddy
			} // ddz
			
		} // if(dilate>0)
		
		
		
		qCLine.pop();
		i++;
	}
	fclose(fp);
}

void storeCenterline2(TNT::Array3D<int> &Cline, int sizeX, int sizeY, int sizeZ, int CentValue, int dilate, ClineVoxel &Cvoxel)
{
	COORD<int> r;
	int ddx, ddy, ddz, dxx, dyy, dzz;
	int i=0;
	FILE *fp;
	if((fp=fopen("c2.txt","wt"))==NULL){
		
		cout << "cline file open error!"<<endl;
	}
	while(!qCLine.empty()){
		r = qCLine.front();
			fprintf(fp,"i=%d: %d, %d, %d\n",i, r.x, r.y, r.z);
		
		Cline[r.x][r.y][r.z] = CentValue;
		
		////////////   added by wook  ////////////////////////
		Cvoxel.SetValue(r);
		//////////////////////////////////////////////////////////////	
		
		if(dilate>0){
			
			for(ddz=-dilate; ddz<dilate+1; ddz++){
				for(ddy=-dilate; ddy<dilate+1; ddy++){
					for(ddx=-dilate; ddx<dilate+1; ddx++){
						
						dxx = ddx + r.x;
						dyy = ddy + r.y;
						dzz = ddz + r.z;
						
						if((dxx<0)||(dyy<0)||(dzz<0)) continue;
						if((dxx>=sizeX)||(dyy>=sizeY)||(dzz>=sizeZ)) continue;
						
						Cline[dxx][dyy][dzz] = CentValue;
						
					} // ddx
				} // ddy
			} // ddz
			
		} // if(dilate>0)
		
		
		
		qCLine.pop();
		i++;
	}
	fclose(fp);
}

unsigned int Dijkstra_main3(TNT::Array3D<int> &bVoxel, int startX, int startY, int startZ, int dimx, int dimy, int dimz, int num)
{
	COORD<int> start;
	unsigned int size=0;
	//clog << "Dijkstra_main()" << endl;
	// Reset gnLongestDfs
	gnLongestDfs =0;
	////////////////////////////////////////////////////////////
	TNT::Array3D<VOXEL> vin(dimx, dimy, dimz);
	
	
	//clog << "after setting VOXEL vin " << endl;
	fill_VOXEL_from_intArray(vin, bVoxel, dimx, dimy, dimz);
	//	arrayFill(vin, in_ptr, dimx, dimy, dimz);
	
	//clog << "after arrayFill()" << endl;
	
	start.x=startX; start.y=startY; start.z=startZ;
	
	size = qCLine.size();
	//clog << "*** starting point of c-line at ("<< start.x <<", "<< start.y<<", "<<start.z<<")" <<endl;
	//clog <<"Before start__Cline size: "<<size<<endl;
	
	//clog << "before selec_node()" << endl;
	select_node(vin, start, dimx, dimy, dimz);
	
	//clog << "before buildCenterLine()" << endl;
	buildCenterLine(vin, start, num);
	size = qCLine.size();
	
	return size;
	
	
}



#if 0
void Dijkstra_main(double *in_ptr, int dimx, int dimy, int dimz)
{
	COORD start;
	
	clog << "Dijkstra_main()" << endl;
	
	TNT::Array3D<VOXEL> vin(dimx, dimy, dimz);      
	
	
	clog << "after setting VOXEL vin " << endl;
	arrayFill(vin, in_ptr, dimx, dimy, dimz);   
	
	clog << "after arrayFill()" << endl;
	
	
	start.x=76; start.y=112; start.z=90;
	//start.x=121; start.y=110; start.z=35;
	
	clog << "before selec_node()" << endl;
	select_node(vin, start, dimx, dimy, dimz);
	
	clog << "before buildCenterLine()" << endl;
	buildCenterLine(vin, start);
}


extern "C" {
	void mexFunction( int nlhs, mxArray *d[], 
					  int nrhs, const mxArray *prhs[] )
	
	{
		const int *dims;
		double *in;
		double *out_centerline;
		unsigned int size=0;
		
		clog << "Dijstra starts !!" << endl;
		
		if (nrhs != 1) { 
			mexErrMsgTxt("One input argument required."); 
		} else if (nlhs != 1) {
			mexErrMsgTxt("Adjust the output arguments."); 
		} 
		
		/* Get the Dimensions of the Volume */
		dims = mxGetDimensions(prhs[0]);
		
		/* Get the Volume */
		in = mxGetPr(prhs[0]);
		
		Dijkstra_main(in, dims[0], dims[1], dims[2]);  
		
		size = qCLine.size(); 
		
		d[0] = mxCreateDoubleMatrix(size, 3, mxREAL);
		out_centerline = mxGetPr(d[0]);  
		
		expressTheResult(out_centerline, size);
		clog << "complete Dijkstra....." << endl;
		return;
	}
}
#endif
