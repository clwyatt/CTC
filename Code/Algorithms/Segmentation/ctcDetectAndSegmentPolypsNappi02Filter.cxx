/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#include <limits>

#include "ctcDetectAndSegmentPolypsNappi02Filter.h"


namespace ctc
{
  
  DetectAndSegmentPolypsNappi02Filter::DetectAndSegmentPolypsNappi02Filter()
  {
    
  }
  
  void DetectAndSegmentPolypsNappi02Filter::
  SetImageInput(const CTCImageType * image)
  {
    m_Image = image;
  }


  void DetectAndSegmentPolypsNappi02Filter::BuildWallRegion()
  {

    // clone the input->output image

    // while the heap is not empty
						
    // get the top

    // add neighbors to the heap with time + 1 if they are not in the region,
    // if within the bounds, and the current "time" is less than a threshold
  }

  void DetectAndSegmentPolypsNappi02Filter::GenerateData()
  {

    // create a thickened region on the wall using a variation of fast
    // marching (no clue how this was done in the original implementation)
    this->BuildWallRegion();

    // flag voxels in this region with SI and CV values within the threshold

    // use the "hysterisis thresholding"

    // apply the "fuzzy cmeans clustering"
    // all this really does is label the connected regions
    // this could be done much simplier, but to be faithful to
    // the paper, I do the FCM instead.

    // output the candidate polyp regions

  }
  
  void DetectAndSegmentPolypsNappi02Filter::PrintSelf( 
				     std::ostream& os, 
				     itk::Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);
  }
  
}
