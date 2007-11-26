#include "tnt/tnt_array3d.h"
#include <string.h>
#include <queue> // Defines both queue and priority_queue
using namespace std;

#include  "YunaHeader.h"

int temp_cnt =0;
float gnLongestDfs=0.0;
int gnLongestDfs_x=0, gnLongestDfs_y=0, gnLongestDfs_z=0;
unsigned long int markCnt ;

// Global Heap
priority_queue<VOXEL, vector<VOXEL>, ElementLess > pq;
queue<COORD<int> > qCLine;


void node_distanceFloat(TNT::Array3D<VOXEL> &v, int nodex, int nodey, int nodez, PathCost<int> BEST, int dimx, int dimy, int dimz, float threshold)
{
	int x,y,z;
	float relative_dist=0.0;
	int num0=0;
	int heap_size;
	
	//	mexPrintf("## %d current node (%d, %d, %d)\n",temp_cnt, nodex, nodey, nodez); temp_cnt	++;
	for (z = -1; z < 2; z++) {
		for (y = -1; y < 2; y++) {
			for (x = -1; x < 2; x++) {
				
				heap_size = pq.size();   
				
				//cout<<" Heap size="<< heap_size <<" Voxel Value="<< v[nodex+x][nodey+y][nodez+z].dfb<<endl;
				/* skip center voxel */
				if(((nodex+x)<0)||((nodey+y)<0)||((nodez+z)<0)) continue;
				if(((nodex+x)>=dimx)||((nodey+y)>=dimy)||((nodez+z)>=dimz)) continue;
				if(v[nodex+x][nodey+y][nodez+z].mark == MARK_ON)      continue;
				if(v[nodex+x][nodey+y][nodez+z].dfb <= threshold)      continue;     // modified here by wook
				
				
				if (!((x == 0) && (y == 0) && (z == 0))) {   //cout<<" Not X==0, Y==0, Z==0!!"<<endl;	
					
					v[nodex+x][nodey+y][nodez+z].mark = MARK_ON;   
					
					if(x==0) num0 += 1;
					if(y==0) num0 += 1;
					if(z==0) num0 += 1;
					
					if(num0==0) relative_dist=1.732;    // root 3
					else if(num0==1) relative_dist=1.414;   // root 2
					else if(num0==2) relative_dist=1;
					else {
						relative_dist=0;
					}
					
					v[nodex+x][nodey+y][nodez+z].dfs = v[nodex][nodey][nodez].dfs + relative_dist;
					
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
						
					      // cout<<" gnLongestDfs updated!!"<<endl;	
					}
					
				}  // if
				
				num0=0;
			}  // x
		}  // y
	}  // z
	
}

void select_node(TNT::Array3D<VOXEL> &v, COORD<int> &start, int dimx, int dimy, int dimz, float threshold)
{
	int nodex, nodey, nodez;
	
	//clog << "in the select_node() " << endl;
	
	PathCost<int> BEST;
	VOXEL result;
	
	nodex=start.x; nodey=start.y; nodez=start.z;  
	
	v[nodex][nodey][nodez].mark = MARK_ON;    
	v[nodex][nodey][nodez].dfs = 0;   

	node_distanceFloat(v, nodex, nodey, nodez, BEST, dimx, dimy, dimz, threshold);
	
	while (!pq.empty()) {
		
		//cout <<" &&&&&&&&&&& Is the pq.empty() ?  &&&&&&"<<endl;
		result = pq.top(); 
		pq.pop(); 
		
		nodex=result.current.x; nodey=result.current.y; nodez=result.current.z;
		v[nodex][nodey][nodez].mark = MARK_ON;   
		node_distanceFloat(v, nodex, nodey, nodez, BEST, dimx, dimy, dimz, threshold);
		
	}
	cout <<" &&& Length="<<gnLongestDfs <<"at ("<<gnLongestDfs_x<<","<< gnLongestDfs_y<<","<< gnLongestDfs_z<<endl;	
}

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



void fill_VOXEL_from_floatArray(TNT::Array3D<VOXEL> &M, TNT::Array3D<float> bVoxel, int dimx, int dimy, int dimz, float threshold)
{
	markCnt =0;
	//mexPrintf ("dimx=%d, dimy=%d, dimz=%d\n",dimx,dimy,dimz);
	double temp=0.0;
	for(int i=0; i < dimx; i++){
		for(int j=0; j < dimy; j++){
			for(int k=0; k < dimz; k++){ 	
				
				M[i][j][k].dfb = bVoxel[i][j][k];   if(bVoxel[i][j][k] > threshold) markCnt+=1;  // modified here by wook
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


unsigned int DijkstraMainFloat(TNT::Array3D<float> &bVoxel, int startX, int startY, int startZ, int dimx, int dimy, int dimz, int num, float threshold)
{
	COORD<int> start;
	clog << "DijkstraMainFloat() =("<<dimx<<","<<dimy<<","<<dimz<<")" << endl;
	
	unsigned int size=0;
	gnLongestDfs =0;     // Reset gnLongestDfs
	
	TNT::Array3D<VOXEL> vin(dimx, dimy, dimz);


	//clog << "after setting VOXEL vin " << endl;
	fill_VOXEL_from_floatArray(vin, bVoxel, dimx, dimy, dimz, threshold);

	
	start.x=startX; start.y=startY; start.z=startZ;
	
	size = qCLine.size();   clog <<"size before start = "<<size<<endl;
	clog << "*** starting point of c-line at ("<< start.x <<", "<< start.y<<", "<<start.z<<")" <<endl;
	
	//clog << "before selec_node()" << endl;
	select_node(vin, start, dimx, dimy, dimz, threshold);

	//clog << "before buildCenterLine()" << endl;
	buildCenterLine(vin, start, num);
	size = qCLine.size();
	
	return size;
	
	
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
