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
  /*   Feature Extraction based on the methdology proposed by 
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

    typedef std::vector< AssociatedFeatureList >       FeatureListType;
    typedef std::vector< AssociatedGrowableRegion >    GrowableRegionType;
    //typedef itk::Statistics::ListSample<AssociatedFeatureList>   FeatureListType;
    //typedef FeatureListType::Pointer                             FeatureListTypePointer;  

    typedef itk::Point<double, 3>                      dcmCoordinate;
    typedef itk::ConstNeighborhoodIterator<BinaryImageType> BinaryIteratorType;
    typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< BinaryImageType > FaceCalculatorType;    

    /* Standard ITK macros */
    itkNewMacro(Self);
    itkTypeMacro(FeatureExtraction, itk::Object);

    void SetFeatureVector(const FeaturePointer);
    FeatureListType GetSeedRegion();
    FeatureListType GetGrowableReg(); 
    void Analyze( void );
 
  protected:

    FeatureExtraction();
    ~FeatureExtraction();

  private:

    const CTCImageType *    m_Image;
    const BinaryImageType * m_ColonImage;

    FeaturePointer m_FeatureVector;
    FeatureListType Raw_Region;
    FeatureListType Seed_Region;
    FeatureListType Growable_Region;

  }


  /*  For each voxel in the dataset(colon_imgage), we assign corresponding features to it. 
   */

  class AssociatedFeatureList: public itk::Object
  {
  public:

    typedef itk::Point<double, 3> PointdcmCoordinate;
    
    //AssociatedFeatureList * GetBelongedClusterCenter() const
    //                        { return BelongedClusterCenter; }
  
    PointdcmCoordiate GetDCMCoordinate() const
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

    void SetDCMCoordinate( PointdcmCoordiate CalculatedDCM ) 
          { DCMCoordinate = CalculatedDCM; }

    //void SetBelongedClusterCenter( AssociatedFeatureList * CalculatedRegionCenter )
    //      { BelongedClusterCenter = CalculatedRegionCente; } 

  protected:

    AssociatedFeatureList() { SI = 0; CV = 0; nSI = 0; nCV = 0; u = 0; } 
    ~AssociatedFeatureList() {}

  private:
    
    PointdcmCoordiate DCMCoordinate;
    float SI;
    float CV;
    float nSI;    // Normalized value
    float nCV;
    float u;      // Membership function
    //AssociatedFeatureList * BelongedClusterCenter;

  }


  /*   Associate each polyp candidate with a growable region in order to extract
   *   a large connected component that corresponds to the major portion of a polyp
   */
  class AssociatedGrowableRegion: public itk::Oject
  {
  public:
    
     typedef itk::Point< double, 3 > dcmCoordinate;

     typedef std::vector< AssociatedFeatureList >     MeasurementVectorType;
     //typedef itk::Vector< AssociatedFeatureList,1 > MeasurementVectorType;
     //typedef itk::Statistics::ListSample< MeasurementVectorType > RegionMemberList; 
       
     RegionMemberList GetGrowableRegionMemberList()
                      { return GrowableRegionMemberList; }

     void SetGrowableRegionMemberList(RegionMemberList NewList)
                      { GrowableRegionMemberList = NewList; }

  protected:
    
     AssociatedGrowableRegion(){}
     ~AssociatedGrowableRegion(){}
    
  private:
     
     //AssociatedFeatureList GrowableRegionCenter;
     RegionMemberList GrowableRegionMemberList;
 
  }
  
  class GrowableRegionCenter: public itk::Object
  {
  public:

     typedef itk::Point< double, 3 > PointdcmCoordinate;
     
     PointdcmCoordiate GetDCMCoordinate() const
                       { return DCMCoordinate; }

     void SetDCMCoordinate( PointdcmCoordiate CalculatedDCM ) 
                       { DCMCoordinate = CalculatedDCM; }

     AssociatedGrowableRegion GetGrowableRegion() const
                       { return OwnedGrowableRegion; }

     void SetGrowableRegion( AssociatedGrowableRegion NewGrowableRegion)
                       { OwnedGrowableRegion = NewGrowableRegion; }      

  protected:
     
     GrowableRegionCenter(){}
     ~GrowableRegionCenter(){}

  private:
   
     PointdcmCoordinate  DCMCoordinate;
     AssociatedGrowableRegion OwnedGrowableRegion; 
     
  }

}

#endif











