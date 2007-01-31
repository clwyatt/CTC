#include "MyHeader.h"
#include "deformByCLine.h"

#include <string.h>
using namespace std;

/* GenerateCline.cxx*/
extern int generateCline(TNT::Array3D<int> Voxel, int startX, int startY, int startZ, ReaderType::Pointer reader,  TNT::Array3D<int> Cline, int num,  ClineVoxel &Cvoxel );
extern int FindFeatures(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures2(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures3(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures4(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures5(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
//extern void ReconstructFeaturedCline(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair  &matchedPair);
extern void ReconstructFeaturedCline(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair);
extern void ReconstructFeaturedClineMORE(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair);

/*  storeLandmark.cxx */
extern void storeLandmarks(const string &fileName, int validCnt,  IndexPair* mIndexPair, MatchedPair *matchedPair, int window_size);
extern void storeImage(TNT::Array3D<int> &Volume, string &fileName, int sizeX, int sizeY, int sizeZ, ReaderType::Pointer reader);

int main( int argc, char * argv[] )
{
	if( argc < 8 )
	{
		std::cerr << "Usage: " << argv[0];
		std::cerr << " SrcVolumeFile  SrcClineFileName TrgVolumeFlie TrgClineFileName SrcFeatureFile TrgFeatureFile dilate (landmarksFile)";
		
		std::cerr << std::endl;  
		return EXIT_FAILURE;
	}
	
	
	cout<< "&&&&&&&&&&&&& Is this correct?  &&&&  AAA &&&&&&&&&&"<<endl;
	int dilate;
	dilate = atoi(argv[7]); cout << "dilate =wwww" << dilate<<endl;
	
	ReaderType::Pointer SrcReader = ReaderType::New();
	ReaderType::Pointer TrgReader = ReaderType::New();
	
	SrcReader->SetFileName( argv[1] );  SrcReader->Update(); 
	TrgReader->SetFileName( argv[3] );  TrgReader->Update(); 
	
	InputImageType::RegionType::SizeType  SrcImagesize;
	InputImageType::RegionType::SizeType  TrgImagesize;
	
	SrcImagesize =  SrcReader->GetOutput()->GetRequestedRegion().GetSize();
	cout<<"Source : sizeX="<<SrcImagesize[0]<<" sizeY="<<SrcImagesize[1]<<" sizeZ="<<SrcImagesize[2]<<endl;
	
	TrgImagesize =  TrgReader->GetOutput()->GetRequestedRegion().GetSize();
	cout<<"Target : sizeX="<<TrgImagesize[0]<<" sizeY="<<TrgImagesize[1]<<" sizeZ="<<TrgImagesize[2]<<endl; 
	
	int sX, sY, sZ, tX, tY, tZ;
	sX =SrcImagesize[0];     sY = SrcImagesize[1];    sZ = SrcImagesize[2];
	tX =TrgImagesize[0];     tY = TrgImagesize[1];    tZ = TrgImagesize[2];
	
	TNT::Array3D<int> sVoxel(sX, sY, sZ, 0); 
	TNT::Array3D<int> tVoxel(tX, tY, tZ, 0); 
	
	TNT::Array3D<int> SrcCline(sX, sY, sZ, 0);
	TNT::Array3D<int> TrgCline(tX, tY, tZ, 0);
	
	int SrcStartX=0, SrcStartY=0, SrcStartZ=0, TrgStartX=0, TrgStartY=0, TrgStartZ=0;
	
	SrcStartX=122; SrcStartY=144; SrcStartZ=22;   //VC386_suHalf.vtk
	TrgStartX=122; TrgStartY=144; TrgStartZ=22;   //VC386_prHalfFlipMoved.vtk
	
	//TrgStartX=124; TrgStartY=129; TrgStartZ=16;   //VC386_prHalfFlip.vtk
	
	int SrcClineSize=0, TrgClineSize=0;
	/////////////////////////////
	ClineVoxel src, trg;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SrcClineSize = generateCline(sVoxel, SrcStartX, SrcStartY, SrcStartZ, SrcReader , SrcCline, 0, src);
	TrgClineSize = generateCline(tVoxel, TrgStartX, TrgStartY, TrgStartZ, TrgReader, TrgCline, 1, trg );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//cout<<"after generateCline()"<<endl;
		
	TNT::Array3D<int> sFea(sX, sY, sZ, 0); 
	TNT::Array3D<int> tFea(tX, tY, tZ, 0); 
	
	//// adjust m, window_size here
	//int m=15, window_size = 50;
	//float fixed_correl = 0.852;
	
	int m=20, window_size = 40; // window_size should be even number by wook
	int MORE_window_size = window_size + 30;   // More Version
	
	float fixed_correl = 0.3;
	int plusSearchRange = 100, minusSearchRange = 20;
	IndexPair mIndexPairZ[20], mIndexPairY[20];
	int validCntZ=0;
	

	validCntZ = FindFeatures5(src, trg, m, window_size, plusSearchRange, minusSearchRange, mIndexPairZ, fixed_correl);  // use Y-Z extrema respectively 	

	
	MatchedPair matchedPair[validCntZ];
	//ReconstructFeaturedClineMORE(mIndexPairZ, validCntZ, MORE_window_size , src, trg, matchedPair);
	
	std::cout <<"before DField4Clines"<<std::endl;
	DField4Clines VecCline;
	VecCline.setSizeOfCline(validCntZ);
	
	
	int current_set = 0;
	int k,j,i, value=0;
	int sx,sy,sz,tx,ty,tz;
	int ddx, ddy, ddz, dsx, dsy, dsz, dtx, dty,dtz;
	
	
	for(i=0; i<validCntZ; i++){ 
		if(current_set == 0){  
			
			cout<<"mIndexPairS"<<i<<" : "<<mIndexPairZ[i].SavedS_Index<< "w_size: "<<MORE_window_size <<" next mIndexPairS: "<<mIndexPairZ[i+1].SavedS_Index<<endl;
			cout<<"mIndexPairL"<<i<<" : "<<mIndexPairZ[i].SavedL_Index<< "w_size: "<<MORE_window_size <<" next mIndexPairL: "<<mIndexPairZ[i+1].SavedL_Index<<endl;
			
			if((i == (validCntZ-1)) ){ // Because the final pair doesn't have the next pair.
				
				for(j=0; j<MORE_window_size ; j++){
						sx = matchedPair[i].src[j].x;		tx = matchedPair[i].trg[j].x;
						sy = matchedPair[i].src[j].y;		ty = matchedPair[i].trg[j].y;
						sz = matchedPair[i].src[j].z;		tz = matchedPair[i].trg[j].z;
					
						for(ddz=-dilate; ddz<dilate+1; ddz++){
						for(ddy=-dilate; ddy<dilate+1; ddy++){
						for(ddx=-dilate; ddx<dilate+1; ddx++){
					
							dsx = ddx + sx;
							dsy = ddy + sy;
							dsz = ddz + sz;
             
							if((dsx<0)||(dsy<0)||(dsz<0)) continue;
							if((dsx>=sX)||(dsy>=sY)||(dsz>=sZ)) continue;			
					
							sFea[dsx][dsy][dsz]= 40; 
						}
					       }
					       }
					
						for(ddz=-dilate; ddz<dilate+1; ddz++){
						for(ddy=-dilate; ddy<dilate+1; ddy++){
						for(ddx=-dilate; ddx<dilate+1; ddx++){
					
							dtx = ddx + tx;
							dty = ddy + ty;
							dtz = ddz + tz;
             
							if((dtx<0)||(dty<0)||(dtz<0)) continue;
							if((dtx>=tX)||(dty>=tY)||(dtz>=tZ)) continue;			
					
							tFea[dtx][dty][dtz]= 40; 
						}
					       }
				              }	
				}// for - j	
				
			}   // if((i == (validCntZ-1)) )
			else if(  ((mIndexPairZ[i].SavedS_Index + MORE_window_size )<mIndexPairZ[i+1].SavedS_Index) &&
			   ( (mIndexPairZ[i].SavedL_Index + MORE_window_size )<mIndexPairZ[i+1].SavedL_Index)  ){ 	 
		   
				current_set = 0;
				for(j=0; j<MORE_window_size ; j++){
					
					sx = matchedPair[i].src[j].x;		tx = matchedPair[i].trg[j].x;
					sy = matchedPair[i].src[j].y;		ty = matchedPair[i].trg[j].y;
					sz = matchedPair[i].src[j].z;		tz = matchedPair[i].trg[j].z;
					
					for(ddz=-dilate; ddz<dilate+1; ddz++){
					for(ddy=-dilate; ddy<dilate+1; ddy++){
					for(ddx=-dilate; ddx<dilate+1; ddx++){
					
						dsx = ddx + sx;
						dsy = ddy + sy;
						dsz = ddz + sz;
             
						if((dsx<0)||(dsy<0)||(dsz<0)) continue;
						if((dsx>=sX)||(dsy>=sY)||(dsz>=sZ)) continue;			
					       
						//cout<<"dsx:"<<dsx<<" dsy:"<<dsy<<" dsz:"<<dsz<<endl;
						sFea[dsx][dsy][dsz]= 40; 
					}
				       }
				       }
					
					for(ddz=-dilate; ddz<dilate+1; ddz++){
					for(ddy=-dilate; ddy<dilate+1; ddy++){
					for(ddx=-dilate; ddx<dilate+1; ddx++){
					
						dtx = ddx + tx;
						dty = ddy + ty;
						dtz = ddz + tz;
             
						if((dtx<0)||(dty<0)||(dtz<0)) continue;
						if((dtx>=tX)||(dty>=tY)||(dtz>=tZ)) continue;			
					
						tFea[dtx][dty][dtz]= 40; 
					}
				       }
				       }														
					
				}  // for - j		
				cout <<i<< " indexS= "<<mIndexPairZ[i].SavedS_Index<<" , indexL= "<<mIndexPairZ[i].SavedL_Index<<endl;
			 }
			else { 
				if(mIndexPairZ[i].SavedCorrel > mIndexPairZ[i+1].SavedCorrel){
					current_set = 1;
					
					for(j=0; j<MORE_window_size ; j++){
						sx = matchedPair[i].src[j].x;		tx = matchedPair[i].trg[j].x;
						sy = matchedPair[i].src[j].y;		ty = matchedPair[i].trg[j].y;
						sz = matchedPair[i].src[j].z;		tz = matchedPair[i].trg[j].z;
					
						for(ddz=-dilate; ddz<dilate+1; ddz++){
						for(ddy=-dilate; ddy<dilate+1; ddy++){
						for(ddx=-dilate; ddx<dilate+1; ddx++){
					
							dsx = ddx + sx;
							dsy = ddy + sy;
							dsz = ddz + sz;
             
							if((dsx<0)||(dsy<0)||(dsz<0)) continue;
							if((dsx>=sX)||(dsy>=sY)||(dsz>=sZ)) continue;			
					
							sFea[dsx][dsy][dsz]= 40; 
						}
					       }
					       }
					
						for(ddz=-dilate; ddz<dilate+1; ddz++){
						for(ddy=-dilate; ddy<dilate+1; ddy++){
						for(ddx=-dilate; ddx<dilate+1; ddx++){
					
							dtx = ddx + tx;
							dty = ddy + ty;
							dtz = ddz + tz;
             
							if((dtx<0)||(dty<0)||(dtz<0)) continue;
							if((dtx>=tX)||(dty>=tY)||(dtz>=tZ)) continue;			
					
							tFea[dtx][dty][dtz]= 40; 
						}
					       }
				              }	
					}// for - j	
					current_set = 0;
					cout <<i<< " indexS= "<<mIndexPairZ[i].SavedS_Index<<" , indexL= "<<mIndexPairZ[i].SavedL_Index<<endl;
				}  
			}  // else
	       } // current_set == 0
		else if(current_set !=0) current_set = 0;
	}  // for - i
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

 
	string SrcClineFile(argv[2]),   TrgClineFile(argv[4]);
	storeImage(SrcCline, SrcClineFile, sX, sY, sZ, SrcReader );
	storeImage(TrgCline, TrgClineFile, tX, tY, tZ, TrgReader );
	
	string SrcClineFeatureFile(argv[5]),   TrgClineFeatureFile(argv[6]);
	storeImage(sFea, SrcClineFeatureFile, sX, sY, sZ, SrcReader );
	storeImage(tFea, TrgClineFeatureFile, tX, tY, tZ, TrgReader );

	if(argc == 9) {
				string landmarksFile(argv[8]);
				storeLandmarks(landmarksFile, validCntZ, mIndexPairZ, matchedPair, MORE_window_size );			
	}
	
}  // main

