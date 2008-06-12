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
#include <stack>
#include <iterator>
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
   { }

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
              for(i = 0; i < 3; i++)   
                   centroid[i] += c[i];  
              num_member++;                   
        }

        for(i = 0; i < 3; i++)
              centroid[i] = centroid[i] / num_member;
   }

   void FeatureExtraction::CalculateDGC( RegionCollectorType &growableregion_vector )
   {
        /* Gradient Concentration */
        BinaryImageType::IndexType idx, idx2;
        int num_regions = growableregion_vector.size();
        typedef itk::Image<float, 3> FloatImageType;
        typedef itk::CropImageFilter<CTCImageType, FloatImageType> CropFilterType;
        CropFilterType::Pointer crop = CropFilterType::New();
        ctc::CTCImageType::SizeType fullsize = m_Image->GetLargestPossibleRegion().GetSize();
        crop->SetInput(m_Image);

        typedef itk::GradientRecursiveGaussianImageFilter<FloatImageType> GradientFilterType;
        typedef GradientFilterType::OutputImageType GradientImageType;
        typedef GradientFilterType::OutputPixelType GradientPixelType;
        GradientFilterType::Pointer gradient = GradientFilterType::New();
        gradient->SetSigma(1.0);
        GradientPixelType gv, gv_next, gv_head, gv_mid;

        itk::Size<3> start, end;
        float gvmag;       
        GrowableRegionType::iterator iter;
        int i,j,k; 

        for(i = 0; i < growableregion_vector.size(); i++)
        {
            iter = growableregion_vector[i].begin();
            float subpart_dgc = 0;
            float subpart_gc = 0;
            for (; iter != growableregion_vector[i].end(); ++iter)	           
            {
                  float cos_array[6][7];
                  float e_directions[6];                

                  idx = iter->GetVoxelIndex();                
                  start[0] = idx[0]-6;
	                 start[1] = idx[1]-6;
	                 start[2] = idx[2]-6;
	                 end[0] = fullsize[0] - (idx[0]+6) - 1;
	                 end[1] = fullsize[1] - (idx[1]+6) - 1;
	                 end[2] = fullsize[2] - (idx[2]+6) - 1;	                  
                  if (start[0] > fullsize[0] || start[1] > fullsize[1] || start[2] > fullsize[2] || end[0] > fullsize[0] || end[1] > fullsize[1] || end[2] > fullsize[2])
                     continue;

                  crop->SetLowerBoundaryCropSize(start);      
                  crop->SetUpperBoundaryCropSize(end);
                  crop->UpdateLargestPossibleRegion();

                  gradient->SetInput(crop->GetOutput());
                  gradient->Update();
         	        gv = gradient->GetOutput()->GetPixel(idx);
                  idx2 = idx;

                  
                  float center_mag = sqrt(gv[0]*gv[0] + gv[1]*gv[1] + gv[2]*gv[2]);
                  cos_array[0][0] = gv[0];
                  cos_array[1][0] = -gv[0];
                  cos_array[2][0] = gv[1];
                  cos_array[3][0] = -gv[1];
                  cos_array[4][0] = gv[2];
                  cos_array[5][0] = -gv[2];

                  for (j = 0; j < 6; j++)
                      cos_array[j][0] = cos_array[j][0]/center_mag;
                    
                  int direction_v = 0;                
                  for (j = 0; j < 6; j++)               
                  {
                       gv_head = gv;
                       int n = 1;
                       for (k = 1; k <= 3; k++)
                       { 
                            if ((j%2) == 0)
                               direction_v = 1;
                            else
                               direction_v = -1;
                           
                            // Neighbourhood starting from x to -x, y, -y, z, -z.
                            idx2[j/2] = idx[(j/2)] + k*direction_v;                           
                           
                       	    gv_next = gradient->GetOutput()->GetPixel(idx2);
                            gv_mid = (gv_head + gv_next)/2;
                            gv_head = gv_next;
 
			                         float a_mag_next = sqrt(gv_next[0]*gv_next[0] + gv_next[1]*gv_next[1] + gv_next[2]*gv_next[2]);
			                         float a_mag_mid = sqrt(gv_mid[0]*gv_mid[0] + gv_mid[1]*gv_mid[1] + gv_mid[2]*gv_mid[2]);
                           	float a_dot_b_next =  gv_next[j/2] * direction_v * (-1);
                            float a_dot_b_mid = gv_mid[j/2] * direction_v * (-1);
                                                   
                            cos_array[j][n++] = a_dot_b_mid/a_mag_mid;                          
                            cos_array[j][n++] = a_dot_b_next/a_mag_next;                                                     
                       }                       
                       idx2 = idx;   
                  }
                  
                  //Calculate gradient concentration here
                  for(j = 0; j < 6; j++)
                  {
                      float local_e_max = -10;
                      float tmp_add = 0;
                      for(k = 0; k <= 6; k++)
                      {
                          tmp_add += cos_array[j][k];
                          float tmp_e = tmp_add/(k+1);
                          if( local_e_max < tmp_e)
                             local_e_max = tmp_e; 
                      }
                      e_directions[j] = local_e_max; 
                  }

                  float emax_sum = 0;
                  float temp_emax[3];
                  float emax_sum2 = 0;
                  for(j = 0; j < 6; j++)
                  {
                      if(j % 2 == 0)
                      {
                           if(e_directions[j] > 0 && e_directions[j+1] > 0)
                                temp_emax[j/2] = abs(e_directions[j] - e_directions[j+1]);
                           else 
                                temp_emax[j/2] = e_directions[j] + e_directions[j+1];
                      } 
                      emax_sum2 +=  e_directions[j];               
                  }                 
                  
                  for(j = 0; j < 3; j++)                       
                       emax_sum += temp_emax[j];                  
    
                  subpart_dgc += emax_sum/6;
                  subpart_gc += emax_sum2/6;
                  iter->SetDGC(emax_sum2/6);
            }	   

       }

   }

   void FeatureExtraction::CTC_Image_104_TransformIndexToPhysicalPoint(BinaryImageType::IndexType idx, dcmCoordinate &dcm )
   {
        BinaryImageType::PointType n_origin = m_Image->GetOrigin();
        BinaryImageType::SpacingType n_spacing = m_Image->GetSpacing();
        n_origin[0] = -n_origin[0];  
        n_origin[1] = -n_origin[1];
        dcm[0] = n_origin[0] + n_spacing[0]*idx[0];
        dcm[1] = n_origin[1] + n_spacing[1]*idx[1];
        dcm[2] = n_origin[2] + n_spacing[2]*idx[2];
        dcm[0] = -dcm[0]; 
        dcm[1] = -dcm[1];        
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
        BinaryIteratorType::OffsetType T1 = {{2,0,0}}; 
        BinaryIteratorType::OffsetType T2 = {{-2,0,0}};
        BinaryIteratorType::OffsetType T3 = {{0,2,0}};
        BinaryIteratorType::OffsetType T4 = {{0,-2,0}};
        BinaryIteratorType::OffsetType T5 = {{0,0,2}};
        BinaryIteratorType::OffsetType T6 = {{0,0,-2}};

        float SI, CV, gmag;

        int SeedRegionTracker = 0;

        // BinaryIteratorType it(radius, m_ColonImage, m_ColonImage->GetRequestedRegion());
  
        cout << "Entering growableregion formation ......" << endl;  
        cout << "Size of input image dataset: " <<  GetVolumeSize() << endl;
        cout << "Spacing of input image dataset: " << m_ColonImage->GetSpacing() << endl;
        cout << "Origin of input image dataset: " << m_ColonImage->GetOrigin() << endl;
        cout << "OK" << endl;
        EVAL(GetSeries());
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
            if(it.GetPixel(T1) == 255) count += 1;
            if(it.GetPixel(T2) == 255) count += 1;
            if(it.GetPixel(T3) == 255) count += 1;
            if(it.GetPixel(T4) == 255) count += 1;
            if(it.GetPixel(T5) == 255) count += 1;
            if(it.GetPixel(T6) == 255) count += 1;

	           if(count == 0) // not on a boundary
                  continue;
 
            voxel_tracker++;
            if((voxel_tracker % 1000) == 0)
            {
                cout << voxel_tracker << endl;
            }

            idx = it.GetIndex();

            int PC_result_Seed = PrincipalCurvatures(idx,SI,CV,gmag);
            
            if( PC_result_Seed == 1 )
                continue;
            
            if( !(SI > 0 && SI < 0.15 && CV > 0.075 && CV < 0.21) )  
              continue; 

            dcmCoordinate each;
            if( GetSeries() == 102 || GetSeries() == 103 || GetSeries() == 107 ) 
                m_Image->TransformIndexToPhysicalPoint(idx,each);
            else if( GetSeries() == 104 || GetSeries() == 105 || GetSeries() == 106 )
                CTC_Image_104_TransformIndexToPhysicalPoint(idx,each);
            else
                EVAL("error");

            AssociatedFeatureList fl;
            fl.SetSI(SI);
            fl.SetCV(CV);
            fl.SetDCMCoordinate(each);
            fl.SetGmag(gmag);
            fl.SetVoxelIndex(idx);
 
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

                     
                       if( SI > 0 && SI < 0.25 && CV > 0.05 && CV < 0.25 )
                       {
                            AssociatedFeatureList fl_tmp;
                            dcmCoordinate neighborpdcm;

                            if( GetSeries() == 102 || GetSeries() == 103 || GetSeries() == 107 ) 
                                m_Image->TransformIndexToPhysicalPoint(idx2,neighborpdcm);
                            else if( GetSeries() == 104 || GetSeries() == 105 || GetSeries() == 106 )
                                CTC_Image_104_TransformIndexToPhysicalPoint(idx2,neighborpdcm);
                            else
                                EVAL("error");

                            // cout << "Index_N: " << idx2 << endl; 
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

                 if (flag_push == 0) {
                    GrowableRegion_List.push_back( GrowableRegion_Stack.top() );
                    GrowableRegion_Stack.pop();
                 }
            }

            GrowableRegion_List.pop_back();
            growableregion_vector.push_back(GrowableRegion_List);

            }        

       }
       // return;
       end1 = clock();
       double elapsed1 = ((double) (end1 - start1)) / CLOCKS_PER_SEC;
       cout << "Time for generating initial regions: " << elapsed1/3600 << " hours" << endl;       
       cout << "Size of Region Vector: " << growableregion_vector.size() << endl;


       GrowableRegionType::iterator iter; 
       for(i = 0; i < growableregion_vector.size(); i++)
       {
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
                  m_ColonImage->SetPixel(iter->GetVoxelIndex(), 0);
       }
      
        
        /* Step 2: Merging different detections on the same polyp candidate */
        cout << "Merging multiple detections on the same polyp candidate ......" << endl;     
 
        int merger = -1;
        start2 = clock();
  
        int merging_flag = 0;
        for(i = 0; i < growableregion_vector.size(); i++)
        {
	             int size_v = growableregion_vector.size();
              if (size_v % 500 == 0)	    
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
   
           /* Step 5: Remove Polyps less than 6mm */
       BinaryImageType::SpacingType spacing_D = m_ColonImage->GetSpacing();    
       cout << "Begin to shrink set." << endl;
       double x = spacing_D[0];
       double y = spacing_D[1]; 
       double z = spacing_D[2];
       double volume = x * y * z;
       int num_voxels = int (25/volume);
       int num_regions = growableregion_vector.size();
       EVAL(num_voxels);
       EVAL(num_regions);

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
       cout << "Number of candidate polyps is " << growableregion_vector.size() << endl;

       GrowableRegionType::iterator iter3;
       int k_voxel= 0;
       cout << "Size of growableregion_vector is " << growableregion_vector.size() << endl;
       for(int counter1=0; counter1 < growableregion_vector.size(); counter1++)     
       {
             iter3 = growableregion_vector[counter1].begin();
             for (; iter3 != growableregion_vector[counter1].end(); ++iter3)
                   k_voxel++;
       } 
       cout << "Total number of voxels extracted: " << k_voxel << endl;       
      
     
        for( i = 0; i < growableregion_vector.size(); i++ ){
             iter = growableregion_vector[i].begin();
             float max_distance = 0;
             for (; iter != growableregion_vector[i].end(); ++iter){
                  iter3 = growableregion_vector[i].begin();
                  for (; iter3 != growableregion_vector[i].end(); ++iter3){
                       dcmCoordinate tmp_a = iter->GetDCMCoordinate();
                       dcmCoordinate tmp_b = iter3->GetDCMCoordinate();
                       float tmp_dis =  (tmp_a[0]-tmp_b[0])*(tmp_a[0]-tmp_b[0]) + (tmp_a[1]-tmp_b[1])*(tmp_a[1]-tmp_b[1]) + (tmp_a[2]-tmp_b[2])*(tmp_a[2]-tmp_b[2]); 
                       if(tmp_dis > max_distance)
                          max_distance = tmp_dis;
                  }
             }
             max_distance = sqrt(max_distance);
             if(max_distance >= 19){
                  growableregion_vector.erase(growableregion_vector.begin()+i); 
                  i--;
             }                
       }

       int groupid = 1;
       for( i = 0; i < growableregion_vector.size(); i++ ){
             iter = growableregion_vector[i].begin();
             float max_distance = 0;
             for (; iter != growableregion_vector[i].end(); ++iter){
                  iter3 = growableregion_vector[i].begin();
                  for (; iter3 != growableregion_vector[i].end(); ++iter3){
                       dcmCoordinate tmp_a = iter->GetDCMCoordinate();
                       dcmCoordinate tmp_b = iter3->GetDCMCoordinate();
                       float tmp_dis =  (tmp_a[0]-tmp_b[0])*(tmp_a[0]-tmp_b[0]) + (tmp_a[1]-tmp_b[1])*(tmp_a[1]-tmp_b[1]) + (tmp_a[2]-tmp_b[2])*(tmp_a[2]-tmp_b[2]); 
                       if(tmp_dis > max_distance)
                          max_distance = tmp_dis;
                  }
             }
             max_distance = sqrt(max_distance);
             cout << groupid << ": " << max_distance << endl;
             groupid++;
       }

       CalculateDGC(growableregion_vector);

       cout << "2. Size of growableregion_vector is " << growableregion_vector.size() << endl;
       for(int counter1=0; counter1 < growableregion_vector.size(); counter1++)     
       {
             iter3 = growableregion_vector[counter1].begin();
             for (; iter3 != growableregion_vector[counter1].end(); ++iter3)
                   k_voxel++;
       } 
       cout << "2. Total number of voxels extracted: " << k_voxel << endl;  

       double sum_SI = 0;
       double sum_CV = 0;
       double sum_DGC = 0;
       double max_SI = 0;
       double min_SI = 0;
       double max_CV = 0;
       double min_CV = 0;
       double max_DGC = 0;
       double min_DGC = 0;
       double mean_SI = 0;
       double mean_CV = 0;
       double mean_DGC = 0;
       double var_SI = 0;
       double var_CV = 0;
       double var_DGC = 0;
       double std_SI = 0;
       double std_CV = 0;
       double std_DGC = 0;
       double skew_SI = 0;
       double skew_CV = 0;
       double skew_DGC = 0;
       double kurt_SI = 0;
       double kurt_CV = 0;
       double kurt_DGC = 0;
       double contrast_SI = 0;
       double contrast_CV = 0;
       double contrast_DGC = 0;
      
       num_regions = growableregion_vector.size();
       float polypsdata[num_regions][26];
       m = 0;

       for(i = 0; i < num_regions; i++)
       {   
             iter = growableregion_vector[i].begin();
             
             /* Initialize the following parameters according to each polyp candidate */
             int counter = 0;             
             dcmCoordinate center_polyp;
             center_polyp[0] = 0;
             center_polyp[1] = 0;
             center_polyp[2] = 0;
             sum_SI = 0;
             sum_CV = 0;
             sum_DGC = 0;
             max_SI = 0;
             min_SI = 1;
             max_CV = 0;
             min_CV = 1;
             max_DGC = -100;
             min_DGC = 100;
             

             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   /* Step 7 part */
                   dcmCoordinate iter_point  = iter->GetDCMCoordinate();
                   center_polyp[0] += iter_point[0];
                   center_polyp[1] += iter_point[1];
                   center_polyp[2] += iter_point[2];
                   counter++;

                   /* Step 8 part */
                   sum_SI += iter->GetSI();
                   sum_CV += iter->GetCV();
                   sum_DGC += iter->GetDGC();

                   if( max_SI < iter->GetSI() )
                       max_SI = iter->GetSI();
                   if( min_SI > iter->GetSI() )
                       min_SI = iter->GetSI();

                   if( max_CV < iter->GetCV() )
                       max_CV = iter->GetCV();                   
                   if( min_CV > iter->GetCV() )
                       min_CV = iter->GetCV();

                   if( max_DGC < iter->GetDGC() )
                       max_DGC = iter->GetDGC();                   
                   if( min_DGC > iter->GetDGC() )
                       min_DGC = iter->GetDGC();

             }

             /* Step 7 part */
             center_polyp[0] = center_polyp[0]/counter;
             center_polyp[1] = center_polyp[1]/counter;
             center_polyp[2] = center_polyp[2]/counter;
             //PolypCenterCollector.push_back(center_polyp);

             /* Step 8 part */
             mean_SI = sum_SI/counter;
             mean_CV = sum_CV/counter;
             mean_DGC = sum_DGC/counter;

             /* Calculate variance */
             iter = growableregion_vector[i].begin();
             double tmp_SI = 0;
             double tmp_CV = 0;
             double tmp_DGC = 0;
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   tmp_SI += ( (iter->GetSI()-mean_SI) * (iter->GetSI()-mean_SI) );
                   tmp_CV += ( (iter->GetCV()-mean_CV) * (iter->GetCV()-mean_CV) );
                   tmp_DGC += ( (iter->GetDGC()-mean_DGC) * (iter->GetDGC()-mean_DGC) );
             }
             var_SI = tmp_SI/(counter-1);
             var_CV = tmp_CV/(counter-1);
             var_DGC = tmp_DGC/(counter-1);
             std_SI = sqrt(var_SI);
             std_CV = sqrt(var_CV);
             std_DGC = sqrt(var_DGC);
             

             /* Calculate skew */
             double tmp_SI_kurt = 0;
             double tmp_CV_kurt = 0;
             double tmp_DGC_kurt = 0;
             iter = growableregion_vector[i].begin();
             tmp_SI = 0;
             tmp_CV = 0;
             tmp_DGC = 0;
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   if (std_SI == 0) {
                       tmp_SI += 0;
                       tmp_SI_kurt += 0;
                   } else {
                       tmp_SI += ( ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) );
                       tmp_SI_kurt += ( ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) * ((iter->GetSI()-mean_SI)/std_SI) );
                   }

                   if (std_CV == 0) {
                       tmp_CV += 0;
                       tmp_CV_kurt += 0;
                   } else {
                       tmp_CV += ( ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) ); 
                       tmp_CV_kurt += ( ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) * ((iter->GetCV()-mean_CV)/std_CV) );                       
                   }

                   if (std_DGC == 0) {
                       tmp_DGC += 0;
                       tmp_DGC_kurt += 0;
                   } else {
                       tmp_DGC += ( ((iter->GetDGC()-mean_DGC)/std_DGC) * ((iter->GetDGC()-mean_DGC)/std_DGC) * ((iter->GetDGC()-mean_DGC)/std_DGC) );                     
                       tmp_DGC_kurt += ( ((iter->GetDGC()-mean_DGC)/std_DGC) * ((iter->GetDGC()-mean_DGC)/std_DGC) * ((iter->GetDGC()-mean_DGC)/std_DGC) * ((iter->GetDGC()-mean_DGC)/std_DGC) );
                   }                 
             }
             skew_SI = tmp_SI/counter;
             skew_CV = tmp_CV/counter;
             skew_DGC = tmp_DGC/counter;
             kurt_SI = tmp_SI_kurt/counter - 3;
             kurt_CV = tmp_CV_kurt/counter - 3;
             kurt_DGC = tmp_DGC_kurt/counter - 3;


             if (max_SI == 0) {
                contrast_SI = 0;
             } else {
                contrast_SI = min_SI/max_SI;
             }

             if (max_CV == 0) {
                contrast_CV = 0;
             } else {
                contrast_CV = min_CV/max_CV;               
             }

             if (max_DGC == 0) {
                contrast_DGC = 0;
             } else {
                contrast_DGC = min_DGC/max_DGC;            
             }
             
             polypsdata[m][0]  = (float)vid;
             polypsdata[m][1]  = (i+1);
             polypsdata[m][2]  = center_polyp[0];
             polypsdata[m][3]  = center_polyp[1];
             polypsdata[m][4]  = center_polyp[2];
             polypsdata[m][5]  = mean_SI;
             polypsdata[m][6]  = max_SI;
             polypsdata[m][7]  = min_SI;
             polypsdata[m][8]  = var_SI;
             polypsdata[m][9]  = skew_SI;
             polypsdata[m][10]  = kurt_SI;
             polypsdata[m][11] = contrast_SI;
             polypsdata[m][12] = mean_CV;
             polypsdata[m][13] = max_CV;
             polypsdata[m][14] = min_CV;
             polypsdata[m][15] = var_CV;
             polypsdata[m][16] = skew_CV;
             polypsdata[m][17] = kurt_CV;
             polypsdata[m][18] = contrast_CV; 
             polypsdata[m][19] = mean_DGC;
             polypsdata[m][20] = max_DGC;
             polypsdata[m][21] = min_DGC;
             polypsdata[m][22] = var_DGC;
             polypsdata[m][23] = skew_DGC;
             polypsdata[m][24] = kurt_DGC;
             polypsdata[m][25] = contrast_DGC;                         
             m++;
       }

       /* WRAMC Volumes */
       string series = "";
       if (GetSeries() == 102)
           series.append("/2_tagsub");
       else if(GetSeries() == 103)
           series.append("/3_tagsub");   
       else if(GetSeries() == 104)
           series.append("/4_tagsub");
       else if(GetSeries() == 105)
           series.append("/5_tagsub");
       else if(GetSeries() == 106)
           series.append("/6_tagsub");
       else if(GetSeries() == 107)
           series.append("/7_tagsub");


       cout << "Writing Feature data into mat files..." << endl;
       if( growableregion_vector.size() != 0 )
       {
            string mat4voxels = "";
            mat4voxels.append(UserNamedOutput);
            mat4voxels.append(series);           
            mat4voxels.append(".ExtractedVoxels.mat");             
            char buf_name1[mat4voxels.size()];
            for(int iii = 0; iii < mat4voxels.size(); iii++)
            {
                 buf_name1[iii] = mat4voxels[iii];
            }
            buf_name1[mat4voxels.size()] = '\0';
            MAT4FeatureVector(growableregion_vector, buf_name1);

            string mat4polyps = "";
            mat4polyps.append(UserNamedOutput);
            mat4polyps.append(series);
            mat4polyps.append(".ExtractedPolyps.mat");
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
       vtkoutput.append(series); 
       vtkoutput.append(".FeatureExtraction_output.vtk");
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
       
       end3 = clock();
       double elapsed3 = ((double) (end3 - start3)) / CLOCKS_PER_SEC;
       cout << "Time for generating new vtk file: " << elapsed3/3600 << " hours" << endl;       

   }
       
}




