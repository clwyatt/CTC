/*====================================================================
  Program:   CTC Project (Wake Forest University - Virginia Tech CTC Software)
  Module:    testMain_multiDemon3Dfilter.cxx
  Language:  C++
  Date:       2006/10/04 
  Author:   Jung Wook Suh (Bioimaging Systems Lab. in Virginia Tech)
  
  Description:  Test program for Multiresolution-demons filter.
				
                Inputs: Two colon-segmented 3D volumes for source(moving image) and target(
                        fixed image)
                Output: - Registered 3D volume
                        - Deformation field ( using GetDeformationField() method )
			
                The number of layers for multiresolution and the iteration numbers in each layer
                can be adjusted using "SetNumberOfLayers(Layer, Iter)" and 
                unsigned int Iter[3]={20,20,10} when Layer = 3.
                Default: Layer = 4, unsigned int Iter[4]={500,500,500, 200};
======================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "multiDemon3Dfilter.h"

int main( int argc, char* argv[] )
{
  if( argc < 5 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " FixedImage MovingImage OutputImage DeformationField" << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::ImageFileReader< ctc::RegistrationInputImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< ctc::RegistrationInputImageType > MovingImageReaderType;
  
  FixedImageReaderType::Pointer fixedImageReader = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();
  
  fixedImageReader->SetFileName( argv[1] );		fixedImageReader->Update();
  movingImageReader->SetFileName( argv[2] );		movingImageReader->Update(); 
   
  typedef ctc::multiDemon3Dfilter FilterType;			 		
  FilterType::Pointer filter = FilterType::New();                   
  
  filter->SetMovingImage(movingImageReader->GetOutput());     
  filter->SetFixedImage(fixedImageReader->GetOutput());           
  
  //int Layer=3;
  //unsigned int Iter[3]={20,20,10};
  //filter->SetNumberOfLayers(Layer, Iter);
  //filter->SetInput(fixedImageReader->GetOutput());
  //filter->Update();                                                                    
  

  typedef itk::ImageFileWriter< ctc::RegistrationOutputImageType >  WriterType;
  
  WriterType::Pointer      writer =  WriterType::New();                   
  writer->SetFileName( argv[3] );
  writer->SetInput( filter->GetOutput() );                                        
  
    try
    {
    writer->Update();                                                                   
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Error: " << e << std::endl;
    }

  
  typedef itk::ImageFileWriter< ctc::multiDemon3Dfilter::DeformationFieldType > FieldWriterType;

    FieldWriterType::Pointer fieldWriter = FieldWriterType::New();
    fieldWriter->SetFileName( argv[4] );
    fieldWriter->SetInput( filter->GetDeformationField() );
    try
    {
    fieldWriter->Update();                                                           
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Error: " << e << std::endl;
    }	
  return 0;
}
