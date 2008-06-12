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
   
    AssociatedFeatureList() { SI = 0; CV = 0; Gmag = 0; nSI = 0; nCV = 0; nGmag = 0; u = 0; groupid = 0; tag = 0; DGC = 0; }
  
    /* Copy Constructor */
    AssociatedFeatureList(const AssociatedFeatureList & another)
          { 
               DCMCoordinate = another.GetDCMCoordinate();
               voxel_index = another.GetVoxelIndex();
               SI = another.GetSI();
               CV = another.GetCV();
               DGC = another.GetDGC();
               Gmag = another.GetGmag();
               nSI = another.GetnSI();
               nCV = another.GetnCV();
               nGmag = another.GetnGmag();
               u = another.GetMembership();
               groupid = another.GetGroupID();
               tag = another.GetTag();               
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
               DGC = copyer.GetDGC();
               Gmag = copyer.GetGmag();
               nSI = copyer.GetnSI();
               nCV = copyer.GetnCV();
               nGmag = copyer.GetnGmag();
               u = copyer.GetMembership();
               groupid = copyer.GetGroupID();
               tag = copyer.GetTag(); 
               return *this;
          }


    PointdcmCoordinate GetDCMCoordinate() const
          { return DCMCoordinate; }

    PointdcmCoordinate Get_nDCMCoordinate() const
          { return nDCMCoordinate; }

    PointIndex GetVoxelIndex() const
          { return voxel_index; }

    float GetSI() const
          { return SI; }

    float GetCV() const
          { return CV; }

    float GetGmag() const
          { return Gmag; }

    float GetDGC() const
          { return DGC; }

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

    int GetTag() const
          { return tag; }

    void SetSI( float CalculatedSI )
          { SI = CalculatedSI; }

    void SetCV( float CalculatedCV )
          { CV = CalculatedCV; }

    void SetGmag( float CalculatedGmag )
          { Gmag = CalculatedGmag; }

    void SetDGC( float CalculatedDGC )
          { DGC = CalculatedDGC; }

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

    void Set_nDCMCoordinate( PointdcmCoordinate CalculatedDCM ) 
          { nDCMCoordinate = CalculatedDCM; }    

    void SetVoxelIndex( PointIndex CalculatedIndex )
          { voxel_index = CalculatedIndex; }

    void SetGroupID( int newgroupid )
          { groupid = newgroupid; }

    void SetTag( int newtag )
          { tag = newtag; }    

  private:
    
    PointdcmCoordinate DCMCoordinate;
    PointdcmCoordinate nDCMCoordinate;    
    PointIndex voxel_index;
    float SI;
    float CV;
    float Gmag;
    float DGC;
    float nSI;    // Normalized value
    float nCV;
    float nGmag;
    float u;      // Membership function
    int groupid;
    int tag;
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

    typedef std::list<AssociatedFeatureList>           GrowableRegionType;
    typedef std::vector<GrowableRegionType>            RegionCollectorType;

    typedef itk::Point<double, 3>                      dcmCoordinate;
    typedef std::list<dcmCoordinate>                   dcmVectorType;

    typedef ctc::CTCImageType::SizeType                VolumeSizeType;

    /* Standard ITK macros */
    itkNewMacro(Self);
    itkTypeMacro(FeatureExtraction, itk::Object);

    void SetSegmentedImageInput( BinaryImageType *image )
                          { m_ColonImage = image; }

    void SetRawImageInput( CTCImageType *image )
                          { m_Image = image; } 

    VolumeSizeType GetVolumeSize() 
                          { return m_Image->GetLargestPossibleRegion().GetSize(); }

    void Analyze( void );

    int PrincipalCurvatures( BinaryImageType::IndexType idx, float &SI, float &CV, float &gmag );

    void CalculateCentroid( RegionCollectorType &growableregion_vector, dcmCoordinate &centroid, int index ); 

    void MergeRegions( RegionCollectorType &growableregion_vector, list<int> merge_list, int merger ); 

    void CTC_Image_104_TransformIndexToPhysicalPoint(BinaryImageType::IndexType idx, dcmCoordinate &dcm );

    void CalculateDGC( RegionCollectorType &growableregion_vector );

    void SetOutputVTK( string name )
          { UserNamedOutput = name; }

    void SetVolumeID( int id )
          { vid = id; }

    void SetSeries( int newseries )
          { series = newseries; }
  
    int GetSeries() const
          { return series; }
        
  protected:

    FeatureExtraction();
    ~FeatureExtraction();

  private:

    CTCImageType *       m_Image;
    BinaryImageType *    m_ColonImage;
    dcmVectorType        PolypCenterCollector;
    string               UserNamedOutput;
    int                  vid;
    int                  series;

  };
 
}

#endif











