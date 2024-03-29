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

namespace ctc
{
  
  SegmentColonFilter::SegmentColonFilter()
  {
    m_ThresholdFilter = ThresholdFilterType::New();
    m_BGRegionGrowFilter = RegionGrowFilterType::New();
    m_DistanceFilter = DistanceFilterType::New();
    m_DownsampleFilter = DownsampleFilterType::New();

    m_DownsampleFactor = 2;

    // set default parameters
    m_MinPixelValue = std::numeric_limits<CTCPixelType>::min();
    m_MaxPixelValue = std::numeric_limits<CTCPixelType>::max();
    m_Threshold = -800;
    m_MaxIterations = 10;
    m_MinDistanceThreshold = 10;
    m_NumberVoxelsInRegion = 0;
    
  }
  
  void SegmentColonFilter::GenerateData()
  {
    // threshold the image
    m_ThresholdFilter->SetInput(this->GetInput());
    m_ThresholdFilter->SetLowerThreshold(m_MinPixelValue);
    m_ThresholdFilter->SetUpperThreshold(m_Threshold);
    m_ThresholdFilter->SetInsideValue(0);
    m_ThresholdFilter->SetOutsideValue(255);

    m_ThresholdFilter->Update();
    
    //m_ChangeLabelFilter->SetReleaseDataFlag(true);
    
    // grow background region starting in the 8 corners
    m_BGRegionGrowFilter->SetInput(m_ThresholdFilter->GetOutput());
    BinaryImageType::SizeType size = \
      m_ThresholdFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
    BinaryImageType::SpacingType spacing = \
      m_ThresholdFilter->GetOutput()->GetSpacing();
    typedef BinaryImageType::SizeType::SizeValueType SizeValueType;

    BinaryImageType::IndexType seed1;
    seed1[0] = 0; seed1[1] = 0; seed1[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed1);
    BinaryImageType::IndexType seed2;
    seed2[0] = 0; seed2[1] = 0; seed2[2] = size[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed2);
    BinaryImageType::IndexType seed3;
    seed3[0] = 0; seed3[1] = size[1]-1; seed3[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed3);
    BinaryImageType::IndexType seed4;
    seed4[0] = 0; seed4[1] = size[1]-1; seed4[2] = size[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed4);
    BinaryImageType::IndexType seed5;
    seed5[0] = size[0]-1; seed5[1] = 0; seed5[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed5);
    BinaryImageType::IndexType seed6;
    seed6[0] = size[0]-1; seed6[1] = 0; seed6[2] = size[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed6);
    BinaryImageType::IndexType seed7;
    seed7[0] = size[0]-1; seed7[1] = size[1]-1; seed7[2] = 0;
    m_BGRegionGrowFilter->AddSeed(seed7);
    BinaryImageType::IndexType seed8;
    seed8[0] = size[0]-1; seed8[1] = size[1]-1; seed8[2] = size[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed8);
    m_BGRegionGrowFilter->SetLower(0);
    m_BGRegionGrowFilter->SetUpper(128);
    m_BGRegionGrowFilter->SetReplaceValue(255);
    
    m_BGRegionGrowFilter->Update();

    // mask out the BG region
    std::clog << "Masking ... ";
    typedef itk::ImageRegionIterator<BinaryImageType> BinaryIteratorType;
    BinaryIteratorType 
      it1(m_ThresholdFilter->GetOutput(), 
	  m_ThresholdFilter->GetOutput()->GetRequestedRegion());
    BinaryIteratorType 
      it2(m_BGRegionGrowFilter->GetOutput(), 
	  m_BGRegionGrowFilter->GetOutput()->GetRequestedRegion());
    for( it1.GoToBegin(), it2.GoToBegin(); !it1.IsAtEnd(); ++it1, ++it2)
      {
	if(it2.Get() == 255)
	  it1.Set(255);
      }
    std::clog << " Done." << std::endl;

    m_BGRegionGrowFilter->SetReleaseDataFlag(true);

    // downsample to speed up distance computation
    typedef itk::IdentityTransform< double, 3 > DownsampleTransformType;
    DownsampleTransformType::Pointer downTransform = 
      DownsampleTransformType::New();
    downTransform->SetIdentity();
    m_DownsampleFilter->SetTransform( downTransform );
    typedef itk::NearestNeighborInterpolateImageFunction<
    BinaryImageType, double > DownsampleInterpolatorType;
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
    m_DownsampleFilter->SetInput(m_ThresholdFilter->GetOutput());

    m_DownsampleFilter->SetOutputOrigin( \
       m_ThresholdFilter->GetOutput()->GetOrigin());

    m_DownsampleFilter->Update();

    // apply distance transform to remaining pixels
    m_DistanceFilter->SetInput(m_DownsampleFilter->GetOutput());
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
    bfunction->SetInputImage(m_ThresholdFilter->GetOutput());
    bfunction->ThresholdBetween(0, 1);

    typedef itk::FloodFilledImageFunctionConditionalIterator
      <DistanceImageType, DistanceFunctionType> DistanceFloodIteratorType;
    typedef itk::FloodFilledImageFunctionConditionalIterator
      <BinaryImageType, BinaryFunctionType> BinaryFloodIteratorType;

    unsigned int pass = 1;
    
    while(pass < m_MaxIterations)
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

	// if in upper 1/3 of z-axis, likely in lung
	bool lungflag = bool( static_cast<float>(maxi[2]) > 
			      (2.0/3.0)*static_cast<float>(downSize[2]) );
	//std::clog << static_cast<float>(maxi[2]) << std::endl;
	//std::clog << static_cast<float>(downSize[2]) << std::endl;
	if(lungflag) std::clog << "in lung, ignoring." << std::endl;


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
	seed[0] = maxi[0]*m_DownsampleFactor;
	seed[1] = maxi[1]*m_DownsampleFactor;
	seed[2] = maxi[2]*m_DownsampleFactor;

	BinaryFloodIteratorType bfit(m_ThresholdFilter->GetOutput(),
				       bfunction,
				       seed);
	bfit.GoToBegin();
	while(!bfit.IsAtEnd())
	  {
	    if(!lungflag)
	      bfit.Set(128);
	    else
	      bfit.Set(0);

	    ++bfit;
	  }

	pass++;
      }

    // mask out non-colon regions and reverse constrast
    std::clog << "Final Masking ... ";
    m_NumberVoxelsInRegion = 0;
    itk::ImageRegionIterator<BinaryImageType>
      it4(m_ThresholdFilter->GetOutput(), 
	  m_ThresholdFilter->GetOutput()->GetRequestedRegion());
    for( it4.GoToBegin(); !it4.IsAtEnd(); ++it4)
      {
	if(it4.Get() != 128)
	  it4.Set(0);
	else
	  {
	    it4.Set(1);
	    m_NumberVoxelsInRegion += 1;
	  }
      }
    std::clog << " Done." << std::endl;

    this->GraftOutput( m_ThresholdFilter->GetOutput() );

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



