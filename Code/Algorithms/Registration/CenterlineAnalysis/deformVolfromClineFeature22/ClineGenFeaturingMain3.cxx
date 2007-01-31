#include "MyHeader.h"
#include "deformByCLine.h"

#include <string.h>
#include <time.h>
using namespace std;

/* GenerateCline.cxx*/
extern int generateCline(TNT::Array3D<int> &Voxel, int startX, int startY, int startZ, ReaderType::Pointer reader,  TNT::Array3D<int> &Cline, int num,  ClineVoxel &Cvoxel );
extern int FindFeatures(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures2(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures3(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures4(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures5(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int FindFeatures5Add(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl);
extern int pruningIndex3(IndexPair* mIndexPair,  IndexPair* mIndexOut, int validCnt, int window_size, int extra_length);
//extern void ReconstructFeaturedCline(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair  &matchedPair);
extern void ReconstructFeaturedCline(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair);
extern void ReconstructFeaturedClineMORE(IndexPair* mIndexPair, int validCnt, int window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair *matchedPair);
extern int pruningIndex(IndexPair* mIndexPair,  IndexPair* mIndexOut, int validCnt, int window_size);
extern int pruningIndex2(IndexPair* mIndexPair,  IndexPair* mIndexOut, int validCnt, int window_size);

/*  storeLandmark.cxx */
extern void storeLandmarks(const string &fileName, int validCnt,  IndexPair* mIndexPair, MatchedPair *matchedPair, int window_size);
extern void storeImage(TNT::Array3D<int> &Volume, string &fileName, int sizeX, int sizeY, int sizeZ, ReaderType::Pointer reader);

extern unsigned long int markCnt ;
	
int main( int argc, char * argv[] )
{
	if( argc < 10 )
	{
		std::cerr << "Usage: " << argv[0];
		std::cerr << " SrcVolumeFile  SrcClineFileName TrgVolumeFlie TrgClineFileName SrcFeatureFile TrgFeatureFile dilate ClineVectorFile VolumeVectorFile (landmarksFile) startX startY startZ";
		
		std::cerr << std::endl;  
		return EXIT_FAILURE;
	}
	
	int dilate;
	dilate = atoi(argv[7]); clog << "dilate =" << dilate<<endl;
	
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
	
	//SrcStartX=122; SrcStartY=144; SrcStartZ=22;   //VC386_suHalf.vtk
	//TrgStartX=122; TrgStartY=144; TrgStartZ=22;   //VC386_prHalfFlipMoved.vtk
	
	SrcStartX=atoi(argv[11]); SrcStartY=atoi(argv[12]); SrcStartZ=atoi(argv[13]);
	TrgStartX=SrcStartX; 	TrgStartY=SrcStartY; TrgStartZ=SrcStartZ;
	//TrgStartX=124; TrgStartY=129; TrgStartZ=16;   //VC386_prHalfFlip.vtk
	
	int SrcClineSize=0, TrgClineSize=0;
	/////////////////////////////
	ClineVoxel src, trg;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned long int srcMarkCnt, trgMarkCnt;
	SrcClineSize = generateCline(sVoxel, SrcStartX, SrcStartY, SrcStartZ, SrcReader , SrcCline, 0, src);
	std::cout <<"     ************   markCnt = "<<markCnt<<" SrcClineSize= "<<SrcClineSize<<"  ************"<<std::endl;
	srcMarkCnt = markCnt;
	
	TrgClineSize = generateCline(tVoxel, TrgStartX, TrgStartY, TrgStartZ, TrgReader, TrgCline, 1, trg );
	std::cout <<"     ************   markCnt = "<<markCnt<<"   TrgClineSize= "<<TrgClineSize<<" ************"<<std::endl;	
	trgMarkCnt = markCnt;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//cout<<"after generateCline()"<<endl;
		
	TNT::Array3D<int> sFea(sX, sY, sZ, 0); 
	TNT::Array3D<int> tFea(tX, tY, tZ, 0); 
	
	//// adjust m, window_size here
	//int m=15, window_size = 50;
	//float fixed_correl = 0.852;
	
	int m=20, window_size = 40; // window_size should be even number by wook
	int extra = 20;
	
	
	int MORE_window_size = window_size + 2*extra;   // More Version
	
	float fixed_correl = 0.3;
	int longerSearchLength = 100, shorterSearchLength = 20;
	int plusSearchRange, minusSearchRange;
	
	if(SrcClineSize >= TrgClineSize){
		
		plusSearchRange =shorterSearchLength;	minusSearchRange = longerSearchLength;
	}
	else{
		
		plusSearchRange =longerSearchLength;	minusSearchRange =shorterSearchLength ;
	}
	
	
	IndexPair mIndexPairZ[20], mIndexPairOrg[20];
	int validCntZ=0;
	int PrunedCnt =0;

	clock_t start_time, end_time;
	double elapsed_time;
	
	start_time =clock();   // check start time
	validCntZ = FindFeatures5Add(src, trg, m, window_size, plusSearchRange, minusSearchRange, mIndexPairOrg, fixed_correl);  // use Y-Z extrema respectively 
	PrunedCnt = pruningIndex3(mIndexPairOrg,  mIndexPairZ, validCntZ, MORE_window_size, extra);
//	PrunedCnt = pruningIndex2(mIndexPairOrg,  mIndexPairZ, validCntZ, MORE_window_size);
	
	MatchedPair matchedPair[PrunedCnt];
	//ReconstructFeaturedClineMORE(mIndexPairZ, validCntZ, MORE_window_size , src, trg, matchedPair);
	ReconstructFeaturedClineMORE(mIndexPairZ, PrunedCnt, MORE_window_size , src, trg, matchedPair);
	
	DField4Clines VecCline;
	cout<< "VecCline 1"<<endl;
	//VecCline.fillFeaturedCline(PrunedCnt, MORE_window_size, TrgClineSize, src, trg, matchedPair,  mIndexPairZ);	cout<< "VecCline 2"<<endl;
	VecCline.fillFeaturedCline2(PrunedCnt, MORE_window_size, TrgClineSize, SrcClineSize, src, trg, matchedPair,  mIndexPairZ);
	string ClineVectorFile(argv[8]);
	VecCline.storeClineDField(ClineVectorFile, TrgReader,  mIndexPairZ, TrgClineSize);                                               cout<< "VecCline 3"<<endl;
	
	string VolumeVectorFile(argv[9]);
	GroupObjVoxel ObjVolume;
	
#if 1
	//unsigned long int total_targetVolume = tX * tY * tZ;
	int src_trgNum = 0;
	src_trgNum = ObjVolume.CountSrcAndTrgObj(sVoxel, tVoxel, tX, tY, tZ);
	ObjVolume.fillGroupObjVoxelObj(src_trgNum, sVoxel, tVoxel, tX, tY, tZ); // fill the volume of source and target with Cline DField
//	ObjVolume.fillGroupObjVoxel(total_targetVolume, tVoxel, tX, tY, tZ); // fill the whole volume of the target with Cline DField
//	ObjVolume.fillGroupObjVoxel(trgMarkCnt, tVoxel, tX, tY, tZ);  // only fill the target object with DField from Cline DField
	ObjVolume.distFromClosestCline2(VecCline);            		cout<< "after ObjVolume.distFromClosestCline(VecCline)"<<endl;
	ObjVolume.DFieldFromClosestCline2(VecCline);                     cout<< "after ObjVolume.DFieldFromClosestCline(VecCline)"<<endl;
	
//	ObjVolume.smootheDField(TrgReader);	
	ObjVolume.storeVolumeDField(TrgReader, VolumeVectorFile);      cout<< "after ObjVolume.storeVolumeDField(TrgReader, VolumeVectorFile)"<<endl;
#endif
	
	int current_set = 0;
	int k,j,i, value=0;
	int sx,sy,sz,tx,ty,tz;
	int ddx, ddy, ddz, dsx, dsy, dsz, dtx, dty,dtz;
	
	
	for(i=0; i<PrunedCnt; i++){ 			
			//cout<<"mIndexPairS"<<i<<" : "<<mIndexPairZ[i].SavedS_Index<< "w_size: "<<MORE_window_size <<" next mIndexPairS: "<<mIndexPairZ[i+1].SavedS_Index<<endl;
			//cout<<"mIndexPairL"<<i<<" : "<<mIndexPairZ[i].SavedL_Index<< "w_size: "<<MORE_window_size <<" next mIndexPairL: "<<mIndexPairZ[i+1].SavedL_Index<<endl;
						
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
				
	
	}  // for - i
	
	end_time = clock();
	elapsed_time = (end_time - start_time) / CLOCKS_PER_SEC;
	std::cout<<"elapsed time =" << elapsed_time << std::endl;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
	string SrcClineFile(argv[2]),   TrgClineFile(argv[4]);
	storeImage(SrcCline, SrcClineFile, sX, sY, sZ, SrcReader );
	storeImage(TrgCline, TrgClineFile, tX, tY, tZ, TrgReader );
	
	string SrcClineFeatureFile(argv[5]),   TrgClineFeatureFile(argv[6]);
	storeImage(sFea, SrcClineFeatureFile, sX, sY, sZ, SrcReader );
	storeImage(tFea, TrgClineFeatureFile, tX, tY, tZ, TrgReader );

	cout <<"so far, so good"<<endl;
	if(argc == 11) {
				string landmarksFile(argv[10]);
				storeLandmarks(landmarksFile, PrunedCnt, mIndexPairZ, matchedPair, MORE_window_size );			
	}
	
}  // main

