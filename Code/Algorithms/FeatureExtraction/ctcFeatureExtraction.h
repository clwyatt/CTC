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
#include <string>

using namespace std;
namespace ctc
{

  /*  
   *  For each voxel in the dataset(colon_imgage), we assign corresponding features to it. 
   */

  class AssociatedFeatureList
  {

  public:

    typedef itk::Point<double,3> PointdcmCoordinate;
    typedef BinaryImageType::IndexType PointIndex;
   
    AssociatedFeatureList() { SI = 0; CV = 0; Gmag = 0; nSI = 0; nCV = 0; nGmag = 0; u = 0; groupid = 0; }
  
    /* Copy Constructor */
    AssociatedFeatureList(const AssociatedFeatureList & another)
          { 
               DCMCoordinate = another.GetDCMCoordinate();
               voxel_index = another.GetVoxelIndex();
               SI = another.GetSI();
               CV = another.GetCV();
               Gmag = another.GetGmag();
               nSI = another.GetnSI();
               nCV = another.GetnCV();
               nGmag = another.GetnGmag();
               u = another.GetMembership();
               groupid = another.GetGroupID();
               x = another.GetX();
               y = another.GetY();
               z = another.GetZ();
               ix = another.GetIX(); 
               iy = another.GetIY();                
               iz = another.GetIZ();                 
          }

    ~AssociatedFeatureList() {}

    /* Assignment */
    AssociatedFeatureList & operator=(const AssociatedFeatureList & copyer)
          { 
               if(this == &copyer)
                  return *this;
               DCMCoordinate = copyer.GetDCMCoordinate();
               voxel_index = copyer.GetVoxelIndex();
               SI = copyer.GetSI();
               CV = copyer.GetCV();
               Gmag = copyer.GetGmag();
               nSI = copyer.GetnSI();
               nCV = copyer.GetnCV();
               nGmag = copyer.GetnGmag();
               u = copyer.GetMembership();
               groupid = copyer.GetGroupID();
               x = copyer.GetX();
               y = copyer.GetY();
               z = copyer.GetZ();
               ix = copyer.GetIX(); 
               iy = copyer.GetIY();                
               iz = copyer.GetIZ();    
               return *this;
          }


    PointdcmCoordinate GetDCMCoordinate() const
          { return DCMCoordinate; }

    PointIndex GetVoxelIndex() const
          { return voxel_index; }

    float GetSI() const
          { return SI; }

    float GetCV() const
          { return CV; }

    float GetGmag() const
          { return Gmag; }

    float GetnSI() const
          { return nSI; }

    float GetnCV() const
          { return nCV; }

    float GetnGmag() const
          { return nGmag; }

    float GetMembership() const
          { return u; }

    int GetGroupID() const
          { return groupid; }

    void SetSI( float CalculatedSI )
          { SI = CalculatedSI; }

    void SetCV( float CalculatedCV )
          { CV = CalculatedCV; }

    void SetGmag( float CalculatedGmag )
          { Gmag = CalculatedGmag; }

    void SetnSI( float CalculatednSI )
          { nSI = CalculatednSI; }

    void SetnCV( float CalculatednCV )
          { nCV = CalculatednCV; }

    void SetnGmag( float CalculatednGmag )
          { nGmag = CalculatednGmag; }

    void SetMembership( float Calculatedu )
          { u = Calculatedu; }

    void SetDCMCoordinate( PointdcmCoordinate CalculatedDCM ) 
          { DCMCoordinate = CalculatedDCM; }

    void SetVoxelIndex( PointIndex CalculatedIndex )
          { voxel_index = CalculatedIndex; }

    void SetGroupID( int newgroupid )
          { groupid = newgroupid; }

    float GetX() const
          { return x; }

    float GetY() const
          { return y; }

    float GetZ() const
          { return z; }

    void SetX( float xx )
          { x == xx; }    

    void SetY( float yy )
          { y == yy; }  

    void SetZ( float zz )
          { z == zz; }  

    int GetIX() const
          { return ix; }

    int GetIY() const
          { return iy; }

    int GetIZ() const
          { return iz; }

    void SetIX( int xx )
          { ix == xx; }    

    void SetIY( int yy )
          { iy == yy; }  

    void SetIZ( int zz )
          { iz == zz; }  

  private:
    
    PointdcmCoordinate DCMCoordinate;
    PointIndex voxel_index;
    float x;
    float y;
    float z;
    int ix;
    int iy;
    int iz;
    float SI;
    float CV;
    float Gmag;
    float nSI;    // Normalized value
    float nCV;
    float nGmag;
    float u;      // Membership function
    int groupid;
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
    
    typedef itk::Vector< float, 9>                     FeatureType;
    typedef itk::Statistics::ListSample<FeatureType>   FeatureSampleType;
    typedef FeatureSampleType::Pointer                 FeaturePointer;

    typedef std::vector<AssociatedFeatureList>         GrowableRegionType;
    typedef std::vector<GrowableRegionType>            RegionCollectorType;

    typedef itk::Point<double, 3>                      dcmCoordinate;
    typedef std::vector<dcmCoordinate>                 dcmVectorType;

    /* Standard ITK macros */
    itkNewMacro(Self);
    itkTypeMacro(FeatureExtraction, itk::Object);

    void SetInitialFeatureVector(const FeaturePointer FeatureVector)
                          { m_FeatureVector = FeatureVector; }

    void SetSegmentedImageInput( const BinaryImageType *image )
                          { m_ColonImage = image; }

    void SetRawImageInput( const CTCImageType *image )
                          { m_Image = image; } 

    GrowableRegionType GetSeedRegion() const
                          { return Seed_Region; }

    GrowableRegionType GetGrowableReg() const
                          { return Growable_Region; }

    RegionCollectorType GetRegionCollector() const
                          { return RegionCollector; }

    dcmVectorType GetPolyCenterCollector() const
                          { return PolypCenterCollector; } 

    void SetPolyCenterCollector(dcmVectorType collector)
                          { PolypCenterCollector = collector; }

    void Analyze( void );

    void SetOutputVTK(string name)
                          { UserNamedOutput = name; }

    void SetSeedRegion( GrowableRegionType InputSeedRegion )
                          { Seed_Region = InputSeedRegion; }
 
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
    RegionCollectorType  RegionCollector;
    dcmVectorType        PolypCenterCollector;
    string               UserNamedOutput;
    
  };
 
}

#endif











