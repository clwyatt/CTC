/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

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
#include "itkTriangleCell.h"
#include "itkQuadrilateralCell.h"

// VTK includes
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetWriter.h"
#include "vtkCellArray.h"

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "ctcSegmentColonFilter.h"
#include "vul_arg.h"

using namespace std;

// global typedefs
typedef itk::ImageFileWriter< ctc::BinaryImageType >  WriterType; 
typedef itk::Mesh<vtkFloatingPointType, 3, 
		  itk::DefaultStaticMeshTraits<vtkFloatingPointType, 3, 3, 
					       vtkFloatingPointType, 
					       vtkFloatingPointType, 
					       vtkFloatingPointType> > MeshType;
typedef itk::BinaryMask3DMeshSource< ctc::BinaryImageType, MeshType > MeshSourceType;


class VistVTKCellsClass
{
  vtkCellArray* m_Cells;
  int* m_LastCell;
  int* m_TypeArray;
public:
  // typedef the itk cells we are interested in
  typedef itk::CellInterface<
                      MeshType::PixelType, 
                      MeshType::CellTraits >  CellInterfaceType;

  typedef itk::TriangleCell<CellInterfaceType>      floatTriangleCell;
  typedef itk::QuadrilateralCell<CellInterfaceType> floatQuadrilateralCell;

  // Set the vtkCellArray that will be constructed
  void SetCellArray(vtkCellArray* a) 
    {
      m_Cells = a;
    }
  // Set the cell counter pointer
  void SetCellCounter(int* i)
    {
      m_LastCell = i;
    }
  // Set the type array for storing the vtk cell types
  void SetTypeArray(int* i)
    {
      m_TypeArray = i;
    }
  // Visit a triangle and create the VTK_TRIANGLE cell 
  void Visit(unsigned long, floatTriangleCell* t)
    {
      m_Cells->InsertNextCell(3,  (vtkIdType*)t->PointIdsBegin());
      m_TypeArray[*m_LastCell] = VTK_TRIANGLE;
      (*m_LastCell)++;
    }
  // Visit a triangle and create the VTK_QUAD cell 
  void Visit(unsigned long, floatQuadrilateralCell* t)
    {
      m_Cells->InsertNextCell(4,  (vtkIdType*)t->PointIdsBegin());
      m_TypeArray[*m_LastCell] = VTK_QUAD;
      (*m_LastCell)++;
    }
};

typedef itk::CellInterfaceVisitorImplementation<
vtkFloatingPointType, MeshType::CellTraits,
  itk::TriangleCell< itk::CellInterface<MeshType::PixelType, MeshType::CellTraits > >, 
  VistVTKCellsClass> TriangleVisitor;


typedef itk::CellInterfaceVisitorImplementation<
vtkFloatingPointType, MeshType::CellTraits,
  itk::QuadrilateralCell< itk::CellInterface<MeshType::PixelType, MeshType::CellTraits > >, 
  VistVTKCellsClass> QuadrilateralVisitor;

// convert ITK mesh to VTK polygonal mesh
vtkUnstructuredGrid* MeshToUnstructuredGrid(MeshType::Pointer mesh)
{
  // Get the number of points in the mesh
  int numPoints = mesh->GetNumberOfPoints();
  if(numPoints == 0)
    {
    mesh->Print(std::cerr);
    std::cerr << "no points in Grid " << std::endl;
    exit(-1);
    }
  // Create a vtkUnstructuredGrid
  vtkUnstructuredGrid* vgrid = vtkUnstructuredGrid::New();

  // Create the vtkPoints object and set the number of points
  vtkPoints* vpoints = vtkPoints::New();
  vpoints->SetNumberOfPoints(numPoints);
  // iterate over all the points in the itk mesh filling in
  // the vtkPoints object as we go
  MeshType::PointsContainer::Pointer points = mesh->GetPoints();
  for(MeshType::PointsContainer::Iterator i = points->Begin();
      i != points->End(); ++i)
    {
    // Get the point index from the point container iterator
    int idx = i->Index();
    // Set the vtk point at the index with the the coord array from itk
    // itk returns a const pointer, but vtk is not const correct, so
    // we have to use a const cast to get rid of the const
    vpoints->SetPoint(idx, const_cast<vtkFloatingPointType*>(i->Value().GetDataPointer()));
    }
  // Set the points on the vtk grid
  vgrid->SetPoints(vpoints);
  
  // Now create the cells using the MulitVisitor
  // 1. Create a MultiVisitor
  MeshType::CellType::MultiVisitor::Pointer mv =
    MeshType::CellType::MultiVisitor::New();
  // 2. Create a triangle and quadrilateral visitor
  TriangleVisitor::Pointer tv = TriangleVisitor::New();
  QuadrilateralVisitor::Pointer qv =  QuadrilateralVisitor::New();
  // 3. Set up the visitors
  int vtkCellCount = 0; // running counter for current cell being inserted into vtk
  int numCells = mesh->GetNumberOfCells();
  int *types = new int[numCells]; // type array for vtk 
  // create vtk cells and estimate the size
  vtkCellArray* cells = vtkCellArray::New();
  cells->EstimateSize(numCells, 4);
  // Set the TypeArray CellCount and CellArray for both visitors
  tv->SetTypeArray(types);
  tv->SetCellCounter(&vtkCellCount);
  tv->SetCellArray(cells);
  qv->SetTypeArray(types);
  qv->SetCellCounter(&vtkCellCount);
  qv->SetCellArray(cells);
  // add the visitors to the multivisitor
  mv->AddVisitor(tv);
  mv->AddVisitor(qv);
  // Now ask the mesh to accept the multivisitor which
  // will Call Visit for each cell in the mesh that matches the
  // cell types of the visitors added to the MultiVisitor
  mesh->Accept(mv);
  
  // Now set the cells on the vtk grid with the type array and cell array
  vgrid->SetCells(types, cells);
  
  // Clean up vtk objects (no vtkSmartPointer ... )
  cells->Delete();
  vpoints->Delete();
  // return the vtkUnstructuredGrid
  return vgrid;
}


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
  

  // compute and write out polygonal representation
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

  vtkUnstructuredGrid* grid = MeshToUnstructuredGrid( meshSource->GetOutput() );
  vtkDataSetWriter* polywriter = vtkDataSetWriter::New();
  polywriter->SetInput(grid);
  string outfilename2 = string(outfilebase()) + ".vtu";
  polywriter->SetFileName(outfilename2.c_str());
  polywriter->Update();

  grid->Delete();
  
  return(EXIT_SUCCESS);
}
