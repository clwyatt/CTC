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
#include <fstream>
#include "StringtoDouble.h"
#include "itkImageFileReader.h"

namespace ctc
{
   FeatureExtraction::FeatureExtraction()
   {
        m_FeatureVector = FeatureSampleType::New();
   }

   FeatureExtraction::~FeatureExtraction()
   { }
    
   void FeatureExtraction::Analyze()
   {
        using namespace std;
        ctc::PrincipleCurvatureExtraction::FeatureType fp;
        dcmCoordinate pdcm;
        BinaryImageType::IndexType idx, idx2;

      //  ofstream out3("datasetDCMSICV.txt");
      //  ofstream out4("numpoints.txt");
        
        AssociatedFeatureList afl; 
        int i,j,k,l,m,n;

    /*     for(i = 0; i < m_FeatureVector->Size(); i++)
        {
              fp = m_FeatureVector->GetMeasurementVector(i);

              cout << fp[0] << "," << fp[1]  << "," << fp[2] << "," << fp[3] << "," << fp[4] << endl;

              pdcm[0] = fp[0];
              pdcm[1] = fp[1];
              pdcm[2] = fp[2];

              afl.SetDCMCoordinate(pdcm);
              afl.SetSI(fp[3]);
              afl.SetCV(fp[4]);

              // Step 1: Selection of "Seed Region" by threshold values 
              if (fp[3] < 1 && fp[3] > 0.9 && fp[4] < 0.2 && fp[4] > 0.08)
              {
                   Seed_Region.push_back(afl); 
              }
 
              Raw_Region.push_back(afl); 
        }*/

        ifstream filereader( "datasetDCMSICV.txt" );
        int num_parameters = 0;
        int num_v = 1;

        while (! filereader.eof() )
        {
             string dumper = "";
             getline (filereader,dumper); 
    
             if( dumper.size() == 0 )
                  break;

             char buf[dumper.size()];

             for(int i = 0; i < dumper.size(); i++)
             {
                  buf[i] = dumper[i];
             }

             double tmp =  string2double(buf);
             
             if( num_parameters == 0 )
             {
                  pdcm[0] = tmp;
                  num_parameters++;
             } else if( num_parameters == 1 )
             {   
                  pdcm[1] = tmp;
                  num_parameters++;
             } else if( num_parameters == 2 )
             {
                  pdcm[2] = tmp;
                  num_parameters++;
                  afl.SetDCMCoordinate(pdcm);
             } else if( num_parameters == 3 )
             {
                  afl.SetSI(tmp);
                  num_parameters++;
             } else if( num_parameters == 4 )
             {
                  afl.SetCV(tmp);
                  num_parameters = 0;
                  Raw_Region.push_back(afl); 
                 // cout << "Voxel: " << num_v << endl;
                  num_v++;
                  if (afl.GetSI() < 1 && afl.GetSI() > 0.8 && afl.GetCV() < 0.2 && afl.GetCV() > 0.05)
                  {
                       Seed_Region.push_back(afl); 
                  }
             } 
        }   
      
        

        /* Step 2: Setup Growable Region by threshold values */
         
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

        std::vector<NeighborhoodIteratorType::OffsetType> offsets(125);
        i = 0;

        cout << "Creating 125-element-cube" << endl;


        /* Improvement is needed here */
        while(i < 125)  
        { 
             for(int a = -2; a < 3; a++)
             {
                  for(int b = -2; b < 3; b++)
                  {
                        for(int c = -2; c < 3; c++)
                        {
                              NeighborhoodIteratorType::OffsetType generatedoffset = {{a,b,c}};
                              offsets[i] = generatedoffset;
                              i++;
                        }
                  }
             }
        }

        BinaryIteratorType::OffsetType center = {{0,0,0}};
        BinaryIteratorType::OffsetType right = {{1,0,0}};
        BinaryIteratorType::OffsetType left = {{-1,0,0}};
        BinaryIteratorType::OffsetType bottom = {{0,1,0}};
        BinaryIteratorType::OffsetType top = {{0,-1,0}};
        BinaryIteratorType::OffsetType front = {{0,0,1}};
        BinaryIteratorType::OffsetType back = {{0,0,-1}}; 

              
        /* Vector of Vector --- Manipulating all the regions in dataset */
        RegionCollectorType growableregion_vector;


        /* Do I need to add FaceCalculatorType here? */
//        BinaryIteratorType it(radius, m_ColonImage, m_ColonImage->GetRequestedRegion());
       
        int flag = 1;

        it.GoToBegin();       
        while( !it.IsAtEnd() )
        {
            idx = it.GetIndex();

            int count = 0;

	           if(it.GetPixel(right) == 255) count += 1;
	           if(it.GetPixel(left) == 255) count += 1;
	           if(it.GetPixel(bottom) == 255) count += 1;
	           if(it.GetPixel(top) == 255) count += 1;
	           if(it.GetPixel(front) == 255) count += 1;
	           if(it.GetPixel(back) == 255) count += 1;
	           if(count == 0) // not on a boundary
            {
                  ++it;
                  continue;
            }

            m_ColonImage->TransformIndexToPhysicalPoint(idx,pdcm);
            AssociatedFeatureList fl;

            flag++;
            for(i = 0; i < Seed_Region.size(); i++)
            { 
                dcmCoordinate tracker;
                fl = Seed_Region[i];
                tracker = fl.GetDCMCoordinate();
                   
                if ( abs(pdcm[0]-tracker[0]) <= 0.01 && abs(pdcm[1]-tracker[1]) <= 0.01 && abs(pdcm[2]-tracker[2]) <= 0.01 )
                {                     
                     goto begincalculation;
                }
            }

        ++it;
        continue;

begincalculation: 
            
            GrowableRegionType agr;
            agr.push_back(fl);

            for(i = 0; i < 125; i++)
            {
                idx2 = it.GetIndex(offsets[i]);
                dcmCoordinate neighborpdcm;             
                m_Image->TransformIndexToPhysicalPoint(idx2,neighborpdcm);
                
                for(i = 0; i < Raw_Region.size(); i++)
                {
                      dcmCoordinate tracker2;
                    
                      /* Double confirm here */
                      AssociatedFeatureList fl_tmp = Raw_Region[i];
                      tracker2 = fl_tmp.GetDCMCoordinate();                      

                      if( abs(neighborpdcm[0]-tracker2[0]) <= 0.01 && abs(neighborpdcm[1]-tracker2[1]) <= 0.01 && abs(neighborpdcm[2]-tracker2[2]) <= 0.01 )
                      {
                          float SI_val = fl_tmp.GetSI();
                          float CV_val = fl_tmp.GetCV();
                          if( SI_val > 0.8 && SI_val < 1.0 && CV_val > 0.05 && CV_val < 0.25 )
                          {
                               agr.push_back(fl_tmp);  
                          }
                      }else{
                          continue;
                      }
                }
            }

            growableregion_vector.push_back(agr);

            ++it;      
        }

   
      
        /* Step 3: Merging different detections on the same polyp candidate */

        int merger = -1;
        int mergee = -1;
        GrowableRegionType tmp;

starttomerge:             
        for(i = 0; i < growableregion_vector.size(); i++)
        {
              for( j = (i+1); j < growableregion_vector.size(); j++)
              {
                    
                    GrowableRegionType::iterator iter1 = growableregion_vector[i].begin();
                    GrowableRegionType::iterator iter2 = growableregion_vector[j].begin();

                    for (; iter1 != growableregion_vector[i].end(); ++iter1)
                    {
                          for (; iter2 != growableregion_vector[j].end(); ++iter2)
                          {
                              dcmCoordinate c1 = iter1->GetDCMCoordinate(); 
                              dcmCoordinate c2 = iter2->GetDCMCoordinate();
                              float distance = sqrt(c1[0]*c2[0] + c1[1]*c2[1] + c1[2]*c2[2]);
                              if(distance < 12.5)
                              {
                                   tmp = growableregion_vector[j];
                                   merger = i;
                                   mergee = j;
                                   goto merging;
                              }
                          }
                    }
                              
              }
        }
        goto finishmerging;
      
merging:   for(m = 0; m < tmp.size(); m++)
           {
               growableregion_vector[merger].push_back(tmp[m]);
           }     
           growableregion_vector.erase(growableregion_vector.begin() + mergee); 
           goto starttomerge;

finishmerging:


           
       /* Step 4: Fuzzy C-Means Algorithm */ 
          
       int num_regions = growableregion_vector.size();
       int num_points = 0;  
       GrowableRegionType::iterator iter;

              /* Subpart 1 --- Normalize the feature vector */

                         // We might need to add more feature vector elements. 
       for(i = 0; i < growableregion_vector.size(); i++)
       {            
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
            {
                  num_points++;
                  iter->SetnSI(iter->GetSI());
                  /* Normalization of CV by the equation of d/sqrt(d*d + a)*/

                  float a = 0.06;
                  float real_CV = iter->GetCV();
                  float normalized_CV = real_CV / sqrt(real_CV*real_CV + a);
                  iter->SetnCV(normalized_CV);                  
            }
       }

             /* Subpart 2 --- Initialize the membership matrix */ 
       
       float membershipmatrix[num_points][num_regions];
       AssociatedFeatureList clustermatrix[num_regions];

       for(i = 0; i < num_points; i++)
       {
            for(j = 0; j < num_regions; j++)
            {
                  membershipmatrix[i][j] = 0;   
            }
       }

       
       m = 0;
       for(i = 0; i < num_regions; i++)
       {            
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
            {
                  membershipmatrix[m][i] = 1;
                  m++;
            }
       }

             /* Subpart 3 --- Initialize centers of clusters */
       for(i = 0; i < num_regions; i++)
       {
            iter = growableregion_vector[i].begin();
            clustermatrix[i] = *iter;
       }    

            /* Subpart 4 --- Acquire Objective function J */
       float J1 = 0;
       float J2 = 1000; 


       // We need to decide this value later. 
       float objectivefunctionthreshold = 100;
       
       while ( (J2-J1) > objectivefunctionthreshold )
       {
            J1 = J2;

            m = 0;
            for(j = 0; j < num_regions; j++)
            {
                 iter = growableregion_vector[j].begin();
                 while( iter !=  growableregion_vector[j].end() )
                 {
                      float p1[2], p2[2];
                      p1[0] = iter->GetnSI();
                      p1[1] = iter->GetnCV();

                      for (k = 0; k < num_regions; k++)
                      {
                            p2[0] = clustermatrix[k].GetnSI();
                            p2[1] = clustermatrix[k].GetnCV();
                            J2 = J2 + sqrt( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) ) * membershipmatrix[m][k] * membershipmatrix[m][k]; 
                      }
                      ++iter; 
                      ++m;
                 }
            }

      
            /* Subpart 5 --- Iteratively update Membershipfunction and clusters' center */
            dcmCoordinate updatedcoordinate;
            dcmCoordinate p;

            m = 0;
            float tmp_val = 0;
            for(j = 0; j < num_regions; j++)
            {
                 updatedcoordinate[0] = 0;
                 updatedcoordinate[1] = 0;
                 updatedcoordinate[2] = 0;
                 tmp_val = 0;
                 iter = growableregion_vector[j].begin();

                 while( iter !=  growableregion_vector[j].end() )
                 {
                     dcmCoordinate p;
                     p = iter->GetDCMCoordinate();
                     p[0] = p[0] * membershipmatrix[m][j] * membershipmatrix[m][j];   
                     p[1] = p[2] * membershipmatrix[m][j] * membershipmatrix[m][j]; 
                     p[2] = p[2] * membershipmatrix[m][j] * membershipmatrix[m][j];
                     updatedcoordinate[0] = updatedcoordinate[0] + p[0]; 
                     updatedcoordinate[1] = updatedcoordinate[1] + p[1];
                     updatedcoordinate[2] = updatedcoordinate[2] + p[2];
                     tmp_val = tmp_val + membershipmatrix[m][j] * membershipmatrix[m][j];
                     ++m;
                 }

                 updatedcoordinate[0] = updatedcoordinate[0]/tmp_val;
                 updatedcoordinate[1] = updatedcoordinate[1]/tmp_val;
                 updatedcoordinate[2] = updatedcoordinate[2]/tmp_val;

                 clustermatrix[j].SetDCMCoordinate(updatedcoordinate);
            }
            
            
            /* Update memberfunction matrix */ 
            m = 0;
            for(i = 0; i < num_regions; i++)
            {            
                 iter = growableregion_vector[i].begin();
                 for (; iter != growableregion_vector[i].end(); ++iter)
                 {
                        
                       float p1[2], p2[2];
                       float distance1 = 0;
                       float distance2 = 0;
                       p1[0] = iter->GetnSI();
                       p1[1] = iter->GetnCV();

                       for(j = 0; j < num_regions; j++)
                       {
                             p2[0] = clustermatrix[j].GetnSI();
                             p2[1] = clustermatrix[j].GetnCV();
                             distance2 += sqrt( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) );
                       }

                       
                       for(j = 0; j < num_regions; j++)
                       {
                             p2[0] = clustermatrix[j].GetnSI();
                             p2[1] = clustermatrix[j].GetnCV();
                             distance1 = sqrt( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) );
                             float unew = distance1/distance2;
                             membershipmatrix[m][j] = unew;
                       }                                     
                       m++;
                 }
            }
       
       } // end of WHILE
            

            /* Subpart 6 --- Remove the points with membership function value less than 0.8 */
       m = 0;
       for(i = 0; i < num_regions; i++)
       {
             iter = growableregion_vector[i].begin();
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   if( membershipmatrix[m][i] < 0.8 )
                   {
                         growableregion_vector[i].erase(iter);
                         --iter;
                   }
                   m++;
             }
       } 


       /* Step 5: Remove the cluster with volume less than 35mm^3 */
           /* Due to the observation of spacing parameters on those DICOM images, each voxel has dimension 
            * x = 0.664062023162842mm
            * y = 0.664062023162842mm
            * z = 1.25mm */
       double x = 0.6640620231;
       double y = 0.6640620231;
       double z = 1.25;
       double volume = x * y * z;
       int num_voxels = int (35/volume);

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
             }
       } 


       /* Step 6: Compute the gradient concentration to reduce false positives */
        
       num_regions = growableregion_vector.size();
       cout << "Current size of GrowableRegion is " << num_regions << endl;
      
       std::vector<NeighborhoodIteratorType::OffsetType> surroundingoffsets(343);
      
       BinaryIteratorType::RadiusType radius2;
       radius2.Fill(3);
      
       i = 0;
       /* Improvement is needed here */
       while(i < 343)  
       { 
            for(int a = -3; a < 4; a++)
            {
                 for(int b = -3; b < 4; b++)
                 {
                       for(int c = -3; c < 4; c++)
                       {
                             NeighborhoodIteratorType::OffsetType generatedoffset = {{a,b,c}};
                             surroundingoffsets[i] = generatedoffset;
                             i++;
                       }
                 }
            }
       }

       /*
        * x-y  --------- 0.939mm
        * x-z, y-z ----- 1.415mm
        * x-y-z -------- 1.563mm 
        */
       float Rmin = 0;
       float Rmax = 4;

       /* Initially, I define 8*3 = 24 direction vectors. Actually, by not counting the duplicated ones, 
        * there are only 8+6+4=18 direction vectors in 3D space. */

             /* Subpart 1: Define line equations of direction vectors through point (a,b,c) */
                 /* 
                  *  Plane 1:
                     (1) y = b, z = c, x > a
                     (2) y = b, z = c, x < a
                     (3) x = a, z = c, y > b/core/Testing/Code/Algorithms/FeatureExtraction/test_ctcMAT4Converter.cxx:5:21:
                     (4) x = a, z = c, y < b
                     (5) y-b = x-a, z = c, x > a
                     (6) y-b = x-a, z = c, x < a
                     (7) y-b = a-x, z = c, x > a
                     (8) y-b = a-x, z = c, x < a

                     Plane 2:   Duplicate (1) & (2)
                     (9)  x = a, y = b, z > c
                     (10) x = a, y = b, z < c
                     (11) z-c = x-a, y = b, x > a 
                     (12) z-c = x-a, y = b, x < a
                     (13) z-c = a-x, y = b, x > a 
                     (14) z-c = a-x, y = b, x < a

                     Plane 3:   Duplicate (3),(4),(9) & (10)
                     (15) z-c = y-b, x = a, z > c
                     (16) z-c = y-b, x = a, z < c
                     (17) z-c = b-y, x = a, z > c
                     (18) z-c = b-y, x = a, z < c
                 */


        /* Step 7 --- Calculate center of polyp candidates */
        /* Step 8 --- Generate Statistics */
        /* The above two steps are implemented together */

       ofstream outfile("FinalPolypStatistics.txt");
       outfile << "****** Final polyp candidates feature value statistics ******\n\n";

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

             outfile << "Polyp candidate " << i << " --- " << "Mean of shape index: " << mean_SI << " Max of shape index: " << max_SI << " Min of shape index: " << min_SI;
             outfile << " Variance of shape index: " << var_SI << " Skew of shape index: " << skew_SI << " Kurt of shape index: " << kurt_SI << " Contrast of shape index: " << contrast_SI << endl;
             outfile << "                     Mean of curvedness: " << mean_CV << " Max of curvedness: " << max_CV << " Min of curvedness: " << min_CV;
             outfile << " Variance of curvedness: " << var_CV << " Skew of curvedness: " << skew_CV << " Kurt of curvedness: " << kurt_CV << " Contrast of curvedness: " << contrast_CV << endl;

       } 
       
       outfile.close();
        

}

       
       

       
}




