#include "YunaHeader.h"
#include "YunaDeformByCLine.h"

#include <string.h>
#include <time.h>
using namespace std;


/* YunaGenerateCline.cxx*/
extern int generateCline(TNT::Array3D<float> &Voxel, int startX, int startY, int startZ, DMapReaderType::Pointer reader,  TNT::Array3D<int> &Cline, int num, ClineVoxel &Cvoxel, float threshold );
extern int FindFeatures(ClineVoxel &Cv1, ClineVoxel &Cv2, int m, int window_size, int plusSearchRange, int minusSearchRange, IndexPair* mIndexPair, float fixed_correl, int prev_index);
extern int pruningIndex(IndexPair* mIndexPair,  IndexPair* mIndexOut, int validCnt, int window_size, int extra_length);
extern void ReconstructFeaturedClineMORE(IndexPair* mIndexPair, int validCnt, int MORE_window_size, ClineVoxel &cS, ClineVoxel &cT, MatchedPair  *matchedPair);


/*  YunaStoreLandmark.cxx */
extern void storeLandmarks(const string &fileName, int validCnt,  IndexPair* mIndexPair, MatchedPair *matchedPair, int window_size);
extern void storeImage(TNT::Array3D<int> &Volume, string &fileName, int sizeX, int sizeY, int sizeZ, ReaderType::Pointer reader);

extern unsigned long int markCnt ;

#define OBJECTS_ONLY	0

int main( int argc, char * argv[] )
{
	if( argc < 16 )
	{

		//std::cerr << "TrgFile TrgInSignedFlie TrgClineFileName threshold (position)start1X start1Y start1Z";
		std::cerr << "TrgFile(1) TrgInSignedFlie(2) TrgClineFileName(3) TrgThreshold(4) TrgFeatureFile(5) ";      //  5 [1-5]
		std::cerr << "SrcFile(6) SrcInSignedFlie(7) SrcClineFileName(8) SrcThreshold(9) SrcFeatureFile(10) ";  // 5 [6-10]
		std::cerr << "(position)start1X(11) start1Y(12) start1Z(13) ";                                                                 // 3 [11-13]
		std::cerr << "VolumeVectorFile(14) DFieldOption(15)[ 0:obejctOnly 1:wholeVolume]";               // 2 [14-15]    total:15     
		std::cerr << std::endl;  
		return EXIT_FAILURE;
	}	
	
	DMapReaderType::Pointer TrgDMapReader =DMapReaderType::New();	
	DMapReaderType::Pointer SrcDMapReader =DMapReaderType::New();	
	TrgDMapReader->SetFileName( argv[2] );  TrgDMapReader->Update(); 
	SrcDMapReader->SetFileName( argv[7] );  SrcDMapReader->Update(); 
	
	DMapInputImageType::RegionType::SizeType  TrgImagesize;
	DMapInputImageType::RegionType::SizeType  SrcImagesize;
	
	DMapInputImageType::SpacingType  TrgSpacing;
	TrgSpacing  =  TrgDMapReader->GetOutput()->GetSpacing() ;  
	cout<<"Trg Spacing="<<TrgSpacing<<endl;
	
	DMapInputImageType::PointType TrgOrigin;
	TrgOrigin = TrgDMapReader->GetOutput()->GetOrigin() ;    
	cout<<"Trg Origin="<<TrgOrigin<<endl;
	
	TrgImagesize =  TrgDMapReader->GetOutput()->GetRequestedRegion().GetSize();
	cout<<"Target : sizeX="<<TrgImagesize[0]<<" sizeY="<<TrgImagesize[1]<<" sizeZ="<<TrgImagesize[2]<<endl; 	
	SrcImagesize = SrcDMapReader->GetOutput()->GetRequestedRegion().GetSize();
	cout<<"Source : sizeX="<<SrcImagesize[0]<<" sizeY="<<SrcImagesize[1]<<" sizeZ="<<SrcImagesize[2]<<endl;
	
	int tX, tY, tZ,sX, sY, sZ;
	tX =TrgImagesize[0];     tY = TrgImagesize[1];    tZ = TrgImagesize[2];
	sX =SrcImagesize[0];     sY = SrcImagesize[1];    sZ = SrcImagesize[2];
	
	TNT::Array3D<float> tVoxel(tX, tY, tZ, 0.0); 
	TNT::Array3D<float> sVoxel(sX, sY, sZ, 0.0); 	
	
	TNT::Array3D<int> TrgCline(tX, tY, tZ, 0);
	TNT::Array3D<int> SrcCline(sX, sY, sZ, 0);
	
	clog <<"Array Setting Completed"<<endl;
	
	float Trgthreshold = atof(argv[4]);
	float Srcthreshold = atof(argv[9]);
	
	int SrcStartX=0, SrcStartY=0, SrcStartZ=0, TrgStartX=0, TrgStartY=0, TrgStartZ=0;
	float fX, fY, fZ;
	fX= atof(argv[11]);  fY= atof(argv[12]);  fZ= atof(argv[13]);
	cout << "anus=(" <<fX <<", "<<fY<<", "<<fZ<<")"<<endl;	
	  
	float fPosX, fPosY, fPosZ;
	  
  	  fPosX = (fX - TrgOrigin[0]) / TrgSpacing[0];
	  fPosY = (fY - TrgOrigin[1]) / TrgSpacing[1];
	  fPosZ = (fZ - TrgOrigin[2]) / TrgSpacing[2]; 
	  
  	  SrcStartX= (int)floor(fPosX + 0.5);
	  SrcStartY= (int)floor(fPosY + 0.5);
	  SrcStartZ= (int)floor(fPosZ + 0.5);  

	TrgStartX=SrcStartX; TrgStartY=SrcStartY;   TrgStartZ=SrcStartZ;
	cout << "anus index=(" <<TrgStartX <<", "<<TrgStartY<<", "<<TrgStartZ<<")"<<endl;
	
// target centerline
	int TrgClineSize=0;
	ClineVoxel trg;
	unsigned long int trgMarkCnt;
	TrgClineSize = generateCline(tVoxel, TrgStartX, TrgStartY, TrgStartZ, TrgDMapReader, TrgCline, 1, trg, Trgthreshold );	
	std::cout <<"     ************   markCnt = "<<markCnt<<"   TrgClineSize= "<<TrgClineSize<<" ************"<<std::endl;	
	trgMarkCnt = markCnt;
	
	string TrgClineFile(argv[3]);
	
	ReaderType::Pointer TrgReader =ReaderType::New();	
	TrgReader->SetFileName( argv[1] );  TrgReader->Update(); 
	storeImage(TrgCline, TrgClineFile, tX, tY, tZ, TrgReader );	
	
// source centerline
	int SrcClineSize=0;
	ClineVoxel src;
	unsigned long int srcMarkCnt;
	SrcClineSize = generateCline(sVoxel, SrcStartX, SrcStartY, SrcStartZ, SrcDMapReader, SrcCline, 0, src, Srcthreshold );	
	std::cout <<"     ************   markCnt = "<<markCnt<<"   SrcClineSize= "<<SrcClineSize<<" ************"<<std::endl;	
	srcMarkCnt = markCnt;
	
	string SrcClineFile(argv[8]);
	
	ReaderType::Pointer SrcReader =ReaderType::New();	
	SrcReader->SetFileName( argv[6] );  SrcReader->Update(); 
	storeImage(SrcCline, SrcClineFile, sX, sY, sZ, SrcReader );	
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
		
	TNT::Array3D<int> sFea(sX, sY, sZ, 0); 
	TNT::Array3D<int> tFea(tX, tY, tZ, 0); 
	
//	int m=20, window_size = 20; // window_size should be even number by wook
	int m=20, window_size = 20;  // V1
	int extra = 10;
	int MORE_window_size = window_size + 2*extra;   // More Version
	int longerSearchLength = 100, shorterSearchLength = 20;
//	int longerSearchLength = 50, shorterSearchLength = 20;  // V1


		
	float fixed_correl = 0.3;   // this value is for the one coordinate correlation value
	//float fixed_correl = 0.25;   // this value is for the one coordinate correlation value
	
	int plusSearchRange, minusSearchRange;
/*	
	if(SrcClineSize >= TrgClineSize){
		
		plusSearchRange =shorterSearchLength;	minusSearchRange = longerSearchLength;
	}
	else{
*/		
		plusSearchRange =longerSearchLength;	minusSearchRange =shorterSearchLength ;
//	}	
		
		
	IndexPair mIndexPairZ[40], mIndexPairOrg[40];
	int validCntZ=0;
	int PrunedCnt =0;

	clock_t start_time, end_time;
	double elapsed_time;
	
	start_time =clock();   // check start time
	validCntZ = FindFeatures(src, trg, m, window_size, plusSearchRange, minusSearchRange, mIndexPairOrg, fixed_correl, 0);  // use Y-Z extrema respectively 	
	
	PrunedCnt = pruningIndex(mIndexPairOrg,  mIndexPairZ, validCntZ, MORE_window_size, extra);
	
	MatchedPair matchedPair[PrunedCnt];
	ReconstructFeaturedClineMORE(mIndexPairZ, PrunedCnt, MORE_window_size , src, trg, matchedPair);
	
	int current_set = 0;
	int k,j,i, value=0;
	int sx,sy,sz,tx,ty,tz;
	int ddx, ddy, ddz, dsx, dsy, dsz, dtx, dty,dtz;
	
	int dilate;
	dilate = 2; clog << "dilate =" << dilate<<endl;
	
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
	string SrcClineFeatureFile(argv[10]),   TrgClineFeatureFile(argv[5]);
	storeImage(sFea, SrcClineFeatureFile, sX, sY, sZ, SrcReader );
	storeImage(tFea, TrgClineFeatureFile, tX, tY, tZ, TrgReader );
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	DField4Clines VecCline;
	VecCline.fillFeaturedCline2(PrunedCnt, MORE_window_size, TrgClineSize, SrcClineSize, src, trg, matchedPair,  mIndexPairZ);   
	string ClineVectorFile = "ClineVectorFile.vtk";
	VecCline.storeClineDField(ClineVectorFile, TrgReader,  mIndexPairZ, TrgClineSize);   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	string VolumeVectorFile(argv[14]);
	GroupObjVoxel ObjVolume;
	
	int area4DField = atoi(argv[15]);	
	
	if(area4DField == OBJECTS_ONLY){
		int src_trgNum = 0;
		src_trgNum = ObjVolume.CountSrcAndTrgFloatObj(sVoxel, Srcthreshold, tVoxel, Trgthreshold, tX, tY, tZ);
		ObjVolume.fillGroupObjVoxelFloatObj(src_trgNum, sVoxel, Srcthreshold, tVoxel, Trgthreshold, tX, tY, tZ); // fill the volume of source and target with Cline DField

	}
	else{
		unsigned long int total_targetVolume = tX * tY * tZ;
	       ObjVolume.fillGroupObjVoxelFloat(total_targetVolume, tVoxel, tX, tY, tZ); // fill the whole volume of the target with Cline DField
       }

	
	ObjVolume.distFromClosestCline2(VecCline);            		cout<< "after ObjVolume.distFromClosestCline(VecCline)"<<endl;
	ObjVolume.DFieldFromClosestCline2(VecCline);                     cout<< "after ObjVolume.DFieldFromClosestCline(VecCline)"<<endl;
	
	if(area4DField != OBJECTS_ONLY){
		ObjVolume.smootheDField(TrgReader);	
       }
	
	
	ObjVolume.storeVolumeDField(TrgReader, VolumeVectorFile);      cout<< "after ObjVolume.storeVolumeDField(TrgReader, VolumeVectorFile)"<<endl;

	

	
	end_time = clock();
	elapsed_time = (end_time - start_time) / CLOCKS_PER_SEC;
	std::cout<<"elapsed time =" << elapsed_time << std::endl;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
}// end of main










