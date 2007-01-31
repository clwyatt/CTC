#include "itkImage.h"
#include "itkDeformationFieldSource.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWarpImageFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkNeighborhoodIterator.h"
#include "itkDiscreteGaussianImageFilter.h"

#include "MyHeader.h"
#include "deformByCLine.h"
#include <stdio.h>
#include <math.h>
using namespace std;

int GroupObjVoxel::CountSrcAndTrgObj(TNT::Array3D<int> &Src, TNT::Array3D<int> &Trg, int dimx, int dimy, int dimz)
{
	int vol_num =0;
	
	for(int k=0; k < dimz; k++){ 	
	for(int j=0; j < dimy; j++){
	for(int i=0; i < dimx; i++){
			
		if((Src[i][j][k] !=0) || (Trg[i][j][k] !=0) ) vol_num++;
	}
	}
	}
	
	cout<<"Src and Trg vol_num = "<<vol_num<<endl;
	return vol_num;
}

void GroupObjVoxel::fillGroupObjVoxelObj(int num, TNT::Array3D<int> &Src, TNT::Array3D<int> &Trg,int dimx, int dimy, int dimz)
{
	unsigned long int vol =0;
	size = num;
	groupObjVoxel = new ObjVoxel[num];
	
		for(int k=0; k < dimz; k++){ 	
		for(int j=0; j < dimy; j++){
		for(int i=0; i < dimx; i++){
			
			if( (Src[i][j][k] !=0) || (Trg[i][j][k] !=0) ) {
				
				groupObjVoxel[vol].closestDistToCvoxel= 0.0;
				groupObjVoxel[vol].closestClineIndex = 0;
				
				groupObjVoxel[vol].closestDistToAuxCvoxel = 0.0;
				groupObjVoxel[vol].closestAuxClineIndex = 0;
				
				groupObjVoxel[vol].position.x =i;   
				groupObjVoxel[vol].position.y =j;  
				groupObjVoxel[vol].position.z =k;
				
				groupObjVoxel[vol].DField.x =0.0; 
				groupObjVoxel[vol].DField.y =0.0; 
				groupObjVoxel[vol].DField.z =0.0;   
				groupObjVoxel[vol].whichClosestCline = 0;
				groupObjVoxel[vol].mark = MARK_OFF    ;
				
				vol++;  
				
			}  //    if( (Src[i][j][k] !=0) || (Trg[i][j][k] !=0) )
		}  // i - x
		}  // j - y
	       }  // k - z
		
	cout << "num of vol ="<<vol<<endl;
}

#if 0
void GroupObjVoxel::fillGroupObjVoxel(unsigned long int num, TNT::Array3D<int> &bVoxel, int dimx, int dimy, int dimz)
{
	unsigned long int vol =0;
	size = num;
	groupObjVoxel = new ObjVoxel[num];
	
		for(int k=0; k < dimz; k++){ 	
		for(int j=0; j < dimy; j++){
		for(int i=0; i < dimx; i++){
			
			if(bVoxel[i][j][k] !=0){
				
				groupObjVoxel[vol].closestDistToCvoxel= 0.0;
				groupObjVoxel[vol].closestClineIndex = 0;
				
				groupObjVoxel[vol].position.x =i;   
				groupObjVoxel[vol].position.y =j;  
				groupObjVoxel[vol].position.z =k;
				
				groupObjVoxel[vol].DField.x =0.0; 
				groupObjVoxel[vol].DField.y =0.0; 
				groupObjVoxel[vol].DField.z =0.0;   
				
				vol++;  
				
			}  //    if(bVoxel[i][j][k] !=0)
		}  // i - x
		}  // j - y
	       }  // k - z
		
	cout << "num of vol ="<<vol<<endl;
}
#endif

void GroupObjVoxel::fillGroupObjVoxel(unsigned long int num, TNT::Array3D<int> &bVoxel, int dimx, int dimy, int dimz)
{
	unsigned long int vol =0;
	size = num;
	groupObjVoxel = new ObjVoxel[num];
	
		for(int k=0; k < dimz; k++){ 	
		for(int j=0; j < dimy; j++){
		for(int i=0; i < dimx; i++){
			
//			if(bVoxel[i][j][k] !=0){
				
				groupObjVoxel[vol].closestDistToCvoxel= 0.0;
				groupObjVoxel[vol].closestClineIndex = 0;
				
				groupObjVoxel[vol].closestDistToAuxCvoxel = 0.0;
				groupObjVoxel[vol].closestAuxClineIndex = 0;
				
				groupObjVoxel[vol].position.x =i;   
				groupObjVoxel[vol].position.y =j;  
				groupObjVoxel[vol].position.z =k;
				
				groupObjVoxel[vol].DField.x =0.0; 
				groupObjVoxel[vol].DField.y =0.0; 
				groupObjVoxel[vol].DField.z =0.0;   
				groupObjVoxel[vol].whichClosestCline = 0;
				groupObjVoxel[vol].mark = MARK_OFF    ;
				
				vol++;  
				
//			}  //    if(bVoxel[i][j][k] !=0)
		}  // i - x
		}  // j - y
	       }  // k - z
		
	cout << "num of vol ="<<vol<<endl;
}


#if  1

void testVolume(int dilate, COORD<int> *pts, int num)
{
	int sizeX=256, sizeY=256, sizeZ=225;
	TNT::Array3D<int> v(sizeX, sizeY, sizeZ, 0); 
	
	int ii;
	int ddx, ddy, ddz, dxx, dyy, dzz;
	
	for(ii=0; ii<num; ii++){
		
		v[pts[ii].x][pts[ii].y][pts[ii].z] = 255;
		cout <<ii<<" testVolume: "<<pts[ii].x<<", "<<pts[ii].y<<", "<<pts[ii].z<<endl;
		
		if(dilate>0){
			
			for(ddz=-dilate; ddz<dilate+1; ddz++){
			for(ddy=-dilate; ddy<dilate+1; ddy++){
			for(ddx=-dilate; ddx<dilate+1; ddx++){
						
					dxx = ddx + pts[ii].x;
					dyy = ddy + pts[ii].y;
					dzz = ddz + pts[ii].z;
						
					if((dxx<0)||(dyy<0)||(dzz<0)) continue;
					if((dxx>=sizeX)||(dyy>=sizeY)||(dzz>=sizeZ)) continue;
						
					v[dxx][dyy][dzz] = 255;
						
			} // ddx
			} // ddy
			} // ddz
			
		} // if(dilate>0)
		
	}  // for - ii
		
	ImportFilterType::Pointer importFilter = ImportFilterType::New();
	ImportFilterType::SizeType  size;

	size[0]  = sizeX;  // size along X
	size[1]  = sizeY;  // size along Y
	size[2]  = sizeZ;  // size along Z
	ImportFilterType::IndexType start;
	start.Fill( 0 );

	ImportFilterType::RegionType region;
	region.SetIndex( start );
	region.SetSize(  size  );

	importFilter->SetRegion( region );
		
	ImportFilterType::SpacingType  Spacing;
	Spacing[0] =2;
	Spacing[1] =2;
	Spacing[2] =2;

	importFilter->SetSpacing( Spacing );
	unsigned int numOfPixels =  size[0] * size[1] * size[2];

	OutputPixelType *localBuffer = new OutputPixelType[numOfPixels];
	OutputPixelType * it = localBuffer;	
	int i,j,k, value;
	
	for(k=0; k < sizeZ; k++){
		for(j=0; j < sizeY; j++){
			for(i=0; i < sizeX; i++){
				
				value = v[i][j][k];
				if(value > 0) *it = 255;
				else *it = 0;
				it++;      
				
			}   
		}
	} // k	
	const bool importImageFilterWillOwnTheBuffer = true;
	importFilter->SetImportPointer( localBuffer, numOfPixels,
                                  importImageFilterWillOwnTheBuffer );
	
	WriterType::Pointer writer = WriterType::New();

	writer->SetFileName( "test.vtk" );
	writer->SetInput(  importFilter->GetOutput()  );

	//cout <<"test--------"<<endl;
	
	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject & exp )
	{
		std::cerr << "Exception caught ! in the storeImage()" << std::endl;
		std::cerr << exp << std::endl;
	}
}

float distanceFromCline(COORD<int> &p, COORD<int> &c)
{
	float dx, dy, dz, dd;
	dx =(float) (p.x - c.x);
	dy =(float) (p.y - c.y);
	dz =(float) (p.z - c.z);
	dd = dx*dx + dy*dy + dz*dz;
	return sqrt(dd);
}

void GroupObjVoxel::distFromClosestCline2(DField4Clines &VecCline)
{
	int i,j, smallIndex;
	float tempDist, smallDist;
	int cnt=0;
	cout<< "cline size= "<< VecCline.cline_size<<endl;
	
	for(i=0; i<size; i++){
	//	i=30;
		smallDist =50000;
		for(j=0; j<VecCline.cline_size; j++){
			
			tempDist = distanceFromCline(groupObjVoxel[i].position, VecCline.DFieldCline[j].position);
			
			if(tempDist ==0.0){   // when the volume voxel is the centerline voxel.

				groupObjVoxel[i].DField.x = VecCline.DFieldCline[j].DField.x;
				groupObjVoxel[i].DField.y = VecCline.DFieldCline[j].DField.y;
				groupObjVoxel[i].DField.z = VecCline.DFieldCline[j].DField.z;
				groupObjVoxel[i].mark = MARK_ON;
				smallDist = tempDist;
				smallIndex = j;
				groupObjVoxel[i].closestDistToCvoxel = smallDist; 
				groupObjVoxel[i].closestClineIndex = smallIndex;
				
				cnt++;
				
				continue;
			}
			
			if(tempDist < smallDist){
				smallDist = tempDist;
				smallIndex = j;
				
				
			/*	cout<< "smallDist="<<smallDist<<" smallIndex="<<smallIndex<<" at("<<groupObjVoxel[i].position.x
					<<", "<<groupObjVoxel[i].position.y<<", "<<groupObjVoxel[i].position.z<<") and ("<<
					VecCline.DFieldCline[j].position.x<<", "<<VecCline.DFieldCline[j].position.y<<", "<<
					VecCline.DFieldCline[j].position.z<<")"<<endl;
			*/
			} // if(tempDist < smallDist)
		}  // j
		groupObjVoxel[i].closestDistToCvoxel = smallDist; 
		groupObjVoxel[i].closestClineIndex = smallIndex;
		
		/////////////    add auxiliary DField for source cline cal.  ///////////////////
		smallDist =50000;
		for(j=0; j<VecCline.auxCline_size; j++){
			
			tempDist = distanceFromCline(groupObjVoxel[i].position, VecCline.auxDFieldCline[j].position);
			
			if(tempDist ==0.0){   // when the volume voxel is the centerline voxel.

				groupObjVoxel[i].DField.x = VecCline.auxDFieldCline[j].DField.x;
				groupObjVoxel[i].DField.y = VecCline.auxDFieldCline[j].DField.y;
				groupObjVoxel[i].DField.z = VecCline.auxDFieldCline[j].DField.z;
				groupObjVoxel[i].mark = MARK_ON;
				smallDist = tempDist;
				smallIndex = j;
				groupObjVoxel[i].closestDistToAuxCvoxel= smallDist; 
				groupObjVoxel[i].closestAuxClineIndex = smallIndex;
				
				//cnt++;
				
				continue;
			}
			
			if(tempDist < smallDist){
				smallDist = tempDist;
				smallIndex = j;
				
				
			/*	cout<< "smallDist="<<smallDist<<" smallIndex="<<smallIndex<<" at("<<groupObjVoxel[i].position.x
					<<", "<<groupObjVoxel[i].position.y<<", "<<groupObjVoxel[i].position.z<<") and ("<<
					VecCline.DFieldCline[j].position.x<<", "<<VecCline.DFieldCline[j].position.y<<", "<<
					VecCline.DFieldCline[j].position.z<<")"<<endl;
			*/
			} // if(tempDist < smallDist)
		}  // j
		groupObjVoxel[i].closestDistToAuxCvoxel  = smallDist; 
		groupObjVoxel[i].closestAuxClineIndex = smallIndex;	
		
		if(groupObjVoxel[i].closestDistToAuxCvoxel >= groupObjVoxel[i].closestDistToCvoxel )  groupObjVoxel[i].whichClosestCline = _TARGET_CLINE_;
		else groupObjVoxel[i].whichClosestCline = _SOURCE_CLINE_ ;
		

	}  // i
		

	
	cout<<"cline cnt in distFromClosestCline(): "<<cnt<< endl;
}


void GroupObjVoxel::distFromClosestCline(DField4Clines &VecCline)
{
	int i,j, smallIndex;
	float tempDist, smallDist;
	int cnt=0;
	cout<< "cline size= "<< VecCline.cline_size<<endl;
	
	for(i=0; i<size; i++){
	//	i=30;
		smallDist =50000;
		for(j=0; j<VecCline.cline_size; j++){
			
			tempDist = distanceFromCline(groupObjVoxel[i].position, VecCline.DFieldCline[j].position);
			
			if(tempDist ==0.0){   // when the volume voxel is the centerline voxel.

				groupObjVoxel[i].DField.x = VecCline.DFieldCline[j].DField.x;
				groupObjVoxel[i].DField.y = VecCline.DFieldCline[j].DField.y;
				groupObjVoxel[i].DField.z = VecCline.DFieldCline[j].DField.z;
				groupObjVoxel[i].mark = MARK_ON;
				smallDist = tempDist;
				smallIndex = j;
				groupObjVoxel[i].closestDistToCvoxel = smallDist; 
				groupObjVoxel[i].closestClineIndex = smallIndex;
				
				cnt++;
				
				continue;
			}
			
			if(tempDist < smallDist){
				smallDist = tempDist;
				smallIndex = j;
				
				
			/*	cout<< "smallDist="<<smallDist<<" smallIndex="<<smallIndex<<" at("<<groupObjVoxel[i].position.x
					<<", "<<groupObjVoxel[i].position.y<<", "<<groupObjVoxel[i].position.z<<") and ("<<
					VecCline.DFieldCline[j].position.x<<", "<<VecCline.DFieldCline[j].position.y<<", "<<
					VecCline.DFieldCline[j].position.z<<")"<<endl;
			*/
			} // if(tempDist < smallDist)
		}  // j
		groupObjVoxel[i].closestDistToCvoxel = smallDist; 
		groupObjVoxel[i].closestClineIndex = smallIndex;
		

	}  // i
		
/*		
		COORD<int> vv[2];
		
		vv[0].x = groupObjVoxel[i].position.x;
		vv[0].y = groupObjVoxel[i].position.y;
		vv[0].z = groupObjVoxel[i].position.z;
		
		vv[1].x = VecCline.DFieldCline[smallIndex].position.x;
		vv[1].y = VecCline.DFieldCline[smallIndex].position.y;
		vv[1].z = VecCline.DFieldCline[smallIndex].position.z;
		
		testVolume(2, vv, 2);
*/
	
	cout<<"cline cnt in distFromClosestCline(): "<<cnt<< endl;
}
#endif

#if 0
void GroupObjVoxel::DFieldFromClosestCline(DField4Clines &VecCline)
{
	int i;
	for(i=0; i<size; i++){
		
		if((groupObjVoxel[i].closestClineIndex-1 < 0) ||(groupObjVoxel[i].closestClineIndex+1 >= VecCline.cline_size)){
			
			groupObjVoxel[i].DField.x = VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex].DField.x;
			groupObjVoxel[i].DField.y = VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex].DField.y;
			groupObjVoxel[i].DField.z = VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex].DField.z;
			
		}
		else{
		groupObjVoxel[i].DField.x =  0.25*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex-1].DField.x
				                          + 0.5*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex].DField.x
							    + 0.25*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+1].DField.x;
		
		groupObjVoxel[i].DField.y =  0.25*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex-1].DField.y
				                          + 0.5*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex].DField.y
							    + 0.25*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+1].DField.y;
		
		groupObjVoxel[i].DField.z =  0.25*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex-1].DField.z
				                          + 0.5*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex].DField.z
							    + 0.25*VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+1].DField.z;
	       }  // else
	} // for - i
}
#endif

#if 0
void GroupObjVoxel::DFieldFromClosestCline(DField4Clines &VecCline)
{
	int i,j, k;
	float Tab[9] = {9, 10, 11, 13, 14, 13, 11, 10, 9};
	float fullSumTab = 100;
	float halfSumTab = 57;
	
	float tempX, tempY, tempZ;
	float tempXX, tempYY, tempZZ;
	
	cout << "start of DFieldFromClosestCline()       size="<< size<<endl;
	
	for(i=0; i<size; i++){
		
		if(groupObjVoxel[i].mark == MARK_ON) continue;
		
		tempX = 0.0;	
		tempY = 0.0;
		tempZ = 0.0;
		
		if(groupObjVoxel[i].closestClineIndex-1 < 0){
			
			for(j=0;j<5;j++){
				 k= j+4;
				 tempX += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i+j].closestClineIndex].DField.x );
				 tempY += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i+j].closestClineIndex].DField.y );	
				 tempZ += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i+j].closestClineIndex].DField.z );	
	              }  // for
			
				groupObjVoxel[i].DField.x = tempX/halfSumTab;
				groupObjVoxel[i].DField.y = tempY/halfSumTab;
				groupObjVoxel[i].DField.z = tempZ/halfSumTab;
			
		}
		else if(groupObjVoxel[i].closestClineIndex+1 >= VecCline.cline_size){
			
			for(j=0;j<5;j++){
				 k= j-4;
				 tempX += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i+k].closestClineIndex].DField.x );
				 tempY += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i+k].closestClineIndex].DField.y );	
				 tempZ += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i+k].closestClineIndex].DField.z );	
	              }  // for
			
				groupObjVoxel[i].DField.x = tempX/halfSumTab;
				groupObjVoxel[i].DField.y = tempY/halfSumTab;
				groupObjVoxel[i].DField.z = tempZ/halfSumTab;			
			
		}
		else{
			for(j=0;j<9;j++){
				 k= j-4;
				 tempX += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i+k].closestClineIndex].DField.x );
				 tempY += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i+k].closestClineIndex].DField.y );	
				 tempZ += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i+k].closestClineIndex].DField.z );	
	              }  // for
				groupObjVoxel[i].DField.x = tempX/fullSumTab;
				groupObjVoxel[i].DField.y = tempY/fullSumTab;
				groupObjVoxel[i].DField.z = tempZ/fullSumTab;
	       }  // else
		
#if 0		
		tempXX = (float)groupObjVoxel[i].position.x + groupObjVoxel[i].DField.x;
		tempYY = (float)groupObjVoxel[i].position.y + groupObjVoxel[i].DField.y;
		tempZZ = (float)groupObjVoxel[i].position.z + groupObjVoxel[i].DField.z;
		
		if((tempXX>=101 && tempXX <=102) && (tempYY>=71 && tempYY <=72) &&(tempZZ>=146 && tempZZ<=147)) 
			cout <<"i"<< i<<" position=("<< groupObjVoxel[i].position.x<<", "<<groupObjVoxel[i].position.y<<
				", "<<groupObjVoxel[i].position.z <<") vector=("<<groupObjVoxel[i].DField.x<<", "<< groupObjVoxel[i].DField.y<<", "<<
				groupObjVoxel[i].DField.z<<")  target position= ("<<tempXX<<", "<<tempYY<<", "<<tempZZ<<")"<<endl;
#endif		
		
	groupObjVoxel[i].mark = MARK_ON;
	//if(i%100 ==0) cout <<"i="<<i<<" (total size: "<<size<<")"<<endl;
	
	} // for - i
}
#endif

void GroupObjVoxel::DFieldFromClosestCline2(DField4Clines &VecCline)
{
	int i,j, k;
	float Tab[9] = {9, 10, 11, 13, 14, 13, 11, 10, 9};
	float fullSumTab = 100;
	float halfSumTab = 57;
	
	float tempX, tempY, tempZ;
	float tempXX, tempYY, tempZZ;
	
	cout << "start of DFieldFromClosestCline()       size="<< size<<endl;
	
	for(i=0; i<size; i++){
		
		if(groupObjVoxel[i].mark == MARK_ON) continue;
		
		tempX = 0.0;	
		tempY = 0.0;
		tempZ = 0.0;
		
		 if(groupObjVoxel[i].whichClosestCline == _TARGET_CLINE_){
			 if((groupObjVoxel[i].closestClineIndex-1 < 0)||(groupObjVoxel[i].closestClineIndex-2 < 0)||
				(groupObjVoxel[i].closestClineIndex-3 < 0)||(groupObjVoxel[i].closestClineIndex-4 < 0)){
				 
				 for(j=0;j<5;j++){
					 k= j+4;
					 
					 tempX += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.x );
					 tempY += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.y );	
					 tempZ += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.z );	
					 
				 }  // for
				 
				 groupObjVoxel[i].DField.x = tempX/halfSumTab;
				 groupObjVoxel[i].DField.y = tempY/halfSumTab;
				 groupObjVoxel[i].DField.z = tempZ/halfSumTab;
				 
#ifdef _Check_BigDField_	
				 if((groupObjVoxel[i].DField.x > 30) || (groupObjVoxel[i].DField.y > 30)  || (groupObjVoxel[i].DField.z > 30)){
					 
					 cout<<"***target1: "<< groupObjVoxel[i].DField.x<<", "<<groupObjVoxel[i].DField.y<<", "<<groupObjVoxel[i].DField.z;
					 cout<<" at i="<<i<<endl;
					 
					 for(j=0;j<5;j++){
						 k= j+4;
						 cout<<"Tab="<<Tab[k] <<" DFieldX="<< VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.x<<
							 " DFieldY="<< VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.y <<
							 " DFieldZ="<<VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.z<<endl;
					 }
					 
				 }  // if - for test
#endif 
			 }
			 else if((groupObjVoxel[i].closestClineIndex+1 >= VecCline.cline_size)||(groupObjVoxel[i].closestClineIndex+2 >= VecCline.cline_size)||
					 (groupObjVoxel[i].closestClineIndex+3 >= VecCline.cline_size)||(groupObjVoxel[i].closestClineIndex+4 >= VecCline.cline_size)){
				 
				 for(j=0;j<5;j++){
					 k= j-4;
					 tempX += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.x );
					 tempY += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.y );	
					 tempZ += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.z );	
				 }  // for
				 
				 groupObjVoxel[i].DField.x = tempX/halfSumTab;
				 groupObjVoxel[i].DField.y = tempY/halfSumTab;
				 groupObjVoxel[i].DField.z = tempZ/halfSumTab;
				 
#ifdef _Check_BigDField_	
				 if((groupObjVoxel[i].DField.x > 30) || (groupObjVoxel[i].DField.y > 30)  || (groupObjVoxel[i].DField.z > 30)){
					 
					 cout<<"***target2: "<< groupObjVoxel[i].DField.x<<", "<<groupObjVoxel[i].DField.y<<", "<<groupObjVoxel[i].DField.z;
					 cout<<" at i="<<i<<endl;
					 
					 for(j=0;j<5;j++){
						 k= j-4;
						 cout<<"Tab="<<Tab[j] <<" DFieldX="<< VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.x<<
							 " DFieldY="<< VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.y <<
							 " DFieldZ="<<VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.z<<endl;
					 }
					 
				 }  // if - for test
#endif				 
			 }
			 else{
				 for(j=0;j<9;j++){
					 k= j-4;
					 tempX += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.x );
					 tempY += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.y );	
					 tempZ += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.z );	
				 }  // for
				 groupObjVoxel[i].DField.x = tempX/fullSumTab;
				 groupObjVoxel[i].DField.y = tempY/fullSumTab;
				 groupObjVoxel[i].DField.z = tempZ/fullSumTab;
				 
#ifdef _Check_BigDField_	
				 if((groupObjVoxel[i].DField.x > 30) || (groupObjVoxel[i].DField.y > 30)  || (groupObjVoxel[i].DField.z > 30)){
					 
					 cout<<"***target3: "<< groupObjVoxel[i].DField.x<<", "<<groupObjVoxel[i].DField.y<<", "<<groupObjVoxel[i].DField.z;
					 cout<<" at i="<<i<<endl;
					 
					 for(j=0;j<9;j++){
						 k= j-4;
						 cout<<"Tab="<<Tab[j] <<" DFieldX="<< VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.x<<
							 " DFieldY="<< VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.y <<
							 " DFieldZ="<<VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.z<<endl;
					 }
					 
				 }  // if - for test
#endif				 
	       }  // else
	}  // if(groupObjVoxel[i].whichClosestCline == _TARGET_CLINE_)
	else {  //   if(groupObjVoxel[i].whichClosestCline == _SOURCE_CLINE_)
		
			 if((groupObjVoxel[i]. closestAuxClineIndex-1 < 0)||(groupObjVoxel[i]. closestAuxClineIndex-2 < 0)||
				(groupObjVoxel[i]. closestAuxClineIndex-3 < 0)||(groupObjVoxel[i]. closestAuxClineIndex-4 < 0)){
				 
				 for(j=0;j<5;j++){
					 k= j+4;
					 
					 tempX += ( Tab[k] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+j].DField.x );
					 tempY += ( Tab[k] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+j].DField.y );	
					 tempZ += ( Tab[k] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+j].DField.z );	
					 
				 }  // for
				 
				 groupObjVoxel[i].DField.x = tempX/halfSumTab;
				 groupObjVoxel[i].DField.y = tempY/halfSumTab;
				 groupObjVoxel[i].DField.z = tempZ/halfSumTab;
				 
#ifdef _Check_BigDField_	
				 if((groupObjVoxel[i].DField.x > 30) || (groupObjVoxel[i].DField.y > 30)  || (groupObjVoxel[i].DField.z > 30)){
					 
					 cout<<"###source1: "<< groupObjVoxel[i].DField.x<<", "<<groupObjVoxel[i].DField.y<<", "<<groupObjVoxel[i].DField.z;
					 cout<<" at i="<<i<<endl;
					 
					 for(j=0;j<5;j++){
						 k= j+4;
						 cout<<"Tab="<<Tab[k] <<" DFieldX="<< VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+j].DField.x<<
							 " DFieldY="<< VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+j].DField.y <<
							 " DFieldZ="<<VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+j].DField.z<<endl;
					 }
					 
				 }  // if - for test
#endif				 
				 
				 
			 }
			 else if((groupObjVoxel[i]. closestAuxClineIndex+1 >= VecCline.auxCline_size)||
					 (groupObjVoxel[i]. closestAuxClineIndex+2 >= VecCline.auxCline_size)||
					 (groupObjVoxel[i]. closestAuxClineIndex+3 >= VecCline.auxCline_size)||
					 (groupObjVoxel[i]. closestAuxClineIndex+4 >= VecCline.auxCline_size)){
				 
				 for(j=0;j<5;j++){
					 k= j-4;
					 tempX += ( Tab[j] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.x );
					 tempY += ( Tab[j] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.y );	
					 tempZ += ( Tab[j] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.z );	
				 }  // for
				 
				 groupObjVoxel[i].DField.x = tempX/halfSumTab;
				 groupObjVoxel[i].DField.y = tempY/halfSumTab;
				 groupObjVoxel[i].DField.z = tempZ/halfSumTab;	
				 
#ifdef _Check_BigDField_					 
				 if((groupObjVoxel[i].DField.x > 30) || (groupObjVoxel[i].DField.y > 30)  || (groupObjVoxel[i].DField.z > 30)){
					 
					 cout<<"###source2: "<< groupObjVoxel[i].DField.x<<", "<<groupObjVoxel[i].DField.y<<", "<<groupObjVoxel[i].DField.z;
					 cout<<" at i="<<i<<endl;
					 
					 for(j=0;j<5;j++){
						 k= j-4;
						 cout<<"Tab="<<Tab[j] <<" DFieldX="<< VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.x<<
							 " DFieldY="<< VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.y <<
							 " DFieldZ="<<VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.z<<endl;
					 }
					 
				 }  // if - for test
#endif				 
				 
				 
			 }
			 else{
				 for(j=0;j<9;j++){
					 k= j-4;
					 tempX += ( Tab[j] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.x );
					 tempY += ( Tab[j] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.y );	
					 tempZ += ( Tab[j] * VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.z );	
				 }  // for
				 groupObjVoxel[i].DField.x = tempX/fullSumTab;
				 groupObjVoxel[i].DField.y = tempY/fullSumTab;
				 groupObjVoxel[i].DField.z = tempZ/fullSumTab;
	
#ifdef _Check_BigDField_				 
				 if((groupObjVoxel[i].DField.x > 30) || (groupObjVoxel[i].DField.y > 30)  || (groupObjVoxel[i].DField.z > 30)){
					 
					 cout<<"###source3: "<< groupObjVoxel[i].DField.x<<", "<<groupObjVoxel[i].DField.y<<", "<<groupObjVoxel[i].DField.z;
					 cout<<" at i="<<i<<endl;
					 
					 for(j=0;j<9;j++){
						 k= j-4;
						 cout<<"Tab="<<Tab[j] <<" DFieldX="<< VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.x<<
							 " DFieldY="<< VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.y <<
							 " DFieldZ="<<VecCline.auxDFieldCline[groupObjVoxel[i].closestAuxClineIndex+k].DField.z;
						 cout<<"  At index = "<< groupObjVoxel[i].closestAuxClineIndex+k  << endl;
					 }
					 
				 }  // if - for test
#endif				 
				 
	       }  // else		
			 
	}
	
	
	
	
#if 0		
		tempXX = (float)groupObjVoxel[i].position.x + groupObjVoxel[i].DField.x;
		tempYY = (float)groupObjVoxel[i].position.y + groupObjVoxel[i].DField.y;
		tempZZ = (float)groupObjVoxel[i].position.z + groupObjVoxel[i].DField.z;
		
		if((tempXX>=101 && tempXX <=102) && (tempYY>=71 && tempYY <=72) &&(tempZZ>=146 && tempZZ<=147)) 
			cout <<"i"<< i<<" position=("<< groupObjVoxel[i].position.x<<", "<<groupObjVoxel[i].position.y<<
				", "<<groupObjVoxel[i].position.z <<") vector=("<<groupObjVoxel[i].DField.x<<", "<< groupObjVoxel[i].DField.y<<", "<<
				groupObjVoxel[i].DField.z<<")  target position= ("<<tempXX<<", "<<tempYY<<", "<<tempZZ<<")"<<endl;
#endif		
		
	groupObjVoxel[i].mark = MARK_ON;
	//if(i%100 ==0) cout <<"i="<<i<<" (total size: "<<size<<")"<<endl;
	
	} // for - i
}


void GroupObjVoxel::DFieldFromClosestCline(DField4Clines &VecCline)
{
	int i,j, k;
	float Tab[9] = {9, 10, 11, 13, 14, 13, 11, 10, 9};
	float fullSumTab = 100;
	float halfSumTab = 57;
	
	float tempX, tempY, tempZ;
	float tempXX, tempYY, tempZZ;
	
	cout << "start of DFieldFromClosestCline()       size="<< size<<endl;
	
	for(i=0; i<size; i++){
		
		if(groupObjVoxel[i].mark == MARK_ON) continue;
		
		tempX = 0.0;	
		tempY = 0.0;
		tempZ = 0.0;
		
		if(groupObjVoxel[i].closestClineIndex-1 < 0){
			
			for(j=0;j<5;j++){
				 k= j+4;
				 tempX += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.x );
				 tempY += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.y );	
				 tempZ += ( Tab[k] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+j].DField.z );	
	              }  // for
			
				groupObjVoxel[i].DField.x = tempX/halfSumTab;
				groupObjVoxel[i].DField.y = tempY/halfSumTab;
				groupObjVoxel[i].DField.z = tempZ/halfSumTab;
			
		}
		else if(groupObjVoxel[i].closestClineIndex+1 >= VecCline.cline_size){
			
			for(j=0;j<5;j++){
				 k= j-4;
				 tempX += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.x );
				 tempY += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.y );	
				 tempZ += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.z );	
	              }  // for
			
				groupObjVoxel[i].DField.x = tempX/halfSumTab;
				groupObjVoxel[i].DField.y = tempY/halfSumTab;
				groupObjVoxel[i].DField.z = tempZ/halfSumTab;			
			
		}
		else{
			for(j=0;j<9;j++){
				 k= j-4;
				 tempX += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.x );
				 tempY += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.y );	
				 tempZ += ( Tab[j] * VecCline.DFieldCline[groupObjVoxel[i].closestClineIndex+k].DField.z );	
	              }  // for
				groupObjVoxel[i].DField.x = tempX/fullSumTab;
				groupObjVoxel[i].DField.y = tempY/fullSumTab;
				groupObjVoxel[i].DField.z = tempZ/fullSumTab;
	       }  // else
		
#if 0		
		tempXX = (float)groupObjVoxel[i].position.x + groupObjVoxel[i].DField.x;
		tempYY = (float)groupObjVoxel[i].position.y + groupObjVoxel[i].DField.y;
		tempZZ = (float)groupObjVoxel[i].position.z + groupObjVoxel[i].DField.z;
		
		if((tempXX>=101 && tempXX <=102) && (tempYY>=71 && tempYY <=72) &&(tempZZ>=146 && tempZZ<=147)) 
			cout <<"i"<< i<<" position=("<< groupObjVoxel[i].position.x<<", "<<groupObjVoxel[i].position.y<<
				", "<<groupObjVoxel[i].position.z <<") vector=("<<groupObjVoxel[i].DField.x<<", "<< groupObjVoxel[i].DField.y<<", "<<
				groupObjVoxel[i].DField.z<<")  target position= ("<<tempXX<<", "<<tempYY<<", "<<tempZZ<<")"<<endl;
#endif		
		
	groupObjVoxel[i].mark = MARK_ON;
	//if(i%100 ==0) cout <<"i="<<i<<" (total size: "<<size<<")"<<endl;
	
	} // for - i
}


void GroupObjVoxel::smootheDField( ReaderType::Pointer reader )
{
	cout << "start DField4Clines::smootheDField"<<endl;  
	
	typedef float         PixelType;
	typedef itk::Image< PixelType, Dimension > InternalImageType;
	InternalImageType::Pointer imageX = InternalImageType::New();
	InternalImageType::Pointer imageY = InternalImageType::New();
	InternalImageType::Pointer imageZ = InternalImageType::New();
	
	InputImageType::IndexType start; 
	start[0] = 0; start[1] = 0;   start[2] = 0;
	
	InputImageType::SizeType size =  reader->GetOutput()->GetBufferedRegion().GetSize();
	 
	 InternalImageType::RegionType region; 
	  clog << " size="<<size<<" start="<<start<<endl;
	region.SetSize( size ); 
	region.SetIndex( start );
	 
	imageX->SetRegions( region );
	imageX->SetOrigin( reader->GetOutput()->GetOrigin() );
	imageX->SetSpacing( reader->GetOutput()->GetSpacing() );
	imageX->Allocate();
	
	imageY->SetRegions( region );
	imageY->SetOrigin( reader->GetOutput()->GetOrigin() );
	imageY->SetSpacing( reader->GetOutput()->GetSpacing() );
	imageY->Allocate();
	
	imageZ->SetRegions( region );
	imageZ->SetOrigin( reader->GetOutput()->GetOrigin() );
	imageZ->SetSpacing( reader->GetOutput()->GetSpacing() );
	imageZ->Allocate();	
	
	typedef itk::ImageRegionIterator< InternalImageType> ImageIteratorType;
	ImageIteratorType itX(imageX, imageX->GetRequestedRegion() );
	ImageIteratorType itY(imageY, imageY->GetRequestedRegion() );
	ImageIteratorType itZ(imageZ, imageZ->GetRequestedRegion() );
	
	int vol=0;
	itX.GoToBegin();	itY.GoToBegin();	itZ.GoToBegin();
		
	for(int k=0; k < size[2]; k++){ 	
	for(int j=0; j < size[1]; j++){
	for(int i=0; i < size[0]; i++){
			
		
		itX.Set( groupObjVoxel[vol].DField.x );
		itY.Set( groupObjVoxel[vol].DField.y );
		itZ.Set( groupObjVoxel[vol].DField.z );
		++itX;	++itY;	++itZ;
		++vol;
	}
	}
	}
	
      typedef itk::DiscreteGaussianImageFilter< InternalImageType, InternalImageType > SmoothingFilterType;
      SmoothingFilterType::Pointer imageSmoothFilterX = SmoothingFilterType::New();
      SmoothingFilterType::Pointer imageSmoothFilterY = SmoothingFilterType::New();
      SmoothingFilterType::Pointer imageSmoothFilterZ = SmoothingFilterType::New();
    
      imageSmoothFilterX->SetInput(imageX);
      imageSmoothFilterX->SetVariance( 10.0 ); 
      imageSmoothFilterX->SetMaximumKernelWidth( 15 );
      imageSmoothFilterX->Update();
	  
      imageSmoothFilterY->SetInput(imageY);
      imageSmoothFilterY->SetVariance( 10.0 ); 
      imageSmoothFilterY->SetMaximumKernelWidth( 15 );
      imageSmoothFilterY->Update();
	 
      imageSmoothFilterZ->SetInput(imageZ);
      imageSmoothFilterZ->SetVariance( 10.0 ); 
      imageSmoothFilterZ->SetMaximumKernelWidth( 15 );
      imageSmoothFilterZ->Update();
	
	ImageIteratorType sitX(imageSmoothFilterX->GetOutput(),  imageSmoothFilterX->GetOutput()->GetRequestedRegion() );
	ImageIteratorType sitY(imageSmoothFilterY->GetOutput(),  imageSmoothFilterY->GetOutput()->GetRequestedRegion() );
	ImageIteratorType sitZ(imageSmoothFilterZ->GetOutput(),  imageSmoothFilterZ->GetOutput()->GetRequestedRegion() );  
	  
	sitX.GoToBegin();	sitY.GoToBegin();	sitZ.GoToBegin();
	vol = 0;
	
	for(int k=0; k < size[2]; k++){ 	
	for(int j=0; j < size[1]; j++){
	for(int i=0; i < size[0]; i++){
			
		
		groupObjVoxel[vol].DField.x =sitX.Get( );
		groupObjVoxel[vol].DField.y =sitY.Get( );
		groupObjVoxel[vol].DField.z =sitZ.Get( );
		++sitX;	++sitY;	++sitZ;
		++vol;
	}
	}
	}	
	  
	  
}


void  GroupObjVoxel::storeVolumeDField(ReaderType::Pointer reader, string &VolumeVectorFile)
{
	cout << "start DField4Clines::storeClineDField"<<endl;  
	typedef itk::Vector< float, Dimension >  VectorType;
	  typedef itk::Image< VectorType, Dimension >  DeformationFieldType;
	  
	  typedef float         PixelType; 
	  typedef itk::Image< PixelType, Dimension > ImageType;
		
	  ImageType::IndexType start; 
	  start[0] = 0; start[1] = 0;   start[2] = 0;
	  ImageType::SizeType Rsize = reader->GetOutput()->GetBufferedRegion().GetSize();
	  ImageType::RegionType region; 
	  clog << " Rsize="<<Rsize<<" start="<<start<<endl;
	  region.SetSize( Rsize ); 
	  region.SetIndex( start );
   
	  DeformationFieldType::Pointer field = DeformationFieldType::New();
	  field->SetRegions( region );
	  field->SetOrigin( reader->GetOutput()->GetOrigin() );
	  field->SetSpacing( reader->GetOutput()->GetSpacing() );
	  field->Allocate();
	
	  TNT::Array3D<float> VecX(Rsize[0], Rsize[1], Rsize[2], (float)0); 
	  TNT::Array3D<float> VecY(Rsize[0], Rsize[1], Rsize[2], (float)0); 
	  TNT::Array3D<float> VecZ(Rsize[0], Rsize[1], Rsize[2], (float)0); 
	 
	int i,j,k;
	
        for(i=0; i<size; i++){ 
	 	
		VecX[groupObjVoxel[i].position.x][groupObjVoxel[i].position.y][groupObjVoxel[i].position.z] = groupObjVoxel[i].DField.x;
		VecY[groupObjVoxel[i].position.x][groupObjVoxel[i].position.y][groupObjVoxel[i].position.z] = groupObjVoxel[i].DField.y;
		VecZ[groupObjVoxel[i].position.x][groupObjVoxel[i].position.y][groupObjVoxel[i].position.z] = groupObjVoxel[i].DField.z;
		
	}
	
	  typedef itk::ImageRegionIterator< DeformationFieldType > FieldIterator;
	  FieldIterator fit( field, field->GetRequestedRegion() );

	  VectorType displacement, test;
	  fit.GoToBegin();
	for(k=0; k < Rsize[2]; k++){
		for(j=0; j < Rsize[1]; j++){
			for(i=0; i < Rsize[0]; i++){
				displacement[0] = VecX[i][j][k];
				displacement[1] = VecY[i][j][k];
				displacement[2] = VecZ[i][j][k];
				fit.Set( displacement );
				
				test = fit.Get();
#if 0				
			if(VecX[i][j][k] != 0){ 
				cout<<"displacement ("<<displacement[0] <<","<<displacement[1]<<","<<displacement[2]<<")"<<"at ("<<i<<","<<j<<","<<k<<")"<<endl;  
				cout<<"test ("<<test[0] <<","<<test[1]<<","<<test[2]<<")"<<"at ("<<i<<","<<j<<","<<k<<")"<<endl;  				
				}
#endif
				++fit;
	      //cout<<"i="<<i<<endl;
			}
		}
	}
	typedef itk::ImageFileWriter< DeformationFieldType >  DFieldWriterType;
	DFieldWriterType::Pointer writer = DFieldWriterType::New();

	writer->SetFileName( VolumeVectorFile.c_str() );
	writer->SetInput(  field  );
	
	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject & exp )
	{
		std::cerr << "Exception caught ! in the storeImage()" << std::endl;
		std::cerr << exp << std::endl;
	}
	
	cout << "End of GroupObjVoxel::storeVolumeDField"<<endl;  
}

void DField4Clines::computeNeighborClineDFieldUsingLinerInterpolation(int LowFeaturedIndex, int HighFeaturedIndex,  int currentIndex)
{
	int m, n, m_n;
	m = currentIndex - LowFeaturedIndex;
	n = HighFeaturedIndex - currentIndex;
	m_n = m+n;
	
	DFieldCline[currentIndex].DField.x =(float)( (n * DFieldCline[LowFeaturedIndex].DField.x) +(m * DFieldCline[HighFeaturedIndex].DField.x))/m_n;
	DFieldCline[currentIndex].DField.y =(float)( (n * DFieldCline[LowFeaturedIndex].DField.y) +(m * DFieldCline[HighFeaturedIndex].DField.y))/m_n;
	DFieldCline[currentIndex].DField.z =(float)( (n * DFieldCline[LowFeaturedIndex].DField.z) +(m * DFieldCline[HighFeaturedIndex].DField.z))/m_n;
	
}

void DField4Clines::computeNeighborClineAuxDFieldUsingLinerInterpolation(int LowFeaturedIndex, int HighFeaturedIndex,  int currentIndex)
{
	int m, n, m_n;
	m = currentIndex - LowFeaturedIndex;
	n = HighFeaturedIndex - currentIndex;
	m_n = m+n;
	
	auxDFieldCline[currentIndex].DField.x =(float)( (n * auxDFieldCline[LowFeaturedIndex].DField.x) +(m * auxDFieldCline[HighFeaturedIndex].DField.x))/m_n;
	auxDFieldCline[currentIndex].DField.y =(float)( (n * auxDFieldCline[LowFeaturedIndex].DField.y) +(m * auxDFieldCline[HighFeaturedIndex].DField.y))/m_n;
	auxDFieldCline[currentIndex].DField.z =(float)( (n * auxDFieldCline[LowFeaturedIndex].DField.z) +(m * auxDFieldCline[HighFeaturedIndex].DField.z))/m_n;
	
}


void DField4Clines::computeNeighborClineDField(float sigmaSquare, float dist, int currentIndex, int baseIndex)
{
	float ddist, withinExp, valueExp;
	ddist = dist * dist;
	withinExp = (-1) * dist* dist / (sigmaSquare * 2) ;
	valueExp = exp(withinExp);
	DFieldCline[currentIndex].DField.x = valueExp * DFieldCline[baseIndex].DField.x ;
	DFieldCline[currentIndex].DField.y = valueExp * DFieldCline[baseIndex].DField.y ;
	DFieldCline[currentIndex].DField.z = valueExp * DFieldCline[baseIndex].DField.z ;
	
#if 0	
	cout<< "i: "<<currentIndex<<" dist="<<dist<<" withinExp="<<withinExp<<" Exp="<<valueExp<<" Dx="<<DFieldCline[currentIndex].DField.x <<
		" Dy="<<DFieldCline[currentIndex].DField.y<<" Dz="<<DFieldCline[currentIndex].DField.z <<" at ("<<
		DFieldCline[currentIndex].position.x<<", "<< DFieldCline[currentIndex].position.y<<", "<< DFieldCline[currentIndex].position.z
		<<")"<<endl;
#endif	
}

void DField4Clines::computeNeighborClineAuxDField(float sigmaSquare, float dist, int currentIndex, int baseIndex)
{
	float ddist, withinExp, valueExp;
	ddist = dist * dist;
	withinExp = (-1) * dist* dist / (sigmaSquare * 2) ;
	valueExp = exp(withinExp);
	auxDFieldCline[currentIndex].DField.x = valueExp * auxDFieldCline[baseIndex].DField.x ;
	auxDFieldCline[currentIndex].DField.y = valueExp * auxDFieldCline[baseIndex].DField.y ;
	auxDFieldCline[currentIndex].DField.z = valueExp * auxDFieldCline[baseIndex].DField.z ;
	
#if 0	
	cout<< "i: "<<currentIndex<<" dist="<<dist<<" withinExp="<<withinExp<<" Exp="<<valueExp<<" Dx="<<DFieldCline[currentIndex].DField.x <<
		" Dy="<<DFieldCline[currentIndex].DField.y<<" Dz="<<DFieldCline[currentIndex].DField.z <<" at ("<<
		DFieldCline[currentIndex].position.x<<", "<< DFieldCline[currentIndex].position.y<<", "<< DFieldCline[currentIndex].position.z
		<<")"<<endl;
#endif	
}


#if 1
void DField4Clines::fillFeaturedCline2(int validCnt, int win_size, int ClineSize, int AuxClineSize, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair, IndexPair* mIndexPair)
{
	featurePair_size = validCnt;
	window_size = win_size;
	cline_size = ClineSize;
	auxCline_size = AuxClineSize;                          
	DFieldCline = new DField4ClineVoxel[cline_size];
	auxDFieldCline = new DField4ClineVoxel[AuxClineSize];
	
	int i,j;
	
	// fill the whole src centerline first.
	for(i=0; i<cline_size; i++){
		
		DFieldCline[i].position = cT.GetVoxelCoord(i);
		DFieldCline[i].nFeatured = NON_FEATURED_POINT	;
		DFieldCline[i].lDistanceFromFeaturePoint = NON_DISTANCE_SETTING; 
		DFieldCline[i].DField.x = 0;
		DFieldCline[i].DField.y = 0;
		DFieldCline[i].DField.z = 0;
	}
	

	for(i=0; i<AuxClineSize ; i++){
		auxDFieldCline[i].position = cS.GetVoxelCoord(i);
		auxDFieldCline[i].nFeatured = NON_FEATURED_POINT	;
		auxDFieldCline[i].lDistanceFromFeaturePoint = NON_DISTANCE_SETTING; 
		auxDFieldCline[i].DField.x = 0;
		auxDFieldCline[i].DField.y = 0;
		auxDFieldCline[i].DField.z = 0;
		
	}

	// fill the featured region of the centerline
	for(i=0; i<featurePair_size; i++){
		for(j=0; j<window_size; j++){
			
			DFieldCline[mIndexPair[i].SavedL_Index+j].position = matchedPair[i].trg[j];
			DFieldCline[mIndexPair[i].SavedL_Index+j].nFeatured = FEATURED_POINT;
			DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x = (float)(matchedPair[i].src[j].x - matchedPair[i].trg[j].x);
			DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y = (float)(matchedPair[i].src[j].y - matchedPair[i].trg[j].y);
			DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z = (float)(matchedPair[i].src[j].z - matchedPair[i].trg[j].z);
			
			auxDFieldCline[mIndexPair[i].SavedS_Index+j].position = matchedPair[i].src[j];   ///   ->   ? 
			auxDFieldCline[mIndexPair[i].SavedS_Index+j].nFeatured = FEATURED_POINT;
			auxDFieldCline[mIndexPair[i].SavedS_Index+j].DField.x = DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x;
			auxDFieldCline[mIndexPair[i].SavedS_Index+j].DField.y = DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y;
			auxDFieldCline[mIndexPair[i].SavedS_Index+j].DField.z = DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z;	
			
#if 1		
			if(j==0){ 
				cout << "Index:"<< mIndexPair[i].SavedL_Index<< endl;
				cout << "DFieldCline position=("<< DFieldCline[mIndexPair[i].SavedL_Index+j].position.x<<", "<<
				DFieldCline[mIndexPair[i].SavedL_Index+j].position.y<<", "<<DFieldCline[mIndexPair[i].SavedL_Index+j].position.z<<")"<<endl;
				
				
				cout<<" source: ("<<matchedPair[i].src[j].x<<", "<<matchedPair[i].src[j].y<<", "<<matchedPair[i].src[j].z<<")   target: ("<<
					matchedPair[i].trg[j].x<<", "<<matchedPair[i].trg[j].y<<", "<<matchedPair[i].trg[j].z<<")"<<endl;
				
				cout << "DFieldCline DField=("<< DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x<<", "<<
				DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y<<", "<<DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z<<")"<<endl;
				cout << "Featured: "<<DFieldCline[mIndexPair[i].SavedL_Index+j].nFeatured <<endl<<endl<<endl;
				
			}  // if(j==0)
#endif
		}  // j-window_size		
	} // i-featurePair_size
	
	
	// fill the non-featured region of the centerline based on the featured region.

	int LowFeaturedIndex=0, HighFeaturedIndex=0, numInbetweenIndex=0;
	int auxLowFeaturedIndex=0, auxHighFeaturedIndex=0, auxnumInbetweenIndex=0;
//	float sigma = 0.4082;  // exp(-3) = 0.0183  => ||x - z||=1/4  => 0.8290DF  (83%),      ||x - z||=3/4  => 0.1849DF  (18%)
	float sigma = 0.3536;  // exp(-4) = 0.0183  => ||x - z||=1/4  => 0.7789DF  (77%),      ||x - z||=3/4  => 0.1055DF  (11%)
         //	sigma = 0.3161;  // exp(-5) = 0.0067  => ||x - z||=1/4  => 0.7314DF  (73%),      ||x - z||=3/4  => 0.0599DF   (6%)
	float sigmaSquare = sigma * sigma;
	float relativeDist = 0.0, auxrelativeDist = 0.0;
	int k;
	
	for(j=0; j<featurePair_size; j++){
#if 0		
			 
				cout << "Index:"<< mIndexPair[j].SavedL_Index<< endl;
				cout << "DFieldCline position=("<< DFieldCline[mIndexPair[j].SavedL_Index].position.x<<", "<<
				DFieldCline[mIndexPair[j].SavedL_Index].position.y<<", "<<DFieldCline[mIndexPair[j].SavedL_Index].position.z<<")"<<endl;
				cout << "DFieldCline DField=("<< DFieldCline[mIndexPair[j].SavedL_Index].DField.x<<", "<<
				DFieldCline[mIndexPair[j].SavedL_Index].DField.y<<", "<<DFieldCline[mIndexPair[j].SavedL_Index].DField.z<<")"<<endl;
				cout << "Featured: "<<DFieldCline[mIndexPair[j].SavedL_Index].nFeatured <<endl;
				
			
#endif	
		if(j==0) {
			//                                  First featured region                  
			//  0---------------------------|ooooooooooooooooooooo|----
			//  ^                           ^                     
			//  |                           |                     
			//  Index0             HighFeaturedIndex
			HighFeaturedIndex = mIndexPair[j].SavedL_Index;
			auxHighFeaturedIndex = mIndexPair[j].SavedS_Index;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
	
			if( HighFeaturedIndex !=0){
		
				numInbetweenIndex = HighFeaturedIndex+1;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				for(i=0; i<numInbetweenIndex; i++){
					relativeDist = (float)(HighFeaturedIndex - i)/numInbetweenIndex ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, HighFeaturedIndex);
			
				}// for =i
			} // if( HighFeaturedIndex !=0)
			
			
			// when j==0,  one more calculation is needed.
			
			//            featured region1                                  featured region2
			// ----|ooooooooooooooooooo|------------------------|ooooooooooooooooooooo|----
			//                                     ^                         ^
			//                                     |                         |
			//                       LowFeaturedIndex     HighFeaturedIndex
			LowFeaturedIndex = mIndexPair[j].SavedL_Index + window_size -1;
			HighFeaturedIndex = mIndexPair[j+1].SavedL_Index;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
			
			if( LowFeaturedIndex != HighFeaturedIndex){
		
				for(i=LowFeaturedIndex; i<=HighFeaturedIndex; i++){
					
					computeNeighborClineDFieldUsingLinerInterpolation( LowFeaturedIndex, HighFeaturedIndex, i);
					
				}
/*				
				numInbetweenIndex =HighFeaturedIndex - LowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				k=0;
				for(i=LowFeaturedIndex; i<=((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex); i++){
					relativeDist = (float)(i - LowFeaturedIndex)/floor((float)numInbetweenIndex/2) ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, LowFeaturedIndex);
					//cout <<"   k="<<k<<endl;   k++;
				}// for -i : Low region
				
				//cout << "    Low - High";   k=0;
				//cout<<"floor="<<((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex)  <<" ceil="<< ((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex)<<endl;
				
				for(i=((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex); i<HighFeaturedIndex; i++){
					relativeDist = (float)(HighFeaturedIndex - i)/ceil((float)numInbetweenIndex/2);
					computeNeighborClineDField(sigmaSquare, relativeDist, i, HighFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : High region				
*/				
			} // if((LowFeaturedIndex != HighFeaturedIndex)								
			
			/////////////////////////////////////////////////////////////////////////////////
			////////////////////  added for auxDField    //////////////////////////
			///////////////////////////////////////////////////////////////////////////////// 
#if 1			
			if( auxHighFeaturedIndex !=0){
		
				auxnumInbetweenIndex = auxHighFeaturedIndex+1;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				for(i=0; i<auxnumInbetweenIndex; i++){
					auxrelativeDist = (float)(auxHighFeaturedIndex - i)/auxnumInbetweenIndex ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineAuxDField(sigmaSquare, auxrelativeDist, i, auxHighFeaturedIndex);
			
				}// for =i
			} // if( HighFeaturedIndex !=0)
			
			
			// when j==0,  one more calculation is needed.
			
			//            featured region1                                  featured region2
			// ----|ooooooooooooooooooo|------------------------|ooooooooooooooooooooo|----
			//                                     ^                         ^
			//                                     |                         |
			//                       LowFeaturedIndex     HighFeaturedIndex
			auxLowFeaturedIndex = mIndexPair[j].SavedS_Index + window_size -1;
			auxHighFeaturedIndex = mIndexPair[j+1].SavedS_Index;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
			
			if( auxLowFeaturedIndex != auxHighFeaturedIndex){
				
				for(i=auxLowFeaturedIndex; i<=auxHighFeaturedIndex; i++){
					
					computeNeighborClineAuxDFieldUsingLinerInterpolation( auxLowFeaturedIndex, auxHighFeaturedIndex, i);
					
				}		
/*				auxnumInbetweenIndex =auxHighFeaturedIndex - auxLowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				k=0;
				for(i=auxLowFeaturedIndex; i<=((int)floor((float)auxnumInbetweenIndex/2)+auxLowFeaturedIndex); i++){
					auxrelativeDist = (float)(i - auxLowFeaturedIndex)/floor((float)auxnumInbetweenIndex/2) ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineAuxDField(sigmaSquare, auxrelativeDist, i, auxLowFeaturedIndex);
					//cout <<"   k="<<k<<endl;   k++;
				}// for -i : Low region
				
				//cout << "    Low - High";   k=0;
				//cout<<"floor="<<((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex)  <<" ceil="<< ((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex)<<endl;
				
				for(i=((int)ceil((float)auxnumInbetweenIndex/2)+auxLowFeaturedIndex); i<auxHighFeaturedIndex; i++){
					auxrelativeDist = (float)(auxHighFeaturedIndex - i)/ceil((float)auxnumInbetweenIndex/2);
					computeNeighborClineAuxDField(sigmaSquare, auxrelativeDist, i, auxHighFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : High region				
*/				
			} // if((auxLowFeaturedIndex != auxHighFeaturedIndex)								
#endif	
			
			
			
	       }//j ==0		
#if 1
		else if(j == featurePair_size-1){
			//              Last featured region
			// --------|ooooooooooooooooooooo|-----------------------------E
			//                                             ^                             ^
			//                                             |                             |
			//                                 LowFeaturedIndex       Last Index
			LowFeaturedIndex = mIndexPair[j].SavedL_Index + window_size -1;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			
			if( LowFeaturedIndex !=ClineSize-1){
		
				numInbetweenIndex =ClineSize - LowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				for(i=LowFeaturedIndex; i<(numInbetweenIndex+LowFeaturedIndex); i++){
					relativeDist = (float)(i - LowFeaturedIndex)/numInbetweenIndex ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, LowFeaturedIndex);
			
				}// for =i
			} // if((LowFeaturedIndex !=ClineSize-1)	
			
			/////////////////////////////////////////////////////////////////////////////////
			////////////////////  added for auxDField    //////////////////////////
			/////////////////////////////////////////////////////////////////////////////////
#if 1		
			auxLowFeaturedIndex = mIndexPair[j].SavedS_Index + window_size -1;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			
			if( auxLowFeaturedIndex !=AuxClineSize-1){
		
				auxnumInbetweenIndex =AuxClineSize - auxLowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				for(i=auxLowFeaturedIndex; i<(auxnumInbetweenIndex+auxLowFeaturedIndex); i++){
					auxrelativeDist = (float)(i - auxLowFeaturedIndex)/auxnumInbetweenIndex ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineAuxDField(sigmaSquare, auxrelativeDist, i, auxLowFeaturedIndex);
			
				}// for =i
			} // if((auxLowFeaturedIndex !=ClineSize-1)			
#endif			
		}  // else if(j == featurePair_size-1)
		else{    
			//            featured region1                                  featured region2
			// ----|ooooooooooooooooooo|------------------------|ooooooooooooooooooooo|----
			//                                     ^                         ^
			//                                     |                         |
			//                       LowFeaturedIndex     HighFeaturedIndex
			LowFeaturedIndex = mIndexPair[j].SavedL_Index + window_size -1;
			HighFeaturedIndex = mIndexPair[j+1].SavedL_Index;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
			
			if( LowFeaturedIndex != HighFeaturedIndex){
				
				for(i=LowFeaturedIndex; i<=HighFeaturedIndex; i++){
					
					computeNeighborClineDFieldUsingLinerInterpolation( LowFeaturedIndex, HighFeaturedIndex, i);
					
				}				
/*				
				numInbetweenIndex =HighFeaturedIndex - LowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				k=0;
				for(i=LowFeaturedIndex; i<=((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex); i++){
					relativeDist = (float)(i - LowFeaturedIndex)/floor((float)numInbetweenIndex/2) ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, LowFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : Low region
				
				//cout << "    Low - High";   k=0;
				//cout<<"floor="<<((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex)  <<" ceil="<< ((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex)<<endl;
				
				for(i=((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex); i<HighFeaturedIndex; i++){
					relativeDist = (float)(HighFeaturedIndex - i)/ceil((float)numInbetweenIndex/2);
					computeNeighborClineDField(sigmaSquare, relativeDist, i, HighFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : High region				
*/				
			} // if((LowFeaturedIndex != HighFeaturedIndex)		
			
			/////////////////////////////////////////////////////////////////////////////////
			////////////////////  added for auxDField    //////////////////////////
			/////////////////////////////////////////////////////////////////////////////////
#if 1		
			auxLowFeaturedIndex = mIndexPair[j].SavedS_Index + window_size -1;
			auxHighFeaturedIndex = mIndexPair[j+1].SavedS_Index;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
			
			if( auxLowFeaturedIndex != auxHighFeaturedIndex){
	
				for(i=auxLowFeaturedIndex; i<=auxHighFeaturedIndex; i++){
					
					computeNeighborClineAuxDFieldUsingLinerInterpolation( auxLowFeaturedIndex, auxHighFeaturedIndex, i);
					
				}				
/*				
				auxnumInbetweenIndex =auxHighFeaturedIndex - auxLowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				k=0;
				for(i=auxLowFeaturedIndex; i<=((int)floor((float)auxnumInbetweenIndex/2)+auxLowFeaturedIndex); i++){
					auxrelativeDist = (float)(i - auxLowFeaturedIndex)/floor((float)auxnumInbetweenIndex/2) ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineAuxDField(sigmaSquare, auxrelativeDist, i, auxLowFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : Low region
				
				//cout << "    Low - High";   k=0;
				//cout<<"floor="<<((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex)  <<" ceil="<< ((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex)<<endl;
				
				for(i=((int)ceil((float)auxnumInbetweenIndex/2)+auxLowFeaturedIndex); i<auxHighFeaturedIndex; i++){
					auxrelativeDist = (float)(auxHighFeaturedIndex - i)/ceil((float)auxnumInbetweenIndex/2);
					computeNeighborClineAuxDField(sigmaSquare, auxrelativeDist, i, auxHighFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : High region				
*/				
			} // if((auxLowFeaturedIndex != auxHighFeaturedIndex)				
#endif			
			
		}  // else - Inbetween regions
#endif		
	}// for - j

	
}
#endif


#if 1
void DField4Clines::fillFeaturedCline(int validCnt, int win_size, int ClineSize, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair, IndexPair* mIndexPair)
{
	featurePair_size = validCnt;
	window_size = win_size;
	cline_size = ClineSize;
	DFieldCline = new DField4ClineVoxel[cline_size];
	
	int i,j;
	
	// fill the whole src centerline first.
	for(i=0; i<cline_size; i++){
		
//		DFieldCline[i].position = cS.GetVoxelCoord(i);
		DFieldCline[i].position = cT.GetVoxelCoord(i);
		DFieldCline[i].nFeatured = NON_FEATURED_POINT	;
		DFieldCline[i].lDistanceFromFeaturePoint = NON_DISTANCE_SETTING; 
		DFieldCline[i].DField.x = 0;
		DFieldCline[i].DField.y = 0;
		DFieldCline[i].DField.z = 0;
		
	}
	
	// fill the featured region of the centerline
	for(i=0; i<featurePair_size; i++){
		for(j=0; j<window_size; j++){
			
//			DFieldCline[mIndexPair[i].SavedL_Index+j].position = matchedPair[i].src[j];
			DFieldCline[mIndexPair[i].SavedL_Index+j].position = matchedPair[i].trg[j];
			DFieldCline[mIndexPair[i].SavedL_Index+j].nFeatured = FEATURED_POINT;
			DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x = (float)(matchedPair[i].src[j].x - matchedPair[i].trg[j].x);
			DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y = (float)(matchedPair[i].src[j].y - matchedPair[i].trg[j].y);
			DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z = (float)(matchedPair[i].src[j].z - matchedPair[i].trg[j].z);
			
			//DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x = (float)(matchedPair[i].trg[j].x - matchedPair[i].src[j].x);
			//DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y = (float)(matchedPair[i].trg[j].y - matchedPair[i].src[j].y);
			//DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z = (float)(matchedPair[i].trg[j].z - matchedPair[i].src[j].z);			
#if 0			
			if(j==0){ 
				cout << "Index:"<< mIndexPair[i].SavedL_Index<< endl;
				cout << "DFieldCline position=("<< DFieldCline[mIndexPair[i].SavedL_Index+j].position.x<<", "<<
				DFieldCline[mIndexPair[i].SavedL_Index+j].position.y<<", "<<DFieldCline[mIndexPair[i].SavedL_Index+j].position.z<<")"<<endl;
				cout << "DFieldCline DField=("<< DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x<<", "<<
				DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y<<", "<<DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z<<")"<<endl;
				cout << "Featured: "<<DFieldCline[mIndexPair[i].SavedL_Index+j].nFeatured <<endl;
				
			}  // if(j==0)
#endif
		}  // j-window_size		
	} // i-featurePair_size
	
	
	// fill the non-featured region of the centerline based on the featured region.

	int LowFeaturedIndex=0, HighFeaturedIndex=0, numInbetweenIndex=0;
//	float sigma = 0.4082;  // exp(-3) = 0.0183  => ||x - z||=1/4  => 0.8290DF  (83%),      ||x - z||=3/4  => 0.1849DF  (18%)
	float sigma = 0.3536;  // exp(-4) = 0.0183  => ||x - z||=1/4  => 0.7789DF  (77%),      ||x - z||=3/4  => 0.1055DF  (11%)
         //	sigma = 0.3161;  // exp(-5) = 0.0067  => ||x - z||=1/4  => 0.7314DF  (73%),      ||x - z||=3/4  => 0.0599DF   (6%)
	float sigmaSquare = sigma * sigma;
	float relativeDist = 0.0;
	int k;
	
	for(j=0; j<featurePair_size; j++){
#if 0		
			 
				cout << "Index:"<< mIndexPair[j].SavedL_Index<< endl;
				cout << "DFieldCline position=("<< DFieldCline[mIndexPair[j].SavedL_Index].position.x<<", "<<
				DFieldCline[mIndexPair[j].SavedL_Index].position.y<<", "<<DFieldCline[mIndexPair[j].SavedL_Index].position.z<<")"<<endl;
				cout << "DFieldCline DField=("<< DFieldCline[mIndexPair[j].SavedL_Index].DField.x<<", "<<
				DFieldCline[mIndexPair[j].SavedL_Index].DField.y<<", "<<DFieldCline[mIndexPair[j].SavedL_Index].DField.z<<")"<<endl;
				cout << "Featured: "<<DFieldCline[mIndexPair[j].SavedL_Index].nFeatured <<endl;
				
			
#endif	
		if(j==0) {
			//                                  First featured region                  
			//  0---------------------------|ooooooooooooooooooooo|----
			//  ^                           ^                     
			//  |                           |                     
			//  Index0             HighFeaturedIndex
			HighFeaturedIndex = mIndexPair[j].SavedL_Index;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
	
			if( HighFeaturedIndex !=0){
		
				numInbetweenIndex = HighFeaturedIndex+1;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				for(i=0; i<numInbetweenIndex; i++){
					relativeDist = (float)(HighFeaturedIndex - i)/numInbetweenIndex ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, HighFeaturedIndex);
			
				}// for =i
			} // if( HighFeaturedIndex !=0)
			
			
			// when j==0,  one more calculation is needed.
			
			//            featured region1                                  featured region2
			// ----|ooooooooooooooooooo|------------------------|ooooooooooooooooooooo|----
			//                                     ^                         ^
			//                                     |                         |
			//                       LowFeaturedIndex     HighFeaturedIndex
			LowFeaturedIndex = mIndexPair[j].SavedL_Index + window_size -1;
			HighFeaturedIndex = mIndexPair[j+1].SavedL_Index;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
			
			if( LowFeaturedIndex != HighFeaturedIndex){
		
				numInbetweenIndex =HighFeaturedIndex - LowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				k=0;
				for(i=LowFeaturedIndex; i<=((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex); i++){
					relativeDist = (float)(i - LowFeaturedIndex)/floor((float)numInbetweenIndex/2) ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, LowFeaturedIndex);
					//cout <<"   k="<<k<<endl;   k++;
				}// for -i : Low region
				
				//cout << "    Low - High";   k=0;
				//cout<<"floor="<<((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex)  <<" ceil="<< ((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex)<<endl;
				
				for(i=((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex); i<HighFeaturedIndex; i++){
					relativeDist = (float)(HighFeaturedIndex - i)/ceil((float)numInbetweenIndex/2);
					computeNeighborClineDField(sigmaSquare, relativeDist, i, HighFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : High region				
				
			} // if((LowFeaturedIndex != HighFeaturedIndex)								
			
			
	       }//j ==0		
#if 1
		else if(j == featurePair_size-1){
			//              Last featured region
			// --------|ooooooooooooooooooooo|-----------------------------E
			//                                             ^                             ^
			//                                             |                             |
			//                                 LowFeaturedIndex       Last Index
			LowFeaturedIndex = mIndexPair[j].SavedL_Index + window_size -1;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			
			if( LowFeaturedIndex !=ClineSize-1){
		
				numInbetweenIndex =ClineSize - LowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				for(i=LowFeaturedIndex; i<(numInbetweenIndex+LowFeaturedIndex); i++){
					relativeDist = (float)(i - LowFeaturedIndex)/numInbetweenIndex ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, LowFeaturedIndex);
			
				}// for =i
			} // if((LowFeaturedIndex !=ClineSize-1)		
			
		}  // else if(j == featurePair_size-1)
		else{    
			//            featured region1                                  featured region2
			// ----|ooooooooooooooooooo|------------------------|ooooooooooooooooooooo|----
			//                                     ^                         ^
			//                                     |                         |
			//                       LowFeaturedIndex     HighFeaturedIndex
			LowFeaturedIndex = mIndexPair[j].SavedL_Index + window_size -1;
			HighFeaturedIndex = mIndexPair[j+1].SavedL_Index;
			//cout << "LowFeaturedIndex="<<  LowFeaturedIndex << endl;
			//cout << "HighFeaturedIndex="<<  HighFeaturedIndex << endl;
			
			if( LowFeaturedIndex != HighFeaturedIndex){
		
				numInbetweenIndex =HighFeaturedIndex - LowFeaturedIndex -1 ;  // cout << "numInbetweenIndex ="<<  numInbetweenIndex << endl;
		
				k=0;
				for(i=LowFeaturedIndex; i<=((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex); i++){
					relativeDist = (float)(i - LowFeaturedIndex)/floor((float)numInbetweenIndex/2) ;
					//cout << "relativeDist  ="<< relativeDist  << endl;
					computeNeighborClineDField(sigmaSquare, relativeDist, i, LowFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : Low region
				
				//cout << "    Low - High";   k=0;
				//cout<<"floor="<<((int)floor((float)numInbetweenIndex/2)+LowFeaturedIndex)  <<" ceil="<< ((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex)<<endl;
				
				for(i=((int)ceil((float)numInbetweenIndex/2)+LowFeaturedIndex); i<HighFeaturedIndex; i++){
					relativeDist = (float)(HighFeaturedIndex - i)/ceil((float)numInbetweenIndex/2);
					computeNeighborClineDField(sigmaSquare, relativeDist, i, HighFeaturedIndex);
					//cout <<"k="<<k<<endl;   k++;
				}// for -i : High region				
				
			} // if((LowFeaturedIndex != HighFeaturedIndex)				
		}  // else - Inbetween regions
#endif		
	}// for - j

	
}
#endif


void DField4Clines::storeClineDField(string &ClineVectorFile, ReaderType::Pointer reader, IndexPair* mIndexPair, int ClineSize)
{
	cout << "start DField4Clines::storeClineDField"<<endl;  
	typedef itk::Vector< float, Dimension >  VectorType;
	  typedef itk::Image< VectorType, Dimension >  DeformationFieldType;
	  
	  typedef float         PixelType; 
	  typedef itk::Image< PixelType, Dimension > ImageType;
		
	  ImageType::IndexType start; 
	  start[0] = 0; start[1] = 0;   start[2] = 0;
	  ImageType::SizeType size = reader->GetOutput()->GetBufferedRegion().GetSize();
	  ImageType::RegionType region; 
	  clog << " size="<<size<<" start="<<start<<endl;
	  region.SetSize( size ); 
	  region.SetIndex( start );
   
	  clog << " DField4Clines::storeClineDField 1 "<<endl;
	  DeformationFieldType::Pointer field = DeformationFieldType::New();      //clog << " DField4Clines::storeClineDField 2 "<<endl;
	  field->SetRegions( region );                                                                  //clog << " DField4Clines::storeClineDField 3 "<<endl;
	  field->SetOrigin( reader->GetOutput()->GetOrigin() );                              //clog << " DField4Clines::storeClineDField 4 "<<endl;
	  field->SetSpacing( reader->GetOutput()->GetSpacing() );                       // clog << " DField4Clines::storeClineDField 5 "<<endl;
	  field->Allocate();                                                                                // clog << " DField4Clines::storeClineDField 6 "<<endl;
	
	  TNT::Array3D<float> VecX(size[0], size[1], size[2], (float)0); 
	  TNT::Array3D<float> VecY(size[0], size[1], size[2], (float)0); 
	  TNT::Array3D<float> VecZ(size[0], size[1], size[2], (float)0); 
/*	  
	  TNT::Array3D<float> VecX(size[0], size[1], size[2], (float)250); 
	  TNT::Array3D<float> VecY(size[0], size[1], size[2], (float)250); 
	  TNT::Array3D<float> VecZ(size[0], size[1], size[2], (float)220); 
*/	  
	  int i,j,k;
	  
	  cout << " ====   featurePair_size= "<< featurePair_size<< endl;
	  
#if 0	  
	// Only the featured regions of the centerline are saved in VecX, VecY and VecZ here for test purpose.  
	for(i=0; i<featurePair_size; i++){
		for(j=0; j<window_size; j++){
			VecX[DFieldCline[mIndexPair[i].SavedS_Index+j].position.x][DFieldCline[mIndexPair[i].SavedS_Index+j].position.y][DFieldCline[mIndexPair[i].SavedS_Index+j].position.z]  = DFieldCline[mIndexPair[i].SavedS_Index+j].DField.x;
			VecY[DFieldCline[mIndexPair[i].SavedS_Index+j].position.x][DFieldCline[mIndexPair[i].SavedS_Index+j].position.y][DFieldCline[mIndexPair[i].SavedS_Index+j].position.z]  = DFieldCline[mIndexPair[i].SavedS_Index+j].DField.y;
			VecZ[DFieldCline[mIndexPair[i].SavedS_Index+j].position.x][DFieldCline[mIndexPair[i].SavedS_Index+j].position.y][DFieldCline[mIndexPair[i].SavedS_Index+j].position.z]  = DFieldCline[mIndexPair[i].SavedS_Index+j].DField.z;
			
			if( j==0) cout << "vec position:"<<DFieldCline[mIndexPair[i].SavedS_Index+j].position.x<<", "<<DFieldCline[mIndexPair[i].SavedS_Index+j].position.y<<", "<< DFieldCline[mIndexPair[i].SavedS_Index+j].position.z<<"= ("<<DFieldCline[mIndexPair[i].SavedS_Index+j].DField.x<<","<<DFieldCline[mIndexPair[i].SavedS_Index+j].DField.y<<","<<DFieldCline[mIndexPair[i].SavedS_Index+j].DField.z<<endl;
		}
	}
#endif

#if 0
	// Only the featured regions of the centerline are saved in VecX, VecY and VecZ here for test purpose.  
	for(i=0; i<featurePair_size; i++){
		for(j=0; j<window_size; j++){
			VecX[DFieldCline[mIndexPair[i].SavedL_Index+j].position.x][DFieldCline[mIndexPair[i].SavedL_Index+j].position.y][DFieldCline[mIndexPair[i].SavedL_Index+j].position.z]  = DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x;
			VecY[DFieldCline[mIndexPair[i].SavedL_Index+j].position.x][DFieldCline[mIndexPair[i].SavedL_Index+j].position.y][DFieldCline[mIndexPair[i].SavedL_Index+j].position.z]  = DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y;
			VecZ[DFieldCline[mIndexPair[i].SavedL_Index+j].position.x][DFieldCline[mIndexPair[i].SavedL_Index+j].position.y][DFieldCline[mIndexPair[i].SavedL_Index+j].position.z]  = DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z;
			
			if( j==0) cout << "vec position:"<<DFieldCline[mIndexPair[i].SavedL_Index+j].position.x<<", "<<DFieldCline[mIndexPair[i].SavedL_Index+j].position.y<<", "<< DFieldCline[mIndexPair[i].SavedL_Index+j].position.z<<"= ("<<DFieldCline[mIndexPair[i].SavedL_Index+j].DField.x<<","<<DFieldCline[mIndexPair[i].SavedL_Index+j].DField.y<<","<<DFieldCline[mIndexPair[i].SavedL_Index+j].DField.z<<endl;
		}
	}	
#endif
	
	FILE *fpTest;
	
	if((fpTest=fopen("CLineMVs3.txt","wt"))==NULL){
		
		cout << "CLineMVs3.txt file open error!"<<endl;
	}
	
	//cout<< "VecCline a1"<<endl;
	for(i=0; i<auxCline_size; i++){
		
		VecX[auxDFieldCline[i].position.x][auxDFieldCline[i].position.y][auxDFieldCline[i].position.z] = auxDFieldCline[i].DField.x;
		VecY[auxDFieldCline[i].position.x][auxDFieldCline[i].position.y][auxDFieldCline[i].position.z] = auxDFieldCline[i].DField.y;
		VecZ[auxDFieldCline[i].position.x][auxDFieldCline[i].position.y][auxDFieldCline[i].position.z] = auxDFieldCline[i].DField.z;
#if 0		
	       fprintf(fpTest,"%d %d %d",DFieldCline[i].position.x, DFieldCline[i].position.y, DFieldCline[i].position.z);
		fprintf(fpTest," %f %f %f %d\n", VecX[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z], 
				VecY[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z] , VecZ[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z], i);   

		fprintf(fpTest,"%d:%f %f %f in (%d, %d, %d)\n",i,VecX[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z], 
				VecY[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z] , VecZ[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z],
				DFieldCline[i].position.x, DFieldCline[i].position.y, DFieldCline[i].position.z);
#endif
	}
	
	
	for(i=0; i<ClineSize; i++){
		
		VecX[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z] = DFieldCline[i].DField.x;
		VecY[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z] = DFieldCline[i].DField.y;
		VecZ[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z] = DFieldCline[i].DField.z;
		
	       fprintf(fpTest,"%d %d %d",DFieldCline[i].position.x, DFieldCline[i].position.y, DFieldCline[i].position.z);
		fprintf(fpTest," %f %f %f %d\n", VecX[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z], 
				VecY[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z] , VecZ[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z], i);   
#if 0
		fprintf(fpTest,"%d:%f %f %f in (%d, %d, %d)\n",i,VecX[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z], 
				VecY[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z] , VecZ[DFieldCline[i].position.x][DFieldCline[i].position.y][DFieldCline[i].position.z],
				DFieldCline[i].position.x, DFieldCline[i].position.y, DFieldCline[i].position.z);
#endif
	}
	
	fclose(fpTest);
	
	//cout<< "VecCline a2"<<endl;
	
	  typedef itk::ImageRegionIterator< DeformationFieldType > FieldIterator;
	  FieldIterator fit( field, field->GetRequestedRegion() );

	  VectorType displacement, test;
	  fit.GoToBegin();                                       //cout<< "VecCline a3"<<endl;
	for(k=0; k < size[2]; k++){
		for(j=0; j < size[1]; j++){
			for(i=0; i < size[0]; i++){
				displacement[0] = VecX[i][j][k];
				displacement[1] = VecY[i][j][k];
				displacement[2] = VecZ[i][j][k];
				fit.Set( displacement );
				
				test = fit.Get();
#if 0				
			if(VecX[i][j][k] != 0){ 
				cout<<"displacement ("<<displacement[0] <<","<<displacement[1]<<","<<displacement[2]<<")"<<"at ("<<i<<","<<j<<","<<k<<")"<<endl;  
				cout<<"test ("<<test[0] <<","<<test[1]<<","<<test[2]<<")"<<"at ("<<i<<","<<j<<","<<k<<")"<<endl;  				
				}
#endif
				++fit;
	      //cout<<"i="<<i<<endl;
			}
		}
	}
	
	cout<< "VecCline a4"<<endl;
	
	typedef itk::ImageFileWriter< DeformationFieldType >  DFieldWriterType;   //clog<< "VecCline a4.5"<<endl;
	DFieldWriterType::Pointer writer = DFieldWriterType::New();                 //clog<< "VecCline a5"<<endl;
      
	writer->SetFileName( ClineVectorFile.c_str() );                                    //clog<< "VecCline a6"<<endl;
	writer->SetInput(  field  );                                                                   //clog<< "VecCline a7"<<endl;
	
	try
	{
		writer->Update();                                                                         //clog<< "VecCline a8"<<endl;
	}
	catch( itk::ExceptionObject & exp )
	{
		std::cerr << "Exception caught ! in the storeImage()" << std::endl;
		std::cerr << exp << std::endl;
	}
	
	cout << "End of DField4Clines::storeClineDField"<<endl;  
}
