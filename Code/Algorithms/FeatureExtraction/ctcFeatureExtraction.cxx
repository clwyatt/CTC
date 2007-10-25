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
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "MAT4Converter.h" 
#include "StringtoDouble.h"

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
        BinaryImageType::IndexType pindex;
        BinaryImageType::IndexType idx, idx2;
        AssociatedFeatureList afl; 
        int i,j,k,l,m,n;

        /* Stacked vector --- Manipulating all the regions in dataset */
        RegionCollectorType growableregion_vector;


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


           /* Step 1 Read the SI,CV & coordinate data from dataset */

        cout << "Polyp seed region generating ......" << endl;

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
                  num_parameters++; 
             } else if( num_parameters == 5 )
             {
                  afl.SetGmag(tmp);
                  num_parameters++; 
             } else if( num_parameters == 6 )
             {
                  pindex[0] = (int) tmp;
                  num_parameters++; 
             } else if( num_parameters == 7 )
             {
                  pindex[1] = (int) tmp;
                  num_parameters++; 
             } else if( num_parameters == 8 )
             {
                  pindex[2] = (int) tmp;
                  num_parameters = 0; 
                  afl.SetVoxelIndex(pindex);
                  Raw_Region.push_back(afl);
                  num_v++; 
                  if (afl.GetSI() < 1 && afl.GetSI() > 0.9 && afl.GetCV() < 0.2 && afl.GetCV() > 0.08)
                  {
                       Seed_Region.push_back(afl); 
                  }
             }           
          
        }   
        filereader.close();

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
        //cout << "Creating 125-element-cube" << endl;     
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
        

        typedef itk::Image<float, 3> FloatImageType;
        typedef itk::CropImageFilter<CTCImageType, FloatImageType> CropFilterType;
        CropFilterType::Pointer crop = CropFilterType::New();
        ctc::CTCImageType::SizeType fullsize = m_Image->GetLargestPossibleRegion().GetSize();
        crop->SetInput(m_Image);
        typedef itk::HessianRecursiveGaussianImageFilter<FloatImageType> HessianFilterType;      
        typedef HessianFilterType::OutputImageType HessianImageType;
        typedef HessianFilterType::OutputPixelType HessianPixelType;
        HessianFilterType::Pointer hessian = HessianFilterType::New();
        hessian->SetSigma(1.0);
        HessianPixelType H;

        typedef itk::GradientRecursiveGaussianImageFilter<FloatImageType> GradientFilterType;
        typedef GradientFilterType::OutputImageType GradientImageType;
        typedef GradientFilterType::OutputPixelType GradientPixelType;
        GradientFilterType::Pointer gradient = GradientFilterType::New();
        gradient->SetSigma(1.0);
        GradientPixelType g;

        float v1[3];
        float f1[6];
        itk::Size<3> start, end;
        float E,F,G,L,M,N,R1,hmag,P1,P2,K,H1,K1,K2,SI,CV,gmag;

        int SeedRegionTracker = 0;

        // BinaryIteratorType it(radius, m_ColonImage, m_ColonImage->GetRequestedRegion());
  
        cout << "Entering growableregion formation ......" << endl;    

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
            dcmCoordinate tracker;

            fl = Seed_Region[SeedRegionTracker];
            tracker = fl.GetDCMCoordinate();

            if ( abs(pdcm[0]-tracker[0]) <= 0.01 && abs(pdcm[1]-tracker[1]) <= 0.01 && abs(pdcm[2]-tracker[2]) <= 0.01 )
            {                     
                  SeedRegionTracker++;
                  goto begincalculation;
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
                if(it.GetPixel(offsets[i]) != 0)
                     continue;

                m_ColonImage->TransformIndexToPhysicalPoint(idx2,neighborpdcm);

	               start[0] = idx2[0]-4;
	               start[1] = idx2[1]-4;
	               start[2] = idx2[2]-4;
	               end[0] = fullsize[0] - (idx2[0]+4) - 1;
	               end[1] = fullsize[1] - (idx2[1]+4) - 1;
	               end[2] = fullsize[2] - (idx2[2]+4) - 1;	    
	               crop->SetLowerBoundaryCropSize(start);      
	               crop->SetUpperBoundaryCropSize(end);
	               crop->UpdateLargestPossibleRegion();
                
          	     // compute the Hessian, scale = 1.0;
	               hessian->SetInput(crop->GetOutput());
	               hessian->Update();
	               H = hessian->GetOutput()->GetPixel(idx);

                f1[0] = H[0];   //hxx
                f1[1] = H[1];   //hxy
                f1[2] = H[2];   //hxz
                f1[3] = H[3];   //hyy
                f1[4] = H[4];   //hyz
                f1[5] = H[5];   //hzz

                gradient->SetInput(crop->GetOutput());
	               gradient->Update();
           	    g = gradient->GetOutput()->GetPixel(idx);
                gmag = sqrt(g[0]*g[0]+g[1]*g[1]+g[2]*g[2]); 

                E = 1 + (g[0]*g[0])/(g[2]*g[2]);
                F = (g[0]*g[1])/(g[2]*g[2]);
                G = 1 + (g[1]*g[1])/(g[2]*g[2]); 
                hmag = g[0]*g[0] + g[1]*g[1] + g[2]*g[2];
                R1 = g[2]*g[2]*sqrt(hmag);
                L = (2*g[0]*g[1]*f1[2] - g[0]*g[0]*f1[5] - g[2]*g[2]*f1[0]) / R1;
                M = (g[0]*g[2]*f1[4] + g[1]*g[2]*f1[2] - g[0]*g[1]*f1[5] - g[2]*g[2]*f1[1]) / R1;
                N = (2*g[1]*g[2]*f1[4] - g[1]*g[1]*f1[5] - g[2]*g[2]*f1[3]) / R1;
                K = (L*N-M*M) / (E*G-F*F);
                H1 = (E*N-2*F*M+G*L) / (2*(E*G-F*F));
                SI = 0.5 - 0.31831*atan((K1+K2)/(K1-K2));
                CV = sqrt(K1*K1/2 + K2*K2/2);

                if( SI > 0.8 && SI < 1.0 && CV > 0.05 && CV < 0.25 )
                {
                      AssociatedFeatureList fl_tmp;
                      fl_tmp.SetDCMCoordinate(neighborpdcm);
                      fl_tmp.SetSI(SI);
                      fl_tmp.SetCV(CV);
                      fl_tmp.SetGmag(gmag);
                      fl_tmp.SetVoxelIndex(idx2);
                      agr.push_back(fl_tmp); 
                }
            }
            growableregion_vector.push_back(agr);
            ++it;      
        }


        GrowableRegionType::iterator iter; 

        for(i = 0; i < growableregion_vector.size(); i++)     
        {
             
             iter = growableregion_vector[i].begin();
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   BinaryImageType::IndexType pointindex = iter->GetVoxelIndex();
                 
                   if( pointindex[0] >= 512 || pointindex[1] >= 512 || pointindex[2] >= 462 )
                   {                      
                           if( pointindex[0] >= 512 )
                               pointindex[0] = (int) (pointindex[0]/10);
                           if( pointindex[1] >= 512 )
                               pointindex[1] = (int) (pointindex[1]/10);
                           if( pointindex[2] >= 462 )
                               pointindex[2] = (int) (pointindex[2]/10);
                           iter->SetVoxelIndex(pointindex);                           
                   }
             }
             
        } 




           /* Step 3: Merging different detections on the same polyp candidate */

        cout << "Merging multiple detections on the same polyp candidate ......" << endl;      
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
                              float distance = sqrt( (c1[0]-c2[0])*(c1[0]-c2[0]) + (c1[1]-c2[1])*(c1[1]-c2[1]) + (c1[2]-c2[2])*(c1[2]-c2[2]) ); 
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
     
        

           // Output contents in growableregion_vector into a txt file for debugging 
   /*     ofstream out("GrowableRegion.txt");
        int counter = 1;

        for(i = 0; i < growableregion_vector.size(); i++)     
        {
             
             iter = growableregion_vector[i].begin();
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   dcmCoordinate pointdcm = iter->GetDCMCoordinate(); 
                   BinaryImageType::IndexType pointindex = iter->GetVoxelIndex();
                   cout << "Region: " << counter << endl; 
                   out << pointdcm[0] << endl;
                   out << pointdcm[1] << endl;
                   out << pointdcm[2] << endl;
                   out << iter->GetSI() << endl;
                   out << iter->GetCV() << endl;
                   out << iter->GetGmag() << endl;
                   out << pointindex[0] << endl;
                   out << pointindex[1] << endl;
                   out << pointindex[2] << endl;                   
             }
             counter++;
             out << "******" << endl;
        } 
        out.close();*/

             // Write the data into growableregion_vector for debugging  
       /* ifstream filereader2( "GrowableRegion.txt" );
        num_parameters = 0;
        GrowableRegionType miner;

        while (! filereader2.eof() )
        {
             string dumper = "";
             getline (filereader2,dumper); 
    
             if( dumper.size() == 0 )
                  break;

             char buf[dumper.size()];

             for(int i = 0; i < dumper.size(); i++)
             {
                  buf[i] = dumper[i];
             }

             if(buf[0] == '*')
             {
                  growableregion_vector.push_back(miner);
                  miner.clear();
                  continue;
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
                  miner.push_back(afl); 
             } 
        } 
        filereader2.close(); 
*/

           
       /* Step 4: Fuzzy C-Means Algorithm */ 
          
       int num_regions = growableregion_vector.size();
       int num_points = 0;  

              /* Subpart 1 --- Normalize the feature vector */
       for(i = 0; i < growableregion_vector.size(); i++)
       {            
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
            {
                  num_points++;
                  iter->SetnSI(iter->GetSI());

                  /* Normalization of CV by the equation of d/sqrt(d*d + a)*/
                  float a = 0.01;
                  float normalized_CV = iter->GetCV() / sqrt(iter->GetCV()*iter->GetCV() + a);
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
       float J1 = 1000;
       float J2 = 0; 

       int trick = 0;
       // We need to decide this value later. 
       float objectivefunctionthreshold = 100;
       
       while ( 1 )
       {
            trick++;
            if(trick > 1)
              break;
         
            cout << "J2: " << J2 << endl;
            cout << "J1: " << J1 << endl;
            J1 = J2;
            J2 = 0;

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
                 m = 0;
                 
                 for(n = 0; n < num_regions; n++)
                 {
                      iter = growableregion_vector[n].begin();                  
                      while( iter !=  growableregion_vector[n].end() )
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
                            ++iter;
                      }

                 }  
                 updatedcoordinate[0] = updatedcoordinate[0]/tmp_val;
                 updatedcoordinate[1] = updatedcoordinate[1]/tmp_val;
                 updatedcoordinate[2] = updatedcoordinate[2]/tmp_val;

                 clustermatrix[j].SetDCMCoordinate(updatedcoordinate);

                 dcmCoordinate ad = updatedcoordinate;
                 GrowableRegionType::iterator iter_tracker;
                 float min_distance = 100; 

                 for(l = 0; l < num_regions; l++)
                 {
                      iter_tracker = growableregion_vector[l].begin();
                      while( iter_tracker !=  growableregion_vector[l].end() )
                      {
                             dcmCoordinate nd = iter_tracker->GetDCMCoordinate();
                             if( sqrt((nd[0]-ad[0])*(nd[0]-ad[0]) + (nd[1]-ad[1])*(nd[1]-ad[1]) + (nd[2]-ad[2])*(nd[2]-ad[2])) < min_distance )
                             {
                                   cout << "Yes " << endl;
                                   clustermatrix[j].SetnSI( iter_tracker->GetnSI() );
                                   clustermatrix[j].SetnCV( iter_tracker->GetnSI() );
                                   min_distance = sqrt((nd[0]-ad[0])*(nd[0]-ad[0]) + (nd[1]-ad[1])*(nd[1]-ad[1]) + (nd[2]-ad[2])*(nd[2]-ad[2]));
                             }
                             ++iter_tracker;
                      }              
                 }              
            }
           
                 /* Update memberfunction matrix */ 
            m = 0;
            for(i = 0; i < num_regions; i++)
            {            
                 iter = growableregion_vector[i].begin();
                 for (; iter != growableregion_vector[i].end(); ++iter)
                 {
                        
                       float p1[2], p2[2], p3[2];
                       float distance1 = 0;
                       float distance2 = 0;                      
                       p1[0] = iter->GetnSI();
                       p1[1] = iter->GetnCV();
                       dcmCoordinate cp = iter->GetDCMCoordinate(); // cp means current point

                       for(j = 0; j < num_regions; j++)
                       {
                             float unew = 0;
                             dcmCoordinate ccc = clustermatrix[j].GetDCMCoordinate();  // ccc means current cluster center
                             if( abs(cp[0]-ccc[0]) <= 0.01 && abs(cp[1]-ccc[1]) <= 0.01 && abs(cp[2]-ccc[2]) <= 0.01 )
                             {
                                  membershipmatrix[m][j] = 1;
                                  continue;
                             }

                             p2[0] = clustermatrix[j].GetnSI();
                             p2[1] = clustermatrix[j].GetnCV();
                             distance1 = sqrt( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) );
                             if((distance1 - 0) <= 0.001)
                                  distance1 = 0.01;
                             for(int q = 0; q < num_regions; q++)
                             {
                                  p3[0] = clustermatrix[q].GetnSI();
                                  p3[1] = clustermatrix[q].GetnCV();
                                  distance2 = sqrt( (p1[0]-p3[0])*(p1[0]-p3[0]) + (p1[1]-p3[1])*(p1[1]-p3[1]) );
                                  if ((distance2 - 0) <= 0.001)
                                      continue;
                                  unew += distance1/distance2; 
                             }                          
                                
                             membershipmatrix[m][j] = (1/unew);
                       } 
                       m++;
                 }
            } 
       
       } // end of WHILE

       cout << "Final M is " << m << endl;       
       cout << "J2: " << J2 << endl;
       cout << "J1: " << J1 << endl;

   
           /* Step 5: Remove the cluster with volume less than 35mm^3 */
           /* Due to the observation of spacing parameters on those DICOM images, each voxel has dimension 
            * x = 0.664062023162842mm
            * y = 0.664062023162842mm
            * z = 1mm */
       double x = 0.6640620231;
       double y = 0.6640620231;
       double z = 1;
       double volume = x * y * z;
       int num_voxels = int (35/volume);
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


       GrowableRegionType::iterator iter3;
       ofstream out2 ( "FinalExtraction.txt" );
       int k_voxel= 0;

       cout << "Size of growableregion_vector is " << growableregion_vector.size() << endl;
       for(int counter1=0; counter1 < growableregion_vector.size(); counter1++)     
       {
             iter3 = growableregion_vector[counter1].begin();
             out2 << "Region: " << (counter1+1) << endl;    
             int k_region = 0;      
             for (; iter3 != growableregion_vector[counter1].end(); ++iter3)
             {               
                   out2 << iter3->GetDCMCoordinate() << " " << iter3->GetVoxelIndex();
                   out2 << " " << iter3->GetSI() << " " << iter3->GetCV() << " " << iter3->GetGmag() << endl;
                   k_region++;
                   k_voxel++;
             }
             out2 << "**********************  " << k_region << "voxels  ************************" << endl;
       } 
       out2.close();
       cout << "Total number of voxels extracted: " << k_voxel << endl; 


             
          
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
      
       num_regions = growableregion_vector.size();
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

             outfile << "Polyp candidate " << (i+1) << " " << center << " --- " << endl; 
             outfile << "   Mean of shape index: " << mean_SI << endl;
             outfile << "   Max of shape index: " << max_SI << endl;
             outfile << "   Min of shape index: " << min_SI << endl;
             outfile << "   Variance of shape index: " << var_SI << endl;
             outfile << "   Skew of shape index: " << skew_SI << endl; 
             outfile << "   Kurt of shape index: " << kurt_SI << endl;
             outfile << "   Contrast of shape index: " << contrast_SI << endl;
             outfile << "   Mean of curvedness: " << mean_CV << endl;
             outfile << "   Max of curvedness: " << max_CV << endl;
             outfile << "   Min of curvedness: " << min_CV << endl;
             outfile << "   Variance of curvedness: " << var_CV << endl;
             outfile << "   Skew of curvedness: " << skew_CV << endl;
             outfile << "   Kurt of curvedness: " << kurt_CV << endl;
             outfile << "   Contrast of curvedness: " << contrast_CV << endl << endl << endl;
       }

       outfile.close();
       MAT4FeatureVector(growableregion_vector);


       /* Write new polyp candidates scheme to the output VTK file */

       typedef itk::ImageFileWriter< BinaryImageType > SegDataWriterType; 
       SegDataWriterType::Pointer segwriter = SegDataWriterType::New();
       segwriter->SetFileName(UserNamedOutputVTK);
     
       typedef itk::ImageRegionIterator< BinaryImageType>  IteratorType;


       BinaryIteratorType::RadiusType radius2;
       radius2.Fill(1);
       BinaryIteratorType input1(radius2, m_ColonImage,m_ColonImage->GetRequestedRegion() );
    
       BinaryImageType::Pointer output = BinaryImageType::New();
       output->SetRegions(m_ColonImage->GetRequestedRegion());     
       output->Allocate();
       IteratorType outvtk(output, m_ColonImage->GetRequestedRegion());  
    
       int polypadder = 101;
       BinaryImageType::IndexType idx_tmp2;
       
       for (input1.GoToBegin(), outvtk.GoToBegin(); !input1.IsAtEnd(); ++input1, ++outvtk)
       {
            dcmCoordinate dcm1;
            int data = input1.GetPixel(center);
            idx_tmp2 = input1.GetIndex();
            m_ColonImage->TransformIndexToPhysicalPoint(idx_tmp2, dcm1);

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
	                 {
                         int flag = 0;
                         for(int counter1=0; counter1 < growableregion_vector.size(); counter1++)     
                         {
                                iter3 = growableregion_vector[counter1].begin();                               
                                for (; iter3 != growableregion_vector[counter1].end(); ++iter3)
                                {                
                                      dcmCoordinate dcm2 = iter3->GetDCMCoordinate();
                                      if( abs(dcm2[0]-dcm1[0]) <= 0.03 && abs(dcm2[1]-dcm1[1]) <= 0.03 && abs(dcm2[2]-dcm1[2]) <= 0.03 )
                                      {
                                          outvtk.Set(polypadder+counter1);
                                          flag = 1;
                                      }
                                }
                         }
                         if(flag == 0)
                             outvtk.Set(100);                   
                  }else
                       outvtk.Set(1);
            }else 
                 outvtk.Set(0);      
       }



}

       
       

       
}




