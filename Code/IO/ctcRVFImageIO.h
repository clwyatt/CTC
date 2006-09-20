/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: ctcRVFImageIO.h 23 2006-06-29 14:45:24Z clwyatt $
Language:  C++
*******************************************************************************/

#ifndef __ctcRVFImageIO_h
#define __ctcRVFImageIO_h

#include <fstream>
#include <itkImageIOBase.h>

namespace ctc
{
  /**
   * \author Chris L. Wyatt
   * \brief Class that defines how to read and write RVF file format.
   * */
  class ITK_EXPORT RVFImageIO : public itk::ImageIOBase
  {
  public:
    /** Standard class typedefs. */
    typedef RVFImageIO               Self;
    typedef itk::ImageIOBase         Superclass;
    typedef itk::SmartPointer<Self>  Pointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(RVFImageIO, ImageIOBase);
    
    /*-------- This part of the interface deals with reading data. ------ */
    
    /** Determine the file type. Returns true if this ImageIO can read the
     * file specified. */
    virtual bool CanReadFile(const char*);
    
    /** Set the spacing and diemention information for the set filename. */
    virtual void ReadImageInformation();
    
    /** Reads the data from disk into the memory buffer provided. */
    virtual void Read(void* buffer);
    
    /*-------- This part of the interfaces deals with writing data. ----- */
    
    /** Determine the file type. Returns true if this ImageIO can read the
     * file specified. */
    virtual bool CanWriteFile(const char*);
    
    /** Writes the spacing and dimentions of the image.
     * Assumes SetFileName has been called with a valid file name. */
    virtual void WriteImageInformation();
    
    /** Writes the data to disk from the memory buffer provided. Make sure
     * that the IORegion has been set properly. */
    virtual void Write(const void* buffer);
    
  protected:
    RVFImageIO();
    ~RVFImageIO();
    void PrintSelf(std::ostream& os, itk::Indent indent) const;
    
  private:
    RVFImageIO(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  };
  
} // end namespace ctc

#endif // __ctcRVFImageIO_h
