#include <string>
#include <vector>
#include <algorithm>

#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"

#include "ctcCTCImage.h"

using namespace std;
using namespace ctc;

int main(int argc, char ** argv)
{

    string fname_in = "/gpfs0/santagoGrp/data/WFU_prior_CTC_data/CTC-0015/102_081747/mask.matched.vtk";

    typedef itk::ImageFileReader<BinaryImageType>  ReaderType;
    typedef itk::ImageRegionIterator<BinaryImageType>  ImgRegIterType;

    // 
    ReaderType::Pointer  reader = ReaderType::New();
    reader->SetFileName(fname_in);
    vector<BinaryImageType::PixelType>  vec_unique_pixel;

    reader->Update();
    ImgRegIterType it(reader->GetOutput(),
                      reader->GetOutput()->GetLargestPossibleRegion());
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
        if (find(vec_unique_pixel.begin(), vec_unique_pixel.end(), it.Get())
            == vec_unique_pixel.end())
            vec_unique_pixel.push_back(it.Get());
    }

    vector<BinaryImageType::PixelType>::iterator  it_vec;
    for (it_vec = vec_unique_pixel.begin(); it_vec != vec_unique_pixel.end(); ++it_vec)
        cout << (unsigned short)*it_vec << endl;

    return EXIT_SUCCESS;
}
