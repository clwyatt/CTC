#include <cstdlib>
#include <iostream>
using std::clog;
using std::cerr;
using std::cout;
using std::endl;
#include <fstream>
using std::ifstream;
using std::ofstream;

#include <string>
using std::string;
#include <cctype>

// ITK includes
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkFloodFilledImageFunctionConditionalIterator.h>
#include <itkBinaryThresholdImageFunction.h>

// ITK/FLTK includes
#include <fltkImageViewer.h>

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "vul_arg.h"

// globals
ctc::BinaryImageType::Pointer gimage;

void mycallbackFunction( float x, float y, float z, float val, void * notused)
{
  std::clog << x << " " << y << " " << z << std::endl;

  // remove connected components
  typedef itk::BinaryThresholdImageFunction<ctc::BinaryImageType> BinaryFunctionType;
  BinaryFunctionType::Pointer bfunction = BinaryFunctionType::New();
  bfunction->SetInputImage(gimage);
  bfunction->ThresholdBetween(1, 255);
  
  typedef itk::FloodFilledImageFunctionConditionalIterator
    <ctc::BinaryImageType, BinaryFunctionType> BinaryFloodIteratorType;
	    
  ctc::BinaryImageType::IndexType seed;
  seed[0] = static_cast<int>(x + 0.5);
  seed[1] = static_cast<int>(y + 0.5);
  seed[2] = static_cast<int>(z + 0.5);

  BinaryFloodIteratorType bfit(gimage,
			       bfunction,
			       seed);
  bfit.GoToBegin();
  while(!bfit.IsAtEnd())
    {
      bfit.Set(0);
      ++bfit;
    }
};

int main( int argc, char* argv[] )
{
  // parse args
  vul_arg<char const*> infilename(0, "Input Segmented File", "");
  vul_arg<char const*> outfilename(0, "Segmented Output File Name", "manual.vtk");
  vul_arg_parse(argc, argv);

  // read segmented data
  typedef itk::ImageFileReader< ctc::BinaryImageType > SegDataReaderType;
  SegDataReaderType::Pointer segreader = SegDataReaderType::New();
  segreader->SetFileName(string(infilename()));

  // invoke readers
  try
    {
      segreader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  // set global image pointer
  gimage = segreader->GetOutput();

  // gui
  typedef fltk::ImageViewer<ctc::BinaryPixelType, ctc::BinaryPixelType> ViewerType;
  ViewerType viewer;
  viewer.SetImage(segreader->GetOutput());
  viewer.ClickSelectCallBack(  mycallbackFunction , 0);
  viewer.Show();
  viewer.Update();

  // force a first redraw
  Fl::check();
  viewer.Update();

  Fl::run();

  // on exit save file
  typedef itk::ImageFileWriter< ctc::BinaryImageType > WriterType;
  WriterType::Pointer segwriter = WriterType::New();
  segwriter->SetFileName(string(outfilename()));
  segwriter->SetInput(gimage);
  try
    {
      segwriter->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
