/*====================================================================
  Program:   CTC Project (Wake Forest University - Virginia Tech CTC Software)
  Module:    multiDemon3Dfilter.h
  Language:  C++
  Date:       2006/10/04 
  Author:   Jung Wook Suh (Bioimaging Systems Lab. in Virginia Tech)
  
  Description:  Multiresolution for demons algorithm is implemented in this filter.
                The number of layers for multiresolution and the iteration numbers in each layer
                can be adjusted using "SetNumberOfLayers(Layer, Iter)" and 
                unsigned int Iter[3]={20,20,10} when Layer = 3.
                Default: Layer = 4, unsigned int Iter[4]={500,500,500, 200};
				
                Inputs: Two colon-segmented 3D volumes for source(moving image) and target(
                          fixed image)
                Output: - Registered 3D volume
                        - Deformation field ( using GetDeformationField() method )
					 
                Internal format: Two distance-mapped 3D volumes of colons (float).
			
   Options:    GET_INFORMATION_ON_ITERATION  => offers image error during iterations.
               TEST_FILTER_UNSIGNED_CHAR => When using testMain_multiDemon3Dfilter.cxx
                    for testing, the "unsigned char" pixel typed input files can be used.
               REAL_FILTER_UNSIGNED_SHORT => When this filter is used as a embedded
                    filter for CTC project, the "unsigned short" pixel typed input images can
                    be used.
======================================================================*/

#include "itkImageToImageFilter.h"

#include "itkImageFileReader.h" 
#include "itkImageFileWriter.h" 
#include "itkImageRegionIterator.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkMultiResolutionPDEDeformableRegistration.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"

#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkExceptionObject.h"


#define GET_INFORMATION_ON_ITERATION
#define TEST_FILTER_UNSIGNED_CHAR
//#define REAL_FILTER_UNSIGNED_SHORT

const unsigned int Dimension = 3;

#ifdef TEST_FILTER_UNSIGNED_CHAR
typedef  unsigned char   InputPixelType;   // unsigned short in final version
typedef  unsigned char   OutputPixelType;   // unsigned short in final version
#endif

#ifdef REAL_FILTER_UNSIGNED_SHORT
typedef  unsigned short   InputPixelType;   
typedef  unsigned short   OutputPixelType;   
#endif


namespace ctc {	  

typedef itk::Image< InputPixelType,  Dimension >   RegistrationInputImageType;
typedef itk::Image< OutputPixelType,  Dimension >   RegistrationOutputImageType;
		
template <typename TRegistration>
class RegistrationInterfaceCommand : public itk::Command
{

public:
  typedef  RegistrationInterfaceCommand   Self;
  typedef  itk::Command                   Superclass;
  typedef  itk::SmartPointer<Self>        Pointer;
  itkNewMacro( Self );
protected:
  RegistrationInterfaceCommand() {};

public:
  typedef   TRegistration                              RegistrationType;
  typedef   RegistrationType *                         RegistrationPointer;

  typedef itk::Vector< float, Dimension >    VectorPixelType;
  typedef itk::Image< float, Dimension > InternalImageType;
  typedef itk::Image<  VectorPixelType, Dimension > DeformationFieldType;
  typedef itk::DemonsRegistrationFilter<
                            InternalImageType,
                            InternalImageType,
                            DeformationFieldType> RegistrationFilterType;
  
  void Execute(itk::Object * object, const itk::EventObject & event)
  {

    if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
      return;
      }
     

    RegistrationPointer registration =
                            dynamic_cast<RegistrationPointer>( object );
    // for MultiResolutionPDEDeformableRegistration


    RegistrationFilterType * filter = dynamic_cast< RegistrationFilterType * >(
                       registration->GetRegistrationFilter() );
       // for Demon Registration algorithm
    

    std::cout << "level:"<<registration->GetCurrentLevel() <<std::endl;
                 
    if ( registration->GetCurrentLevel() == 1 )
      {
        std::cout << "*****   level 111111" <<std::endl;

        std::cout<<"In Multilevel : "  <<  filter->GetElapsedIterations()<< std::endl;

      }
  }


  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }
};

//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
  class CommandIterationUpdate : public itk::Command
  {
  public:
    typedef  CommandIterationUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<CommandIterationUpdate>  Pointer;

    double lPreviousValue;
    double lCurrentValue ;
    double lDiff;
    double lDiff_Diff;
    double lPrevDiff;
    int     nContinueCount, nIncreased;

    itkNewMacro( CommandIterationUpdate );
  protected:
    CommandIterationUpdate() {};

    typedef itk::Image< float, Dimension > InternalImageType;
    typedef itk::Vector< float, Dimension >    VectorPixelType;
    typedef itk::Image<  VectorPixelType, Dimension > DeformationFieldType;
    typedef itk::DemonsRegistrationFilter<
                                  InternalImageType,
                                  InternalImageType,
                                  DeformationFieldType> RegistrationFilterType;
  public:
     
    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
           RegistrationFilterType * filter =
            dynamic_cast< RegistrationFilterType * >( caller );

            unsigned int unElapsedIter;  // by wook

          if( !(itk::IterationEvent().CheckEvent( &event )) )
            {
            return;
            }
            
          unElapsedIter = filter->GetElapsedIterations();
          lPreviousValue = lCurrentValue;
          lCurrentValue = filter->GetMetric();

          lPrevDiff = lDiff;
          lDiff = fabs(lPreviousValue - lCurrentValue);
          lDiff_Diff = fabs(lPrevDiff - lDiff) *100;
                   
          
	   if(unElapsedIter == 1){
			  nContinueCount=0;
			  nIncreased =0;
		  }
          if(lDiff < 0.0001) nContinueCount++;    // when input data is signed float (minus value for inside)
          //if(lDiff < 0.1) nContinueCount++;  // when input data is unsigned short
          else  nContinueCount = 0;

          #ifdef GET_INFORMATION_ON_ITERATION
          if(lPreviousValue > lCurrentValue){
            std::cout <<unElapsedIter<<" : prv="<<lPreviousValue<<" curr="<<lCurrentValue<< " diff= -"<<lDiff<<" nCnt="<<nContinueCount<<" nIn="<<nIncreased<<std::endl;
            nIncreased = 0;                                                                                                              
          } 
          else{
            std::cout <<unElapsedIter<<" : prv="<<lPreviousValue<<" curr="<<lCurrentValue<< " diff= +"<<lDiff<<" nCnt="<<nContinueCount<<" nIn="<<nIncreased<< std::endl;
            nIncreased++;                                                                                                                         
          } 
                                                                                                                                                                                                                                
          if(nContinueCount > 10){
                nContinueCount = 0;
                filter->StopRegistration();
                std::cout << "**************** registration Stop by continuous negligible variations     "<<std::endl;                                
          }

          if(nIncreased > 20){
                nIncreased = 0;
                filter->StopRegistration();
                std::cout << "**************** registration Stop by continumes metric increasement      "<<std::endl;                              
          }
          
                        
          #endif
                                                         
      }

    void Execute(const itk::Object * object, const itk::EventObject & event)
      {
   
      }
  };
  

class ITK_EXPORT multiDemon3Dfilter :
	public  itk::ImageToImageFilter< RegistrationInputImageType, RegistrationOutputImageType >	
{
	public:
	
		typedef float PixelType;
		typedef itk::Image< PixelType, Dimension >  ImageType;

		typedef itk::Image< float, Dimension > ImageType;  
		typedef itk::Vector< float, Dimension >    VectorPixelType;
		typedef itk::Image<  VectorPixelType, Dimension > DeformationFieldType;
		
		typedef itk::DemonsRegistrationFilter<
			                         ImageType,
						    ImageType,
						    DeformationFieldType> RegistrationFilterType;
	  
		typedef itk::MultiResolutionPDEDeformableRegistration<
                                              ImageType,
						    ImageType,
						    DeformationFieldType >   MultiResRegistrationFilterType;
	
		typedef itk::HistogramMatchingImageFilter<                      
			                        ImageType,
						   ImageType >   MatchingFilterType;	

		typedef itk::WarpImageFilter<                                               
			                        RegistrationInputImageType, 
						   RegistrationOutputImageType,
						   DeformationFieldType  >     WarperType;
  
		typedef itk::LinearInterpolateImageFunction<
                                   RegistrationInputImageType,
                                   double          >  InterpolatorType;	
		
		typedef  float  LevelsetPixelType;
              typedef itk::Image< LevelsetPixelType, Dimension >   LevelsetImageType;
	       typedef itk::SignedDanielssonDistanceMapImageFilter<
			                    RegistrationInputImageType, 
                                         LevelsetImageType >  DistanceMapFilterType;
		
	public:
	  typedef multiDemon3Dfilter               Self;
	  typedef itk::ImageToImageFilter< RegistrationInputImageType, RegistrationOutputImageType > Superclass;
	  typedef itk::SmartPointer<Self>                        Pointer;
	  typedef itk::SmartPointer<const Self>                  ConstPointer;
	  
	  itkNewMacro(Self);
	  itkTypeMacro(multiDemon3Dfilter, itk::ImageToImageFilter);
	  //void PrintSelf( std::ostream& os, Indent indent ) const;
	  void multiDemon3Dfilter::SetNumberOfLayers(int LayerNumbers, unsigned int* Iterations);
	  
 	  void SetMovingImage( RegistrationInputImageType*);  
	  void SetFixedImage( RegistrationInputImageType*);
	  DeformationFieldType* GetDeformationField();
	  	 
	protected:
	  
	  multiDemon3Dfilter();
	  //virtual ~multiDemon3Dfilter() {};
	  void GenerateData();
	  		
	private:	  
		multiDemon3Dfilter(Self&);   // intentionally not implemented
		void operator=(const Self&);          // intentionally not implemented  	
		
		DistanceMapFilterType::Pointer m_DistanceMapFilterMoving;
		DistanceMapFilterType::Pointer m_DistanceMapFilterFixed;
		
		MatchingFilterType::Pointer m_HistogramMatcher;
		RegistrationFilterType::Pointer m_RegistrationFilter;
		MultiResRegistrationFilterType::Pointer m_MultiRes;	
		WarperType::Pointer m_Warper;
		InterpolatorType::Pointer m_Interpolator;
		
		// parameters
		int m_NumberOfLayers;
		unsigned int *m_Iterations;	
		
};

multiDemon3Dfilter::multiDemon3Dfilter()
{
	m_DistanceMapFilterMoving = DistanceMapFilterType::New();
	m_DistanceMapFilterFixed = DistanceMapFilterType::New();	
	m_HistogramMatcher = MatchingFilterType::New();                                        
	m_RegistrationFilter = RegistrationFilterType::New();                        
	m_MultiRes = MultiResRegistrationFilterType::New();                                                 
	
	m_NumberOfLayers =4;  // Default of the number of layers is 4
	m_Iterations = new unsigned int[m_NumberOfLayers];
	m_Iterations[0] =500;	m_Iterations[1] =500;	m_Iterations[2] =500;	m_Iterations[3] =200;                                            
	
	m_Warper = WarperType::New();        
	m_Interpolator = InterpolatorType::New();                                                                                                                              	
	
}  // constructor

void multiDemon3Dfilter::SetNumberOfLayers(int LayerNumbers, unsigned int* Iterations)
{
	m_NumberOfLayers = LayerNumbers;
	delete []m_Iterations;   // memory free because of the default memory assigned within 
	                                 // constructor.
	
	m_Iterations = new unsigned int[m_NumberOfLayers];
	
	for(int i=0; i<LayerNumbers; i++){
		std::cout<<"Iteration "<<i<<" "<<Iterations[i]<<std::endl;
		m_Iterations[i] = Iterations[i];
	}
	m_MultiRes->SetNumberOfLevels( m_NumberOfLayers );
	m_MultiRes->SetNumberOfIterations( m_Iterations ); 
}

void multiDemon3Dfilter::SetMovingImage( RegistrationInputImageType* movingImage)
{
	m_DistanceMapFilterMoving->SetInput(movingImage);  	
	m_Warper->SetInput( movingImage );                              
	// warping the input moving image instead of level-set represented image.
	
	
	// for test	
	SetInput(movingImage);
} // SetMovingImage

void multiDemon3Dfilter::SetFixedImage( RegistrationInputImageType* fixedImage)
{
	m_DistanceMapFilterFixed->SetInput(fixedImage);
	  
	m_Warper->SetOutputSpacing( fixedImage->GetSpacing() );
	m_Warper->SetOutputOrigin( fixedImage->GetOrigin() );         
	
} // SetFixedImage

void multiDemon3Dfilter::GenerateData()
{       
	std::cout << "Now converting the moving image into level-set representation...."<<std::endl;
	m_DistanceMapFilterMoving->Update();			
	
	std::cout << "Now converting the Fixed image into level-set representation...."<<std::endl;
	m_DistanceMapFilterFixed->Update();						
	
	std::cout << "Now Histogram Filtering...."<<std::endl;
	m_HistogramMatcher->SetNumberOfHistogramLevels( 1024 );           
	m_HistogramMatcher->SetNumberOfMatchPoints( 7 );
	m_HistogramMatcher->ThresholdAtMeanIntensityOn();     
	m_HistogramMatcher->SetInput( m_DistanceMapFilterMoving->GetOutput() ); 
	m_HistogramMatcher->SetReferenceImage( m_DistanceMapFilterFixed->GetOutput() );  
	//m_HistogramMatcher->Update();			
	
	m_RegistrationFilter->SetNumberOfIterations( 50 );
	m_RegistrationFilter->SetStandardDeviations( 1.0 );                           
	
        /////////////////////// Add observer  for RegistrationFilter  /////////////////////////	
	 CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();           
	 m_RegistrationFilter->AddObserver( itk::IterationEvent(), observer );                           
	 /////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::cout << "Now Multiresolution Filtering...."<<std::endl; 
	m_MultiRes->SetNumberOfLevels( m_NumberOfLayers );
	m_MultiRes->SetRegistrationFilter( m_RegistrationFilter );                                          
	m_MultiRes->SetNumberOfIterations( m_Iterations );  
	
	 /////////////////// Add observer  for MultiResolutionFilter  ///////////////////////	
	typedef RegistrationInterfaceCommand<MultiResRegistrationFilterType> CommandType;
	CommandType::Pointer command = CommandType::New();
	m_MultiRes->AddObserver( itk::IterationEvent(), command );                                      
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	
	m_MultiRes->SetFixedImage( m_DistanceMapFilterFixed->GetOutput() );  	
	m_MultiRes->SetMovingImage( m_HistogramMatcher->GetOutput() );                                                       		
	std::cout << "Now warping the resulting image...."<<std::endl; 
	m_Warper->SetDeformationField( m_MultiRes->GetOutput() );      
	//m_Warper->SetEdgePaddingValue(65535L);     
	m_Warper->SetInterpolator( m_Interpolator ); 
	
	m_Warper->Update();                                                                
	
	 this->GraftOutput( m_Warper->GetOutput() );  // Final output       
	                                                                                               
}

multiDemon3Dfilter::DeformationFieldType*  multiDemon3Dfilter::GetDeformationField()
{
	return m_MultiRes->GetOutput();  
}  // GetDeformationField()

}  // namespace ctc



