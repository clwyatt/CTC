/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#include <limits>

#include "ctcSegmentColonWithContrastFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkBinaryThresholdImageFunction.h"

#include "itkCastImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

#include "itkImageFileWriter.h"

namespace ctc
{
  
  SegmentColonWithContrastFilter::SegmentColonWithContrastFilter()
  {
    m_BGRegionGrowFilter = RegionGrowFilterType::New();
    m_DistanceFilter = DistanceFilterType::New();
    m_DownsampleFilter = DownsampleFilterType::New();

    // set default parameters
    m_DownsampleFactor = 2;
    m_Threshold = -800;

  }
  
  void SegmentColonWithContrastFilter::GenerateData()
  {
    // get size and spacing info
    BinaryImageType::SizeType size = \
      this->GetInput()->GetLargestPossibleRegion().GetSize();
    CTCImageType::SpacingType spacing = \
      this->GetInput()->GetSpacing();
    typedef CTCImageType::SizeType::SizeValueType SizeValueType;

    // allocate full classified image
    CTCImageType::IndexType start;
    start[0] = 0; start[1] = 0; start[2] = 0;
    BinaryImageType::RegionType fullregion;
    fullregion.SetSize(size);
    fullregion.SetIndex(start);
    BinaryImageType::Pointer full = BinaryImageType::New();
    full->SetRegions( fullregion );
    full->Allocate();
    full->SetSpacing( this->GetInput()->GetSpacing());
    full->SetOrigin( this->GetInput()->GetOrigin());
    full->SetDirection( this->GetInput()->GetDirection());

    // allocate temp image
    CTCImageType::SizeType newsize;
    newsize[0] = size[0];
    newsize[1] = size[1];
    newsize[2] = static_cast<SizeValueType>(size[2]/4);
    CTCImageType::RegionType region;
    region.SetSize(newsize);
    region.SetIndex(start);
    CTCImageType::Pointer temp = CTCImageType::New();
    temp->SetRegions(region);
    temp->Allocate();

    for(int sliceinc = 0; sliceinc < 4; sliceinc++)
      {

	// iterate through pulling slice pixels
	std::clog << "Resampling " << sliceinc << " ... ";
	CTCImageType::ConstPointer input = this->GetInput();
	for(int i = 0; i < newsize[0]; i++)
	  for(int j = 0; j < newsize[1]; j++)
	    for(int k = 0; k < newsize[2]; k++)
	      {
		CTCImageType::IndexType index0;
		index0[0] = i;
		index0[1] = j;
		index0[2] = k;

		CTCImageType::IndexType index1;
		index1[0] = i;
		index1[1] = j;
		index1[2] = 4*k + sliceinc;

		temp->SetPixel(index0, input->GetPixel(index1));	    
	      }
	std::clog << "Done." << std::endl; 

	std::clog << "Filtering " << sliceinc << " ... ";
	typedef itk::CastImageFilter< CTCImageType, itk::Image<double,3> >
	  ToDoubleType;
	ToDoubleType::Pointer tod = ToDoubleType::New();
	tod->SetInput(temp);
	SmootherType::Pointer smooth = SmootherType::New();
	smooth->SetInput(tod->GetOutput());
	smooth->SetTimeStep(0.0625);
	smooth->SetNumberOfIterations(10);
	smooth->SetStencilRadius(1);
	smooth->Update();
	std::clog << "Done." << std::endl;

	// segment the image using k-means
	std::clog << "Clustering " << sliceinc << " ... ";
	KMeansFilterType::Pointer kmeans = KMeansFilterType::New();
	kmeans->SetInput(smooth->GetOutput());
	kmeans->AddClassWithInitialMean(-3000);
	kmeans->AddClassWithInitialMean(-1000);
	kmeans->AddClassWithInitialMean(-100);
	kmeans->AddClassWithInitialMean(30);
	kmeans->AddClassWithInitialMean(400);
	kmeans->AddClassWithInitialMean(600);
	kmeans->Update(); 
	std::clog << "Done." << std::endl;

	smooth->GetOutput()->ReleaseData();

	//reassemble image
	std::clog << "Reassembling " << sliceinc << " ... ";
	for(int i = 0; i < newsize[0]; i++)
	  for(int j = 0; j < newsize[1]; j++)
	    for(int k = 0; k < newsize[2]; k++)
	      {
		BinaryImageType::IndexType index0;
		index0[0] = i;
		index0[1] = j;
		index0[2] = k;

		BinaryImageType::IndexType index1;
		index1[0] = i;
		index1[1] = j;
		index1[2] = 4*k + sliceinc;

		full->SetPixel(index1, kmeans->GetOutput()->GetPixel(index0));	    
	      }

	kmeans->GetOutput()->ReleaseData();

	std::clog << "Done." << std::endl;
      }
    temp->ReleaseData();

    // write out binary image
    typedef itk::ImageFileWriter<BinaryImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("full.vtk"); 
    writer->SetInput( full );
    writer->Update(); 

    // grow background region starting in the 8 corners
    std::clog << "Masking BG ... ";
    m_BGRegionGrowFilter->SetInput(full);
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
    seed8[0] = size[0]-1; 
    seed8[1] = size[1]-1; 
    seed8[2] = size[2]-1;
    m_BGRegionGrowFilter->AddSeed(seed8);
    m_BGRegionGrowFilter->SetLower(0);
    m_BGRegionGrowFilter->SetUpper(1);
    m_BGRegionGrowFilter->SetReplaceValue(255);
    m_BGRegionGrowFilter->Update();

    // mask out the BG region and combine air-fluid classes
    typedef itk::NeighborhoodIterator<BinaryImageType> BinaryIteratorType;
    BinaryIteratorType::RadiusType radius;
    radius[0] = 0; radius[1] = 5; radius[2] = 0;
    BinaryIteratorType::OffsetType offset1 = {{0,0,0}};
    BinaryIteratorType::OffsetType offset2 = {{0,-5,0}};
    BinaryIteratorType::OffsetType offset3 = {{0,-1,0}};
    BinaryIteratorType::OffsetType offset4 = {{0,1,0}};
    BinaryIteratorType::OffsetType offset5 = {{0,5,0}};
    typedef itk::ImageRegionIterator<BinaryImageType> BGIteratorType;
    typedef itk::ImageRegionConstIterator<CTCImageType> InputIteratorType;
    BinaryIteratorType 
      it1(radius,
	  full, 
	  fullregion);
     BGIteratorType 
      it2(m_BGRegionGrowFilter->GetOutput(), 
	  m_BGRegionGrowFilter->GetOutput()->GetRequestedRegion());
     InputIteratorType 
       it3(this->GetInput(), 
	   this->GetInput()->GetRequestedRegion());
     for( it1.GoToBegin(), it2.GoToBegin(), it3.GoToBegin(); 
	  !it1.IsAtEnd(); 
	  ++it1, ++it2, ++it3)
      {
	if( it2.Get() == 255 )
	  it1.SetPixel(offset1, 10);
	
	switch( it1.GetPixel(offset1) )
	  {
	  case 10:
	    it2.Set(10);
	    break;
	  case 1: //kmeans sometimes oversegments
	    if((it3.Get() < m_Threshold) ||
	       ((it1.GetPixel(offset2) == 1) && (it1.GetPixel(offset5) == 5))) 
	      it2.Set(0);
	    else
	      it2.Set(10);
	    break;
	  case 5:
	    it2.Set(0);
	    break;
	  default:
	    if((it1.GetPixel(offset2) == 1) &&
	       (it1.GetPixel(offset5) == 5))
	      it2.Set(0);
	    else
	      it2.Set(10);
	    break;
	  }
      }
    std::clog << " Done." << std::endl;


//     std::clog << "---- full -----" << std::endl << std::endl;
//     std::clog << full << std::endl << std::endl;
//     std::clog << "---- temp -----" << std::endl << std::endl;
//     std::clog << temp << std::endl;
//     std::clog << "---- region grow result -----" << std::endl;
//     BinaryImageType::Pointer bgimage = m_BGRegionGrowFilter->GetOutput();
//     std::clog << bgimage << std::endl;
    
    // downsample for distance transform
    std::clog << "Downsampling ... ";
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
    m_DownsampleFilter->SetInput(m_BGRegionGrowFilter->GetOutput());
    m_DownsampleFilter->SetOutputOrigin( m_BGRegionGrowFilter->GetOutput()->GetOrigin());
    m_DownsampleFilter->SetOutputDirection( m_BGRegionGrowFilter->GetOutput()->GetDirection());

    std::clog << downSize << std::endl;

    try
      {
	m_DownsampleFilter->Update();
      }
    catch(itk::ExceptionObject & err)
      {
	std::cerr << "ExceptionObject caught !" << std::endl; 
	std::cerr << err << std::endl; 
	exit(EXIT_FAILURE); 
      }
    
    std::clog << "Done." << std::endl;

    // apply distance transform to remaining pixels-
    m_DistanceFilter->SetInput(m_DownsampleFilter->GetOutput());
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


    // extract connected largest region, should be the colon
    typedef itk::ImageRegionIterator<DistanceImageType> 
      DistanceIteratorType;
    typedef itk::BinaryThresholdImageFunction<BinaryImageType> 
      BinaryFunctionType;
    BinaryFunctionType::Pointer bfunction = 
      BinaryFunctionType::New();
    bfunction->SetInputImage(m_BGRegionGrowFilter->GetOutput());
    bfunction->ThresholdBetween(0, 1);
    
    typedef itk::FloodFilledImageFunctionConditionalIterator
      <BinaryImageType, BinaryFunctionType> BinaryFloodIteratorType;

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
    
    std::clog << ": Seed Value " << maxd << " at " << maxi << std::endl;
	    
    BinaryImageType::IndexType seed;
    seed[0] = 2*maxi[0];
    seed[1] = 2*maxi[1];
    seed[2] = 2*maxi[2];




    std::clog << "Growing at seed " << seed << " value = " 
	      << static_cast<float>(m_BGRegionGrowFilter->GetOutput()->GetPixel(seed));

    BinaryFloodIteratorType bfit(m_BGRegionGrowFilter->GetOutput(),
				 bfunction,
				 seed);
    bfit.GoToBegin();
    while(!bfit.IsAtEnd())
      {
	bfit.Set(128);
	++bfit;
      }
    std::clog << " Done." << std::endl;
    
    // mask out non-colon regions and reverse constrast
    std::clog << "Final Masking ... ";
    itk::ImageRegionIterator<BinaryImageType>
      it4(m_BGRegionGrowFilter->GetOutput(), 
	  m_BGRegionGrowFilter->GetOutput()->GetRequestedRegion());
    for( it4.GoToBegin(); !it4.IsAtEnd(); ++it4)
      {
	if(it4.Get() != 128)
	  it4.Set(0);
	else
	  it4.Set(255);
      }
    std::clog << " Done." << std::endl;

    full->ReleaseData();

    // attach filter output 
    this->GraftOutput( m_BGRegionGrowFilter->GetOutput() );

  }
  
  void SegmentColonWithContrastFilter::PrintSelf( 
				     std::ostream& os, 
				     itk::Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);
    
    os
      << indent << "Segment Colon Threshold:" << this->m_Threshold
      << std::endl;
  }
  
}

