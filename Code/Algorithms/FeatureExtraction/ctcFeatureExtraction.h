/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcFeatureExtraction_h
#define __ctcFeatureExtraction_h

#include "itkObject.h"
#include "ctcCTCImage.h"
#include "itkListSample.h"

namespace ctc
{

  /*  
   *  For each voxel in the dataset(colon_imgage), we assign corresponding features to it. 
   */

  class AssociatedFeatureList
  {

  public:

    typedef itk::Point<double,3> PointdcmCoordinate;
   
    AssociatedFeatureList() { SI = 0; CV = 0; nSI = 0; nCV = 0; u = 0; } 
    ~AssociatedFeatureList() {}

    PointdcmCoordinate GetDCMCoordinate() const
          { return DCMCoordinate; }

    float GetSI() const
          { return SI; }

    float GetCV() const
          { return CV; }

    float GetnSI() const
          { return nSI; }

    float GetnCV() const
          { return nCV; }

    float GetMembership() const
          { return u; }

    void SetSI( float CalculatedSI )
          { SI = CalculatedSI; }

    void SetCV( float CalculatedCV )
          { CV = CalculatedCV; }

    void SetnSI( float CalculatednSI )
          { nSI = CalculatednSI; }

    void SetnCV( float CalculatednCV )
          { nCV = CalculatednCV; }

    void SetMembership( float Calculatedu )
          { u = Calculatedu; }

    void SetDCMCoordinate( PointdcmCoordinate CalculatedDCM ) 
          { DCMCoordinate = CalculatedDCM; }

  private:
    
    PointdcmCoordinate DCMCoordinate;
    float SI;
    float CV;
    float nSI;    // Normalized value
    float nCV;
    float u;      // Membership function
    //AssociatedFeatureList * BelongedClusterCenter;

  };

  /*  
   *   Feature Extraction based on the methdology proposed by 
   *   Yoshida and Nappi
   */
  class FeatureExtraction: public itk::Object
  {
  public:

    typedef FeatureExtraction                          Self;
    typedef itk::SmartPointer<Self>                    Pointer;
    typedef itk::SmartPointer<const Self>              ConstPointer;
    
    typedef itk::Vector< float, 5>                     FeatureType;
    typedef itk::Statistics::ListSample<FeatureType>   FeatureSampleType;
    typedef FeatureSampleType::Pointer                 FeaturePointer;

    typedef std::vector<AssociatedFeatureList>         GrowableRegionType;
    typedef std::vector<GrowableRegionType>            RegionCollectorType;

    typedef itk::Point<double, 3>                      dcmCoordinate;

    /* Standard ITK macros */
    itkNewMacro(Self);
    itkTypeMacro(FeatureExtraction, itk::Object);

    void SetFeatureVector(const FeaturePointer);

    GrowableRegionType GetSeedRegion()
                          { return Seed_Region; }

    GrowableRegionType GetGrowableReg() 
                          { return Growable_Region; }
    void Analyze( void );
 
  protected:

    FeatureExtraction();
    ~FeatureExtraction();

  private:

    const CTCImageType *    m_Image;
    const BinaryImageType * m_ColonImage;

    FeaturePointer       m_FeatureVector;
    GrowableRegionType   Raw_Region;
    GrowableRegionType   Seed_Region;
    GrowableRegionType   Growable_Region;

  };
 
}

#endif











