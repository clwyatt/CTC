#include "YunaHeader.h"


void ClineVoxel::SetSize_ClineVoxel(int size) {
		
		co = new COORD<int>[size];
		cnt = 0;
		clineSize = size;
	}

void ClineVoxel::SetValue(COORD<int> r) {
		
		co[cnt].x = r.x;	co[cnt].y = r.y;	co[cnt].z = r.z;
		cnt++;
	}

COORD<int> ClineVoxel::GetVoxelCoord(int index) {
		
		return co[index];
	}

int ClineVoxel::Get_ClineSize(void){
		
		return clineSize;
	}

COORD<float>* ClineVoxel::FindSliding3Dmv(int m) {
	
	int size = Get_ClineSize();
	
	dump_size = size-2*m+1;
	dump = new COORD<float>[size];
	
	COORD<int> curr, next;
	int i,j;
	
	for (i=0; i<dump_size; i++){
		
		curr.x =0; curr.y=0; curr.z=0;
		next.x=0; next.y=0; next.z=0;
		
		for(j=i; j<(i+m); j++) {
			curr.x += co[j].x;	next.x += co[j+m].x;
			curr.y += co[j].y;	next.y += co[j+m].y;
			curr.z +=co[j].z;    next.z += co[j+m].z;
#if 0			
		if(i==(size-2*m-1)){
		
			cout<<j<<": "<< curr.x<<", "<<curr.y<<", "<<curr.z<<endl;
				
		}
	
#endif
		}
		
		dump[i].x = (float)(next.x - curr.x)/m;
		dump[i].y = (float)(next.y - curr.y)/m;
		dump[i].z = (float)(next.z - curr.z)/m;
	}
	
	for (i=dump_size; i<size; i++){
		dump[i].x =0.0;    dump[i].y =0.0;   dump[i].z =0.0;  
	}
	return dump;
}

COORD<float>* ClineVoxel::MovingAvg(int m) {
	
	int size = Get_ClineSize();
	
	dump_size = size-2*m+1;
	dump = new COORD<float>[size];
	
	COORD<int> curr;
	int i,j;
	
	for (i=0; i<dump_size; i++){
		
		curr.x =0; curr.y=0; curr.z=0;
		
		for(j=i; j<(i+m); j++) {
			curr.x += co[j].x;   curr.y += co[j].y;	curr.z +=co[j].z;    
#if 0			
		if(i==(size-2*m-1)){
		
			cout<<j<<": "<< curr.x<<", "<<curr.y<<", "<<curr.z<<endl;
				
		}
	
#endif
		}
		
		dump[i].x = (float) curr.x/m;
		dump[i].y = (float) curr.y/m;
		dump[i].z = (float) curr.z/m;
	}
	
	for (i=dump_size; i<size; i++){
		dump[i].x =0.0;    dump[i].y =0.0;   dump[i].z =0.0;  
	}
	return dump;
}
	

void MatchedPair::initMatchedPair(int window_size)
{
	win_size = window_size;
	src = new COORD<int>[win_size]; 
	trg = new COORD<int>[win_size]; 
	
}

void MatchedPair::fillMatchedPair(ClineVoxel &cS, ClineVoxel &cT, int startS, int startL )
{
	int i;
	for(i=0; i<win_size; i++){
		
		src[i] = cS.GetVoxelCoord(startS+i);
		trg[i] = cT.GetVoxelCoord(startL+i);
		
#if 0
		cout<<i<<" saved src point: ("<<src[i].x<<", "<<src[i].y<<", "<<src[i].z<<")"<<
			"     saved trg point: ("<<trg[i].x<<", "<<trg[i].y<<", "<<trg[i].z<<")"<<endl;
#endif
		
	}
}
