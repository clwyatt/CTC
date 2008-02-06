/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include "ctcPrincipleCurvatureExtraction.h"
#include "ctcFeatureExtraction.h"
#include <itkConstNeighborhoodIterator.h>
#include <itkCropImageFilter.h>
#include <itkHessianRecursiveGaussianImageFilter.h>
#include <itkGradientRecursiveGaussianImageFilter.h>
#include <itkNeighborhoodAlgorithm.h>
#include <itkSymmetricEigenSystem.h>
#include <vnl/algo/vnl_real_eigensystem.h>
#include <vector>
#include <iterator>
#include <stack>
#include <list>
#include <fstream>
#include <string>
#include <time.h>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "MAT4Converter.h" 
#include "StringtoDouble.h"

#define DEBUG

#ifndef DEBUG
    #define EVAL(x)
    #define PRINT(x)
    #define ASSERT(x)
#else
    #define EVAL(x) \
           cout << #x << ": " << x << endl; 
    #define PRINT(x) \
           cout << x << " ";
    #define ASSERT(x) \
           if (!x) { \
              cout << "ERROR! Assert --- " << #x << " failed\n"; \
              cout << "Line --- " << __LINE__ << endl; \
              cout << "File --- " << __FILE__ << endl; \
           }
#endif

namespace ctc
{
   FeatureExtraction::FeatureExtraction()
   {
        m_FeatureVector = FeatureSampleType::New();
   }

   FeatureExtraction::~FeatureExtraction()
   { }
   
   int FeatureExtraction::PrincipalCurvatures( BinaryImageType::IndexType idx, float &SI, float &CV, float &gmag )
   {
        // Image Crop
        typedef itk::Image<float, 3> FloatImageType;
        typedef itk::CropImageFilter<CTCImageType, FloatImageType> CropFilterType;
        CropFilterType::Pointer crop = CropFilterType::New();
        ctc::CTCImageType::SizeType fullsize = m_Image->GetLargestPossibleRegion().GetSize();
        crop->SetInput(m_Image);
        
        // Hessian Matrix
        typedef itk::HessianRecursiveGaussianImageFilter<FloatImageType> HessianFilterType;      
        typedef HessianFilterType::OutputImageType HessianImageType;
        typedef HessianFilterType::OutputPixelType HessianPixelType;
        HessianFilterType::Pointer hessian = HessianFilterType::New();
        hessian->SetSigma(1.0);
        HessianPixelType H;

        // Gradient 
        typedef itk::GradientRecursiveGaussianImageFilter<FloatImageType> GradientFilterType;
        typedef GradientFilterType::OutputImageType GradientImageType;
        typedef GradientFilterType::OutputPixelType GradientPixelType;
        GradientFilterType::Pointer gradient = GradientFilterType::New();
        gradient->SetSigma(1.0);
        GradientPixelType g;

        itk::Size<3> start, end;
        float E,F,G,L,M,N,R1,hmag,P1,P2,K1,K2,Gaussian_Curvature,Mean_Curvature;

        start[0] = idx[0]-4;
	       start[1] = idx[1]-4;
	       start[2] = idx[2]-4;
	       end[0] = fullsize[0] - (idx[0]+4) - 1;
	       end[1] = fullsize[1] - (idx[1]+4) - 1;
	       end[2] = fullsize[2] - (idx[2]+4) - 1;

                       
        if(start[0] > fullsize[0] || start[1] > fullsize[1] || start[2] > fullsize[2] || end[0] > fullsize[0] || end[1] > fullsize[1] || end[2] > fullsize[2] )
            return 1;
	          
        crop->SetLowerBoundaryCropSize(start);      
        crop->SetUpperBoundaryCropSize(end);
        crop->UpdateLargestPossibleRegion();

        // Compute the Hessian, scale = 1.0;
	       hessian->SetInput(crop->GetOutput());
	       hessian->Update();
	       H = hessian->GetOutput()->GetPixel(idx);

        // Compute gradient 
        gradient->SetInput(crop->GetOutput());
	       gradient->Update();
        g = gradient->GetOutput()->GetPixel(idx);
        gmag = sqrt(g[0]*g[0]+g[1]*g[1]+g[2]*g[2]);

        E = 1 + (g[0]*g[0])/(g[2]*g[2]);
        F = (g[0]*g[1])/(g[2]*g[2]);
        G = 1 + (g[1]*g[1])/(g[2]*g[2]); 
        hmag = g[0]*g[0] + g[1]*g[1] + g[2]*g[2];
        R1 = g[2]*g[2]*sqrt(hmag);
        L = (2*g[0]*g[2]*H[2] - g[0]*g[0]*H[5] - g[2]*g[2]*H[0]) / R1;
        M = (g[0]*g[2]*H[4] + g[1]*g[2]*H[2] - g[0]*g[1]*H[5] - g[2]*g[2]*H[1]) / R1;
        N = (2*g[1]*g[2]*H[4] - g[1]*g[1]*H[5] - g[2]*g[2]*H[3]) / R1;

        Gaussian_Curvature = (L*N-M*M) / (E*G-F*F);
        Mean_Curvature = (E*N-2*F*M+G*L) / (2*(E*G-F*F));

        K1 = Mean_Curvature + sqrt(Mean_Curvature*Mean_Curvature-Gaussian_Curvature);
        K2 = Mean_Curvature - sqrt(Mean_Curvature*Mean_Curvature-Gaussian_Curvature);  

        SI = 0.5 - 0.31831*atan((K1+K2)/(K1-K2));
        CV = sqrt(K1*K1/2 + K2*K2/2);

        return 0;
   } 

   
   void FeatureExtraction::CalculateCentroid( RegionCollectorType &growableregion_vector, dcmCoordinate &centroid, int index )
   {
        GrowableRegionType::iterator iter = growableregion_vector[index].begin();
        int num_member = 0;
        int i = 0;
        centroid[0] = 0;
        centroid[1] = 0;
        centroid[2] = 0;

        for (; iter != growableregion_vector[index].end(); ++iter)
        {
              dcmCoordinate c = iter->GetDCMCoordinate();
              //for(i = 0; i < 3; i++)   
                  centroid[0] =  centroid[0] + c[0];
                  centroid[1] =  centroid[1] + c[1];
                  centroid[2] =  centroid[2] + c[2];               

              num_member++;                   
        }

        //for (i = 0; i < 3; i++)
            centroid[0] = centroid[0] / num_member;
            centroid[1] = centroid[1] / num_member;
            centroid[2] = centroid[2] / num_member;            

   }


   void FeatureExtraction::MergeRegions( RegionCollectorType &growableregion_vector, list<int> merge_list, int merger )
   {     
    
        list<int>::iterator iter_int = merge_list.begin();

        while ( iter_int != merge_list.end() )
        {
             int index = *iter_int;
             while( !growableregion_vector[index].empty() )
             {
                  growableregion_vector[merger].push_back( growableregion_vector[index].back() );                   
                  growableregion_vector[index].pop_back();
                  //growableregion_vector[merger].push_back( growableregion_vector[index].front() ); 
                  //growableregion_vector[index].pop_front();
             }
             iter_int++;     
        }

        for (int i = 0; i < growableregion_vector.size(); i++ )
        {
            if( growableregion_vector[i].empty() )
            {
                growableregion_vector.erase( growableregion_vector.begin() + i );
                i--;
            }
        }
   }

   void FeatureExtraction::Analyze()
   {
        using namespace std;
        dcmCoordinate pdcm;
        BinaryImageType::IndexType pindex;
        BinaryImageType::IndexType idx, idx2, head_idx;
        AssociatedFeatureList afl; 
        int i,j,k,l,m,n;
        clock_t start1, end1, start2, end2, start3, end3;

        /* Stacked vector --- Manipulating all the regions in dataset */
        RegionCollectorType growableregion_vector;

        std::ofstream finalout("summary.txt");
           
        
        /* Step 1: Setup Growable Region by threshold values */   

        typedef itk::ConstNeighborhoodIterator< BinaryImageType >   BinaryIteratorType;
        typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< BinaryImageType >   FaceCalculatorType;   
        typedef itk::ConstNeighborhoodIterator< BinaryImageType > NeighborhoodIteratorType;


        FaceCalculatorType faceCalculator;
        FaceCalculatorType::FaceListType faceList;
        BinaryIteratorType::RadiusType cropradius;
        cropradius.Fill(4);
        BinaryIteratorType::RadiusType radius;
        radius.Fill(2);  

        faceList = faceCalculator(m_ColonImage, m_ColonImage->GetRequestedRegion(),cropradius);    
        BinaryIteratorType it(radius, m_ColonImage, *(faceList.begin()));
        
        std::vector<NeighborhoodIteratorType::OffsetType> offsets;
        i = 0;

        BinaryIteratorType::OffsetType center = {{0,0,0}};
        BinaryIteratorType::OffsetType right = {{1,0,0}};
        BinaryIteratorType::OffsetType left = {{-1,0,0}};
        BinaryIteratorType::OffsetType bottom = {{0,1,0}};
        BinaryIteratorType::OffsetType top = {{0,-1,0}};
        BinaryIteratorType::OffsetType front = {{0,0,1}};
        BinaryIteratorType::OffsetType back = {{0,0,-1}};               

        float SI, CV, gmag;

        int SeedRegionTracker = 0;

        // BinaryIteratorType it(radius, m_ColonImage, m_ColonImage->GetRequestedRegion());
  
        cout << "Entering growableregion formation ......" << endl;  
        finalout << "Entering growableregion formation ......" << endl;  
        cout << "Size of input image dataset: " <<  GetVolumeSize() << endl;
        finalout << "Size of input image dataset: " <<  GetVolumeSize() << endl;
        cout << "Spacing of input image dataset: " << m_ColonImage->GetSpacing() << endl;
        finalout << "Spacing of input image dataset: " << m_ColonImage->GetSpacing() << endl;
        
        std::ofstream out3("datasetDCMSICV.txt");
        std::ofstream out4("Seedpoint.txt");

        start1 = clock();
  
        typedef stack <AssociatedFeatureList> Region_Stack_Type; 
        typedef list <AssociatedFeatureList>  Region_List_Type;     


        start1 = clock();
        int voxel_tracker = 1;
        for(it.GoToBegin(); !it.IsAtEnd(); ++it)
        {
            if(it.GetPixel(center) == 0)
            {        
            int count = 0;

	           if(it.GetPixel(right) == 255) count += 1;
	           if(it.GetPixel(left) == 255) count += 1;
	           if(it.GetPixel(bottom) == 255) count += 1;
	           if(it.GetPixel(top) == 255) count += 1;
	           if(it.GetPixel(front) == 255) count += 1;
	           if(it.GetPixel(back) == 255) count += 1;

	           if(count == 0) // not on a boundary
                  continue;
 
            voxel_tracker++;
            if((voxel_tracker % 50) == 0)
            {
                cout << voxel_tracker << endl;
                finalout << voxel_tracker << endl;
            }

            idx = it.GetIndex();
 
            int PC_result_Seed = PrincipalCurvatures(idx,SI,CV,gmag);
            
            if( PC_result_Seed == 1 )
                continue;
            

            dcmCoordinate each;
            m_ColonImage->TransformIndexToPhysicalPoint(idx,each);

            if( !(SI > 0.9 && SI < 1.0 && CV > 0.08 && CV < 0.2) )
                 continue;          
            
            AssociatedFeatureList fl;
            fl.SetSI(SI);
            fl.SetCV(CV);
            fl.SetDCMCoordinate(each);
            fl.SetGmag(gmag);
            fl.SetVoxelIndex(idx);
            out4 << each << "  " << idx << "  " <<  SI << "   " << CV << endl;               
 
            Region_Stack_Type GrowableRegion_Stack; 
            Region_List_Type GrowableRegion_List;

            GrowableRegion_List.push_back(fl);
            GrowableRegion_Stack.push(fl);
            head_idx = idx;
 
            m_ColonImage->SetPixel(idx, 100); 

            while( !GrowableRegion_Stack.empty() )
            {
                 int flag_push = 0;
                 for(i = 0; i < 6; i++)
                 {
                       int direction = 0;
                            
                       if ((i%2) == 0)
                           direction = 1;
                       else
                           direction = -1;
               
                       head_idx = (GrowableRegion_Stack.top()).GetVoxelIndex();
                       idx2 = head_idx;

                       // Neighbourhood starting from x to -x, y, -y, z, -z.
                       idx2[i/2] = head_idx[(i/2)] + direction;

                       if(m_ColonImage->GetPixel(idx2) != 0)
                              continue;
               
                       int result_Neigh = PrincipalCurvatures(idx2,SI,CV,gmag);
                       if( result_Neigh == 1 )
                             continue;

                       if( SI > 0.8 && SI < 1.0 && CV > 0.05 && CV < 0.25 )            
                       {
                            AssociatedFeatureList fl_tmp;
                            dcmCoordinate neighborpdcm;
                            m_ColonImage->TransformIndexToPhysicalPoint(idx2,neighborpdcm);
                            fl_tmp.SetDCMCoordinate(neighborpdcm);
                            fl_tmp.SetSI(SI);
                            fl_tmp.SetCV(CV);
                            fl_tmp.SetGmag(gmag);
                            fl_tmp.SetVoxelIndex(idx2);
                            m_ColonImage->SetPixel(idx2, 100); 
                            GrowableRegion_Stack.push(fl_tmp);
                            flag_push++;
                       }
                 }

                 if(flag_push == 0)
                    GrowableRegion_List.push_back( GrowableRegion_Stack.top() );
                    GrowableRegion_Stack.pop();
            }

            GrowableRegion_List.pop_back();
            growableregion_vector.push_back(GrowableRegion_List);

            }        

       }

        

       end1 = clock();
       double elapsed1 = ((double) (end1 - start1)) / CLOCKS_PER_SEC;
       cout << "Time for generating initial regions: " << elapsed1/3600 << " hours" << endl;       
       finalout << "Time for generating initial regions: " << elapsed1/3600 << " hours" << endl;       

       out3.close();
       out4.close();
              
       cout << "Size of Region Vector: " << growableregion_vector.size() << endl;
       finalout << "Size of Region Vector: " << growableregion_vector.size() << endl;

       GrowableRegionType::iterator iter; 

       std::ofstream out5("Prechecking3.txt");
       int total = 0;
       int region_id = 1;
       for(i = 0; i < growableregion_vector.size(); i++)
       {
            iter = growableregion_vector[i].begin();
            int prechecking = 0;
            out5 << endl << " ********************** " << endl << endl;
            out5 << "Region " << region_id << ": " << endl;
            for (; iter != growableregion_vector[i].end(); ++iter)
            { 
                  prechecking++; 
                  total++;
                  m_ColonImage->SetPixel(iter->GetVoxelIndex(), 0);
                  
                  out5 << "Index: " << iter->GetVoxelIndex() << "   Coordinate: " << iter->GetDCMCoordinate() << "  SI: " << iter->GetSI() << "  CV: " << iter->GetCV() << endl;
            }
            out5 << "This region contains " << prechecking << endl;
            region_id++;
       }
       out5 << "Total number of voxels: " << total << endl;
       out5.close();
 


        /* Step 2: Merging different detections on the same polyp candidate */

        cout << "Merging multiple detections on the same polyp candidate ......" << endl;     
        finalout << "Merging multiple detections on the same polyp candidate ......" << endl;       
  
        int merger = -1;
        start2 = clock();
  
        int merging_flag = 0;
        for(i = 0; i < growableregion_vector.size(); i++)
        {
	             int size_v = growableregion_vector.size();
              if (size_v % 50 == 0)	      
                  cout << "Size of growableregion_vector: " << size_v << endl; 

              dcmCoordinate centroid1;
              CalculateCentroid(growableregion_vector, centroid1, i);

              list<int> merging_list;
              for( j = (i+1); j < growableregion_vector.size(); j++)
              {
                    dcmCoordinate centroid2;
                    CalculateCentroid(growableregion_vector, centroid2, j);                    
                    float distance = (centroid1[0]-centroid2[0])*(centroid1[0]-centroid2[0]) + (centroid1[1]-centroid2[1])*(centroid1[1]-centroid2[1]) + (centroid1[2]-centroid2[2])*(centroid1[2]-centroid2[2]);                     
                    if(distance < 100)
                    {
                            merging_list.push_back(j);
                            merger = i;       
                            merging_flag = 1;
                    }                                    
              }

              if(merging_flag == 1)
              {
                  merging_flag = 0;
                  MergeRegions(growableregion_vector, merging_list, merger);
                  i--;
              }
        }      


       end2 = clock();
       double elapsed2 = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
       cout << "Time for grouping regions: " << elapsed2/3600 << " hours" << endl;       

       std::ofstream out1("After merging.txt");
       int total2 = 0;
       int region_id2 = 1;
       for(i = 0; i < growableregion_vector.size(); i++)
       {
            iter = growableregion_vector[i].begin();
            int prechecking = 0;
            out1 << endl << " ********************** " << endl << endl;
            out1 << "Region " << region_id2 << ": " << endl;
            for (; iter != growableregion_vector[i].end(); ++iter)
            { 
                  prechecking++; 
                  total2++;
                  out1 << "Index: " << iter->GetVoxelIndex() << "  SI: " << iter->GetSI() << "  CV: " << iter->GetCV() << endl;;
            }
            out1 << "This region contains " << prechecking << endl;
            region_id2++;
       }
       out1 << "Total number of voxels: " << total2 << endl;
       out1.close();


       start3 = clock();
       /* Step 3: Fuzzy C-Means Algorithm */       
       int num_regions = growableregion_vector.size();
       int num_points = total2;
       float CV_range = 0;
       dcmCoordinate max_range;
       max_range[0] = 0; 
       max_range[1] = 0; 
       max_range[2] = 0;       

       // Subpart 1 --- Initialize the membership matrix
       std::vector< float > membershipmatrix;
       AssociatedFeatureList clustermatrix[num_regions];

       for(i = 0; i < num_points; i++)
       {
            for(j = 0; j < num_regions; j++)
            {
                 membershipmatrix.push_back(0); 
            }
       }
      
       m = 0;
       for(i = 0; i < num_regions; i++)
       {            
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
            {
                  //Intialize the membership matrix
                  membershipmatrix[m*num_regions+i] = 1;
                  m++;
                  //preparation for feature vector normalization
                  if(iter->GetCV() > CV_range)
                     CV_range = iter->GetCV();
                  dcmCoordinate tmp_co = iter->GetDCMCoordinate();
                  if( abs(tmp_co[0]) > max_range[0] )
                      max_range[0] = abs(tmp_co[0]);
                  if( abs(tmp_co[1]) > max_range[1] )
                      max_range[1] = abs(tmp_co[1]);
                  if( abs(tmp_co[2]) > max_range[2] )
                      max_range[2] = abs(tmp_co[2]);
            }     
       }

       // Normalization of feature vectors from extracted voxels
       for(i = 0; i < growableregion_vector.size(); i++)
       {            
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
            {
                  iter->SetnSI(iter->GetSI());
                  float normalized_CV = iter->GetCV() / CV_range;
                  iter->SetnCV(normalized_CV);
                  dcmCoordinate p = iter->GetDCMCoordinate();
                  p[0] = p[0] / max_range[0];     
                  p[1] = p[1] / max_range[1]; 
                  p[2] = p[2] / max_range[2];
                  iter->Set_nDCMCoordinate(p);                 
            }
       } 

       // Subpart 2 --- Fuzzy C-Means clustering 
       float J1 = 0;
       float J2 = 0; 
       int loopnum = 0; 

       while ( 1 )
       {
            J1 = J2;

                 //(1) Update centroids
                 dcmCoordinate updatedcoordinate;
                 for(j = 0; j < num_regions; j++)
                 {
                       updatedcoordinate[0] = 0;
                       updatedcoordinate[1] = 0;
                       updatedcoordinate[2] = 0;
                       m = 0;
                       float tmp_val = 0;

                       for(n = 0; n < num_regions; n++)
                       {
                            iter = growableregion_vector[n].begin();                  
                            while( iter !=  growableregion_vector[n].end() )
                            {
                                  dcmCoordinate p = iter->GetDCMCoordinate();
                                  float tmp_mem =  membershipmatrix[m*num_regions+j] * membershipmatrix[m*num_regions+j];
                                  p[0] = p[0] * tmp_mem;
                                  p[1] = p[1] * tmp_mem;
                                  p[2] = p[2] * tmp_mem;
                                  updatedcoordinate[0] = updatedcoordinate[0] + p[0]; 
                                  updatedcoordinate[1] = updatedcoordinate[1] + p[1];
                                  updatedcoordinate[2] = updatedcoordinate[2] + p[2];
                                  tmp_val = tmp_val + tmp_mem; 
                                  ++m;
                                  ++iter;
                            }
                       }
                       updatedcoordinate[0] = updatedcoordinate[0]/tmp_val;
                       updatedcoordinate[1] = updatedcoordinate[1]/tmp_val;
                       updatedcoordinate[2] = updatedcoordinate[2]/tmp_val;

                       BinaryImageType::IndexType updated_index;
                       clustermatrix[j].SetDCMCoordinate(updatedcoordinate);
                       clustermatrix[j].SetVoxelIndex(updated_index);

                       updatedcoordinate[0] = updatedcoordinate[0] / max_range[0]; 
                       updatedcoordinate[1] = updatedcoordinate[1] / max_range[1]; 
                       updatedcoordinate[2] = updatedcoordinate[2] / max_range[2]; 
                       clustermatrix[j].Set_nDCMCoordinate(updatedcoordinate);

                       GrowableRegionType::iterator iter_tracker = growableregion_vector[j].begin();
                       float min_distance = 400;
                       float tmp_SI = 0;
                       float tmp_CV = 0;

                       // Find the closest voxel from updated cluster center and update center's SI & CV value
                       for (; iter_tracker != growableregion_vector[j].end(); ++iter_tracker)
                       {
                            dcmCoordinate nd = iter_tracker->GetDCMCoordinate(); 
                            float tmp_dis = (nd[0]-updatedcoordinate[0])*(nd[0]-updatedcoordinate[0]) + (nd[1]-updatedcoordinate[1])*(nd[1]-updatedcoordinate[1]) + (nd[2]-updatedcoordinate[2])*(nd[2]-updatedcoordinate[2]);               
                            if( tmp_dis <= min_distance )
                            { 
                                  tmp_SI = iter_tracker->GetSI();
                                  tmp_CV = iter_tracker->GetCV(); 
                                  min_distance = tmp_dis;                                  
                            } 
                       }

                       clustermatrix[j].SetSI(tmp_SI);
                       clustermatrix[j].SetCV(tmp_CV);
                       clustermatrix[j].SetnSI(tmp_SI);
                       clustermatrix[j].SetnCV(tmp_CV/CV_range);                       
                 }        
          
            //(2) Update dissimilarity function
            m = 0;
            J2 = 0;
            for(j = 0; j < num_regions; j++)
            {
                 iter = growableregion_vector[j].begin();
                 while( iter !=  growableregion_vector[j].end() )
                 {
                      dcmCoordinate p1 = iter->Get_nDCMCoordinate(); 
                      float si1 = iter->GetnSI();   
                      float cv1 = iter->GetnCV();                   
                      for (k = 0; k < num_regions; k++)
                      {
                            dcmCoordinate p2 = clustermatrix[k].Get_nDCMCoordinate();
                            float si2 = clustermatrix[k].GetnSI();
                            float cv2 = clustermatrix[k].GetnCV();
                            float tmp_mem =  membershipmatrix[m*num_regions+k];
                            float coordinate_part = (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) + (p1[2]-p2[2])*(p1[2]-p2[2]);
                            float si_part = (si1 - si2) * (si1 - si2);
                            float cv_part = (cv1 - cv2) * (cv1 - cv2) ;
                            J2 = J2 + sqrt( coordinate_part + si_part + cv_part ) * tmp_mem * tmp_mem; 
                      }
                      ++iter; 
                      ++m;
                 }
            }

            if( abs(J2 - J1) < 100 )
                 break;   
           
            //(3) Update memberfunction for each voxel
            m = 0;
            for(i = 0; i < num_regions; i++)
            {            
                 iter = growableregion_vector[i].begin();
                 for (; iter != growableregion_vector[i].end(); ++iter)
                 {
                       dcmCoordinate p1 = iter->Get_nDCMCoordinate();   
                       float si1 = iter->GetnSI();   
                       float cv1 = iter->GetnCV();                           
                       BinaryImageType::IndexType c1 = iter->GetVoxelIndex();  
                       
                       for(j = 0; j < num_regions; j++)
                       {
                             float unew = 0;
                             dcmCoordinate p2 = clustermatrix[j].Get_nDCMCoordinate();  // ccc means current cluster center
                             float si2 =  clustermatrix[k].GetnSI();
                             float cv2 = clustermatrix[k].GetnCV();                             
                             BinaryImageType::IndexType cc_index = clustermatrix[j].GetVoxelIndex(); 

                             if( cc_index[0] == c1[0] && cc_index[1] == c1[1] && cc_index[2] == c1[2] )
                             {
                                  membershipmatrix[m*num_regions+j] = 1;
                             }
                             else
                             {
                                  float distance1 = sqrt( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) + (p1[2]-p2[2])*(p1[2]-p2[2]) + (si1-si2)*(si1-si2) + (cv1-cv2)*(cv1-cv2) );

                                  for(int q = 0; q < num_regions; q++)
                                  {
                                        dcmCoordinate p3 = clustermatrix[q].GetDCMCoordinate();
                                        float si3 =  clustermatrix[q].GetnSI();
                                        float cv3 = clustermatrix[q].GetnCV();
                                        float distance2 = sqrt( (p1[0]-p3[0])*(p1[0]-p3[0]) + (p1[1]-p3[1])*(p1[1]-p3[1]) + (p1[2]-p3[2])*(p1[2]-p3[2]) + (si1-si3)*(si1-si3) + (cv1-cv3)*(cv1-cv3) );
                                        unew += distance1 / distance2;
                                  }
                                  membershipmatrix[m*num_regions+j] = (1/unew);
                             }                              
                       } 
                       m++;
                 }
            } 
        } 

       float memberthreshold = 0.02;
       m = 0;
       for(i = 0; i < num_regions; i++)
       {
             iter = growableregion_vector[i].begin();
             for (; iter != growableregion_vector[i].end();)
             {
                   if(membershipmatrix[m*num_regions+i] < memberthreshold)
                        growableregion_vector[i].erase(iter++);
                   else
                     ++iter;

                   m++;                   
             }
       } 


       /* Step 4: Remove the cluster with volume less than 30mm^3 */
       
       BinaryImageType::SpacingType spacing_D = m_ColonImage->GetSpacing();
      
       cout << "Begin to shrink set." << endl;
       finalout << "Begin to shrink set." << endl;

       double x = spacing_D[0];
       double y = spacing_D[1]; 
       double z = spacing_D[2];
       cout << "X: " << x << "  Y: " << y << "  Z: " << z << endl; 
       finalout << "X: " << x << "  Y: " << y << "  Z: " << z << endl; 

       double volume = x * y * z;
       int num_voxels = int (30/volume);
       num_regions = growableregion_vector.size();

       for(i = 0; i < num_regions; i++)
       {
             int counter = 0;
             iter = growableregion_vector[i].begin();
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   counter++;
             }
             if (counter < num_voxels)
             {
                  growableregion_vector.erase(growableregion_vector.begin()+i); 
                  i--;
                  num_regions--;
             }
       } 
     
       /* Step 5: Directional Concentration */

       dcmCoordinate center_polyp;
       dcmCoordinate iter_point;

       double sum_SI = 0;
       double sum_CV = 0;
       double max_SI = 0;
       double min_SI = 0;
       double max_CV = 0;
       double min_CV = 0;
       double mean_SI = 0;
       double mean_CV = 0;
       double var_SI = 0;
       double var_CV = 0;
       double std_SI = 0;
       double std_CV = 0;
       double skew_SI = 0;
       double skew_CV = 0;
       double kurt_SI = 0;
       double kurt_CV = 0;
       double contrast_SI = 0;
       double contrast_CV = 0;
      
       num_regions = growableregion_vector.size();
       float polypsdata[num_regions][18];
       m = 0;

       for(i = 0; i < num_regions; i++)
       {   
             iter = growableregion_vector[i].begin();
             
             /* Initialize the following parameters according to each polyp candidate */
             int counter = 0;
             center_polyp[0] = 0;
             center_polyp[1] = 0;
             center_polyp[2] = 0;
             sum_SI = 0;
             sum_CV = 0;
             max_SI = 0;
             min_SI = 1;
             max_CV = 0;
             min_CV = 1;

             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   /* Step 7 part */
                   iter_point = iter->GetDCMCoordinate();
                   center_polyp[0] += iter_point[0];
                   center_polyp[1] += iter_point[1];
                   center_polyp[2] += iter_point[2];
                   counter++;

                   /* Step 8 part */
                   sum_SI += iter->GetSI();
                   sum_CV += iter->GetCV();

                   if( max_SI < iter->GetSI() )
                       max_SI = iter->GetSI();
                   if( min_SI > iter->GetSI() )
                       min_SI = iter->GetSI();

                   if( max_CV < iter->GetCV() )
                       max_CV = iter->GetCV();                   
                   if( min_CV > iter->GetCV() )
                       min_CV = iter->GetCV();

             }

             /* Step 7 part */
             center_polyp[0] = center_polyp[0]/counter;
             center_polyp[1] = center_polyp[1]/counter;
             center_polyp[2] = center_polyp[2]/counter;
             PolypCenterCollector.push_back(center_polyp);

             /* Step 8 part */
             mean_SI = sum_SI/counter;
             mean_CV = sum_CV/counter;

             /* Calculate variance */
             iter = growableregion_vector[i].begin();
             double tmp_SI = 0;
             double tmp_CV = 0;
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   tmp_SI += ( (iter->GetSI()-mean_SI) * (iter->GetSI()-mean_SI) );
                   tmp_CV += ( (iter->GetCV()-mean_CV) * (iter->GetCV()-mean_CV) );
             }
             var_SI = tmp_SI/(counter-1);
             var_CV = tmp_CV/(counter-1);
             std_SI = sqrt(var_SI);
             std_CV = sqrt(var_CV);
             

             /* Calculate skew */
             double tmp_SI_kurt = 0;
             double tmp_CV_kurt = 0;
             iter = growableregion_vector[i].begin();
             tmp_SI = 0;
             tmp_CV = 0;
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   tmp_SI += ( ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) );
                   tmp_CV += ( ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) );                                      
                   tmp_SI_kurt += ( ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) );
                   tmp_CV_kurt += ( ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) );
             }
             skew_SI = tmp_SI/counter;
             skew_CV = tmp_CV/counter;
             kurt_SI = tmp_SI_kurt/counter - 3;
             kurt_CV = tmp_CV_kurt/counter - 3;

             contrast_SI = min_SI/max_SI;
             contrast_CV = min_CV/max_CV;

             polypsdata[m][0]  = (i+1);
             polypsdata[m][1]  = center_polyp[0];
             polypsdata[m][2]  = center_polyp[1];
             polypsdata[m][3]  = center_polyp[2];
             polypsdata[m][4]  = mean_SI;
             polypsdata[m][5]  = max_SI;
             polypsdata[m][6]  = min_SI;
             polypsdata[m][7]  = var_SI;
             polypsdata[m][8]  = skew_SI;
             polypsdata[m][9]  = kurt_SI;
             polypsdata[m][10] = contrast_SI;
             polypsdata[m][11] = mean_CV;
             polypsdata[m][12] = max_CV;
             polypsdata[m][13] = min_CV;
             polypsdata[m][14] = var_CV;
             polypsdata[m][15] = skew_CV;
             polypsdata[m][16] = kurt_CV;
             polypsdata[m][17] = contrast_CV;            

             m++;
       }


       /* Step 6: Generate mat4 and vtk files */
       cout << "Writing Feature data into mat files..." << endl;
       finalout << "Writing Feature data into mat files..." << endl;   

       if( growableregion_vector.size() != 0 )
       {
            string mat4voxels = "";
            mat4voxels.append(UserNamedOutput);
            mat4voxels.append("ExtractedVoxels.mat");             
            char buf_name1[mat4voxels.size()];
            for(int iii = 0; iii < mat4voxels.size(); iii++)
            {
                 buf_name1[iii] = mat4voxels[iii];
            }
            buf_name1[mat4voxels.size()] = '\0';
            MAT4FeatureVector(growableregion_vector, buf_name1);

            string mat4polyps = "";
            mat4polyps.append(UserNamedOutput);
            mat4polyps.append("ExtractedPolyps.mat");
            char buf_name2[mat4polyps.size()];
            for(int iii = 0; iii < mat4polyps.size(); iii++)
            {
                 buf_name2[iii] = mat4polyps[iii];
            }
            buf_name2[mat4polyps.size()] = '\0';
            MAT4FeatureVectorPolyps(polypsdata, num_regions, buf_name2); 
       }



       /* Write new polyp candidates scheme to the output VTK file */

       typedef itk::ImageFileWriter< BinaryImageType > SegDataWriterType; 
       SegDataWriterType::Pointer segwriter = SegDataWriterType::New();

       string vtkoutput = "";
       vtkoutput.append(UserNamedOutput);
       vtkoutput.append("_output.vtk");
       segwriter->SetFileName(vtkoutput);
     
       typedef itk::ImageRegionIterator< BinaryImageType>  IteratorType;
       start3 = clock();

       BinaryIteratorType::RadiusType radius2;
       radius2.Fill(1);
       BinaryIteratorType input1(radius2, m_ColonImage,m_ColonImage->GetRequestedRegion() );
    
       BinaryImageType::Pointer output = BinaryImageType::New();
       output->SetRegions(m_ColonImage->GetRequestedRegion());   
       output->SetSpacing(m_ColonImage->GetSpacing());
       output->SetOrigin(m_ColonImage->GetOrigin());    
       output->Allocate();
       IteratorType outvtk(output, m_ColonImage->GetRequestedRegion());  
    
       int polypadder = 101;
 
       for (input1.GoToBegin(), outvtk.GoToBegin(); !input1.IsAtEnd(); ++input1, ++outvtk)
       {

            if(input1.GetPixel(center) == 0)
	           {	
	                 int count_v = 0;
	                 if(input1.GetPixel(right) == 255) count_v += 1;
	                 if(input1.GetPixel(left) == 255) count_v += 1;
	                 if(input1.GetPixel(bottom) == 255) count_v += 1;
	                 if(input1.GetPixel(top) == 255) count_v += 1;
	                 if(input1.GetPixel(front) == 255) count_v += 1;
	                 if(input1.GetPixel(back) == 255) count_v += 1;
                  if(count_v != 0) 
                     outvtk.Set(100);
                  else
                     outvtk.Set(1);
            }else 
                  outvtk.Set(0);  
       }


       for(i = 0; i < growableregion_vector.size(); i++)
       {
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
	           {
                  BinaryImageType::IndexType idx_voxel = iter->GetVoxelIndex();
                  output->SetPixel(idx_voxel,(polypadder+i));
	           }	    
       }
      
       segwriter->SetInput(output);
       segwriter->Update();
       cout << vtkoutput << " is generated!\n" << endl; 
       finalout << vtkoutput << " is generated!\n" << endl;   
       
       end3 = clock();
       double elapsed3 = ((double) (end3 - start3)) / CLOCKS_PER_SEC;
       cout << "Time for generating new vtk file: " << elapsed3/3600 << " hours" << endl;       
       finalout << "Time for generating new vtk file: " << elapsed3/3600 << " hours" << endl;  

       finalout.close(); 



   }
       
}




