/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SignedDanielssonDistanceMapImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/21 19:50:19 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif


#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkShiftScaleImageFilter.h"



int main( int argc, char * argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile outputDistanceMapImageFile ";
    //std::cerr << " outputVoronoiMapImageFilter ";
    //std::cerr << " outputVectorMapImageFilter ";

    std::cerr << std::endl;  
    return EXIT_FAILURE;
    }

  typedef  unsigned char   InputPixelType;
  typedef  float  OutputPixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;
 
  typedef itk::SignedDanielssonDistanceMapImageFilter<
                                         InputImageType, 
                                         OutputImageType >  FilterType;
  
  FilterType::Pointer filter = FilterType::New();
  // Software Guide : EndCodeSnippet
  

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[2] );

  reader->Update();
          
  filter->SetInput( reader->GetOutput() );
  //////////////////////////////////////////////////////////////////
  filter->InsideIsPositiveOn();    // Inside is Positive
  /////////////////////////////////////////////////////////////////
  filter->Update();
  //scaler->SetInput( filter->GetOutput() );
  //writer->SetInput( scaler->GetOutput() );

  std::cout << "here?" <<std::endl;
  // modified by wook
  writer->SetInput( filter->GetOutput() );

  std::cout << "here2" <<std::endl;


  writer->Update();    std::cout << "here3" <<std::endl;
  
///////////////////////////////////////////////////////////
//  std::cout<< "scale value = "<<scaler->GetScale()<<"  shift value="<<scaler->GetShift()<<std::endl; // by wook to get the scale value.
                                                                                                                          
   return EXIT_SUCCESS;
}

