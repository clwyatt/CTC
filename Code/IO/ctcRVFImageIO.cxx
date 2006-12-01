/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: ctcRVFImageIO.cxx 23 2006-06-29 14:45:24Z clwyatt $
Language:  C++
*******************************************************************************/

#include <fstream>

#include "ctcConfigure.h"
#include "ctcByteSwap.h"
#include "ctcRVFImageIO.h"
#include "ctcRVFHeader.h"

namespace ctc
{
  
  bool RVFImageIO::CanReadFile(const char* file) 
  { 
    // First check the extension
    std::string filename = file;
    if(  filename == "" )
      {
	itkDebugMacro(<<"No filename specified.");
	return false;
      }
    
    bool extensionFound = false;
    std::string::size_type RVFPos = filename.rfind(".rvf");
    if ((RVFPos != std::string::npos)
	&& (RVFPos == filename.length() - 4))
      {
	extensionFound = true;
      }
    
    RVFPos = filename.rfind(".RVF");
    if ((RVFPos != std::string::npos)
	&& (RVFPos == filename.length() - 4))
      {
	extensionFound = true;
      }
    
    if( !extensionFound )
      {
	itkDebugMacro(<<"The filename extension is not recognized");
	return false;
      }
    
    // Now check the file header
    RVF_header header;
    
    std::ifstream infile(file);
    if(!infile){
      itkExceptionMacro("Error RVFImageIO could not open file: " 
			<< file);
      return false;
    }
    
    infile.read((char *) &header, sizeof(header));
    
    infile.close();
    
    // determine endianness of the current machine
    // RVF files should always be big endian
    bool swabreq;
    if(!CTC_BIG_ENDIAN) //little endian
      swabreq = true;
    else // big endian
      swabreq = false;
    
    // swap if needed
    if(swabreq)
      {
	swab(header.magic);
	swab(header.version);
	swab(header.serial);
	swab(header.xDim);
	swab(header.yDim);
	swab(header.zDim);
	swab(header.bits);
	swab(header.x_flipX);
	swab(header.x_flipY);
	swab(header.y_flipX);
	swab(header.y_flipY);
	swab(header.z_flipX);
	swab(header.z_flipY);
	swab(header.xPat_org);
	swab(header.yPat_org);
	swab(header.zPat_org);
	swab(header.xPat_step);
	swab(header.yPat_step);
	swab(header.zPat_step);
	swab(header.dicomHeaderFollows);
	swab(header.dicomHeaderSize);
      }

    if(header.magic != RVF_MAGIC_NUMBER){  
      itkExceptionMacro("Error RVFImageIO could not open file: " 
			<< this->GetFileName());
      return false;
    }
    
    // seems like we can read 
    return true;
  }
  

  void RVFImageIO::Read(void* buffer)
  {
    itkDebugMacro("Read: file dimensions = " << this->GetNumberOfDimensions() );
    
    RVF_header header;
    
    std::ifstream infile(m_FileName.c_str());
  
    if(!infile){
      itkExceptionMacro("Error RVFImageIO could not open file: " 
			<< this->GetFileName());
      return;
    }
    
    infile.read((char *) &header, sizeof(header));
      
    //swap if needed
    bool swabreq;
    if(!CTC_BIG_ENDIAN)
      swabreq = true;
    else
      swabreq = false;
    
    //swap if needed
    if(swabreq)
      {
	swab(header.magic);
	swab(header.version);
	swab(header.serial);
	swab(header.xDim);
	swab(header.yDim);
	swab(header.zDim);
	swab(header.bits);
	swab(header.x_flipX);
	swab(header.x_flipY);
	swab(header.y_flipX);
	swab(header.y_flipY);
	swab(header.z_flipX);
	swab(header.z_flipY);
	swab(header.xPat_org);
	swab(header.yPat_org);
	swab(header.zPat_org);
	swab(header.xPat_step);
	swab(header.yPat_step);
	swab(header.zPat_step);
	swab(header.dicomHeaderFollows);
	swab(header.dicomHeaderSize);
      }

    if(header.magic != RVF_MAGIC_NUMBER){  
      itkExceptionMacro("Error File not of type rvf: " 
			<< this->GetFileName());
      return;
    }


    unsigned long size[3];
    size[0]  = header.xDim;
    size[1]  = header.yDim;
    size[2]  = header.zDim;
    
    // read voxels into buffer argument
    unsigned short * const p = static_cast<unsigned short *>(buffer);
    unsigned short temp;
    for(int k=0; k < size[2]*size[1]*size[0]; k++){
      infile.read((char *)(&temp), sizeof(unsigned short));
      if(swabreq) swab(temp);
      *(p + k) = temp;
    }

    // close the file
    infile.close();
  }


  RVFImageIO::RVFImageIO()
  {
    this->SetNumberOfDimensions(3);
    m_PixelType = SCALAR;
    m_ComponentType = USHORT;
    m_Spacing[0] = 1.0;
    m_Spacing[1] = 1.0;
    m_Spacing[2] = 1.0;
    
    m_Origin[0] = 0.0;
    m_Origin[1] = 0.0;
    m_Origin[2] = 0.0;
  }

  RVFImageIO::~RVFImageIO()
  {
  }

  void RVFImageIO::PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    //os << indent << "Compression Level : " << m_CompressionLevel << "\n";
  }
  
  void RVFImageIO::ReadImageInformation()
  {
    RVF_header header;
    
    std::ifstream infile(m_FileName.c_str());
    if(!infile){
      itkExceptionMacro("Error RVFImageIO could not open file: " 
			<< this->GetFileName());
      return;
    }
    
    infile.read((char *) &header, sizeof(header));
    
    infile.close();

    //swap if needed
    if(!CTC_BIG_ENDIAN)
      swab(header.magic);
    
    if(header.magic != RVF_MAGIC_NUMBER){  
      itkExceptionMacro("Error File not of type rvf: " 
			<< this->GetFileName());
      return;
    }
  
  //swap if needed
  if(!CTC_BIG_ENDIAN)
    {
      swab(header.magic);
      swab(header.version);
      swab(header.serial);
      swab(header.xDim);
      swab(header.yDim);
      swab(header.zDim);
      swab(header.bits);
      swab(header.x_flipX);
      swab(header.x_flipY);
      swab(header.y_flipX);
      swab(header.y_flipY);
      swab(header.z_flipX);
      swab(header.z_flipY);
      swab(header.xPat_org);
      swab(header.yPat_org);
      swab(header.zPat_org);
      swab(header.xPat_step);
      swab(header.yPat_step);
      swab(header.zPat_step);
      swab(header.dicomHeaderFollows);
      swab(header.dicomHeaderSize);
    }
  
  this->SetNumberOfDimensions(3);
  
  m_Spacing[0] = header.xPat_step;  
  m_Spacing[1] = header.yPat_step;
  m_Spacing[2] = header.zPat_step;

  m_Origin[0] = header.xPat_org;
  m_Origin[1] = header.yPat_org;
  m_Origin[2] = header.zPat_org;
  
  m_Dimensions[0] = header.xDim;
  m_Dimensions[1] = header.yDim;
  m_Dimensions[2] = header.zDim;
  
  m_PixelType = SCALAR;
  m_ComponentType = USHORT;
  
  return;
}

bool RVFImageIO::CanWriteFile( const char * name )
{
  std::string filename = name;

  if (filename == "")
    {
      return false;
    }
  
  std::string::size_type RVFPos = filename.rfind(".rvf");
  if ( (RVFPos != std::string::npos)
       && (RVFPos == filename.length() - 4) )
    {
      return true;
    }

  RVFPos = filename.rfind(".RVF");
  if ( (RVFPos != std::string::npos)
       && (RVFPos == filename.length() - 4) )
    {
      return true;
    }
  
  // make sure the output voxel type is ushort
  if ( (m_PixelType != SCALAR) || ( m_ComponentType != USHORT) )
    {
      return false;
    }
  
  return false;
}


void RVFImageIO::WriteImageInformation(void)
{
}

void RVFImageIO::Write(const void* buffer)
{
  itk::ImageIORegion ioRegion = this->GetIORegion();

  // make sure the region to be written is 3D
  const unsigned int ImageDimension = ioRegion.GetRegionDimension();
  if ( ImageDimension != 3 )
    {
      itkExceptionMacro(<<"RVF Writer can only write 3-dimensional sclar ushort images. You are requesting to write an image of dimension = " << ImageDimension << " with filename " << m_FileName);
    }

  // make sure the output voxel type is ushort
  if ( (m_PixelType != SCALAR) || ( m_ComponentType != USHORT) )
    {
      itkExceptionMacro(<<"RVF Writer can only write 3-dimensional scalar ushort images.");
    }
 
  
  RVF_header header;
  header.magic =  RVF_MAGIC_NUMBER;
  header.version =  RVF_CURRENT_VERSION;
  header.serial = 1; // could use rand
  header.xDim = this->GetDimensions(0);
  header.yDim = this->GetDimensions(1);
  header.zDim = this->GetDimensions(2);
  header.bits = 16;
  header.x_flipX = 0;
  header.x_flipY = 0;
  header.y_flipX = 0;
  header.y_flipY = 0;
  header.z_flipX = 0;
  header.z_flipY = 0;
  header.xPat_org = this->GetOrigin(0);
  header.yPat_org = this->GetOrigin(1);
  header.zPat_org = this->GetOrigin(2);
  header.xPat_step = this->GetSpacing(0);
  header.yPat_step = this->GetSpacing(1);
  header.zPat_step = this->GetSpacing(2);
  header.dicomHeaderFollows = 0;
  header.dicomHeaderSize = 0;

  // determine endianness of the current machine
  // RVF files should always be written in big endian
  bool swabreq;
  if(!CTC_BIG_ENDIAN) //little endian
    swabreq = true;
  else // big endian
    swabreq = false;

  // swap if needed
  if(swabreq)
    {
      swab(header.magic);
      swab(header.version);
      swab(header.serial);
      swab(header.xDim);
      swab(header.yDim);
      swab(header.zDim);
      swab(header.bits);
      swab(header.x_flipX);
      swab(header.x_flipY);
      swab(header.y_flipX);
      swab(header.y_flipY);
      swab(header.z_flipX);
      swab(header.z_flipY);
      swab(header.xPat_org);
      swab(header.yPat_org);
      swab(header.zPat_org);
      swab(header.xPat_step);
      swab(header.yPat_step);
      swab(header.zPat_step);
      swab(header.dicomHeaderFollows);
      swab(header.dicomHeaderSize);
    }


  std::ofstream  outfile;
  outfile.open(m_FileName.c_str());
  if(!outfile){
    // workaround for a bug in Visual Studio 7.1 in release mode
    // see ITK bug tracking for details
    ::itk::ExceptionObject excp(__FILE__, __LINE__, "Problem while opening the file.", "Write"); 
    throw excp; 
  }
  outfile.write((char *) (&header), sizeof(header));


  // write voxels from buffer argument
  const unsigned short *p = ( (const unsigned short *) buffer);
  unsigned short temp;
  int size = (this->GetDimensions(2))*(this->GetDimensions(1))*(this->GetDimensions(0));
  for(int k=0; k < size; k++){
    temp = *(p + k);
    if(swabreq) swab(temp);
    outfile.write((char *)(&temp), sizeof(unsigned short));
  }
  
  outfile.close();
}

} // end namespace ctc
