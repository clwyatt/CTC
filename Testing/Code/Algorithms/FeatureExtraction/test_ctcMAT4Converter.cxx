#include <cstdlib>
#include <iostream>
#include <string>
#include "ctcConfigure.h"
#include "vul_arg.h"
#include "ctcCTCImage.h"
#include "itkImageFileReader.h"
#include "ctcSegmentColonFilter.h"
#include "ctcPrincipleCurvatureExtraction.h"
#include "ctcFeatureExtraction.h"
#include "MAT4Converter.h"

using namespace ctc;
using namespace std;

/* Initial version of the MAT4 Converter test program*/
int main( int argc, char* argv[] )
{
    
    /* make sure testing data path is defined */
    if(!getenv("CTC_DATA_DIR"))
    {
      cerr << "Error: Environment variable CTC_DATA_DIR not defined" << endl;
      return EXIT_FAILURE;
    }

    vul_arg<char const*> datainfile(0, "input volume data file");
    vul_arg<char const*> seginfile(0, "input segmented volume file");
    vul_arg<char const*> outfilename(0, "Output feature filename");
    vul_arg_parse(argc, argv);

    /* get paths to input/output files */
    string rawinfilename = getenv("CTC_DATA_DIR");
    rawinfilename.append("/");
    rawinfilename.append(datainfile());
    string seginfilename = getenv("CTC_DATA_DIR");
    seginfilename.append("/");
    seginfilename.append(seginfile());

    /* read data */
    typedef itk::ImageFileReader< ctc::CTCImageType > RawDataReaderType;
    RawDataReaderType::Pointer rawreader = RawDataReaderType::New();
    rawreader->SetFileName(rawinfilename);

    /* read segmented data */
    typedef itk::ImageFileReader< ctc::BinaryImageType > SegDataReaderType;
    SegDataReaderType::Pointer segreader = SegDataReaderType::New();
    segreader->SetFileName(seginfilename);

    /* invoke readers */
    try
    {
        rawreader->Update();
        segreader->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::cout << ex << std::endl;
        return EXIT_FAILURE;
    }

    /* extract the features */
    typedef ctc::PrincipleCurvatureExtraction FilterType;
    typedef ctc::FeatureExtraction FeatureExtractionFilterType; 

    FilterType::Pointer filter = FilterType::New();
    filter->SetBinaryInput(segreader->GetOutput());
    filter->SetImageInput(rawreader->GetOutput());
    std::clog << "Computing Features ...";

    try
    {
        filter->Compute();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::cout << ex << std::endl;
        return EXIT_FAILURE;
    }
    std::clog << " Done." << std::endl;
    
    /* output */
    std::ofstream out(outfilename());
      
    FeatureExtractionFilterType::Pointer FEfilter = FeatureExtractionFilterType::New();
    FEfilter->SetInitialFeatureVector(filter->GetOutput());

    /* Core of feature extraction */
    FEfilter->Analyze();

    /* Retrieve the feature vectors for those polyp candidates */
    FeatureExtractionFilterType::RegionCollectorType PolypRegionCollector = FEfilter->GetRegionCollector();

    /* Generate a MAT4 file according to feature vectors */
    MAT4FeatureVector(PolypRegionCollector);    

    out.close();
        
}
