/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#include <limits>

#include "ctcSegmentColonFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionIterator.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkBinaryThresholdImageFunction.h"

#include "itkCastImageFilter.h"
#include "itkBinaryMedianImageFilter.h"

namespace ctc
{
  
  SegmentColonFilter::SegmentColonFilter()
  {
    m_KMeansFilter = KMeansFilterType::New();
    m_BGRegionGrowFilter = RegionGrowFilterType::New();
    m_DistanceFilter = DistanceFilterType::New();
    m_DownsampleFilter = DownsampleFilterType::New();
    m_UpsampleFilter = UpsampleFilterType::New();
    m_SmoothFilter = SmootherType::New();

    m_DownsampleFactor = 2;

    // set default parameters
    m_MinPixelValue = std::numeric_limits<CTCPixelType>::min();
    m_MaxPixelValue = std::numeric_limits<CTCPixelType>::max();
    m_Threshold = -800;
    m_MaxIterations = 10;
    m_MinDistanceThreshold = 10;
    
  }
  
  void SegmentColonFilter::GenerateData()
  {
    // get size and spacing info
    BinaryImageType::SizeType size = \
      this->GetInput()->GetLargestPossibleRegion().GetSize();
    CTCImageType::SpacingType spacing = \
      this->GetInput()->GetSpacing();
    typedef CTCImageType::SizeType::SizeValueType SizeValueType;

    // downsample to speed up computation and reduce memory requirements
    std::clog << "Resampling ... ";
    typedef itk::IdentityTransform< double, 3 > DownsampleTransformType;
    DownsampleTransformType::Pointer downTransform = 
      DownsampleTransformType::New();
    downTransform->SetIdentity();
    m_DownsampleFilter->SetTransform( downTransform );
    typedef itk::NearestNeighborInterpolateImageFunction<
      CTCImageType, double > DownsampleInterpolatorType;
    DownsampleInterpolatorType::Pointer downInterpolator = 
      DownsampleInterpolatorType::New();
    m_DownsampleFilter->SetInterpolator( downInterpolator );
    BinaryImageType::SpacingType downSpacing;
    downSpacing[0] = spacing[0] * m_DownsampleFactor;
    downSpacing[1] = spacing[1] * m_DownsampleFactor;
    downSpacing[2] = spacing[2] * m_DownsampleFactor;
    m_DownsampleFilter->SetOutputSpacing( downSpacing );
    BinaryImageType::SizeType downSize;
    downSize[0] = static_cast<SizeValueType>(size[0]/m_DownsampleFactor);
    downSize[1] = static_cast<SizeValueType>(size[1]/m_DownsampleFactor);
    downSize[2] = static_cast<SizeValueType>(size[2]/m_DownsampleFactor);
    m_DownsampleFilter->SetSize( downSize );
    m_DownsampleFilter->SetInput(this->GetInput());
    m_DownsampleFilter->SetOutputOrigin( \
      this->GetInput()->GetOrigin());
    m_DownsampleFilter->SetOutputDirection( \
      this->GetInput()->GetDirection()); 
    m_DownsampleFilter->Update();
    std::clog << "Done." << std::endl;

    typedef itk::CastImageFilter< CTCImageType, itk::Image<double,3> >
      ToDoubleType;
    ToDoubleType::Pointer tod = ToDoubleType::New();
    tod->SetInput(m_DownsampleFilter->GetOutput());

    // non-linearly smooth the image
//     int rad = atoi(getenv("CTC_COND"));
//     int iter = atoi(getenv("CTC_ITER"));
//     float ts = atof(getenv("CTC_TS"));
//     std::clog << "Stencil Radius: " << rad << std::endl;
//     std::clog << "Number of iterations: " << iter << std::endl;
//     std::clog << "Time Step: " << ts << std::endl;
    std::clog << "Filtering ... ";
    m_SmoothFilter->SetInput(tod->GetOutput());
    m_SmoothFilter->SetTimeStep(0.0625);
    m_SmoothFilter->SetNumberOfIterations(10);
    m_SmoothFilter->SetStencilRadius(1);
    m_SmoothFilter->Update();
    std::clog << "Done." << std::endl;

    // segment the image using k-means
    std::clog << "Clustering ... ";
    m_KMeansFilter->SetInput(m_SmoothFilter->GetOutput());
    m_KMeansFilter->AddClassWithInitialMean(-3000);
    m_KMeansFilter->AddClassWithInitialMean(-1000);
    m_KMeansFilter->AddClassWithInitialMean(-100);
    m_KMeansFilter->AddClassWithInitialMean(30);
    m_KMeansFilter->AddClassWithInitialMean(400);
    m_KMeansFilter->AddClassWithInitialMean(600);
    m_KMeansFilter->Update(); 
    std::clog << "Done." << std::endl;
    
    // upsample to original size
//     std::clog << "Upsampling ... ";
//     typedef itk::IdentityTransform< double, 3 > UpsampleTransformType;
//     UpsampleTransformType::Pointer upTransform = 
//       UpsampleTransformType::New();
//     upTransform->SetIdentity();
//     m_UpsampleFilter->SetTransform( upTransform );
//     typedef itk::NearestNeighborInterpolateImageFunction<
//       BinaryImageType, double > UpsampleInterpolatorType;
//     UpsampleInterpolatorType::Pointer upInterpolator = 
//       UpsampleInterpolatorType::New();
//     m_UpsampleFilter->SetInterpolator( upInterpolator );
//     BinaryImageType::SpacingType upSpacing;
//     upSpacing[0] = spacing[0];
//     upSpacing[1] = spacing[1];
//     upSpacing[2] = spacing[2];
//     m_UpsampleFilter->SetOutputSpacing( upSpacing );
//     BinaryImageType::SizeType upSize;
//     upSize[0] = static_cast<SizeValueType>(size[0]);
//     upSize[1] = static_cast<SizeValueType>(size[1]);
//     upSize[2] = static_cast<SizeValueType>(size[2]);
//     m_UpsampleFilter->SetSize( upSize );
//     m_UpsampleFilter->SetInput(m_KMeansFilter->GetOutput());
//     m_UpsampleFilter->SetOutputOrigin( \
//        this->GetInput()->GetOrigin());
//     m_UpsampleFilter->Update();
//     std::clog << "Done." << std::endl;

    //free up some pipline memory
      m_DownsampleFilter->SetReleaseDataFlag(true);
      m_SmoothFilter->SetReleaseDataFlag(true);
      //m_KMeansFilter->SetReleaseDataFlag(true);

    // grow background region starting in the 8 corners
    m_BGRegionGrowFilter->SetInput(m_KMeansFilter->GetOutput());
    BinaryImageType::IndexType seed1;
    seed1[0] = 0; seed1[1] = 0; seed1[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed1);
    BinaryImageType::IndexType seed2;
    seed2[0] = 0; seed2[1] = 0; seed2[2] = downSize[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed2);
    BinaryImageType::IndexType seed3;
    seed3[0] = 0; seed3[1] = downSize[1]-1; seed3[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed3);
    BinaryImageType::IndexType seed4;
    seed4[0] = 0; seed4[1] = size[1]-1; seed4[2] = downSize[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed4);
    BinaryImageType::IndexType seed5;
    seed5[0] = downSize[0]-1; seed5[1] = 0; seed5[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed5);
    BinaryImageType::IndexType seed6;
    seed6[0] = downSize[0]-1; seed6[1] = 0; seed6[2] = downSize[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed6);
    BinaryImageType::IndexType seed7;
    seed7[0] = downSize[0]-1; seed7[1] = downSize[1]-1; seed7[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed7);
    BinaryImageType::IndexType seed8;
    seed8[0] = downSize[0]-1; 
    seed8[1] = downSize[1]-1; 
    seed8[2] = downSize[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed8);
    m_BGRegionGrowFilter->SetLower(0);
    m_BGRegionGrowFilter->SetUpper(1);
    m_BGRegionGrowFilter->SetReplaceValue(255);
    
    m_BGRegionGrowFilter->Update();

    // mask out the BG region and combine air-fluid classes
    std::clog << "Masking ... ";
    typedef itk::ImageRegionIterator<BinaryImageType> BinaryIteratorType;
    BinaryIteratorType 
      it1(m_KMeansFilter->GetOutput(), 
	  m_KMeansFilter->GetOutput()->GetRequestedRegion());
    BinaryIteratorType 
      it2(m_BGRegionGrowFilter->GetOutput(), 
	  m_BGRegionGrowFilter->GetOutput()->GetRequestedRegion());
    for( it1.GoToBegin(), it2.GoToBegin(); !it1.IsAtEnd(); ++it1, ++it2)
      {
	if( it2.Get() == 255 )
	  it1.Set(10);
	
	switch( it1.Get() )
	  {
	  case 10:
	    break;
	  case 1:
	    it1.Set(0);
	    break;
	  case 5:
	    it1.Set(0);
	    break;
	  default:
	    it1.Set(10);
	    break;
	  }
      }
    std::clog << " Done." << std::endl;

    m_BGRegionGrowFilter->SetReleaseDataFlag(true);        

    // apply distance transform to remaining pixels-
    m_DistanceFilter->SetInput(m_KMeansFilter->GetOutput());
    //m_DistanceFilter->InputIsBinaryOn();

    try
      {
	std::clog << "Calling Distance Filter ";
	m_DistanceFilter->Update();
	std::clog << " Success." << std::endl;
      }
    catch(itk::ExceptionObject & err)
      {
	std::cerr << "ExceptionObject caught !" << std::endl; 
	std::cerr << err << std::endl; 
	exit(EXIT_FAILURE); 
      }

    // loop through regions
    typedef itk::ImageRegionIterator<DistanceImageType> 
      DistanceIteratorType;
    typedef itk::BinaryThresholdImageFunction<DistanceImageType> 
      DistanceFunctionType;
    DistanceFunctionType::Pointer dfunction = 
      DistanceFunctionType::New();
    dfunction->SetInputImage(m_DistanceFilter->GetOutput());
    dfunction->ThresholdBetween(1, 1536);
    typedef itk::BinaryThresholdImageFunction<BinaryImageType> 
      BinaryFunctionType;
    BinaryFunctionType::Pointer bfunction = 
      BinaryFunctionType::New();
    bfunction->SetInputImage(m_KMeansFilter->GetOutput());
    bfunction->ThresholdBetween(0, 1);

    typedef itk::FloodFilledImageFunctionConditionalIterator
      <DistanceImageType, DistanceFunctionType> DistanceFloodIteratorType;
    typedef itk::FloodFilledImageFunctionConditionalIterator
      <BinaryImageType, BinaryFunctionType> BinaryFloodIteratorType;

    unsigned int pass = 1;
    
    while(pass < 2)
      {
	DistanceIteratorType 
	  dit(m_DistanceFilter->GetOutput(), 
	      m_DistanceFilter->GetOutput()->GetRequestedRegion());
	DistancePixelType maxd = 0;
	dit.GoToBegin();
	DistanceImageType::IndexType maxi = dit.GetIndex();
	while(!dit.IsAtEnd())
	  {
	    if(dit.Get() > maxd)
	      {
		maxd = dit.Get();
		maxi = dit.GetIndex();
	      }
	    ++dit;
	  }

	std::clog << "Pass " << pass << 
	  ": Seed Value " << maxd << " at " << maxi << std::endl;
	
	if(maxd < m_MinDistanceThreshold)
	  break;

	DistanceFloodIteratorType dfit(m_DistanceFilter->GetOutput(),
				       dfunction,
				       maxi);
	dfit.GoToBegin();
	while(!dfit.IsAtEnd())
	  {
	    dfit.Set(0);
	    ++dfit;
	  }

	BinaryImageType::IndexType seed;
	seed[0] = maxi[0];
	seed[1] = maxi[1];
	seed[2] = maxi[2];

	BinaryFloodIteratorType bfit(m_KMeansFilter->GetOutput(),
				     bfunction,
				     seed);
	bfit.GoToBegin();
	while(!bfit.IsAtEnd())
	  {
	    bfit.Set(128);
	    ++bfit;
	  }

	pass++;
      }

    // mask out non-colon regions
    std::clog << "Final Masking ... ";
    BinaryIteratorType 
      it3(m_KMeansFilter->GetOutput(), 
	  m_KMeansFilter->GetOutput()->GetRequestedRegion());
    for( it3.GoToBegin(); !it3.IsAtEnd(); ++it3)
      {
	if(it3.Get() != 128)
	  it3.Set(255);
      }
    std::clog << " Done." << std::endl;

    // cleanup the flat areas
//     typedef itk::BinaryMedianImageFilter<
//       BinaryImageType, BinaryImageType > CleanupFilterType;
//     CleanupFilterType::Pointer clean = CleanupFilterType::New();

//     BinaryImageType::SizeType indexRadius;
//     indexRadius[0] = 1; // radius along x
//     indexRadius[1] = 1; // radius along y
//     indexRadius[2] = 1; // radius along z

//     clean->SetRadius( indexRadius );

//     clean->SetInput( m_KMeansFilter->GetOutput() );
//     clean->Update();

    this->GraftOutput( m_KMeansFilter->GetOutput() );

  }
  
  void SegmentColonFilter::PrintSelf( 
				     std::ostream& os, 
				     itk::Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);
    
    os
      << indent << "Segment Colon Threshold:" << this->m_Threshold
      << std::endl;
  }
  
}

