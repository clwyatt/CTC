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
#include <iterator>


namespace ctc
{
   FeatureExtraction::FeatureExtraction()
   {
        m_FeatureVector = FeatureSampleType::New();
   }

   FeatureExtraction::~FeatureExtraction()
   { }
    
   void FeatureExtraction::SetFeatureVector(const FeaturePointer FeatureVector)
   {
        m_FeatureVector = FeatureVector;
   }

   void FeatureExtraction::Analyze()
   {
        using namespace std;
        ctc::PrincipleCurvatureExtraction::FeatureType fp;
        dcmCoordinate pdcm;
        BinaryImageType::IndexType idx, idx2;

        AssociatedFeatureList afl; 
        int i,j,k,l,m,n;

        for(i = 0; i < m_FeatureVector->Size(); i++)
        {
              fp = m_FeatureVector->GetMeasurementVector(i);

              cout << fp[0] << "," << fp[1]  << "," << fp[2] << "," << fp[3] << "," << fp[4] << endl;

              pdcm[0] = fp[0];
              pdcm[1] = fp[1];
              pdcm[2] = fp[2];

              afl.SetDCMCoordinate(pdcm);
              afl.SetSI(fp[3]);
              afl.SetCV(fp[4]);

              /* Step 1: Selection of "Seed Region" by threshold values */
              if (fp[3] < 1 && fp[3] > 0.9 && fp[4] < 0.2 && fp[4] > 0.08)
              {
                   Seed_Region.push_back(afl); 
              }
 
              Raw_Region.push_back(afl); 
        }


        /* Step 2: Setup Growable Region by threshold values */
         
        typedef itk::ConstNeighborhoodIterator< BinaryImageType >   BinaryIteratorType;
        typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< BinaryImageType >   FaceCalculatorType;   
        typedef itk::ConstNeighborhoodIterator< BinaryImageType > NeighborhoodIteratorType; 

        BinaryIteratorType::RadiusType radius;
        radius.Fill(2);

        std::vector<NeighborhoodIteratorType::OffsetType> offsets(125);
        i = 0;
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

               
        /* Vector of Vector --- Manipulating all the regions in dataset */
        RegionCollectorType growableregion_vector;

        /* Do I need to add FaceCalculatorType here? */
        BinaryIteratorType it(radius, m_ColonImage, m_ColonImage->GetRequestedRegion());
        it.GoToBegin();       
        while( !it.IsAtEnd() )
        {
            idx = it.GetIndex();
            m_ColonImage->TransformIndexToPhysicalPoint(idx,pdcm);
            AssociatedFeatureList fl;

            for(i = 0; i < Seed_Region.size(); i++)
            { 
                dcmCoordinate tracker;
                //fl = *(Seed_Region.begin()+i);
                fl = Seed_Region[i];
                tracker = fl.GetDCMCoordinate();
                if( pdcm[0] == tracker[0] && pdcm[1] == tracker[1] && pdcm[2] == tracker[2] )
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
                m_ColonImage->TransformIndexToPhysicalPoint(idx2,neighborpdcm);
                
                for(i = 0; i < Raw_Region.size(); i++)
                {
                      dcmCoordinate tracker2;

                      /* Double confirm here */
                      AssociatedFeatureList fl_tmp = Raw_Region[i];
                      tracker2 = fl_tmp.GetDCMCoordinate();                      

                      if( neighborpdcm[0] == tracker2[0] && neighborpdcm[1] == tracker2[1] && neighborpdcm[2] == tracker2[2] )
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
      
         


   }

       
       

       
}




