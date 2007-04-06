/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
/*
Oriented Volume Rasterized as Feature
*/

#include "FeatureExtractionMethods.h"

#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"
#include "ctcRawVolumeExtraction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkCastImageFilter.h"
#include "itkSymmetricEigenSystem.h"

using namespace std;

void ExtractOrientedVolumeFeature(ofstream & out, const EntryType & e)
{

  // construct the directory name
  string dir = getenv("CTC_IMG_DIR");
  dir.append(e.mrn);
  dir.append("/");

  char temp[100];
  sprintf(&(temp[0]), "%06i.STU/", atoi(e.study.c_str()));
  dir.append(temp);
  sprintf(&(temp[0]), "%06i.SER/000001.ACQ", atoi(e.series.c_str()));
  dir.append(temp);
  
  ctc::CTCImageReader::Pointer reader = ctc::CTCImageReader::New();

  reader->SetDirectory(dir);

  try
    {
      reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      std::clog << reader << std::endl;
      throw EXIT_FAILURE;
    }

  // extract subregion around polyp
  typedef ctc::RawVolumeExtraction FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(reader->GetOutput());
  itk::Point<double, 3> point;
  point[0] = e.x;
  point[1] = e.y;
  point[2] = e.z;
  filter->SetDICOMCoordinate(point);
  filter->SetCropSize(e.size);
  try
    {
      filter->Update();
    }
  catch(itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      std::clog << filter << std::endl;
      return;
    }

  // compute the Hessian at each point in the subvolume
  typedef itk::Image<float, 3> FloatImageType;
  typedef itk::CastImageFilter< ctc::CTCImageType,
    FloatImageType > CastFilterType;
  typedef itk::RecursiveGaussianImageFilter< FloatImageType,
    FloatImageType>  DerivativeFilterType;
  typedef itk::ImageDuplicator< FloatImageType >   DuplicatorType;

  DuplicatorType::Pointer Ixx = DuplicatorType::New();
  DuplicatorType::Pointer Ixy = DuplicatorType::New();
  DuplicatorType::Pointer Ixz = DuplicatorType::New();
  DuplicatorType::Pointer Iyy = DuplicatorType::New();
  DuplicatorType::Pointer Iyz = DuplicatorType::New();
  DuplicatorType::Pointer Izz = DuplicatorType::New();

  DerivativeFilterType::Pointer ga = DerivativeFilterType::New();
  DerivativeFilterType::Pointer gb = DerivativeFilterType::New();
  DerivativeFilterType::Pointer gc = DerivativeFilterType::New();

  //Izz
  ga->SetDirection(0);
  ga->SetSigma(1.0);
  ga->SetZeroOrder();
  gb->SetDirection(1);
  gb->SetSigma(1.0);
  gb->SetZeroOrder();
  gc->SetDirection(2);
  gc->SetSigma(1.0);
  gc->SetSecondOrder();

  CastFilterType::Pointer castfilter = CastFilterType::New();
  castfilter->SetInput(filter->GetOutput());
  castfilter->Update();

  ga->SetInput( castfilter->GetOutput() );
  gb->SetInput( ga->GetOutput() );
  gc->SetInput( gb->GetOutput() );
  Izz->SetInputImage( gc->GetOutput() );
  gc->Update();
  Izz->Update();

  // Iyy
  gc->SetDirection( 1 );  // gc now works along Y
  gb->SetDirection( 2 );  // gb now works along Z
  Iyy->SetInputImage( gc->GetOutput() );
  gc->Update();
  Iyy->Update();

  // Ixx
  gc->SetDirection( 0 );  // gc now works along X
  ga->SetDirection( 1 );  // ga now works along Y
  Ixx->SetInputImage( gc->GetOutput() );
  gc->Update();
  Ixx->Update();

  // Iyz
  ga->SetDirection( 0 );
  gb->SetDirection( 1 );
  gc->SetDirection( 2 );

  ga->SetZeroOrder();
  gb->SetFirstOrder();
  gc->SetFirstOrder();
  Iyz->SetInputImage( gc->GetOutput() );
  gc->Update();
  Iyz->Update();

  // Ixz
  ga->SetDirection( 1 );
  gb->SetDirection( 0 );
  gc->SetDirection( 2 );

  ga->SetZeroOrder();
  gb->SetFirstOrder();
  gc->SetFirstOrder();
  Ixz->SetInputImage( gc->GetOutput() );
  gc->Update();
  Ixz->Update();

  // Ixy
  ga->SetDirection( 2 );
  gb->SetDirection( 0 );
  gc->SetDirection( 1 );

  ga->SetZeroOrder();
  gb->SetFirstOrder();
  gc->SetFirstOrder();
  Ixy->SetInputImage( gc->GetOutput() );
  gc->Update();
  Ixy->Update();


  // walk the subregion, if a voxel is on the boundary
  // compute the oriented subvolume as a feature
  // and write it to the file
  typedef itk::ImageRegionIteratorWithIndex< ctc::CTCImageType > IteratorType;
  ctc::CTCImageType::Pointer region = filter->GetOutput();
  IteratorType it(region, 
		  region->GetRequestedRegion());
  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      // is there is a better way to do this in ITK?
      ctc::CTCImageType::IndexType center = it.GetIndex();
      ctc::CTCImageType::IndexType right = center;
      right[0] += 1;
      ctc::CTCImageType::IndexType left = center;
      left[0] -= 1;
      ctc::CTCImageType::IndexType bottom = center;
      bottom[1] += 1;
      ctc::CTCImageType::IndexType top = center;
      top[1] -= 1;
      ctc::CTCImageType::IndexType front = center;
      front[2] += 1;
      ctc::CTCImageType::IndexType back = center;
      back[2] -= 1;

      int count = 0;
      if(region->GetPixel(right) < -800) count += 1;
      if(region->GetPixel(left) < -800) count += 1;
      if(region->GetPixel(bottom) < -800) count += 1;
      if(region->GetPixel(top) < -800) count += 1;
      if(region->GetPixel(front) < -800) count += 1;
      if(region->GetPixel(back) < -800) count += 1;

      // boundary check
      if( (count > 0) && (count < 6) )
	{
	  // detemine the normalized local frame
	  typedef itk::SymmetricEigenSystem< double, 3 > SystemType;
	  SystemType::MatrixType H;
	  H[0][0]= Ixx->GetOutput()->GetPixel(center);
	  H[0][1]= Ixy->GetOutput()->GetPixel(center);
	  H[0][2]= Ixz->GetOutput()->GetPixel(center);
	  H[1][0]= Ixy->GetOutput()->GetPixel(center);
	  H[1][1]= Iyy->GetOutput()->GetPixel(center);
	  H[1][2]= Iyz->GetOutput()->GetPixel(center);
	  H[2][0]= Ixz->GetOutput()->GetPixel(center);
	  H[2][1]= Iyy->GetOutput()->GetPixel(center);
	  H[2][2]= Izz->GetOutput()->GetPixel(center);
	
	  SystemType::Pointer S = SystemType::New();
	  S->SetMatrix(&H);
	  S->SetUseAbsoluteOrder(true);
	  S->Update();
	  
	  SystemType::EigenVectorArrayType* ev = S->GetEigenVectors();
	  SystemType::ArrayType norm = ev[0][0];
	  double mag = sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
	  norm[0] = norm[0]/mag;
	  norm[1] = norm[1]/mag;
	  norm[2] = norm[2]/mag;
	  SystemType::ArrayType axis1 = ev[0][1];
	  mag = sqrt(axis1[0]*axis1[0]+axis1[1]*axis1[1]+axis1[2]*axis1[2]);
	  axis1[0] = axis1[0]/mag;
	  axis1[1] = axis1[1]/mag;
	  axis1[2] = axis1[2]/mag;
	  SystemType::ArrayType axis2 = ev[0][2];
	  mag = sqrt(axis2[0]*axis2[0]+axis2[1]*axis2[1]+axis2[2]*axis2[2]);
	  axis2[0] = axis2[0]/mag;
	  axis2[1] = axis2[1]/mag;
	  axis2[2] = axis2[2]/mag;	  

	  // walk the oriented box in the original image space
	  // interpolating as we go and writing output
	  out << e.mrn.c_str() << "," 
	      << e.series.c_str() << ","
	      << e.x << ","
	      << e.y << ","
	      << e.z;
	  float delta = 1;
	  float boxsize = 1;
	  for(float i = -boxsize; i <= boxsize; i += delta)
	    for(float j = -boxsize; j <= boxsize; j += delta)
	      for(float k = -boxsize; k <= boxsize; k += delta)
		{
		  itk::Point<double, 3> p;
		  p[0] = e.x + i*axis1[0] + j*axis1[1] + k*axis1[2]; 
		  p[1] = e.y + i*axis2[0] + j*axis2[1] + k*axis2[2]; 
		  p[2] = e.z + i*norm[0] + j*norm[1] + k*norm[2];

		  ctc::CTCImageType::IndexType index;
		  ctc::CTCImageType::Pointer image = reader->GetOutput();
		  bool inside = image->TransformPhysicalPointToIndex(p, index);
		  if(!inside)
		    {
		      clog << "ERROR: point " << p << "not inside volume." << endl;
		    }
		  else
		    {
		      float value = static_cast<float>(image->GetPixel(index));
		      out << "," << value;
		    }
		}
	  out << endl;
	}

    }
 
  
};
