/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

// command line app to segment colon
 
#include <iostream>
#include <cstdlib>
#include <string>

// ITK includes
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
#include "ctcSegmentColonWithContrastFilter.h"
#include "ctcSegmentProjectionFilter.h"
#include "vul_arg.h"

using namespace std;

// global typedefs
typedef itk::ImageFileWriter< ctc::BinaryImageType >  WriterType; 
typedef itk::ImageFileWriter< ctc::ProjectionImageType >  ProjectionWriterType;
typedef itk::DefaultDynamicMeshTraits< float , 3, 3 > MeshTrait;
typedef itk::Mesh<float, 3, MeshTrait > MeshType;
typedef itk::BinaryMask3DMeshSource< ctc::BinaryImageType, MeshType > MeshSourceType;

int main(int argc, char ** argv)
{
  vul_arg<char const*> infile(0, "Dicom directory");
  vul_arg<char const*> outfilebase(0, "Output base for filenames (no extension)");
  vul_arg<bool> segcontrast("-s", "Segment Contrast", false);
  vul_arg<int> maxiter("-i", "Maximum number of iterations (ignored if -s)", 5);
  vul_arg<int> mindist("-d", "Minimum distance considered (ignored if -s)", 10);
  vul_arg<int> thresh("-t", "Air/Tissue threshold (ignored if -s)", -800);
  vul_arg<bool> analyze("-a", "Write binary output as analyze 7.5 rather than vtk (the default)", false);
  vul_arg_parse(argc, argv);

  // read in the DICOM series
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();
  reader->SetDirectory(string(infile()));
  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }

  // this if/else needs to be refactored badly, alot of repetative code
  if( !segcontrast() )
    {
      // segment air only
      ctc::SegmentColonFilter::Pointer filter = ctc::SegmentColonFilter::New();
      filter->SetInput( reader->GetOutput() );
      filter->SetThreshold(thresh());
      filter->SetMaxIterations(maxiter());
      filter->SetMinDistanceThreshold(mindist());
  
      // write out binary image
      WriterType::Pointer writer = WriterType::New();
      string outfilename1;
      if( analyze() )
	{
	   outfilename1 = string(outfilebase()) + ".img";
	}
      else
	{
	   outfilename1 = string(outfilebase()) + ".vtk";
	}

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

      // compute and write projection image
      ctc::SegmentProjectionFilter::Pointer pfilter = ctc::SegmentProjectionFilter::New();
      pfilter->SetInput( filter->GetOutput() );
      pfilter->Update();

      ProjectionWriterType::Pointer pwriter = ProjectionWriterType::New();
      string outfilename2 = string(outfilebase()) + "_projection.png";
      pwriter->SetFileName(outfilename2.c_str()); 
      pwriter->SetInput( pfilter->GetOutput() );
      try 
	{ 
	  pwriter->Update(); 
	} 
      catch( itk::ExceptionObject & err ) 
	{ 
	  cerr << "ExceptionObject caught !" << endl; 
	  cerr << err << endl; 
	  return EXIT_FAILURE; 
	} 

      // create mesh spatial object
      MeshSourceType::Pointer meshSource = MeshSourceType::New();
      const ctc::BinaryPixelType objectValue = 255;
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
      string outfilename3 = string(outfilebase()) + ".meta";
      mwriter->SetFileName(outfilename3.c_str());
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
    }
  else
    {
      // segment air + constrast
      ctc::SegmentColonWithContrastFilter::Pointer filter = ctc::SegmentColonWithContrastFilter::New();
      filter->SetInput( reader->GetOutput() );
  
      // write out binary image
      WriterType::Pointer writer = WriterType::New();
      string outfilename1;
      if( analyze() )
	{
	   outfilename1 = string(outfilebase()) + ".img";
	}
      else
	{
	   outfilename1 = string(outfilebase()) + ".vtk";
	}
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

      // compute and write projection image
      ctc::SegmentProjectionFilter::Pointer pfilter = ctc::SegmentProjectionFilter::New();
      pfilter->SetInput( filter->GetOutput() );
      pfilter->Update();
      
      ProjectionWriterType::Pointer pwriter = ProjectionWriterType::New();
      string outfilename2 = string(outfilebase()) + "_projection.png";
      pwriter->SetFileName(outfilename2.c_str()); 
      pwriter->SetInput( pfilter->GetOutput() );
      try 
	{ 
	  pwriter->Update(); 
	} 
      catch( itk::ExceptionObject & err ) 
	{ 
	  cerr << "ExceptionObject caught !" << endl; 
	  cerr << err << endl; 
	  return EXIT_FAILURE; 
	} 

      // create mesh spatial object
      MeshSourceType::Pointer meshSource = MeshSourceType::New();
      const ctc::BinaryPixelType objectValue = 255;
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
      string outfilename3 = string(outfilebase()) + ".meta";
      mwriter->SetFileName(outfilename3.c_str());
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

    }
  
  return(EXIT_SUCCESS);
}
