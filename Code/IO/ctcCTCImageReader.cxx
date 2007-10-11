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
#include "itkMetaDataObject.h"
#include "itkMetaDataDictionary.h"


namespace ctc
{
  
  CTCImageReader::CTCImageReader()
  {
    m_UnsignedType = false;
    m_DictArray = new DictionaryArrayType;
  }
 
  CTCImageReader::~CTCImageReader()
  {
    for(int slice = 0; 
	slice < m_DictArray->size(); 
	slice++)
      {
	delete ((*(m_DictArray))[slice]);
      }
    delete m_DictArray;
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

    // Deep copy the MetaDataDictionary into the array
    for(int slice = 0; 
	slice < reader->GetMetaDataDictionaryArray()->size(); 
	slice++)
      {
	DictionaryRawPointer newDictionary = new DictionaryType;
	*newDictionary = *((*(reader->GetMetaDataDictionaryArray()))[slice]);
	m_DictArray->push_back( newDictionary );
      }

    // check the modality
    // for some reason this fails sometimes
    //char name[512];
    //dicomIO->GetModality(name);
    //assert(name == "CT");

    // get prone/supine info
    std::string PatientPosition;
    std::string tag = "0018|5100";
    itk::MetaDataDictionary dict;
    if(dicomIO->GetValueFromTag(tag, PatientPosition))
      {
	itk::EncapsulateMetaData<std::string>(dict, 
					      std::string("Orientation"), 
					      PatientPosition);
      }
    else
      {
	itk::EncapsulateMetaData<std::string>(dict, 
					      std::string("Orientation"), 
					      std::string("UNKNOWN"));
      }
      
    reader->GetOutput()->SetMetaDataDictionary(dict);

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
  

  CTCImageReader::DictionaryArrayRawPointer 
  CTCImageReader::GetMetaDataDictionaryArray() const
  {

    return m_DictArray;
  }
}

