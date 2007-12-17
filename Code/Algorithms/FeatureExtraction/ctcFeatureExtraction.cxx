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
#include <string>
#include <time.h>
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
       
             for(int a = -1; a < 2; a++)
             {
                  for(int b = -1; b < 2; b++)
                  {
                        for(int c = -1; c < 2; c++)
                        {
                              NeighborhoodIteratorType::OffsetType generatedoffset = {{a,b,c}};
                              offsets.push_back(generatedoffset);
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
        finalout << "Entering growableregion formation ......" << endl;  
        cout << "Size of input image dataset: " <<  fullsize << endl;
        finalout << "Size of input image dataset: " <<  fullsize << endl;
        cout << "Spacing of input image dataset: " << m_ColonImage->GetSpacing() << endl;
        finalout << "Spacing of input image dataset: " << m_ColonImage->GetSpacing() << endl;
        
        std::ofstream out3("datasetDCMSICV.txt");
        std::ofstream out4("Seedpoint.txt");

        start1 = clock();
        int voxel_tracker = 0;
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
            {
                  continue;
            }

            voxel_tracker++;
            if((voxel_tracker % 10000) == 0)
            {
                cout << voxel_tracker << endl;
                finalout << voxel_tracker << endl;
            }

            idx = it.GetIndex();


                start[0] = idx[0]-4;
	               start[1] = idx[1]-4;
	               start[2] = idx[2]-4;
	               end[0] = fullsize[0] - (idx[0]+4) - 1;
	               end[1] = fullsize[1] - (idx[1]+4) - 1;
	               end[2] = fullsize[2] - (idx[2]+4) - 1;	
               
                if(start[0] > fullsize[0] || start[1] > fullsize[1] || start[2] > fullsize[2] || end[0] > fullsize[0] || end[1] > fullsize[1] || end[2] > fullsize[2] )
                     continue; 
	          
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
                L = (2*g[0]*g[2]*f1[2] - g[0]*g[0]*f1[5] - g[2]*g[2]*f1[0]) / R1;
                M = (g[0]*g[2]*f1[4] + g[1]*g[2]*f1[2] - g[0]*g[1]*f1[5] - g[2]*g[2]*f1[1]) / R1;
                N = (2*g[1]*g[2]*f1[4] - g[1]*g[1]*f1[5] - g[2]*g[2]*f1[3]) / R1;
                K = (L*N-M*M) / (E*G-F*F);
                H1 = (E*N-2*F*M+G*L) / (2*(E*G-F*F));
                K1 = H1 + sqrt(H1*H1-K);
                K2 = H1 - sqrt(H1*H1-K);  
                SI = 0.5 - 0.31831*atan((K1+K2)/(K1-K2));
                CV = sqrt(K1*K1/2 + K2*K2/2);

                dcmCoordinate each;
                m_ColonImage->TransformIndexToPhysicalPoint(idx,each);

                out3 << each[0] << endl;
                out3 << each[1] << endl;
                out3 << each[2] << endl;                
                out3 << idx[0] << endl;
                out3 << idx[1] << endl;
                out3 << idx[2] << endl;
                out3 << SI << endl;                
                out3 << CV << endl;

            AssociatedFeatureList fl;
 
            if( SI > 0.875 && SI < 1.0 && CV > 0.07 && CV < 0.22 ) 
            {
                 fl.SetSI(SI);
                 fl.SetCV(CV);
                 fl.SetDCMCoordinate(each);
                 fl.SetGmag(gmag);
                 fl.SetVoxelIndex(idx);
                 fl.SetX( each[0] );
                 fl.SetY( each[1] );
                 fl.SetZ( each[2] );
                 fl.SetIX( idx[0] );
                 fl.SetIY( idx[1] );
                 fl.SetIZ( idx[2] );

                 out4 << each << "  " << idx << "  " <<  SI << "   " << CV << endl;
                 goto begincalculation;
            }
           

        continue;

begincalculation: 
            
            GrowableRegionType agr;
            agr.push_back(fl);

            for(i = 0; i < 27; i++)
            {
                idx2 = it.GetIndex(offsets[i]);
                dcmCoordinate neighborpdcm;   
                if(it.GetPixel(offsets[i]) != 0)
                     continue;

                if(idx2[0] == idx[0] && idx2[1] == idx[1] && idx2[2] == idx[2])
                     continue;
            
                start[0] = idx2[0]-4;
	               start[1] = idx2[1]-4;
	               start[2] = idx2[2]-4;
	               end[0] = fullsize[0] - (idx2[0]+4) - 1;
	               end[1] = fullsize[1] - (idx2[1]+4) - 1;
	               end[2] = fullsize[2] - (idx2[2]+4) - 1;

                if(start[0] > fullsize[0] || start[1] > fullsize[1] || start[2] > fullsize[2] || end[0] > fullsize[0] || end[1] > fullsize[1] || end[2] > fullsize[2] )
                     continue;    
	          
                crop->SetLowerBoundaryCropSize(start);      
	               crop->SetUpperBoundaryCropSize(end);
	               crop->UpdateLargestPossibleRegion();
                
          	     // compute the Hessian, scale = 1.0;
	               hessian->SetInput(crop->GetOutput());
	               hessian->Update();
	               H = hessian->GetOutput()->GetPixel(idx2);

                f1[0] = H[0];   //hxx
                f1[1] = H[1];   //hxy
                f1[2] = H[2];   //hxz
                f1[3] = H[3];   //hyy
                f1[4] = H[4];   //hyz
                f1[5] = H[5];   //hzz

                gradient->SetInput(crop->GetOutput());
	               gradient->Update();
           	    g = gradient->GetOutput()->GetPixel(idx2);
                gmag = sqrt(g[0]*g[0]+g[1]*g[1]+g[2]*g[2]); 

                E = 1 + (g[0]*g[0])/(g[2]*g[2]);
                F = (g[0]*g[1])/(g[2]*g[2]);
                G = 1 + (g[1]*g[1])/(g[2]*g[2]); 
                hmag = g[0]*g[0] + g[1]*g[1] + g[2]*g[2];
                R1 = g[2]*g[2]*sqrt(hmag);
                L = (2*g[0]*g[2]*f1[2] - g[0]*g[0]*f1[5] - g[2]*g[2]*f1[0]) / R1;
                M = (g[0]*g[2]*f1[4] + g[1]*g[2]*f1[2] - g[0]*g[1]*f1[5] - g[2]*g[2]*f1[1]) / R1;
                N = (2*g[1]*g[2]*f1[4] - g[1]*g[1]*f1[5] - g[2]*g[2]*f1[3]) / R1;
                K = (L*N-M*M) / (E*G-F*F);
                H1 = (E*N-2*F*M+G*L) / (2*(E*G-F*F));
                K1 = H1 + sqrt(H1*H1-K);
                K2 = H1 - sqrt(H1*H1-K);  
                SI = 0.5 - 0.31831*atan((K1+K2)/(K1-K2));
                CV = sqrt(K1*K1/2 + K2*K2/2);

                if( SI > 0.77 && SI < 1.0 && CV > 0.04 && CV < 0.25 )            
                {
                      AssociatedFeatureList fl_tmp;
                      m_ColonImage->TransformIndexToPhysicalPoint(idx2,neighborpdcm);
                      fl_tmp.SetDCMCoordinate(neighborpdcm);
                      fl_tmp.SetSI(SI);
                      fl_tmp.SetCV(CV);
                      fl_tmp.SetGmag(gmag);
                      fl_tmp.SetVoxelIndex(idx2);
                      fl_tmp.SetX( neighborpdcm[0] );
                      fl_tmp.SetY( neighborpdcm[1] );
                      fl_tmp.SetZ( neighborpdcm[2] );
                      fl_tmp.SetIX( idx2[0] );
                      fl_tmp.SetIY( idx2[1] );
                      fl_tmp.SetIZ( idx2[2] );

                      agr.push_back(fl_tmp);
                }
            }
            growableregion_vector.push_back(agr);             
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

       std::ofstream out5("Prechecking.txt");
       for(i = 0; i < growableregion_vector.size(); i++)
       {
            iter = growableregion_vector[i].begin();
            int prechecking = 0;
            for (; iter != growableregion_vector[i].end(); ++iter)
            { 
                  prechecking++; 
            }
            out5 << prechecking << endl;
       }
       out5.close();

       
       /* Re-acquire voxel index data */ 
        for(i = 0; i < growableregion_vector.size(); i++)
       {
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
            { 
                   dcmCoordinate voxel_dcm;
                   m_ColonImage->TransformIndexToPhysicalPoint(iter->GetVoxelIndex(),voxel_dcm);
                   iter->SetDCMCoordinate(voxel_dcm);
                   iter->SetX( voxel_dcm[0] );
                   iter->SetY( voxel_dcm[1] );
                   iter->SetZ( voxel_dcm[2] );
            }
       }
 

        ofstream out9("GrowableRegionBefore.txt");

        for(i = 0; i < growableregion_vector.size(); i++)     
        {
             
             iter = growableregion_vector[i].begin();
 
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   out9 << iter->GetX() << endl;
                   out9 << iter->GetY() << endl;
                   out9 << iter->GetZ() << endl;
                   out9 << iter->GetIX() << endl;
                   out9 << iter->GetIY() << endl;
                   out9 << iter->GetIZ() << endl;     
                   out9 << iter->GetSI() << endl;
                   out9 << iter->GetCV() << endl;
             }

             out9 << "******" << endl;
        } 
        out9.close();


        /* Step 2: Merging different detections on the same polyp candidate */

        cout << "Merging multiple detections on the same polyp candidate ......" << endl;     
        finalout << "Merging multiple detections on the same polyp candidate ......" << endl;       
        int merger = -1;
        int mergee = -1;
        GrowableRegionType tmp;   
        int bigsize;    
        start2 = clock();

        
starttomerge:            
        if(1)
        {

        RegionCollectorType merging_vector;
        int merging_bank[growableregion_vector.size()];
        for(i = 0; i < growableregion_vector.size(); i++)
            merging_bank[i] = 0;

        int merging_flag = 0;
        int num_merged = 0;

        cout << "Size is: " << growableregion_vector.size() << endl; 
        finalout << "Size is: " << growableregion_vector.size() << endl; 
        for(i = 0; i < growableregion_vector.size(); i++)
        {
              GrowableRegionType::iterator iter1 = growableregion_vector[i].begin();
              dcmCoordinate centroid1;
              centroid1[0] = 0;
              centroid1[1] = 0;
              centroid1[2] = 0;
              int num_member1 = 0;

              //Merging depends on the centroid distance between 2 regions 
              for (; iter1 != growableregion_vector[i].end(); ++iter1)
              {
                    dcmCoordinate c1 = iter1->GetDCMCoordinate();  
                    centroid1[0] += c1[0];
                    centroid1[1] += c1[1];
                    centroid1[2] += c1[2]; 
                    num_member1++;                   
              }
              centroid1[0] = centroid1[0] / num_member1;
              centroid1[1] = centroid1[1] / num_member1;
              centroid1[2] = centroid1[2] / num_member1;
              //cout << "Centroid is " << centroid1 << endl;

              for( j = (i+1); j < growableregion_vector.size(); j++)
              {
                    GrowableRegionType::iterator iter2 = growableregion_vector[j].begin();
                    dcmCoordinate centroid2;
                    centroid2[0] = 0;
                    centroid2[1] = 0;
                    centroid2[2] = 0;
                    int num_member2 = 0;
              
                    for (; iter2 != growableregion_vector[j].end(); ++iter2)
                    {
                            dcmCoordinate c2 = iter2->GetDCMCoordinate();  
                            centroid2[0] += c2[0];
                            centroid2[1] += c2[1];
                            centroid2[2] += c2[2]; 
                            num_member2++;          
                    }
                    centroid2[0] = centroid2[0] / num_member2;
                    centroid2[1] = centroid2[1] / num_member2;
                    centroid2[2] = centroid2[2] / num_member2;
                    float distance2 = (centroid1[0]-centroid2[0])*(centroid1[0]-centroid2[0]) + (centroid1[1]-centroid2[1])*(centroid1[1]-centroid2[1]) + (centroid1[2]-centroid2[2])*(centroid1[2]-centroid2[2]);                     
                    if(distance2 < 100)
                    {
                            merging_vector.push_back(growableregion_vector[j]);
                            merger = i;
                            merging_bank[j] = 1;
                            merging_flag = 1;
                            goto jnext;
                    }                         
jnext:              cout << "";                  
              }
              if(merging_flag == 1)
              {
                   merging_flag = 0;
                   goto merging;
              }
        }
        goto finishmerging;
      
merging:   for(m = 0; m < merging_vector.size(); m++)
           {
                tmp = merging_vector[m];
                for(n = 0; n < tmp.size(); n++)
                {                   
                      int repete = 0; 
                      GrowableRegionType::iterator iter3;
                      iter3 = growableregion_vector[merger].begin();
                      for( ; iter3 != growableregion_vector[merger].end(); ++iter3 )
                      {
                           BinaryImageType::IndexType i1 = tmp[n].GetVoxelIndex();
                           BinaryImageType::IndexType i2 = iter3->GetVoxelIndex();
                           if( i1[0] == i2[0] && i1[1] == i2[1] && i1[2] == i2[2] )
                           {
                              repete = 1;
                              break;
                           }                          
                      }                 
                      if( repete == 0 )   
                           growableregion_vector[merger].push_back(tmp[n]); 
                }                
           }

           bigsize = growableregion_vector.size();
          
           //Tricky stuff here 
           for(m = 0; m < bigsize; m++)
           {
                 if(merging_bank[m] == 1)
                 {
                       growableregion_vector.erase(growableregion_vector.begin() + (m-num_merged)); 
                       merging_bank[m] = 0;
                       num_merged++;
                 }
           } 
                  
           goto starttomerge;

finishmerging:
           cout << "Merging is done!" << endl;
           finalout << "Merging is done!" << endl;
        }       


        end2 = clock();
        double elapsed2 = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
        cout << "Time for grouping regions: " << elapsed2/3600 << " hours" << endl;       
        finalout << "Time for grouping regions: " << elapsed2/3600 << " hours" << endl;  
        

        // Output contents in growableregion_vector into a txt file for debugging 
        ofstream out("GrowableRegion.txt");
        int counter = 1;

        for(i = 0; i < growableregion_vector.size(); i++)     
        {
             
             iter = growableregion_vector[i].begin();
             //cout << "Region: " << counter << endl; 
             for (; iter != growableregion_vector[i].end(); ++iter)
             {
                   dcmCoordinate pointdcm = iter->GetDCMCoordinate(); 
                   BinaryImageType::IndexType pointindex = iter->GetVoxelIndex();
                   iter->SetX( pointdcm[0] );
                   iter->SetY( pointdcm[1] );
                   iter->SetZ( pointdcm[2] );
                   iter->SetIX( pointindex[0] );
                   iter->SetIY( pointindex[1] );
                   iter->SetIZ( pointindex[2] );
                   out << iter->GetX() << endl;
                   out << iter->GetY() << endl;
                   out << iter->GetZ() << endl;
                   out << iter->GetIX() << endl;
                   out << iter->GetIY() << endl;
                   out << iter->GetIZ() << endl;
                   out << iter->GetSI() << endl;
                   out << iter->GetCV() << endl;                   
             }
             counter++;
             out << "******" << endl;
        } 
        out.close();

        cout << "GrowableRegion.txt is generated." << endl;
        finalout << "GrowableRegion.txt is generated." << endl;
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

             double tmp =  string2double(buf);iter->SetX( (float)(voxel_dcm[0]) );
                   iter->SetY( (float)(voxel_dcm[1]) );
                   iter->SetZ( (float)(voxel_dcm[2]) );
             
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

           
       /* Step 3: Fuzzy C-Means Algorithm */ 
         
       cout << "growableregion_vector Size: " << growableregion_vector.size() << endl; 
       finalout << "growableregion_vector Size: " << growableregion_vector.size() << endl; 
       int num_regions = growableregion_vector.size();
       int num_points = 0; 

       for(i = 0; i < growableregion_vector.size(); i++)
       {            
            iter = growableregion_vector[i].begin();
            for (; iter != growableregion_vector[i].end(); ++iter)
            {
                  num_points++;
            }
       }

       cout << "Total number of voxels: " << num_points << endl;
       finalout << "Total number of voxels: " << num_points << endl;

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
                  membershipmatrix[m*num_regions+i] = 1;
                  m++;
            }
       }
       

       /* Subpart 2 --- Fuzzy C-Means clustering */
       float J1 = 0;
       float J2 = 0; 
       float objectivefunctionthreshold = 100;
       int loopnum = 0; 
       //ofstream outc ( "centroid.txt" );

       while ( 1 )
       {
            if(loopnum > 10)
                break;
            else
               loopnum++;
            J1 = J2;

                 //(1) Update centroids
                 dcmCoordinate updatedcoordinate;
                 for(j = 0; j < num_regions; j++)
                 {
                       //outc << end << "Processing group " << j << " ..." << endl;
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
                                  float tmp_mem =  membershipmatrix[m*num_regions+j];
                                  p[0] = p[0] * tmp_mem * tmp_mem;
                                  p[1] = p[1] * tmp_mem * tmp_mem;
                                  p[2] = p[2] * tmp_mem * tmp_mem;
                                  updatedcoordinate[0] = updatedcoordinate[0] + p[0]; 
                                  updatedcoordinate[1] = updatedcoordinate[1] + p[1];
                                  updatedcoordinate[2] = updatedcoordinate[2] + p[2];
                                  tmp_val = tmp_val + tmp_mem * tmp_mem; 
                                  ++m;
                                  ++iter;
                            }
                       }
                       updatedcoordinate[0] = updatedcoordinate[0]/tmp_val;
                       updatedcoordinate[1] = updatedcoordinate[1]/tmp_val;
                       updatedcoordinate[2] = updatedcoordinate[2]/tmp_val;

                       BinaryImageType::IndexType updated_index;
                       bool isInside = m_ColonImage->TransformPhysicalPointToIndex(updatedcoordinate, updated_index); 

                       /*
                       if(!isInside)
                       {
                            outc << "Error! it is not inside" << endl; 
                            outc << "New coordinate: " << updatedcoordinate << endl;
                       }
                       else
                           outc << "Updated Centroid Index is " << updated_index << endl;
                       */

                       clustermatrix[j].SetDCMCoordinate(updatedcoordinate);
                       clustermatrix[j].SetVoxelIndex(updated_index);

                       GrowableRegionType::iterator iter_tracker;
                       iter_tracker = growableregion_vector[j].begin();
                       float min_distance = 400;
                       float tmp_SI = 0;
                       float tmp_CV = 0;
                       BinaryImageType::IndexType tmp_index;
                       dcmCoordinate tmpdcm;

                       for (; iter_tracker != growableregion_vector[j].end(); ++iter_tracker)
                       {
                            dcmCoordinate nd = iter_tracker->GetDCMCoordinate(); 
                            float tmp_dis = (nd[0]-updatedcoordinate[0])*(nd[0]-updatedcoordinate[0]) + (nd[1]-updatedcoordinate[1])*(nd[1]-updatedcoordinate[1]) + (nd[2]-updatedcoordinate[2])*(nd[2]-updatedcoordinate[2]);               
                            if( tmp_dis <= min_distance )
                            { 
                                  tmp_SI = iter_tracker->GetSI();
                                  tmp_CV = iter_tracker->GetCV(); 
                                  tmpdcm = nd;
                                  min_distance = tmp_dis;                                  
                            } 
                       }
                      
                       m_ColonImage->TransformPhysicalPointToIndex(tmpdcm, tmp_index);
                       //outc << "Matching index in ColonImage " << tmp_index << endl; 
                       clustermatrix[j].SetSI(tmp_SI);
                       clustermatrix[j].SetCV(tmp_CV);
                 }        
          
            //(2) Update dissimilarity function
            m = 0;
            J2 = 0;
            for(j = 0; j < num_regions; j++)
            {
                 iter = growableregion_vector[j].begin();
                 while( iter !=  growableregion_vector[j].end() )
                 {
                      dcmCoordinate p1 = iter->GetDCMCoordinate();                       
                      for (k = 0; k < num_regions; k++)
                      {
                            dcmCoordinate p2 = clustermatrix[k].GetDCMCoordinate();
                            float tmp_mem =  membershipmatrix[m*num_regions+k];
                            J2 = J2 + sqrt( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) + (p1[2]-p2[2])*(p1[2]-p2[2]) ) * tmp_mem * tmp_mem; 
                      }
                      ++iter; 
                      ++m;
                 }
            }

            if( abs(J2 - J1) < 9 )
                 break;   
           
            //(3) Update memberfunction for each voxel
            m = 0;
            for(i = 0; i < num_regions; i++)
            {            
                 iter = growableregion_vector[i].begin();
                 for (; iter != growableregion_vector[i].end(); ++iter)
                 {
                       dcmCoordinate p1 = iter->GetDCMCoordinate();   
                       BinaryImageType::IndexType c1 = iter->GetVoxelIndex();         
                       for(j = 0; j < num_regions; j++)
                       {
                             float unew = 0;
                             dcmCoordinate p2 = clustermatrix[j].GetDCMCoordinate();  // ccc means current cluster center
                             BinaryImageType::IndexType cc_index = clustermatrix[j].GetVoxelIndex(); 

                             if( cc_index[0] == c1[0] && cc_index[1] == c1[1] && cc_index[2] == c1[2] )
                             {
                                  membershipmatrix[m*num_regions+j] = 1;
                             }
                             else
                             {
                                  float distance1 = sqrt( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) + (p1[2]-p2[2])*(p1[2]-p2[2]) );
                                  for(int q = 0; q < num_regions; q++)
                                  {
                                        dcmCoordinate p3 = clustermatrix[q].GetDCMCoordinate();
                                        float distance2 = sqrt( (p1[0]-p3[0])*(p1[0]-p3[0]) + (p1[1]-p3[1])*(p1[1]-p3[1]) + (p1[2]-p3[2])*(p1[2]-p3[2]) );
                                        unew += distance1 / distance2;
                                  }
                                  membershipmatrix[m*num_regions+j] = (1/unew);
                             }                              
                       } 
                       m++;
                 }
            } 
       }

       //outc.close();
   
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
             //if (counter < num_voxels)
             if (counter < 10)
             {
                  growableregion_vector.erase(growableregion_vector.begin()+i); 
                  i--;
                  num_regions--;
             }
       } 


       /* Step 5: Output the results of feature extraction */
       cout << "Number of candidate polyps is " << growableregion_vector.size() << endl;
       finalout << "Number of candidate polyps is " << growableregion_vector.size() << endl;


       GrowableRegionType::iterator iter3;
       ofstream out2 ( "FinalExtraction.txt" );
       int k_voxel= 0;

       //cout << "Size of growableregion_vector is " << growableregion_vector.size() << endl;
       
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
       finalout << "Total number of voxels extracted: " << k_voxel << endl; 
          
    //   ofstream outfile("FinalPolypStatistics.txt");
    //   outfile << "****** Final polyp candidates feature value statistics ******\n\n";

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
/*
             outfile << "Polyp candidate " << (i+1) << " " << center_polyp << " --- " << endl; 
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
*/
             m++;
       }
//       outfile.close();



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
            MAT4FeatureVector(growableregion_vector, buf_name1);

            string mat4polyps = "";
            mat4polyps.append(UserNamedOutput);
            mat4polyps.append("ExtractedPolyps.mat");
            char buf_name2[mat4polyps.size()];
            for(int iii = 0; iii < mat4polyps.size(); iii++)
            {
                 buf_name2[iii] = mat4polyps[iii];
            }
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




