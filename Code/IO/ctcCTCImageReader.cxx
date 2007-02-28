/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#include <vector>
#include <string>
#include <cassert>

// CTC includes
#include "ctcConfigure.h"
#include "ctcCTCImage.h"
#include "ctcCTCImageReader.h"

// ITK IO includes
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageFileReader.h"

namespace ctc
{
  
  CTCImageReader::CTCImageReader()
  {
    m_UnsignedType = false;
  }
 
  CTCImageReader::~CTCImageReader()
  {
    
  }
  
  void CTCImageReader::GenerateData()
  {

    assert(!m_Directory.empty());

    itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

    // Get the DICOM filenames from the directory
    itk::GDCMSeriesFileNames::Pointer nameGenerator = 
      itk::GDCMSeriesFileNames::New();
    
    nameGenerator->SetDirectory( m_Directory.c_str() );

    typedef std::vector<std::string> seriesIdContainer;
    const seriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

    if(seriesUID.empty())
      {
	throw itk::ImageFileReaderException(__FILE__, 
					    __LINE__, 
					    "No DICOM Series Found");
      }

    seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    seriesIdContainer::const_iterator seriesEnd = seriesUID.end();

    typedef std::vector<std::string> fileNamesContainer;
    fileNamesContainer fileNames;
    fileNames = nameGenerator->GetFileNames(seriesUID.begin()->c_str());

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileNames( fileNames );
    reader->SetImageIO( dicomIO );
    reader->ReleaseDataFlagOn();

    reader->Update();

    this->GraftOutput( reader->GetOutput() );
  }


  void CTCImageReader::GenerateOutputInformation()
  {

  }

  void CTCImageReader::PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    
    os
      << indent << "CTCImageReader: Directory = " 
      << this->m_Directory.c_str()
      << std::endl;
  }
  
}

