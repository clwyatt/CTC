/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#include <limits>

#include "ctcRawVolumeExtraction.h"

namespace ctc
{
  
  RawVolumeExtraction::RawVolumeExtraction()
  {

    m_DICOMCoordinate[0] = 0;
    m_DICOMCoordinate[1] = 0;
    m_DICOMCoordinate[2] = 0;

    m_CropSize = 1.0;

    m_Sigma = 1.0;

    m_CropFilter = FilterType::New();
    
  }
  
  void RawVolumeExtraction::GenerateData()
  {
    
    CTCImageType::ConstPointer image = this->GetInput();
    
    ctc::CTCImageType::IndexType index;
    bool inside = image->TransformPhysicalPointToIndex(m_DICOMCoordinate, 
						       index);

    if(!inside)
      {
	throw false;
      }
    
    // setup crop filter
    m_CropFilter->SetInput(image);

    itk::Size<3> start, end;
    ctc::CTCImageType::SizeType fullsize = 
      image->GetLargestPossibleRegion().GetSize();
    
    // convert cropsize in mm to voxels
    CTCImageType::SpacingType spacing = image->GetSpacing();
    int cropx = static_cast<int>(ceil(m_CropSize/spacing[0]));
    int cropy = static_cast<int>(ceil(m_CropSize/spacing[1]));
    int cropz = static_cast<int>(ceil(m_CropSize/spacing[2]));

    start[0] = index[0]-cropx;
    start[1] = index[1]-cropy;
    start[2] = index[2]-cropz;
    
    end[0] = fullsize[0] - (index[0]+cropx);
    end[1] = fullsize[1] - (index[1]+cropy);
    end[2] = fullsize[2] - (index[2]+cropz);
    
    m_CropFilter->SetLowerBoundaryCropSize(start);      
    m_CropFilter->SetUpperBoundaryCropSize(end);
    
    m_CropFilter->Update();

    this->GraftOutput( m_CropFilter->GetOutput() );

  }
  
  void RawVolumeExtraction::PrintSelf( 
				     std::ostream& os, 
				     itk::Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);
    
    os
      << indent << "RawVolumeExtraction DICOM Coordinate:" 
      << this->m_DICOMCoordinate
      << std::endl;

    os
      << indent << "RawVolumeExtraction Crop Size:" 
      << this->m_CropSize
      << std::endl;

    os
      << indent << "RawVolumeExtraction Sigma:" 
      << this->m_Sigma
      << std::endl;

  }
  
}
