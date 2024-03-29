/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include "ctcPrincipleCurvatureExtraction.h"
#include <itkConstNeighborhoodIterator.h>
#include <itkCropImageFilter.h>
#include <itkHessianRecursiveGaussianImageFilter.h>
#include <itkGradientRecursiveGaussianImageFilter.h>
#include <itkNeighborhoodAlgorithm.h>
#include <itkSymmetricEigenSystem.h>
#include <vnl/algo/vnl_real_eigensystem.h>
#include <fstream>

//#include "itkRecursiveGaussianImageFilter.h"
//#include "itkImageDuplicator.h"
//#include "itkCastImageFilter.h"


namespace ctc
{
  
  PrincipleCurvatureExtraction::PrincipleCurvatureExtraction()
  {
    m_ColonImage = 0;
    m_Image = 0;
    m_FeatureVector = FeatureSampleType::New();
  }

  PrincipleCurvatureExtraction::~PrincipleCurvatureExtraction()
  {
  }
  
  void  PrincipleCurvatureExtraction::SetBinaryInput(const BinaryImageType * image)
  {
    m_ColonImage = image;
  }

  void  PrincipleCurvatureExtraction::SetImageInput(const CTCImageType * image)
  {
    m_Image = image;
  }
  
  void PrincipleCurvatureExtraction::Compute()
  {
    typedef itk::NeighborhoodAlgorithm
      ::ImageBoundaryFacesCalculator< BinaryImageType > FaceCalculatorType;    
    typedef itk::ConstNeighborhoodIterator<BinaryImageType> BinaryIteratorType;
    BinaryIteratorType::RadiusType radius;
    radius.Fill(1);
    BinaryIteratorType::OffsetType center = {{0,0,0}};
    BinaryIteratorType::OffsetType right = {{1,0,0}};
    BinaryIteratorType::OffsetType left = {{-1,0,0}};
    BinaryIteratorType::OffsetType bottom = {{0,1,0}};
    BinaryIteratorType::OffsetType top = {{0,-1,0}};
    BinaryIteratorType::OffsetType front = {{0,0,1}};
    BinaryIteratorType::OffsetType back = {{0,0,-1}};

    // for testing
    bool oneshot = true;


    FeatureType f;
    BinaryImageType::IndexType idx;
    itk::Point<double, 3> dcmCoordinate;
    typedef itk::Image<float, 3> FloatImageType;
    typedef itk::CropImageFilter<CTCImageType, FloatImageType> CropFilterType;
    CropFilterType::Pointer crop = CropFilterType::New();
    itk::Size<3> start, end;
    ctc::CTCImageType::SizeType fullsize = 
      m_Image->GetLargestPossibleRegion().GetSize();
    crop->SetInput(m_Image);
    typedef itk::HessianRecursiveGaussianImageFilter<FloatImageType> HessianFilterType;      
    typedef HessianFilterType::OutputImageType HessianImageType;
    typedef HessianFilterType::OutputPixelType HessianPixelType;
    HessianFilterType::Pointer hessian = HessianFilterType::New();
    hessian->SetSigma(1.0);
    HessianPixelType::EigenValuesArrayType la;
    HessianPixelType::EigenVectorsMatrixType va;
    HessianPixelType H;
    float v1[3];
    float f1[6];
    int seedpoint = 0;

    std::ofstream out3("datasetDCMSICV.txt");


/* Yuan  */
    float E,F,G,L,M,N,R1,hmag,P1,P2,K,H1,K1,K2,SI,CV;
    //typedef ctc::AssociatedFeatureList AssociatedFeatureVector;
    
    
    typedef itk::GradientRecursiveGaussianImageFilter<FloatImageType> GradientFilterType;
    typedef GradientFilterType::OutputImageType GradientImageType;
    typedef GradientFilterType::OutputPixelType GradientPixelType;
    GradientFilterType::Pointer gradient = GradientFilterType::New();
    gradient->SetSigma(1.0);
    GradientPixelType g;
    float gmag;
    float mag;
    float angle;
    float cross[3];
    typedef itk::SymmetricEigenSystem< double, 3 > SystemType3D;
    SystemType3D::MatrixType R;
    SystemType3D::MatrixType S;
    SystemType3D::MatrixType RS;
    AssociatedFeatureList afl;

    FaceCalculatorType faceCalculator;
    FaceCalculatorType::FaceListType faceList;
    BinaryIteratorType::RadiusType cropradius;
    cropradius.Fill(4);
    faceList = faceCalculator(m_ColonImage, m_ColonImage->GetRequestedRegion(),
			      cropradius);
    BinaryIteratorType bit(radius, m_ColonImage, *(faceList.begin()));
    for(bit.GoToBegin(); !bit.IsAtEnd(); ++bit)
      {
	// wall check (volume == 255 "inside" lumen
	// a 0 on that boundary indicates a wall voxel location
	if(bit.GetPixel(center) == 0)
	  {	
	    int count = 0;

	    if(bit.GetPixel(right) == 255) count += 1;
	    if(bit.GetPixel(left) == 255) count += 1;
	    if(bit.GetPixel(bottom) == 255) count += 1;
	    if(bit.GetPixel(top) == 255) count += 1;
	    if(bit.GetPixel(front) == 255) count += 1;
	    if(bit.GetPixel(back) == 255) count += 1;
	    if(count == 0) // not on a boundary
	      continue;
	   
	    if( (m_FeatureVector->Size() % 10000) == 0 && (m_FeatureVector->Size() > 0)){
      		 std::clog << m_FeatureVector->Size() << std::endl;
//		     std::clog << m_FeatureVector->GetMeasurementVector(m_FeatureVector->Size()-1) << std::endl;
	    }
 
	    idx = bit.GetIndex();
	    m_ColonImage->TransformIndexToPhysicalPoint(idx, dcmCoordinate);
	    f[0] = dcmCoordinate[0];
	    f[1] = dcmCoordinate[1];
	    f[2] = dcmCoordinate[2];     

	    // crop the region
	    start[0] = idx[0]-4;
	    start[1] = idx[1]-4;
	    start[2] = idx[2]-4;
	    end[0] = fullsize[0] - (idx[0]+4) - 1;
	    end[1] = fullsize[1] - (idx[1]+4) - 1;
	    end[2] = fullsize[2] - (idx[2]+4) - 1;	    
	    crop->SetLowerBoundaryCropSize(start);      
	    crop->SetUpperBoundaryCropSize(end);
	    crop->UpdateLargestPossibleRegion();

	    // compute the Hessian, scale = 1.0;
	    hessian->SetInput(crop->GetOutput());
	    hessian->Update();

	    // do EV decomposition
	    H = hessian->GetOutput()->GetPixel(idx);
	    H.ComputeEigenAnalysis(la, va); 
	    v1[0] = va[2][0];
	    v1[1] = va[2][1];
	    v1[2] = va[2][2];
/*
     f1[0] = va[0][0];   //hxx
     f1[1] = va[0][1];   //hxy
     f1[2] = va[0][2];   //hxz
     f1[3] = va[1][1];   //hyy
     f1[4] = va[1][2];   //hyz
     f1[5] = va[2][2];   //hzz
*/

     f1[0] = H[0];   //hxx
     f1[1] = H[1];   //hxy
     f1[2] = H[2];   //hxz

     f1[3] = H[3];   //hyy
     f1[4] = H[4];   //hyz
     f1[5] = H[5];   //hzz

 /*    
     std::cout << "H[0][0] " << H[0][0] << " H[0][1] " << H[0][1] << " H[0][2] " << H[0][2] << std::endl;
     std::cout << "H[1][0] " << H[1][0] << " H[1][1] " << H[1][1] << " H[1][2] " << H[1][2] << std::endl;
     std::cout << "H[2][0] " << H[2][0] << " H[2][1] " << H[2][1] << " H[2][2] " << H[2][2] << std::endl;
*/
/*
     std::cout << "va[0][0] " << va[0][0] << " va[0][1] " << va[0][1] << " va[0][2] " << va[0][2] << va[0][3] << std::endl;
     std::cout << "va[1][1] " << va[1][1] << " va[1][2] " << va[0][1] << " va[2][2] " << va[2][2] << va[3][2] << std::endl;
*/


    
	    // compute the gradient, same scale
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

     //P1 =   g[0]*g[0]*(f1[3]*f1[5] - f1[4]*f1[4]) + 2*g[1]*g[2]*(f1[2]*f1[1] - f1[0]*f1[0]*f1[4])
     //     + g[1]*g[1]*(f1[5]*f1[0] - f1[2]*f1[2]) + 2*g[0]*g[2]*(f1[1]*f1[4] - f1[3]*f1[3]*f1[2])
     //     + g[2]*g[2]*(f1[0]*f1[3] - f1[1]*f1[1]) + 2*g[0]*g[1]*(f1[4]*f1[2] - f1[5]*f1[5]*f1[1]);

     //P2 =   2*g[1]*g[2]*f1[4] - g[0]*g[0]*(f1[3]+f1[5])
     //     + 2*g[0]*g[2]*f1[2] - g[1]*g[1]*(f1[0]+f1[5])
     //     + 2*g[0]*g[1]*f1[1] - g[2]*g[2]*(f1[0]+f1[3]);

     /* Gaussian Curvature & Mean Curvature */
     K = (L*N-M*M) / (E*G-F*F);
     H1 = (E*N-2*F*M+G*L) / (2*(E*G-F*F));


	    // rotate the Hessian to align the first eigenvector to the
	    // gradient direction
	 /* 
     mag = sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2])*sqrt(g[0]*g[0]+g[1]*g[1]+g[2]*g[2]);
	    angle = acos((v1[0]*g[0]+v1[1]*g[1]+v1[2]*g[2])/mag);  
	    cross[0] = v1[1]*g[2]-v1[2]*g[1];
	    cross[1] = v1[2]*g[0]-v1[0]*g[2];
	    cross[2] = v1[0]*g[1]-v1[1]*g[0];
	    mag = sqrt(cross[0]*cross[0]+cross[1]*cross[1]+cross[2]*cross[2]);
	    cross[0] = cross[0]/mag;for(int counter=0;counter < Seed_Region.size(); counter++)
{
    cout << "Voxel: " << counter << endl; 
    AssociatedFeatureList tmp = Seed_Region[counter];
    out << tmp.GetDCMCoordinate() << "   ";       
    out << tmp.GetSI() << "   " << tmp.GetCV() << endl;
}

for(int counter=0;counter < Raw_Region.size(); counter++)
{
    cout << "Voxel: " << counter << endl; 
    AssociatedFeatureList tmp = Raw_Region[counter];
    out2 << tmp.GetDCMCoordinate() << "   ";       
    out2 << tmp.GetSI() << "   " << tmp.GetCV() << endl;
}

	    cross[1] = cross[1]/mag;
	    cross[2] = cross[2]/mag;
	    R[0][0] = 1 + (1-cos(angle))*(cross[0]*cross[0]-1);
	    R[0][1] = -cross[2]*sin(angle)+(1-cos(angle))*cross[0]*cross[1];
	    R[0][2] = cross[1]*sin(angle)+(1-cos(angle))*cross[0]*cross[2];
	    R[1][0] = cross[2]*sin(angle)+(1-cos(angle))*cross[0]*cross[1];
	    R[1][1] = 1 + (1-cos(angle))*(cross[1]*cross[1]-1);
	    R[1][2] = -cross[0]*sin(angle)+(1-cos(angle))*cross[1]*cross[2];
	    R[2][0] = -cross[1]*sin(angle)+(1-cos(angle))*cross[0]*cross[2];
	    R[2][1] = cross[0]*sin(angle)+(1-cos(angle))*cross[1]*cross[2];
	    R[2][2] = 1 + (1-cos(angle))*(cross[2]*cross[2]-1);
	    S[0][0] = H(0,0);
	    S[0][1] = H(0,1);
	    S[0][2] = H(0,2);
	    S[1][0] = H(1,0);
	    S[1][1] = H(1,1);
	    S[1][2] = H(1,2);
	    S[2][0] = H(2,0);
	    S[2][1] = H(2,1);
	    S[2][2] = H(2,2);
	    RS = R*S;
*/
	    // extract submatrix
/*	    vnl_matrix<double> Hs(2,2);
	    Hs[0][0] = RS[1][1];
	    Hs[0][1] = RS[1][2];
	    Hs[1][0] = RS[2][1];
	    Hs[1][1] = RS[2][2];
	    
 	    // compute the eienvalues of submatrix
	    vnl_real_eigensystem ev2d(Hs);
	    /gpfs0/santagoGrp/data/WFU_prior_CTC_data/CTC-0018/104_110637
	    // compute the principle curvatures
	     ofstream out3("datasetDCMSICV.txt");float kappa1 = -ev2d.D[0].real()/gmag;
	    float kappa2 = -ev2d.D[1].real()/gmag;

	    // f[3] = kappa1;
	    // f[4] = kappa2;
*/

     K1 = H1 + sqrt(H1*H1-K);
     K2 = H1 - sqrt(H1*H1-K);  

     SI = 0.5 - 0.31831*atan((K1+K2)/(K1-K2));
     CV = sqrt(K1*K1/2 + K2*K2/2);

     f[3] = SI;
     f[4] = CV; 
     f[5] = gmag;
     f[6] = idx[0];
     f[7] = idx[1];
     f[8] = idx[2];     

/*     if (f[3] < 1 && f[3] > 0.9 && f[4] < 0.2 && f[4] > 0.08)
     {
          afl.SetDCMCoordinate(dcmCoordinate);   
          afl.SetSI(SI);
          afl.SetCV(CV);
          afl.SetGmag(gmag);
          afl.SetVoxelIndex(idx);
          Seed_Region.push_back(afl);
          seedpoint++;
     }
*/
     out3 << f[0] << std::endl;
     out3 << f[1] << std::endl;
     out3 << f[2] << std::endl;
     out3 << f[3] << std::endl;
     out3 << f[4] << std::endl;
     out3 << f[5] << std::endl;
     out3 << idx[0] << std::endl;
     out3 << idx[1] << std::endl;
     out3 << idx[2] << std::endl;

// 	    if(oneshot)
// 	      {
// 	    std::clog << std::endl << idx << std::endl;
// 	    std::clog << "H = " << H << std::endl;
// 	    std::clog << la << std::endl;
// 	    std::clog << va << std::endl;
// 	    std::clog << "v1 = " << v1[0] << "," << v1[1] <<","<< v1[2] << std::endl;

// 	    std::clog << g << std::endl;
// 	    std::clog << cross[0] << std::endl;
// 	    std::clog << cross[1] << std::endl;
// 	    std::clog << cross[2] << std::endl;
// 	    std::clog << angle << std::endl;
// 	    std::clog << R << std::endl;
// 	    std::clog << S << std::endl;
// 	    std::clog << RS << std::endl;
// 	    std::clog << "Hs = " << Hs << std::endl;
// 	    std::clog << ev2d.D[0].real() << ","
// 		      << ev2d.D[1].real() << std::endl;
// 	    std::clog << kappa1 << std::endl;
// 	    std::clog << kappa2 << std::endl << std::endl << std::endl;
	    
		
// 		oneshot = false;
// 	      }

	    // add feature to the list
	    m_FeatureVector->PushBack(f);
	  }

// 	if(m_FeatureVector->Size() > 200)
// 	  break;
      }
 //   std::clog << "Number of voxels on the colon wall: " << m_FeatureVector->Size() << std::endl;
 //   std::cout << "Number of seeds: " << seedpoint << std::endl;

    out3.close();

  }

  PrincipleCurvatureExtraction::FeatureSampleType::Pointer 
  PrincipleCurvatureExtraction::GetOutput()
  {
    return m_FeatureVector;
  }

  void PrincipleCurvatureExtraction::PrintSelf( 
					       std::ostream& os, 
					       itk::Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);
    
    os
      << indent << "PrincipleCurvatureExtraction:" 
      << std::endl;
  }
  
}
