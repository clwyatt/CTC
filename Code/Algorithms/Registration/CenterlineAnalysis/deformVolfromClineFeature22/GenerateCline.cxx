#include "MyHeader.h"
#include <queue> // Defines both queue and priority_queue
using namespace std;
/* Dijstra_heap.cpp  */
extern unsigned int Dijkstra_main3(TNT::Array3D<int> &bVoxel, int startX, int startY, int startZ, int dimx, int dimy, int dimz, int num);
extern void storeCenterline(TNT::Array3D<int> &Cline, int sizeX, int sizeY, int sizeZ, int CentValue, int dilate, ClineVoxel &Cvoxel);
extern void storeCenterline1(TNT::Array3D<int> &Cline, int sizeX, int sizeY, int sizeZ, int CentValue, int dilate, ClineVoxel &Cvoxel);
extern void storeCenterline2(TNT::Array3D<int> &Cline, int sizeX, int sizeY, int sizeZ, int CentValue, int dilate, ClineVoxel &Cvoxel);

/** distancehook2.cpp  **/
extern void distancehook(TNT::Array3D<int> &vin, int dimx,int dimy,int dimz);

int pruningIndex2(IndexPair* mIndexPair,  IndexPair* mIndexOut, int validCnt, int window_size)
{
	//cout<<"pruningIndex: window_size="<<window_size<<endl;
	int i, j, cnt, before_Overlap;
	j=0;  cnt=0;   before_Overlap = 0;
	
	float tempCorrel = 0.0;
	int tempS_Index = 0;
	int tempL_Index = 0;
	
	for(i=0; i<validCnt-1; i++){
		
		if(mIndexPair[i].Overlap != _OVERLAP_ ){
			
			tempCorrel = mIndexPair[i].SavedCorrel;
			tempS_Index = mIndexPair[i].SavedS_Index;
			tempL_Index = mIndexPair[i].SavedL_Index;
			
			for(j=i+1; j<validCnt; j++){
				
				//cout<<"current S_index="<< mIndexPair[i].SavedS_Index<< "+window="<< (mIndexPair[i].SavedS_Index + window_size)<<" next="<<mIndexPair[j].SavedS_Index<<endl;
				//cout<<"current L_index="<< mIndexPair[i].SavedL_Index<<  "+window="<< (mIndexPair[i].SavedL_Index + window_size)<<" next="<<mIndexPair[j].SavedL_Index<<endl;
				
				if( ((mIndexPair[i].SavedS_Index + window_size)>=mIndexPair[j].SavedS_Index) ||
					( (mIndexPair[i].SavedL_Index + window_size)>=mIndexPair[j].SavedL_Index) ){
					
					if( tempCorrel >= mIndexPair[j].SavedCorrel){
						
						mIndexPair[j].Overlap =  _OVERLAP_ ;
					}
					else {
						tempCorrel = mIndexPair[j].SavedCorrel;
						tempS_Index = mIndexPair[j].SavedS_Index;
						tempL_Index = mIndexPair[j].SavedL_Index;						
					}	
					
				}  //   overlapped
				
			}  // for-j

			if(cnt == 0) {  
				mIndexOut[cnt].SavedCorrel = tempCorrel;
				mIndexOut[cnt].SavedS_Index = tempS_Index;
				mIndexOut[cnt].SavedL_Index = tempL_Index;
				cnt++;
		       }
			else if ((tempS_Index != mIndexOut[cnt-1].SavedS_Index)&&(tempL_Index != mIndexOut[cnt-1].SavedL_Index)){
				mIndexOut[cnt].SavedCorrel = tempCorrel;
				mIndexOut[cnt].SavedS_Index = tempS_Index;
				mIndexOut[cnt].SavedL_Index = tempL_Index;
				cnt++;				
			}
							
		} // if(mIndexPair[i].Overlap != _OVERLAP_)
	}  // for-i	
	

	
	for(i=0;i<cnt;i++){
		
		cout<<i<<" pruned S index="<<mIndexOut[i].SavedS_Index<<" L index="<<mIndexOut[i].SavedL_Index<<endl;
	}
	
	return cnt;
}

int pruningIndex(IndexPair* mIndexPair,  IndexPair* mIndexOut, int validCnt, int window_size)
{
	//cout<<"pruningIndex: window_size="<<window_size<<endl;
	int i, j, cnt;
	j=0;  cnt=0;
	for(i=0; i<validCnt-1; i++){
		
		cout<<"current S_index="<< mIndexPair[i].SavedS_Index<< "+window="<< (mIndexPair[i].SavedS_Index + window_size)<<" next="<<mIndexPair[i+1].SavedS_Index<<endl;
		cout<<"current L_index="<< mIndexPair[i].SavedL_Index<<  "+window="<< (mIndexPair[i].SavedL_Index + window_size)<<" next="<<mIndexPair[i+1].SavedL_Index<<endl;
		 
			if( ((mIndexPair[i].SavedS_Index + window_size)>=mIndexPair[i+1].SavedS_Index) ||
				( (mIndexPair[i].SavedL_Index + window_size)>=mIndexPair[i+1].SavedL_Index) ){
				
				if(mIndexPair[i].SavedCorrel > mIndexPair[i+1].SavedCorrel) {
				 
						if( ((mIndexPair[i].SavedS_Index + window_size)>=mIndexPair[i+2].SavedS_Index) ||
							( (mIndexPair[i].SavedL_Index + window_size)>=mIndexPair[i+2].SavedL_Index) ){
							
							        if(mIndexPair[i].SavedCorrel > mIndexPair[i+2].SavedCorrel) {
										//cout <<"  ** 0.5  **"<<endl;
										mIndexOut[j].SavedCorrel = mIndexPair[i].SavedCorrel;
										mIndexOut[j].SavedS_Index = mIndexPair[i].SavedS_Index;
										mIndexOut[j].SavedL_Index = mIndexPair[i].SavedL_Index;
										j++;	
										
									}							
						}
						else{
							//cout <<"  ** 1  **"<<endl;
							mIndexOut[j].SavedCorrel = mIndexPair[i].SavedCorrel;
							mIndexOut[j].SavedS_Index = mIndexPair[i].SavedS_Index;
							mIndexOut[j].SavedL_Index = mIndexPair[i].SavedL_Index;
							j++;
						}
				}	
				else if((i==validCnt-2) &&
						(mIndexPair[i].SavedCorrel <= mIndexPair[i+1].SavedCorrel)){
					
					//cout<<"  ** 2 **"<<endl;
					mIndexOut[j].SavedCorrel = mIndexPair[i+1].SavedCorrel;
					mIndexOut[j].SavedS_Index = mIndexPair[i+1].SavedS_Index;
					mIndexOut[j].SavedL_Index = mIndexPair[i+1].SavedL_Index;
					j++;
				}
			}   // if-   When the current range is overlaped with the next range
			else{  
				//cout<<"  ** 3 **"<<endl;
				mIndexOut[j].SavedCorrel = mIndexPair[i].SavedCorrel;
				mIndexOut[j].SavedS_Index = mIndexPair[i].SavedS_Index;
				mIndexOut[j].SavedL_Index = mIndexPair[i].SavedL_Index;	
				j++;
				
				if(i== validCnt-2){
					
					//cout<<"  ** 4 **"<<endl;
					mIndexOut[j].SavedCorrel = mIndexPair[i+1].SavedCorrel;
					mIndexOut[j].SavedS_Index = mIndexPair[i+1].SavedS_Index;
					mIndexOut[j].SavedL_Index = mIndexPair[i+1].SavedL_Index;
					j++;
				} // because the last element of the Index is not included by above formula
				
			}  // else- When the current range is NOT overlapped with the next range
		
		 
	}  // for	
	
	cnt = j;
	
	for(i=0;i<cnt;i++){
		
		cout<<i<<" pruned S index="<<mIndexOut[i].SavedS_Index<<" L index="<<mIndexOut[i].SavedL_Index<<endl;
	}
	
	return cnt;
}

int pruningIndexORG(IndexPair* mIndexPair,  IndexPair* mIndexOut, int validCnt, int window_size)
{
	cout<<"pruningIndex: window_size="<<window_size<<endl;
	int i, j, cnt;
	j=0;  cnt=0;
	for(i=0; i<validCnt-1; i++){
		
		cout<<"current S_index="<< mIndexPair[i].SavedS_Index<< "+window="<< (mIndexPair[i].SavedS_Index + window_size)<<" next="<<mIndexPair[i+1].SavedS_Index<<endl;
		cout<<"current L_index="<< mIndexPair[i].SavedL_Index<<  "+window="<< (mIndexPair[i].SavedL_Index + window_size)<<" next="<<mIndexPair[i+1].SavedL_Index<<endl;
		 
			if( ((mIndexPair[i].SavedS_Index + window_size)>=mIndexPair[i+1].SavedS_Index) ||
				( (mIndexPair[i].SavedL_Index + window_size)>=mIndexPair[i+1].SavedL_Index) ){
				
				if(mIndexPair[i].SavedCorrel > mIndexPair[i+1].SavedCorrel) {
				 
					cout <<"  ** 1  **"<<endl;
					mIndexOut[j].SavedCorrel = mIndexPair[i].SavedCorrel;
					mIndexOut[j].SavedS_Index = mIndexPair[i].SavedS_Index;
					mIndexOut[j].SavedL_Index = mIndexPair[i].SavedL_Index;
					j++;
				}	
				else if((i==validCnt-2) &&
						(mIndexPair[i].SavedCorrel <= mIndexPair[i+1].SavedCorrel)){
					
					cout<<"  ** 2 **"<<endl;
					mIndexOut[j].SavedCorrel = mIndexPair[i+1].SavedCorrel;
					mIndexOut[j].SavedS_Index = mIndexPair[i+1].SavedS_Index;
					mIndexOut[j].SavedL_Index = mIndexPair[i+1].SavedL_Index;
					j++;
				}
			}   // if-   When the current range is overlaped with the next range
			else{  
				cout<<"  ** 3 **"<<endl;
				mIndexOut[j].SavedCorrel = mIndexPair[i].SavedCorrel;
				mIndexOut[j].SavedS_Index = mIndexPair[i].SavedS_Index;
				mIndexOut[j].SavedL_Index = mIndexPair[i].SavedL_Index;	
				j++;
				
				if(i== validCnt-2){
					
					cout<<"  ** 4 **"<<endl;
					mIndexOut[j].SavedCorrel = mIndexPair[i+1].SavedCorrel;
					mIndexOut[j].SavedS_Index = mIndexPair[i+1].SavedS_Index;
					mIndexOut[j].SavedL_Index = mIndexPair[i+1].SavedL_Index;
					j++;
				} // because the last element of the Index is not included by above formula
				
			}  // else- When the current range is NOT overlapped with the next range
		
		 
	}  // for	
	
	cnt = j;
	
	for(i=0;i<cnt;i++){
		
		cout<<i<<" pruned S index="<<mIndexOut[i].SavedS_Index<<" L index="<<mIndexOut[i].SavedL_Index<<endl;
	}
	
	return cnt;
}

int generateCline(TNT::Array3D<int> &Voxel, int startX, int startY, int startZ, ReaderType::Pointer reader,  TNT::Array3D<int> &Cline, int num, ClineVoxel &Cvoxel )
{
	
	InputImageType::RegionType inputFixedRegion;
	InputImageType::RegionType::SizeType  Fixedsize;
	
	Fixedsize =  reader->GetOutput()->GetRequestedRegion().GetSize();
	
	InputImageType::RegionType::IndexType inputFixedStart;
	inputFixedStart[0]=0;   inputFixedStart[1]=0;  inputFixedStart[2]=0;
	std::cout<<"sizeX="<<Fixedsize[0]<<" sizeY="<<Fixedsize[1]<<" sizeZ="<<Fixedsize[2]<<std::endl;
	int sizeZ=0, sizeX=0, sizeY=0;
	
	sizeX= Fixedsize[0]; sizeY= Fixedsize[1]; sizeZ= Fixedsize[2];
	
	inputFixedRegion.SetSize( Fixedsize );
	inputFixedRegion.SetIndex( inputFixedStart );
	
	ConstIteratorType inputFixedIt(   reader->GetOutput(), inputFixedRegion  );
	int value1 = 0;
	int i,j,k;
	
	for(k=0; k < sizeZ; k++){
		for(j=0; j < sizeY; j++){
			for(i=0; i < sizeX; i++){
				
				value1 = inputFixedIt.Get();   //if(value1 != 0) std::cout<< "value="<<value1<<" at"<<i<<", "<<j<<", "<<k<<std::endl;
				Voxel[i][j][k] = value1;
				++inputFixedIt;
				
			}
		}
	}
	
	distancehook(Voxel, sizeX, sizeY, sizeZ);
	unsigned int size=0; 
	
	size = Dijkstra_main3(Voxel, startX, startY, startZ, sizeX, sizeY, sizeZ, num);
	int CentValue, DilateSize;
	/**     Arbitrary setting by wook     **/
	CentValue = 40;
	DilateSize = 0;
	/*************************************/            
	
	Cvoxel.SetSize_ClineVoxel(size); 
	
	
	//storeCenterline(Cline, sizeX, sizeY, sizeZ, CentValue, DilateSize, Cvoxel);
	if(num==0) storeCenterline1(Cline, sizeX, sizeY, sizeZ, CentValue, DilateSize, Cvoxel);
	if(num==1) storeCenterline2(Cline, sizeX, sizeY, sizeZ, CentValue, DilateSize, Cvoxel);
	return size;
	
}

void copyAndFillZero(COORD<float> *e, ClineVoxel &Cv, int extendedNum)
{
	int i;
	for(i=0;i<Cv.clineSize;i++){
		
		e[i].x = Cv.dump[i].x;	e[i].y = Cv.dump[i].y; 	e[i].z = Cv.dump[i].z;
	}
	
	if(Cv.clineSize < extendedNum){
		
		for(i=Cv.clineSize; i<extendedNum; i++){
			e[i].x = 0.0;	e[i].y = 0.0; 	e[i].z = 0.0;
		}
		
	}  // if(Cv.clineSize < extendedNum)
	
}


void copyToWindow(COORD<float> *e, COORD<float> *w, int window_size)
{
	int i;
	for(i=0; i<window_size; i++){
		
		w[i].x = e[i].x;		w[i].y = e[i].y;		w[i].z = e[i].z;
	}
	
}

float CorrelationCoefficient(COORD<float>*s, COORD<float>*t, int size)
{
	int i;
	float sumS=0.0, sumT=0.0, sumRemovedMean=0.0;
	float sumRemovedMeanA1 =0.0, sumRemovedMeanA2 =0.0;
	float meanS, meanT;
	
	for(i=0; i<size; i++){
		sumS += (s[i].x + s[i].y + s[i].z);
		sumT += (t[i].x + t[i].y + t[i].z);
	}
	
//	cout<< "sumS= "<<sumS<<" sumT= "<<sumT<<endl;
	
	int totalSize = size * 3;  // because (x,y,z) elements are summed up
	meanS = sumS/totalSize;  	meanT = sumT/totalSize;
	
	for(i=0; i<size; i++){
		sumRemovedMean +=( (s[i].x - meanS)*(t[i].x - meanT)+(s[i].y - meanS)*(t[i].y - meanT)
						  +(s[i].z - meanS)*(t[i].z - meanT) );
		sumRemovedMeanA1 +=( (s[i].x - meanS)*(s[i].x - meanS)+(s[i].y - meanS)*(s[i].y - meanS)
						  +(s[i].z - meanS)*(s[i].z - meanS) );
		sumRemovedMeanA2 +=( (t[i].x - meanT)*(t[i].x - meanT)+(t[i].y - meanT)*(t[i].y - meanT)
						  +(t[i].z - meanT)*(t[i].z - meanT) );
	}
	
	float cross, auto1, auto2;
	
	cross = sumRemovedMean/(totalSize -1);
	auto1 = sumRemovedMeanA1/(totalSize -1); 
	auto2 = sumRemovedMeanA2/(totalSize -1);
	
#if 0	
	cout << "meanS= "<< meanS << " meanT= "<<meanT<<endl;
	cout << "sumRemovedMean  <<= "<<sumRemovedMean  <<" sumRemovedMeanA1= "<<sumRemovedMeanA1 <<" sumRemovedMeanA2="<<sumRemovedMeanA2 <<endl;
	cout << "cross= "<<cross  <<" auto1= "<<auto1 <<" auto2="<<auto2 <<endl;
#endif
	
	float correlCoef;
	correlCoef =  cross / sqrt(auto1 * auto2);
	return correlCoef;
	
}


float CorrelationCoefficientYZ(COORD<float>*s, COORD<float>*t, int size)
{
	int i;
	float sumS=0.0, sumT=0.0, sumRemovedMean=0.0;
	float sumRemovedMeanA1 =0.0, sumRemovedMeanA2 =0.0;
	float meanS, meanT;
	
	for(i=0; i<size; i++){
		sumS += (s[i].y + s[i].z);
		sumT += (t[i].y + t[i].z);
	}
	
//	cout<< "sumS= "<<sumS<<" sumT= "<<sumT<<endl;
	
	int totalSize = size * 2;  // because (y,z) elements are summed up
	meanS = sumS/totalSize;  	meanT = sumT/totalSize;
	
	for(i=0; i<size; i++){
		sumRemovedMean +=( (s[i].y - meanS)*(t[i].y - meanT) +(s[i].z - meanS)*(t[i].z - meanT) );
		sumRemovedMeanA1 +=( (s[i].y - meanS)*(s[i].y - meanS) +(s[i].z - meanS)*(s[i].z - meanS) );
		sumRemovedMeanA2 +=( (t[i].y - meanT)*(t[i].y - meanT) +(t[i].z - meanT)*(t[i].z - meanT) );
	}
	
	float cross, auto1, auto2;
	
	cross = sumRemovedMean/(totalSize -1);
	auto1 = sumRemovedMeanA1/(totalSize -1); 
	auto2 = sumRemovedMeanA2/(totalSize -1);
	
#if 0	
	cout << "meanS= "<< meanS << " meanT= "<<meanT<<endl;
	cout << "sumRemovedMean  <<= "<<sumRemovedMean  <<" sumRemovedMeanA1= "<<sumRemovedMeanA1 <<" sumRemovedMeanA2="<<sumRemovedMeanA2 <<endl;
	cout << "cross= "<<cross  <<" auto1= "<<auto1 <<" auto2="<<auto2 <<endl;
#endif
	
	float correlCoef;
	correlCoef =  cross / sqrt(auto1 * auto2);
	return correlCoef;
	
}

float CorrelationCoefficient(float *s, float *t, int size)
{
	int i;
	float sumS=0.0, sumT=0.0, sumRemovedMean=0.0;
	float sumRemovedMeanA1 =0.0, sumRemovedMeanA2 =0.0;
	float meanS, meanT;
	
	for(i=0; i<size; i++){
		sumS += s[i];
		sumT += t[i];
	}
	
//	cout<< "sumS= "<<sumS<<" sumT= "<<sumT<<endl;
	
	int totalSize = size ; 
	meanS = sumS/totalSize;  	meanT = sumT/totalSize;
	
	for(i=0; i<size; i++){
		sumRemovedMean +=( (s[i] - meanS)*(t[i] - meanT) );
		sumRemovedMeanA1 +=(  (s[i] - meanS)*(s[i] - meanS)  );
		sumRemovedMeanA2 +=(  (t[i] - meanT)*(t[i] - meanT)  );
	}
	
	float cross, auto1, auto2;
	
	cross = sumRemovedMean/(totalSize -1);
	auto1 = sumRemovedMeanA1/(totalSize -1); 
	auto2 = sumRemovedMeanA2/(totalSize -1);
	
#if 0	
	cout << "meanS= "<< meanS << " meanT= "<<meanT<<endl;
	cout << "sumRemovedMean  <<= "<<sumRemovedMean  <<" sumRemovedMeanA1= "<<sumRemovedMeanA1 <<" sumRemovedMeanA2="<<sumRemovedMeanA2 <<endl;
	cout << "cross= "<<cross  <<" auto1= "<<auto1 <<" auto2="<<auto2 <<endl;
#endif
	
	float correlCoef;
	correlCoef =  cross / sqrt(auto1 * auto2);
	return correlCoef;
	
}

inline float minmod(float x, float y)
{
  if(x*y > 0)
    {
      float absx = fabs(x);
      float absy = fabs(y);
      if(x > 0)
	{
	  return (absx < absy) ? absx : absy;
	}
      else
	{
	  return (absx < absy) ? -absx : -absy;
	}
    }
  else
    {
      return 0;
    }

}

void copyAndFillBulk(COORD<float> *e, ClineVoxel &Cv, int extendedNum)
{
	int i;
	for(i=0;i<Cv.clineSize;i++){
		
		e[i].x = Cv.dump[i].x;	e[i].y = Cv.dump[i].y; 	e[i].z = Cv.dump[i].z;
	}
	
	if(Cv.clineSize < extendedNum){
		
		for(i=Cv.clineSize; i<extendedNum; i++){
			e[i].x = 0.0;	e[i].y = 0.0; 	e[i].z = 0.0;
		}
		
	}  // if(Cv.clineSize < extendedNum)
	
}
void copyAndMinmodToWindow2( ClineVoxel &Cv, COORD<float> *w, int window_size)
{
	int i;
	float gradxplus, gradxminus, gradyplus, gradyminus, gradzplus, gradzminus;
	float mmgradx, mmgrady, mmgradz;
	
	for(i=0; i<window_size; i++){
		
		//w[i].x = e[i].x;		w[i].y = e[i].y;		w[i].z = e[i].z;
		if(i+1 >= window_size){	gradxplus = 100; 	gradyplus = 100;  	gradzplus = 100;} 
		else{	gradxplus = Cv.dump[i+1].x - Cv.dump[i].x;	
			       gradyplus = Cv.dump[i+1].y - Cv.dump[i].y;	
				gradzplus = Cv.dump[i+1].z - Cv.dump[i].z;
			}
		
		if(i-1 <= 0){ gradxminus = 100;	gradyminus = 100;		gradzminus = 100; }
		else {  	gradxminus = Cv.dump[i].x - Cv.dump[i-1].x;
			       gradyminus = Cv.dump[i].y - Cv.dump[i-1].y;
				gradzminus = Cv.dump[i].z - Cv.dump[i-1].z;
		}
		
		mmgradx = minmod(gradxplus, gradxminus);
		mmgrady = minmod(gradyplus, gradyminus);
		mmgradz = minmod(gradzplus, gradzminus);
		
		w[i].x = mmgradx;
		w[i].y = mmgrady;
		w[i].z = mmgradz;
		
	}
	
}
void copyAndMinmodToWindow(COORD<float> *e, COORD<float> *w, int window_size)
{
	int i;
	float gradxplus, gradxminus, gradyplus, gradyminus, gradzplus, gradzminus;
	float mmgradx, mmgrady, mmgradz;
	
	for(i=0; i<window_size; i++){
		
		//w[i].x = e[i].x;		w[i].y = e[i].y;		w[i].z = e[i].z;
		if(i+1 >= window_size){	gradxplus = 100; 	gradyplus = 100;  	gradzplus = 100;} 
		else{	gradxplus = e[i+1].x - e[i].x;	
			       gradyplus = e[i+1].y - e[i].y;	
				gradzplus = e[i+1].z - e[i].z;
			}
		
		if(i-1 <= 0){ gradxminus = 100;	gradyminus = 100;		gradzminus = 100; }
		else {  	gradxminus = e[i].x - e[i-1].x;
			       gradyminus = e[i].y - e[i-1].y;
				gradzminus = e[i].z - e[i-1].z;
		}
		
		mmgradx = minmod(gradxplus, gradxminus);
		mmgrady = minmod(gradyplus, gradyminus);
		mmgradz = minmod(gradzplus, gradzminus);
		
		w[i].x = mmgradx;
		w[i].y = mmgrady;
		w[i].z = mmgradz;
		
	}
	
}

#define OK_ZERO_NUM 0
#define NOTGOOD_ZERO_NUM	1
int copyAroundZeroZ(COORD<float> *v, float *wv, int window_index, int halfWindow)
{
	int i,j;
	int cnt_zero=0;
	for(i= -halfWindow, j=0 ; i<= halfWindow;  i++, j++){
		
		wv[j] = v[i+window_index].z;
		//cout<<"   wv="<<wv[j];	
		
		if(wv[j]==0) cnt_zero++;
		
	}
	//cout<<endl;
	
	if(cnt_zero>5) return NOTGOOD_ZERO_NUM;
	else return OK_ZERO_NUM ;
}

int copyZAroundZeroY2(COORD<float> *v, float *wv, int window_index, int halfWindow)
{
	int i,j;
	int cnt_zero=0;
	for(i= -halfWindow, j=0 ; i<= halfWindow;  i++, j++){
		
		wv[j] = v[i+window_index].z;
		//cout<<"   wv="<<wv[j];	
		
		if(wv[j]==0) cnt_zero++;
		
	}
	//cout<<endl;
	//cout<< "cnt_zero="<<cnt_zero<<endl;
	if(cnt_zero>8) return NOTGOOD_ZERO_NUM;
	else return OK_ZERO_NUM ;
}

void copyAroundZeroZ2(COORD<float> *v, float *wv, int window_index, int halfWindow)
{
	int i,j;
	for(i= -halfWindow, j=0 ; i<= halfWindow;  i++, j++){
		
		wv[j] = v[i+window_index].z;
		//cout<<"   wv="<<wv[j];	
				
	}
	//cout<<endl;

}

void copyYAroundZeroZ(COORD<float> *v, float *wv, int window_index, int halfWindow)
{
	int i,j;
	for(i= -halfWindow, j=0 ; i<= halfWindow;  i++, j++){
		
		wv[j] = v[i+window_index].y;
		//cout<<"   wv="<<wv[j];	
				
	}
	//cout<<endl;

}

void copyXAroundZeroZ(COORD<float> *v, float *wv, int window_index, int halfWindow)
{
	int i,j;
	for(i= -halfWindow, j=0 ; i<= halfWindow;  i++, j++){
		
		wv[j] = v[i+window_index].x;
		//cout<<"   wv="<<wv[j];	
				
	}
	//cout<<endl;

}

void copyZAroundZeroY(COORD<float> *v, float *wv, int window_index, int halfWindow)
{
	int i,j;
	for(i= -halfWindow, j=0 ; i<= halfWindow;  i++, j++){
		
		wv[j] = v[i+window_index].z;
		//cout<<"   wv="<<wv[j];	
				
	}
	//cout<<endl;

}



void copyAndExtremaToWindow(ClineVoxel &Cv, COORD<float> *w, int window_size)
{
	int i;
	float gradxplus, gradxminus, gradyplus, gradyminus, gradzplus, gradzminus;
	
	for(i=0; i<window_size; i++){
		
		//w[i].x = e[i].x;		w[i].y = e[i].y;		w[i].z = e[i].z;
		if(i+1 >= window_size){	gradxplus = 100; 	gradyplus = 100;  	gradzplus = 100;} 
		else{	gradxplus = Cv.dump[i+1].x - Cv.dump[i].x;	
			       gradyplus = Cv.dump[i+1].y - Cv.dump[i].y;	
				gradzplus = Cv.dump[i+1].z - Cv.dump[i].z;
			}
		
		w[i].x = gradxplus; 
		w[i].y = gradyplus; 
		w[i].z = gradzplus; 
		
	}
	
}

#define MATLAB_TEST
#define _MINIMA_		  

int FindFeatures5Add(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl)
{
#ifdef MATLAB_TEST
	FILE *fp_M1, *fp_M2, *fp_max1, *fp_max2;
	if((fp_M1=fopen( "matlab_testCV1.txt","w"))==NULL){
		
		cout << "matlab_testCV1.txt file open error!"<<endl;
	}	
	
	if((fp_M2=fopen( "matlab_testCV2.txt","w"))==NULL){
		
		cout << "matlab_testCV2.txt file open error!"<<endl;
	}
	
	
	if((fp_max1=fopen( "matlab_testMax1.txt","w"))==NULL){
		
		cout << "matlab_testMax1.txt file open error!"<<endl;
	}
	
	
	if((fp_max2=fopen( "matlab_testMax2.txt","w"))==NULL){
		
		cout << "matlab_testMax2.txt file open error!"<<endl;
	}
#endif
	
	Cv1.MovingAvg(m);
	cout<< "src dump size : "<<Cv1.dump_size<<endl;
	
	Cv2.MovingAvg(m);
	cout<< "trg dump size : "<<Cv2.dump_size<<endl;
	
	
	// modified from here by wook
	COORD<float> vS[Cv1.clineSize], vL[Cv2.clineSize];


	copyAndExtremaToWindow(Cv1, vS, Cv1.clineSize);
	copyAndExtremaToWindow(Cv2, vL, Cv2.clineSize);
	
#ifdef MATLAB_TEST
	int i;
	for(i=0; i< Cv1.dump_size; i++){
	
		fprintf(fp_M1,"%f %f %f\n",Cv1.dump[i].x, Cv1.dump[i].y, Cv1.dump[i].z);
	}
	
	for(i=0; i< Cv2.dump_size; i++){
	
		fprintf(fp_M2,"%f %f %f\n",Cv2.dump[i].x, Cv2.dump[i].y, Cv2.dump[i].z);
	}	
	for(i=0; i< Cv1.clineSize; i++){
	
		fprintf(fp_max1,"%f %f %f\n",vS[i].x, vS[i].y, vS[i].z);
	}
	
	for(i=0; i< Cv2.clineSize; i++){
	
		fprintf(fp_max2,"%f %f %f\n",vL[i].x, vL[i].y, vL[i].z);
	}	
#endif	

	int window_indexS, window_indexL, prev_indexS=0;
	int local_max_indexS=0, local_max_indexL=0, cont_on =0, cnt =0, max_cnt=0;
	int contY_on=0;
	float local_max = 0.0;
	float correl=0.0, max_confident=0.0;
	float DvSz_Plus, DvSz_Minus, DvLz_Plus, DvLz_Minus;
	float DvSy_Plus, DvSy_Minus, DvLy_Plus, DvLy_Minus;
	int halfWindow = window_size >>1; cout<<"half window size="<<halfWindow<<endl;
	
	float wvS[window_size+1], wvL[window_size+1];
	int OptZindex=0, OptYindex=0;
	float OptZvalue=0.0, OptZdiff=1000.0, diff=0.0;
	float OptYvalue=0.0, OptYdiff=1000.0, diffY=0.0;
	int diff_set =0;
	IndexPair tmpIndexPair;
	///// Priority Queue !!
	priority_queue<IndexPair, vector<IndexPair>, Less_Sindex > PrQ;
	
	for(window_indexS=1; window_indexS<(Cv1.clineSize - window_size-1); window_indexS++){
	
		
		//////////////////////   Compare Z elements   ////////////////////////////
		DvSz_Plus = vS[window_indexS+1].z;
		DvSz_Minus = vS[window_indexS].z;
	      if((DvSz_Plus <= 0)&&(DvSz_Minus >0)) { 		     // on maxima
		  
		  copyYAroundZeroZ(vS, wvS, window_indexS, halfWindow); // in order to compare the correlation of Y elements between the similar Z window candidates.
		 // cout << " %%%% vS       indexS    =        "<<window_indexS<<" -1:"<<vS[window_indexS-1].z<<" 0:"<<vS[window_indexS].z<<" +1:"<<vS[window_indexS+1].z <<endl;
	  
		OptZindex =0; OptZvalue=0.0;  OptZdiff=1000.0;  diff_set =0;
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				DvLz_Plus = vL[window_indexL+1].z;
				DvLz_Minus = vL[window_indexL].z;
				if((DvLz_Plus <= 0)&&(DvLz_Minus >0)) {  // on maxima
					
					diff = fabs(Cv1.dump[window_indexS].z - Cv2.dump[window_indexL].z);
					if( diff < OptZdiff) {
						diff_set =1;
						OptZdiff = diff;	OptZvalue = Cv2.dump[window_indexL].z;	OptZindex = window_indexL;
						//cout <<"Opt Z index: "<<OptZindex <<" with "<< OptZvalue <<" at Source : "<<Cv1.dump[window_indexS].z<<endl;	
						
					}  // if(fabs(Cv1.dump[window_indexS] - Cv2.dump[window_indexL]) > OptZdiff
				 }	//  f((DvLz_Plus <= 0)&&(DvLz_Minus >0))
				
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
			
		} //for(window_indexL)	
		
		if(diff_set == 1){
			copyYAroundZeroZ(vL, wvL, OptZindex, halfWindow);
			correl = CorrelationCoefficient(wvS, wvL, window_size+1);
			//cout << " vL       indexL    =    "<<OptZindex<<" -1:"<<vL[OptZindex-1].z<<" 0:"<<vL[OptZindex].z<<" +1:"<<vL[OptZindex+1].z <<endl;
			//cout <<"  correlation coef= "<< correl << " with "<<Cv2.dump[OptZindex].z<< endl;
					
			if(correl > fixed_correl){						
				mIndexPair[cnt].SavedS_Index = window_indexS;
				mIndexPair[cnt].SavedL_Index = OptZindex;
				mIndexPair[cnt].SavedCorrel = correl;
				mIndexPair[cnt].Overlap = _NON_OVERLAP_	;
				
				PrQ.push(mIndexPair[cnt]);
				cnt++; // increase cnt due to current correl		
										
				if(cnt > 20) {
					cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
					cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
					continue;
				}		
			} // if(correl > fixed_correl)					       
	       }// 		if(diff_set == 1)

	} //  if((DvSz_Plus <= 0)&&(DvSz_Minus >0))  //   maxima
		  
		  
	//////////////// added by wook   ///////////////////////////////
#ifdef _MINIMA_		  
		  if((DvSz_Plus >= 0)&&(DvSz_Minus < 0)) { 		     // on minima
		  
		  copyXAroundZeroZ(vS, wvS, window_indexS, halfWindow); // in order to compare the correlation of X elements between the similar Z window candidates.
		 // cout << " %%%% vS       indexS    =        "<<window_indexS<<" -1:"<<vS[window_indexS-1].z<<" 0:"<<vS[window_indexS].z<<" +1:"<<vS[window_indexS+1].z <<endl;
	  
		OptZindex =0; OptZvalue=0.0;  OptZdiff=1000.0;  diff_set =0;
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				DvLz_Plus = vL[window_indexL+1].z;
				DvLz_Minus = vL[window_indexL].z;
				if((DvLz_Plus >= 0)&&(DvLz_Minus < 0)) {  // on minima
					
					diff = fabs(Cv1.dump[window_indexS].z - Cv2.dump[window_indexL].z);
					if( diff < OptZdiff) {
						diff_set =1;
						OptZdiff = diff;	OptZvalue = Cv2.dump[window_indexL].z;	OptZindex = window_indexL;
						//cout <<"Opt Z index: "<<OptZindex <<" with "<< OptZvalue <<" at Source : "<<Cv1.dump[window_indexS].z<<endl;	
						
					}  // if(fabs(Cv1.dump[window_indexS] - Cv2.dump[window_indexL]) > OptZdiff
				 }	//  f((DvLz_Plus <= 0)&&(DvLz_Minus >0))
				
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
			
		} //for(window_indexL)	
		
		if(diff_set == 1){
			copyXAroundZeroZ(vL, wvL, OptZindex, halfWindow);
			correl = CorrelationCoefficient(wvS, wvL, window_size+1);
			//cout << " vL       indexL    =    "<<OptZindex<<" -1:"<<vL[OptZindex-1].z<<" 0:"<<vL[OptZindex].z<<" +1:"<<vL[OptZindex+1].z <<endl;
			//cout <<"X correlation coef= "<< correl << " with "<<Cv2.dump[OptZindex].z<< endl;
					
			if(correl > fixed_correl){						
				mIndexPair[cnt].SavedS_Index = window_indexS;
				mIndexPair[cnt].SavedL_Index = OptZindex;
				mIndexPair[cnt].SavedCorrel = correl;
				mIndexPair[cnt].Overlap = _NON_OVERLAP_	;
				
				PrQ.push(mIndexPair[cnt]);
				cnt++; // increase cnt due to current correl		
										
				if(cnt > 20) {
					cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
					cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
					continue;
				}		
			} // if(correl > fixed_correl)					       
	       }// 		if(diff_set == 1)

	} //  if((DvSz_Plus >= 0)&&(DvSz_Minus < 0))  //   minima	  
#endif		  
		  
	} // for(window_indexS)		  
	
#if 1
	local_max = 0.0;	local_max_indexS =0; 	local_max_indexL =0;  prev_indexS=0;
	for(window_indexS=1; window_indexS<(Cv1.clineSize - window_size-1); window_indexS++){
		//////////////////////   Compare Y elements   ////////////////////////////
	  DvSy_Plus = vS[window_indexS+1].y;
	  DvSy_Minus = vS[window_indexS].y;
	  
	  if((DvSy_Plus <= 0)&&(DvSy_Minus >0)) { 		     // on maxima
		  
		  if(OK_ZERO_NUM !=copyZAroundZeroY2(vL, wvL, window_indexL, halfWindow)) continue; 	  
		  //copyZAroundZeroY(vS, wvS, window_indexS, halfWindow);  // in order to compare the correlation of Y elements between the similar Z window candidates.
		  //cout << " %%%% vS       indexS    =        "<<window_indexS<<" -1:"<<vS[window_indexS-1].y<<" 0:"<<vS[window_indexS].y<<" +1:"<<vS[window_indexS+1].y <<endl;
	  
		OptYindex =0; OptYvalue=0.0;  OptYdiff=1000.0; diff_set =0;
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				DvLy_Plus = vL[window_indexL+1].y;
				DvLy_Minus = vL[window_indexL].y;
       
				if((DvLy_Plus <= 0)&&(DvLy_Minus >0)) {  // on maxima
					
					diffY = fabs(Cv1.dump[window_indexS].y - Cv2.dump[window_indexL].y);
					if( diffY < OptYdiff) {
						diff_set =1;
						OptYdiff = diffY;	OptYvalue = Cv2.dump[window_indexL].y;	OptYindex = window_indexL;
						//cout <<"Opt Y index: "<<OptYindex <<" with "<< OptYvalue <<" at Source : "<<Cv1.dump[window_indexS].y<<endl;	
						
					}  // if(fabs(Cv1.dump[window_indexS] - Cv2.dump[window_indexL]) > OptYdiff
				 }	//  f((DvLy_Plus <= 0)&&(DvLy_Minus >0))
				
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
			
		} //for(window_indexL)	
		
		if(diff_set == 1){
			//copyZAroundZeroY(vL, wvL, OptYindex, halfWindow);
			 if(OK_ZERO_NUM !=copyZAroundZeroY2(vL, wvL, OptYindex, halfWindow)) continue;
			correl = CorrelationCoefficient(wvS, wvL, window_size+1);
			//cout << " vL       indexL    =    "<<OptYindex<<" -1:"<<vL[OptYindex-1].y<<" 0:"<<vL[OptYindex].y<<" +1:"<<vL[OptYindex+1].y <<endl;
			//cout <<"  correlation coef= "<< correl << " with "<<Cv2.dump[OptYindex].y<< endl;
					
			if(correl > fixed_correl){						
				mIndexPair[cnt].SavedS_Index = window_indexS;
				mIndexPair[cnt].SavedL_Index = OptYindex;
				mIndexPair[cnt].SavedCorrel = correl;
				mIndexPair[cnt].Overlap = _NON_OVERLAP_	;
				
				PrQ.push(mIndexPair[cnt]);
				cnt++; // increase cnt due to current correl		
										
				if(cnt > 20) {
					cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
					cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
					continue;
				}		
			} // if(correl > fixed_correl)					       
	       }// 		if(diff_set == 1)
	   } //  if((DvSy_Plus <= 0)&&(DvSy_Minus >0))		  
	 
	  
	  /////    skip the calculation of minima for Y direction because there is no meaningful variation for minima in Y  /////

	  
	  
	  
	} // for(window_indexS)
#endif
	int tmpCnt=0;
	while(!PrQ.empty()){
		
		tmpIndexPair = PrQ.top(); 
		PrQ.pop();
		
		mIndexPair[tmpCnt].SavedS_Index = tmpIndexPair.SavedS_Index ;
		mIndexPair[tmpCnt].SavedL_Index = tmpIndexPair.SavedL_Index ;
		mIndexPair[tmpCnt].SavedCorrel = tmpIndexPair.SavedCorrel ;
		mIndexPair[tmpCnt].Overlap = tmpIndexPair.Overlap	;
		
		tmpCnt++;	
		
       }
#if 1
	for(int j=0 ; j<cnt; j++){
		
		cout<<"SavedSindex: "<<mIndexPair[j].SavedS_Index<<" SavedLindex: "<<mIndexPair[j].SavedL_Index<<" correl: "<<mIndexPair[j].SavedCorrel
			<<" Overlap: "<<mIndexPair[j].Overlap<<endl;
	}
	
	cout << " ^^^   max_confident correl= "<<max_confident<<" at "<<max_cnt<<endl;
#endif	
	
#ifdef MATLAB_TEST
	fclose(fp_M1);
	fclose(fp_M2);
	fclose(fp_max1);
	fclose(fp_max2);
#endif

	return cnt;
}


int FindFeatures5(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl)
{
#ifdef MATLAB_TEST
	FILE *fp_M1, *fp_M2, *fp_max1, *fp_max2;
	if((fp_M1=fopen( "matlab_testCV1.txt","w"))==NULL){
		
		cout << "matlab_testCV1.txt file open error!"<<endl;
	}	
	
	if((fp_M2=fopen( "matlab_testCV2.txt","w"))==NULL){
		
		cout << "matlab_testCV2.txt file open error!"<<endl;
	}
	
	
	if((fp_max1=fopen( "matlab_testMax1.txt","w"))==NULL){
		
		cout << "matlab_testMax1.txt file open error!"<<endl;
	}
	
	
	if((fp_max2=fopen( "matlab_testMax2.txt","w"))==NULL){
		
		cout << "matlab_testMax2.txt file open error!"<<endl;
	}
#endif
	
	Cv1.MovingAvg(m);
	cout<< "src dump size : "<<Cv1.dump_size<<endl;
	
	Cv2.MovingAvg(m);
	cout<< "trg dump size : "<<Cv2.dump_size<<endl;
	
	
	// modified from here by wook
	COORD<float> vS[Cv1.clineSize], vL[Cv2.clineSize];


	copyAndExtremaToWindow(Cv1, vS, Cv1.clineSize);
	copyAndExtremaToWindow(Cv2, vL, Cv2.clineSize);
	
#ifdef MATLAB_TEST
	int i;
	for(i=0; i< Cv1.dump_size; i++){
	
		fprintf(fp_M1,"%f %f %f\n",Cv1.dump[i].x, Cv1.dump[i].y, Cv1.dump[i].z);
	}
	
	for(i=0; i< Cv2.dump_size; i++){
	
		fprintf(fp_M2,"%f %f %f\n",Cv2.dump[i].x, Cv2.dump[i].y, Cv2.dump[i].z);
	}	
	for(i=0; i< Cv1.clineSize; i++){
	
		fprintf(fp_max1,"%f %f %f\n",vS[i].x, vS[i].y, vS[i].z);
	}
	
	for(i=0; i< Cv2.clineSize; i++){
	
		fprintf(fp_max2,"%f %f %f\n",vL[i].x, vL[i].y, vL[i].z);
	}	
#endif	

	int window_indexS, window_indexL, prev_indexS=0;
	int local_max_indexS=0, local_max_indexL=0, cont_on =0, cnt =0, max_cnt=0;
	int contY_on=0;
	float local_max = 0.0;
	float correl=0.0, max_confident=0.0;
	float DvSz_Plus, DvSz_Minus, DvLz_Plus, DvLz_Minus;
	float DvSy_Plus, DvSy_Minus, DvLy_Plus, DvLy_Minus;
	int halfWindow = window_size >>1; cout<<"half window size="<<halfWindow<<endl;
	
	float wvS[window_size+1], wvL[window_size+1];
	int OptZindex=0, OptYindex=0;
	float OptZvalue=0.0, OptZdiff=1000.0, diff=0.0;
	float OptYvalue=0.0, OptYdiff=1000.0, diffY=0.0;
	int diff_set =0;
	IndexPair tmpIndexPair;
	///// Priority Queue !!
	priority_queue<IndexPair, vector<IndexPair>, Less_Sindex > PrQ;
	
	for(window_indexS=1; window_indexS<(Cv1.clineSize - window_size-1); window_indexS++){
	
		
		//////////////////////   Compare Z elements   ////////////////////////////
		DvSz_Plus = vS[window_indexS+1].z;
		DvSz_Minus = vS[window_indexS].z;
	      if((DvSz_Plus <= 0)&&(DvSz_Minus >0)) { 		     // on maxima
		  
		  copyYAroundZeroZ(vS, wvS, window_indexS, halfWindow); // in order to compare the correlation of Y elements between the similar Z window candidates.
		 // cout << " %%%% vS       indexS    =        "<<window_indexS<<" -1:"<<vS[window_indexS-1].z<<" 0:"<<vS[window_indexS].z<<" +1:"<<vS[window_indexS+1].z <<endl;
	  
		OptZindex =0; OptZvalue=0.0;  OptZdiff=1000.0;  diff_set =0;
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				DvLz_Plus = vL[window_indexL+1].z;
				DvLz_Minus = vL[window_indexL].z;
				if((DvLz_Plus <= 0)&&(DvLz_Minus >0)) {  // on maxima
					
					diff = fabs(Cv1.dump[window_indexS].z - Cv2.dump[window_indexL].z);
					if( diff < OptZdiff) {
						diff_set =1;
						OptZdiff = diff;	OptZvalue = Cv2.dump[window_indexL].z;	OptZindex = window_indexL;
						//cout <<"Opt Z index: "<<OptZindex <<" with "<< OptZvalue <<" at Source : "<<Cv1.dump[window_indexS].z<<endl;	
						
					}  // if(fabs(Cv1.dump[window_indexS] - Cv2.dump[window_indexL]) > OptZdiff
				 }	//  f((DvLz_Plus <= 0)&&(DvLz_Minus >0))
				
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
			
		} //for(window_indexL)	
		
		if(diff_set == 1){
			copyYAroundZeroZ(vL, wvL, OptZindex, halfWindow);
			correl = CorrelationCoefficient(wvS, wvL, window_size+1);
			//cout << " vL       indexL    =    "<<OptZindex<<" -1:"<<vL[OptZindex-1].z<<" 0:"<<vL[OptZindex].z<<" +1:"<<vL[OptZindex+1].z <<endl;
			//cout <<"  correlation coef= "<< correl << " with "<<Cv2.dump[OptZindex].z<< endl;
					
			if(correl > fixed_correl){						
				mIndexPair[cnt].SavedS_Index = window_indexS;
				mIndexPair[cnt].SavedL_Index = OptZindex;
				mIndexPair[cnt].SavedCorrel = correl;
				
				PrQ.push(mIndexPair[cnt]);
				cnt++; // increase cnt due to current correl		
										
				if(cnt > 20) {
					cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
					cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
					continue;
				}		
			} // if(correl > fixed_correl)					       
	       }// 		if(diff_set == 1)

	} //  if((DvSz_Plus <= 0)&&(DvSz_Minus >0))
	} // for(window_indexS)		  
	
#if 1
	local_max = 0.0;	local_max_indexS =0; 	local_max_indexL =0;  prev_indexS=0;
	for(window_indexS=1; window_indexS<(Cv1.clineSize - window_size-1); window_indexS++){
		//////////////////////   Compare Y elements   ////////////////////////////
	  DvSy_Plus = vS[window_indexS+1].y;
	  DvSy_Minus = vS[window_indexS].y;
	  if((DvSy_Plus <= 0)&&(DvSy_Minus >0)) { 		     // on maxima
		  
		  if(OK_ZERO_NUM !=copyZAroundZeroY2(vL, wvL, window_indexL, halfWindow)) continue; 	  
		  //copyZAroundZeroY(vS, wvS, window_indexS, halfWindow);  // in order to compare the correlation of Y elements between the similar Z window candidates.
		  //cout << " %%%% vS       indexS    =        "<<window_indexS<<" -1:"<<vS[window_indexS-1].y<<" 0:"<<vS[window_indexS].y<<" +1:"<<vS[window_indexS+1].y <<endl;
	  
		OptYindex =0; OptYvalue=0.0;  OptYdiff=1000.0; diff_set =0;
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				DvLy_Plus = vL[window_indexL+1].y;
				DvLy_Minus = vL[window_indexL].y;
       
				if((DvLy_Plus <= 0)&&(DvLy_Minus >0)) {  // on maxima
					
					diffY = fabs(Cv1.dump[window_indexS].y - Cv2.dump[window_indexL].y);
					if( diffY < OptYdiff) {
						diff_set =1;
						OptYdiff = diffY;	OptYvalue = Cv2.dump[window_indexL].y;	OptYindex = window_indexL;
						//cout <<"Opt Y index: "<<OptYindex <<" with "<< OptYvalue <<" at Source : "<<Cv1.dump[window_indexS].y<<endl;	
						
					}  // if(fabs(Cv1.dump[window_indexS] - Cv2.dump[window_indexL]) > OptYdiff
				 }	//  f((DvLy_Plus <= 0)&&(DvLy_Minus >0))
				
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
			
		} //for(window_indexL)	
		
		if(diff_set == 1){
			//copyZAroundZeroY(vL, wvL, OptYindex, halfWindow);
			 if(OK_ZERO_NUM !=copyZAroundZeroY2(vL, wvL, OptYindex, halfWindow)) continue;
			correl = CorrelationCoefficient(wvS, wvL, window_size+1);
			//cout << " vL       indexL    =    "<<OptYindex<<" -1:"<<vL[OptYindex-1].y<<" 0:"<<vL[OptYindex].y<<" +1:"<<vL[OptYindex+1].y <<endl;
			//cout <<"  correlation coef= "<< correl << " with "<<Cv2.dump[OptYindex].y<< endl;
					
			if(correl > fixed_correl){						
				mIndexPair[cnt].SavedS_Index = window_indexS;
				mIndexPair[cnt].SavedL_Index = OptYindex;
				mIndexPair[cnt].SavedCorrel = correl;
				PrQ.push(mIndexPair[cnt]);
				cnt++; // increase cnt due to current correl		
										
				if(cnt > 20) {
					cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
					cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
					continue;
				}		
			} // if(correl > fixed_correl)					       
	       }// 		if(diff_set == 1)
	   } //  if((DvSy_Plus <= 0)&&(DvSy_Minus >0))		  
	  
	} // for(window_indexS)
#endif
	int tmpCnt=0;
	while(!PrQ.empty()){
		
		tmpIndexPair = PrQ.top(); 
		PrQ.pop();
		
		mIndexPair[tmpCnt].SavedS_Index = tmpIndexPair.SavedS_Index ;
		mIndexPair[tmpCnt].SavedL_Index = tmpIndexPair.SavedL_Index ;
		mIndexPair[tmpCnt].SavedCorrel = tmpIndexPair.SavedCorrel ;
		
		tmpCnt++;	
		
       }
#if 1
	for(int j=0 ; j<cnt; j++){
		
		cout<<"SavedSindex: "<<mIndexPair[j].SavedS_Index<<" SavedLindex: "<<mIndexPair[j].SavedL_Index<<" correl: "<<mIndexPair[j].SavedCorrel<<endl;
	}
	
	cout << " ^^^   max_confident correl= "<<max_confident<<" at "<<max_cnt<<endl;
#endif	
	
#ifdef MATLAB_TEST
	fclose(fp_M1);
	fclose(fp_M2);
	fclose(fp_max1);
	fclose(fp_max2);
#endif

	return cnt;
}







int FindFeatures4(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl)
{
	Cv1.MovingAvg(m);
	cout<< "src dump size : "<<Cv1.dump_size<<endl;
	
	Cv2.MovingAvg(m);
	cout<< "trg dump size : "<<Cv2.dump_size<<endl;
	
	// modified from here by wook
	COORD<float> vS[Cv1.clineSize], vL[Cv2.clineSize];
	

	copyAndMinmodToWindow2(Cv1, vS, Cv1.clineSize);
	copyAndMinmodToWindow2(Cv2, vL, Cv2.clineSize);
	
	int window_indexS, window_indexL, prev_indexS=0;
	int local_max_indexS=0, local_max_indexL=0, cont_on =0, cnt =0, max_cnt=0;
	float local_max = 0.0;
	float correl=0.0, max_confident=0.0;
	
	for(window_indexS=0; window_indexS<(Cv1.clineSize - window_size); window_indexS++){
		
	  if((vS[window_indexS].y==0)||(vS[window_indexS].z==0)) {
		
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				if((vL[window_indexL].y==0)||(vL[window_indexL].z==0)) {
				  //for(int i=0; i<window_size; i++)   // for test
					//cout<<"vS: "<<vS[i].x<<", "<<vS[i].y<<", "<<vS[i].z<<"     vL: "<<vL[i].x<<", "<<vL[i].y<<", "<<vL[i].z<<endl;
				
					correl = CorrelationCoefficientYZ(&vS[window_indexS], &vL[window_indexL], window_size);
				
				//cout <<"+++  correlation coef= "<< correl <<endl;
				
				if(correl > fixed_correl){
					
					if((window_indexS == (prev_indexS + 1))||(window_indexS == prev_indexS)){
						
						if(local_max < correl){
							local_max = correl;
							local_max_indexS = window_indexS;
							local_max_indexL = window_indexL;
							cont_on = 1;  
						}
						
					}
					else  //  if(window_indexS != (prev_indexS + 1)
					{
						if(cont_on == 1){	
							
							mIndexPair[cnt].SavedS_Index = local_max_indexS;
							mIndexPair[cnt].SavedL_Index = local_max_indexL;
							mIndexPair[cnt].SavedCorrel = local_max;
							cnt ++; //  increase cnt due to prev local_max correl
							cont_on = 0;
							
							local_max = 0.0;	local_max_indexS =0; 	local_max_indexL =0;
							
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;
							cnt++; // increase cnt due to current correl		
						} 
						else// if( cont_on != 1)
						{
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;	
							cnt++; // increase cnt due to current correl
						}  //else- if( cont_on != 1)
						
					}  //else- if(window_indexS != (prev_indexS + 1)
					
					if(correl > max_confident){	
						max_confident = correl;
						max_cnt = cnt; }
					
					prev_indexS = window_indexS;
					
					if(cnt > 100) {
						cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
						cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
						continue;
						
					}
					
				} // if(correl > fixed_correl)				
				
			} //if((vL[window_indexL].y==0)||(vL[window_indexL].z==0)) 		
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
		
			
		} //for(window_indexL)
	} // if((vS[window_indexS].y==0)||(vS[window_indexS].z==0))
	} // for(window_indexS)
	
	
#if 1	
	for(int j=0 ; j<cnt; j++){
		
		cout<<"SavedSindex: "<<mIndexPair[j].SavedS_Index<<" SavedLindex: "<<mIndexPair[j].SavedL_Index<<" correl: "<<mIndexPair[j].SavedCorrel<<endl;
	}
	
	cout << " ^^^   max_confident correl= "<<max_confident<<" at "<<max_cnt<<endl;
#endif	
	
	return cnt;
}


int FindFeatures3(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl)
{
	Cv1.MovingAvg(m);
	cout<< "src dump size : "<<Cv1.dump_size<<endl;
	
	Cv2.MovingAvg(m);
	cout<< "trg dump size : "<<Cv2.dump_size<<endl;
	
#if 0
	int i;
//	for(i=568;i<Cv1.dump_size;i++){
	for(i=568;i<Cv1.clineSize;i++){	
		
		cout<<i<<" dump("<<Cv1.dump[i].x<<", "<<Cv1.dump[i].y<<", "<<Cv1.dump[i].z<<")"<<endl;
	}
#endif	
	
#if 0
	int i;
//	for(i=568;i<Cv1.dump_size;i++){
	for(i=568;i<Cv2.clineSize;i++){	
		
		cout<<i<<" dump("<<Cv2.dump[i].x<<", "<<Cv2.dump[i].y<<", "<<Cv2.dump[i].z<<")"<<endl;
	}
#endif	
	int num_extend;
	if(Cv1.clineSize > 	Cv2.clineSize	 ) num_extend = Cv1.clineSize;
	else num_extend = Cv2.clineSize;
	
	COORD<float> eCv1[num_extend], eCv2[num_extend];
	COORD<float> vS[window_size], vL[window_size];
	
	copyAndFillZero(eCv1, Cv1, num_extend);
	copyAndFillZero(eCv2, Cv2, num_extend);
	
	int window_indexS, window_indexL, prev_indexS=0;
	int local_max_indexS=0, local_max_indexL=0, cont_on =0, cnt =0, max_cnt=0;
	float local_max = 0.0;
	float correl=0.0, max_confident=0.0;
	
	for(window_indexS=0; window_indexS<(Cv1.clineSize - window_size); window_indexS++){
		
	       //window_indexS = 0;//(Cv1.clineSize - window_size-1);  // for test
		//copyToWindow(&eCv1[window_indexS], vS, window_size);
		copyAndMinmodToWindow(&eCv1[window_indexS], vS, window_size);
			
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				//copyToWindow(&eCv2[window_indexL], vL, window_size);
				copyAndMinmodToWindow(&eCv2[window_indexL], vL, window_size);
				for(int i=0; i<window_size; i++)   // for test
					cout<<"vS: "<<vS[i].x<<", "<<vS[i].y<<", "<<vS[i].z<<"     vL: "<<vL[i].x<<", "<<vL[i].y<<", "<<vL[i].z<<endl;
				
				correl = CorrelationCoefficientYZ(vS, vL, window_size);
				
				//cout <<"+++  correlation coef= "<< correl <<endl;
				
				if(correl > fixed_correl){
					
					if((window_indexS == (prev_indexS + 1))||(window_indexS == prev_indexS)){
						
						if(local_max < correl){
							local_max = correl;
							local_max_indexS = window_indexS;
							local_max_indexL = window_indexL;
							cont_on = 1;
						}
						
					}
					else  //  if(window_indexS != (prev_indexS + 1)
					{
						if(cont_on == 1){	
							
							mIndexPair[cnt].SavedS_Index = local_max_indexS;
							mIndexPair[cnt].SavedL_Index = local_max_indexL;
							mIndexPair[cnt].SavedCorrel = local_max;
							cnt ++; //  increase cnt due to prev local_max correl
							cont_on = 0;
							
							local_max = 0.0;	local_max_indexS =0; 	local_max_indexL =0;
							
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;
							cnt++; // increase cnt due to current correl		
						} 
						else// if( cont_on != 1)
						{
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;	
							cnt++; // increase cnt due to current correl
						}  //else- if( cont_on != 1)
						
					}  //else- if(window_indexS != (prev_indexS + 1)
					
					if(correl > max_confident){	
						max_confident = correl;
						max_cnt = cnt; }
					
					prev_indexS = window_indexS;
					
					if(cnt > 100) {
						cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
						cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
						continue;
						
					}
					
				} // if(correl > fixed_correl)				
				
					
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
		
		} //for(window_indexL)
	} // for(window_indexS)
	
	
#if 1	
	for(int j=0 ; j<cnt; j++){
		
		cout<<"SavedSindex: "<<mIndexPair[j].SavedS_Index<<" SavedLindex: "<<mIndexPair[j].SavedL_Index<<" correl: "<<mIndexPair[j].SavedCorrel<<endl;
	}
	
	cout << " ^^^   max_confident correl= "<<max_confident<<" at "<<max_cnt<<endl;
#endif	
	
	return cnt;
}

int FindFeatures2(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl)
{
	Cv1.MovingAvg(m);
	cout<< "src dump size : "<<Cv1.dump_size<<endl;
	
	Cv2.MovingAvg(m);
	cout<< "trg dump size : "<<Cv2.dump_size<<endl;
	
#if 0
	int i;
//	for(i=568;i<Cv1.dump_size;i++){
	for(i=568;i<Cv1.clineSize;i++){	
		
		cout<<i<<" dump("<<Cv1.dump[i].x<<", "<<Cv1.dump[i].y<<", "<<Cv1.dump[i].z<<")"<<endl;
	}
#endif	
	
#if 0
	int i;
//	for(i=568;i<Cv1.dump_size;i++){
	for(i=568;i<Cv2.clineSize;i++){	
		
		cout<<i<<" dump("<<Cv2.dump[i].x<<", "<<Cv2.dump[i].y<<", "<<Cv2.dump[i].z<<")"<<endl;
	}
#endif	
	int num_extend;
	if(Cv1.clineSize > 	Cv2.clineSize	 ) num_extend = Cv1.clineSize;
	else num_extend = Cv2.clineSize;
	
	COORD<float> eCv1[num_extend], eCv2[num_extend];
	COORD<float> vS[window_size], vL[window_size];
	
	copyAndFillZero(eCv1, Cv1, num_extend);
	copyAndFillZero(eCv2, Cv2, num_extend);
	
	int window_indexS, window_indexL, prev_indexS=0;
	int local_max_indexS=0, local_max_indexL=0, cont_on =0, cnt =0, max_cnt=0;
	float local_max = 0.0;
	float correl=0.0, max_confident=0.0;
	
	for(window_indexS=0; window_indexS<(Cv1.clineSize - window_size); window_indexS++){
		
	       //window_indexS = 0;//(Cv1.clineSize - window_size-1);  // for test
		copyToWindow(&eCv1[window_indexS], vS, window_size);
			
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				copyToWindow(&eCv2[window_indexL], vL, window_size);
				
				//for(int i=0; i<window_size; i++)   // for test
					//cout<<"vS: "<<vS[i].x<<", "<<vS[i].y<<", "<<vS[i].z<<"     vL: "<<vL[i].x<<", "<<vL[i].y<<", "<<vL[i].z<<endl;
				
				correl = CorrelationCoefficientYZ(vS, vL, window_size);
				
				//cout <<"+++  correlation coef= "<< correl <<endl;
				
				if(correl > fixed_correl){
					
					if((window_indexS == (prev_indexS + 1))||(window_indexS == prev_indexS)){
						
						if(local_max < correl){
							local_max = correl;
							local_max_indexS = window_indexS;
							local_max_indexL = window_indexL;
							cont_on = 1;
						}
						
					}
					else  //  if(window_indexS != (prev_indexS + 1)
					{
						if(cont_on == 1){	
							
							mIndexPair[cnt].SavedS_Index = local_max_indexS;
							mIndexPair[cnt].SavedL_Index = local_max_indexL;
							mIndexPair[cnt].SavedCorrel = local_max;
							cnt ++; //  increase cnt due to prev local_max correl
							cont_on = 0;
							
							local_max = 0.0;	local_max_indexS =0; 	local_max_indexL =0;
							
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;
							cnt++; // increase cnt due to current correl		
						} 
						else// if( cont_on != 1)
						{
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;	
							cnt++; // increase cnt due to current correl
						}  //else- if( cont_on != 1)
						
					}  //else- if(window_indexS != (prev_indexS + 1)
					
					if(correl > max_confident){	
						max_confident = correl;
						max_cnt = cnt; }
					
					prev_indexS = window_indexS;
					
					if(cnt > 100) {
						cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
						cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
						continue;
						
					}
					
				} // if(correl > fixed_correl)				
				
					
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
		
		} //for(window_indexL)
	} // for(window_indexS)
	
	
#if 1	
	for(int j=0 ; j<cnt; j++){
		
		cout<<"SavedSindex: "<<mIndexPair[j].SavedS_Index<<" SavedLindex: "<<mIndexPair[j].SavedL_Index<<" correl: "<<mIndexPair[j].SavedCorrel<<endl;
	}
	
	cout << " ^^^   max_confident correl= "<<max_confident<<" at "<<max_cnt<<endl;
#endif	
	
	return cnt;
}


int FindFeatures(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl)
{
	Cv1.FindSliding3Dmv(m);
	cout<< "src dump size : "<<Cv1.dump_size<<endl;
	
	Cv2.FindSliding3Dmv(m);
	cout<< "trg dump size : "<<Cv2.dump_size<<endl;
	
#if 0
	int i;
//	for(i=568;i<Cv1.dump_size;i++){
	for(i=568;i<Cv1.clineSize;i++){	
		
		cout<<i<<" dump("<<Cv1.dump[i].x<<", "<<Cv1.dump[i].y<<", "<<Cv1.dump[i].z<<")"<<endl;
	}
#endif	
	
#if 0	
	int i;
//	for(i=568;i<Cv1.dump_size;i++){
	for(i=568;i<Cv2.clineSize;i++){	
		
		cout<<i<<" dump("<<Cv2.dump[i].x<<", "<<Cv2.dump[i].y<<", "<<Cv2.dump[i].z<<")"<<endl;
	}
#endif	
	int num_extend;
	if(Cv1.clineSize > 	Cv2.clineSize	 ) num_extend = Cv1.clineSize;
	else num_extend = Cv2.clineSize;
	
	COORD<float> eCv1[num_extend], eCv2[num_extend];
	COORD<float> vS[window_size], vL[window_size];
	
	copyAndFillZero(eCv1, Cv1, num_extend);
	copyAndFillZero(eCv2, Cv2, num_extend);
	
	int window_indexS, window_indexL, prev_indexS=0;
	int local_max_indexS=0, local_max_indexL=0, cont_on =0, cnt =0, max_cnt=0;
	float local_max = 0.0;
	float correl=0.0, max_confident=0.0;
	
	for(window_indexS=0; window_indexS<(Cv1.clineSize - window_size); window_indexS++){
		
	       //window_indexS = 0;//(Cv1.clineSize - window_size-1);  // for test
		copyToWindow(&eCv1[window_indexS], vS, window_size);
			
		for(window_indexL=(window_indexS-minusSearchRange-1); window_indexL<(window_indexS+plusSearchRange); window_indexL++){
	        	
		//window_indexL = window_indexS;
			if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>=0)) {
				
				copyToWindow(&eCv2[window_indexL], vL, window_size);
				
				//for(int i=0; i<window_size; i++)   // for test
					//cout<<"vS: "<<vS[i].x<<", "<<vS[i].y<<", "<<vS[i].z<<"     vL: "<<vL[i].x<<", "<<vL[i].y<<", "<<vL[i].z<<endl;
				
				correl = CorrelationCoefficient(vS, vL, window_size);
				
				//cout <<"+++  correlation coef= "<< correl <<endl;
				
				if(correl > fixed_correl){
					
					if((window_indexS == (prev_indexS + 1))||(window_indexS == prev_indexS)){
						
						if(local_max < correl){
							local_max = correl;
							local_max_indexS = window_indexS;
							local_max_indexL = window_indexL;
							cont_on = 1;
						}
						
					}
					else  //  if(window_indexS != (prev_indexS + 1)
					{
						if(cont_on == 1){	
							
							mIndexPair[cnt].SavedS_Index = local_max_indexS;
							mIndexPair[cnt].SavedL_Index = local_max_indexL;
							mIndexPair[cnt].SavedCorrel = local_max;
							cnt ++; //  increase cnt due to prev local_max correl
							cont_on = 0;
							
							local_max = 0.0;	local_max_indexS =0; 	local_max_indexL =0;
							
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;
							cnt++; // increase cnt due to current correl		
						} 
						else// if( cont_on != 1)
						{
							mIndexPair[cnt].SavedS_Index = window_indexS;
							mIndexPair[cnt].SavedL_Index = window_indexL;
							mIndexPair[cnt].SavedCorrel = correl;	
							cnt++; // increase cnt due to current correl
						}  //else- if( cont_on != 1)
						
					}  //else- if(window_indexS != (prev_indexS + 1)
					
					if(correl > max_confident){	
						max_confident = correl;
						max_cnt = cnt; }
					
					prev_indexS = window_indexS;
					
					if(cnt > 100) {
						cout << "      ERROR !! The number of saved index is more than 100 !!"<<endl;
						cout <<"                       Use a higher  fixed_correl than "<<fixed_correl <<endl;
						continue;
						
					}
					
				} // if(correl > fixed_correl)				
				
					
			} //if((window_indexL<(Cv2.clineSize-window_size)) &&(window_indexL>0)) 
		
		} //for(window_indexL)
	} // for(window_indexS)
	
	
#if 1	
	for(int j=0 ; j<cnt; j++){
		
		cout<<"SavedSindex: "<<mIndexPair[j].SavedS_Index<<" SavedLindex: "<<mIndexPair[j].SavedL_Index<<" correl: "<<mIndexPair[j].SavedCorrel<<endl;
	}
	
	cout << " ^^^   max_confident correl= "<<max_confident<<" at "<<max_cnt<<endl;
#endif	
	
	return cnt;
}

#if 0
void ReconstructFeaturedCline(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair  &matchedPair)
{
	int indexS=0, indexL=0; 
	
	cout << "validCnt= "<<validCnt<<" in ReconstructFeaturedCline();"<<endl;
	matchedPair.initMatchedPair(window_size);
	
	for(int i=0; i<validCnt; i++){
		indexS = mIndexPair[i].SavedS_Index;
		indexL = mIndexPair[i].SavedL_Index;
		cout << "indexS= "<<indexS<<" , indexL= "<<indexL<<endl;
		matchedPair.fillMatchedPair(cS, cT, indexS, indexL);	
	}
}
#endif
void ReconstructFeaturedCline(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair  *matchedPair)
{
	cout << "validCnt= "<<validCnt<<" in ReconstructFeaturedCline();"<<endl;
	int i, indexS=0, indexL=0; 
	
	for(i=0; i<validCnt; i++){
		matchedPair[i].initMatchedPair(window_size);
       }
	
	for(i=0; i<validCnt; i++){
		indexS = mIndexPair[i].SavedS_Index;
		indexL = mIndexPair[i].SavedL_Index;
		cout << "indexS= "<<indexS<<" , indexL= "<<indexL<<endl;
		matchedPair[i].fillMatchedPair(cS, cT, indexS, indexL);	
	}

	#if 0
	for(i=0; i<validCnt; i++){
		for(int j=0; j<window_size; j++){
		cout<<i<<" saved src point: ("<<matchedPair[i].src[j].x<<", "<<matchedPair[i].src[j].y<<", "<<matchedPair[i].src[j].z<<")"<<
			"     saved trg point: ("<<matchedPair[i].trg[j].x<<", "<<matchedPair[i].trg[j].y<<", "<<matchedPair[i].trg[j].z<<")"<<endl;
	      }
	}
      #endif
}


void ReconstructFeaturedClineMORE(IndexPair* mIndexPair, int validCnt, int MORE_window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair  *matchedPair)
{
	cout << "validCnt= "<<validCnt<<" in ReconstructFeaturedCline();"<<endl;
	int i, indexS=0, indexL=0; 
	
	for(i=0; i<validCnt; i++){
		matchedPair[i].initMatchedPair(MORE_window_size);
       }
	
	for(i=0; i<validCnt; i++){
		indexS = mIndexPair[i].SavedS_Index ;   // MORE version has more elements within same index: 
		indexL = mIndexPair[i].SavedL_Index ;   // 30 more after original index.
		cout << "indexS= "<<indexS<<" , indexL= "<<indexL<<endl;
		matchedPair[i].fillMatchedPair(cS, cT, indexS, indexL);	
	}

	#if 1
	for(i=0; i<validCnt; i++){
		for(int j=0; j<MORE_window_size; j++){
			
			if(j==0){	
		cout<<i<<" saved src point: ("<<matchedPair[i].src[j].x<<", "<<matchedPair[i].src[j].y<<", "<<matchedPair[i].src[j].z<<")"<<
			"     saved trg point: ("<<matchedPair[i].trg[j].x<<", "<<matchedPair[i].trg[j].y<<", "<<matchedPair[i].trg[j].z<<")"<<endl;
	}
	      }
	}
      #endif
}
