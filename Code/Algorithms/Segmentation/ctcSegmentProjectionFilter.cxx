/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#include <limits>

#include "ctcSegmentProjectionFilter.h"

#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageSliceConstIteratorWithIndex.h"

namespace ctc
{
  
  SegmentProjectionFilter::SegmentProjectionFilter()
  {
    
  }

  void SegmentProjectionFilter::GenerateOutputInformation()
  {
     
    // get pointers to the input and output
    Superclass::OutputImagePointer      outputPtr = this->GetOutput();
    Superclass::InputImageConstPointer  inputPtr  = this->GetInput();

    if ( !outputPtr || !inputPtr)
      {
	return;
      }
    BinaryImageType::RegionType inputRegion = 
      inputPtr->GetLargestPossibleRegion();

    ProjectionImageType::RegionType outputRegion;
    ProjectionImageType::RegionType::SizeType outputSize;
    ProjectionImageType::RegionType::IndexType outputIndex;
    outputIndex[0] = 0;
    outputIndex[1] = 0;
    outputSize[0] = inputRegion.GetSize()[1];
    outputSize[1] = inputRegion.GetSize()[2];
    outputRegion.SetSize( outputSize );
    outputRegion.SetIndex( outputIndex );
    outputPtr->SetLargestPossibleRegion( outputRegion );
    outputPtr->SetBufferedRegion( outputRegion );
    outputPtr->SetRequestedRegion( outputRegion );

    BinaryImageType::SpacingType inputSpacing = inputPtr->GetSpacing();
    ProjectionImageType::SpacingType outputSpacing;
    outputSpacing[0] = inputSpacing[1];
    outputSpacing[1] = inputSpacing[2];
    outputPtr->SetSpacing( outputSpacing );

    BinaryImageType::DirectionType inputDirection = inputPtr->GetDirection();
    ProjectionImageType::DirectionType outputDirection;
    outputDirection[0][0] = inputDirection[0][1];
    outputDirection[1][0] = inputDirection[1][1];
    outputDirection[2][0] = inputDirection[2][1];
    outputDirection[0][1] = inputDirection[0][2];
    outputDirection[1][1] = inputDirection[1][2];
    outputDirection[2][1] = inputDirection[2][2];
    outputPtr->SetDirection(outputDirection);

    BinaryImageType::PointType inputOrigin = inputPtr->GetOrigin();
    ProjectionImageType::PointType outputOrigin;
    outputOrigin[0] = inputOrigin[0];
    outputOrigin[1] = inputOrigin[1];
    outputOrigin[2] = inputOrigin[2];
    outputPtr->SetOrigin( outputOrigin );
    outputPtr->SetNumberOfComponentsPerPixel(1);
  }

  void SegmentProjectionFilter::GenerateData()
  {
    // get pointer to input image
    BinaryImageType::ConstPointer inputImage = this->GetInput();

    // allocate the output image
    ProjectionImageType::Pointer outputImage = this->GetOutput();
    outputImage->Allocate();

    // create the projection
    BinaryImageType::RegionType inputRegion = 
      this->GetInput()->GetLargestPossibleRegion();
    typedef itk::ImageSliceConstIteratorWithIndex<BinaryImageType> BinaryIteratorType;
    BinaryIteratorType 
      iit(this->GetInput(), 
	 inputRegion);
    typedef itk::ImageLinearIteratorWithIndex< ProjectionImageType > 
      ProjectionIteratorType;
    ProjectionIteratorType oit(outputImage, outputImage->GetBufferedRegion());

    // integrate in the sagital direction
    iit.SetFirstDirection(0);
    iit.SetSecondDirection(2);

    oit.SetDirection(0);

    iit.GoToBegin();
    oit.GoToBegin();
    while ( ! iit.IsAtEnd() )
      {
	while ( ! iit.IsAtEndOfSlice() )
	  {
	    while ( ! iit.IsAtEndOfLine() )
	      {
		oit.Set( oit.Get() + iit.Get() );
		++iit;
		++oit;
	      }
	    iit.NextLine();
	    oit.NextLine();
	  }
	iit.NextSlice();
	oit.GoToBegin();
      }
  }
  
  void SegmentProjectionFilter::PrintSelf( 
				     std::ostream& os, 
				     itk::Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);
  }
  
}



