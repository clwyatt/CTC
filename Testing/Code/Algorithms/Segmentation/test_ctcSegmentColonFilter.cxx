/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// revision 52 is version that writes VTK point sets
// later versions use ITK Spatial Objects for IO

// test for ctcSegmentColonFilter
 
#include <iostream>
#include <cstdlib>
#include <string>

// ITK IO includes
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkMesh.h"
#include "itkDefaultDynamicMeshTraits.h"
#include <itkMeshSpatialObject.h>
#include <itkSpatialObjectWriter.h>

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "ctcSegmentColonFilter.h"
#include "vul_arg.h"

using namespace std;

// global typedefs
typedef itk::ImageFileWriter< ctc::BinaryImageType >  WriterType; 
typedef itk::DefaultDynamicMeshTraits< float , 3, 3 > MeshTrait;
typedef itk::Mesh<float, 3, MeshTrait > MeshType;
typedef itk::BinaryMask3DMeshSource< ctc::BinaryImageType, MeshType > MeshSourceType;


int main(int argc, char ** argv)
{

  clog << "Testing ctcSegmentColonFilter version ";
  clog <<  CTC_VERSION_STRING << CTC_REVISION_STRING << endl; 
  
  // make sure testing data path is defined
  if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }

  vul_arg<char const*> infile(0, "Dicom directory");
  vul_arg<char const*> outfilebase(0, "Output base for filenames (no extension)");
  vul_arg<int> maxiter("-i", "Maximum number of iterations", 5);
  vul_arg<int> mindist("-d", "Minimum distance considered", 10);
  vul_arg<int> thresh("-t", "Air/Tissue threshold", -800);
  vul_arg_parse(argc, argv);

  // get paths to input/output files
  string infilename = getenv("CTC_DATA_DIR");
  infilename.append("/");
  infilename.append(infile());

  // read in the DICOM series
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();
  
  reader->SetDirectory(infilename);

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  // segment
  ctc::SegmentColonFilter::Pointer filter = ctc::SegmentColonFilter::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetThreshold(thresh());
  filter->SetMaxIterations(maxiter());
  filter->SetMinDistanceThreshold(mindist());
  
  // write out binary image
  WriterType::Pointer writer = WriterType::New();
  string outfilename1 = string(outfilebase()) + ".vtk";
  writer->SetFileName(outfilename1.c_str()); 
  writer->SetInput( filter->GetOutput() );
  try 
    { 
      writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      cerr << "ExceptionObject caught !" << endl; 
      cerr << err << endl; 
      return EXIT_FAILURE; 
    } 
  
  // create mesh spatial object
  MeshSourceType::Pointer meshSource = MeshSourceType::New();
  const ctc::BinaryPixelType objectValue = 128;
  meshSource->SetObjectValue( objectValue );
  
  meshSource->SetInput( filter->GetOutput() );
  try
    {
      meshSource->Update();
    }
  catch( itk::ExceptionObject & err )
    {
      cerr << "ExceptionObject caught !" << endl;
      cerr << err << endl;
      return EXIT_FAILURE;
    }
  
  typedef itk::MeshSpatialObject<MeshType> MeshSpatialObjectType;
  MeshSpatialObjectType::Pointer myMeshSpatialObject =
    MeshSpatialObjectType::New();
  

  myMeshSpatialObject->SetMesh(meshSource->GetOutput());
  clog << "Mesh bounds : " <<
    myMeshSpatialObject->GetBoundingBox()->GetBounds() << endl;

  // write the mesh object
  typedef itk::SpatialObjectWriter<3,float,MeshTrait> WriterType;
  WriterType::Pointer mwriter = WriterType::New();

  mwriter->SetInput(myMeshSpatialObject);
  string outfilename2 = string(outfilebase()) + ".meta";
  mwriter->SetFileName(outfilename2.c_str());
  try
    {
      mwriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
      cerr << "ExceptionObject caught !" << endl;
      cerr << err << endl;
      return EXIT_FAILURE;
    }
  
  return(EXIT_SUCCESS);
}
