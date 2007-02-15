/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcCTCImageReader_h
#define __ctcCTCImageReader_h

#include "itkImageSource.h"
#include "itkImageSeriesReader.h"

#include "ctcCTCImage.h"

namespace ctc
{

  class ITK_EXPORT CTCImageReader :
    public itk::ImageSource<CTCImageType>
  {
  public:
    typedef CTCImageReader                        Self;
    typedef itk::ImageSource<CTCImageType>        Superclass;
    typedef itk::SmartPointer<Self>               Pointer;
    typedef itk::SmartPointer<const Self>         ConstPointer;

    // standard ITK macros
    itkNewMacro(Self);
    itkTypeMacro(CTCImageReader, itk::ImageSource);

    void PrintSelf( std::ostream& os, itk::Indent indent ) const;

    itkGetMacro(Directory, std::string);
    itkSetMacro(Directory, std::string);

  
  protected:

    CTCImageReader();
    ~CTCImageReader();
    void GenerateData();
    void GenerateOutputInformation();

    typedef itk::ImageSeriesReader< CTCImageType >     ReaderType;

  private:

  CTCImageReader(Self&);                // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented

  std::string m_Directory;

  bool m_UnsignedType;

  };

} // end namespace ctc


#endif // __ctcCTCImageReader_h
