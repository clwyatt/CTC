/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <climits>
#include <cassert>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAnalyzeImageIO.h"
#include "itkVTKImageIO.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "ctcCTCImage.h"
#include "ctcCSVReader.h"
#include "ctcFinding.h"
#include "ctcPolyp.h"
#include "ctcPolypSet.h"
#include "ctcVoxel.h"
#include "ctcVoxelSet.h"


using namespace std;
using namespace ctc;


namespace ctc {
    /**
     * Matching candidate polyps to true polyps found by OC/CTC. And output 
     * VTK file according to ...(see wiki file format).
     *
     * The corss-relation between polypset and voxelset should be established
     * before calling this function: 
     *      assert(CTCPolyp.vec_voxel.size() != 0)
     *      assert(CTCPolyp.vec_voxel.at(i) != 0)
     *      assert(CTCVoxel.polyp != 0)
     *
     * @param vec_finding  polyps found by OC/CTC.
     * @param polypset  Candidate polyps found in feature extractrion.
     * @param voxelset  Voxels contained in candidate polyps.
     */
    void matching(vector<CTCFinding> vec_finding,
                  CTCPolypSet & polypset,
                  CTCVoxelSet & voxelset,
                  const char* fname_in,
                  const char* fname_out)
    {
        vector<CTCFinding>::const_iterator  it_finding;
        CTCPolypSet::iterator  it_polyp;
        vector<CTCVoxel*>::iterator  it_voxel;

        // set tp_fp candidate polyps and containing voxels.
        for (it_polyp = polypset.begin();
             it_polyp != polypset.end();
             ++it_polyp)
        {
            it_polyp->second.tp_fp = false;

            for (it_finding = vec_finding.begin();
                 it_finding != vec_finding.end();
                 ++it_finding)
            {
                if (it_finding->tp_fp == false)
                    continue;

                float  d2 = 
					pow(it_finding->coord[0] - it_polyp->second.coord[0], 2)
                  + pow(it_finding->coord[1] - it_polyp->second.coord[1], 2)
                  + pow(it_finding->coord[2] - it_polyp->second.coord[2], 2);

                if (sqrt(d2) <= it_finding->size)
                {
                    it_polyp->second.tp_fp = true;
                    //cout << "true polyp: " << it_polyp->second.pid << endl;
                }
            }

            // set tp_fp for voxels
            // TODO:
            // another way to mark tp_fp voxel is to measure the distance from
            // the voxel to the closest TP polyp found by OC/CTC.
            for (it_voxel = it_polyp->second.vec_voxel.begin();
                 it_voxel != it_polyp->second.vec_voxel.end();
                 ++it_voxel)
            {
                (*it_voxel)->tp_fp = it_polyp->second.tp_fp;
            }
        }

        // calculate ROI for each candidate polyp
        BinaryImageType::IndexType  roi_start;
        BinaryImageType::SizeType  roi_size;
        BinaryImageType::RegionType  roi;
        vector<BinaryImageType::RegionType>  vec_roi;

        for (it_polyp = polypset.begin();
             it_polyp != polypset.end();
             ++it_polyp)
        {
            const CTCPolyp & polyp = it_polyp->second;
            roi_start = polyp.min_idx;
            roi_size[0] = polyp.max_idx[0] - polyp.min_idx[0] + 1;
            roi_size[1] = polyp.max_idx[1] - polyp.min_idx[1] + 1;
            roi_size[2] = polyp.max_idx[2] - polyp.min_idx[2] + 1;

            roi.SetIndex(roi_start);
            roi.SetSize(roi_size);
            vec_roi.push_back(roi);
        }

        typedef itk::ImageFileReader< ctc::BinaryImageType >  ReaderType;
        typedef itk::ImageFileWriter< ctc::BinaryImageType >  WriterType;
        typedef itk::RegionOfInterestImageFilter<ctc::BinaryImageType, 
                        ctc::BinaryImageType>  ROIFilterType;
        typedef itk::ImageRegionIteratorWithIndex<BinaryImageType>  
                        ImgRegIterType;

        // 
        ReaderType::Pointer  reader = ReaderType::New();
        WriterType::Pointer  writer = WriterType::New();
        ROIFilterType::Pointer  roiFilter = ROIFilterType::New();
        
        reader->SetFileName(fname_in);
        writer->SetFileName(fname_out);
        reader->Update();
        roiFilter->SetInput(reader->GetOutput());

        try {
            // for each ROI, mask 
            vector<BinaryImageType::RegionType>::iterator  it_roi;
            for (it_roi = vec_roi.begin();
                 it_roi != vec_roi.end();
                 ++it_roi)
            {
                //cout << "it_roi->GetIndex() = " << it_roi->GetIndex() << endl;
                roiFilter->SetRegionOfInterest(*it_roi);
                roiFilter->Update();

				BinaryImageType::IndexType  originIdx = it_roi->GetIndex();
                ImgRegIterType it(roiFilter->GetOutput(), 
								roiFilter->GetOutput()->GetRequestedRegion());
                for (it.GoToBegin(); !it.IsAtEnd(); ++it)
                {
                    BinaryImageType::IndexType  idx = it.GetIndex();
					idx[0] += originIdx[0];
					idx[1] += originIdx[1];
					idx[2] += originIdx[2];
                    CTCVoxel * voxel = voxelset.find(idx);
                    if (voxel)
                    {
                        if (voxel->tp_fp == true)
							reader->GetOutput()->SetPixel(idx, 100);
                        else
							reader->GetOutput()->SetPixel(idx, 200);
                    }
                }
            }
        }
        catch (itk::ExceptionObject& err)
        {
            std::cerr << "ExceptionObject caught!" << std::endl;
            std::cerr << err << std::endl;
            throw exception();
        }

        // write the output
        writer->SetInput(reader->GetOutput());

        try {
            writer->Update();
        }
        catch (itk::ExceptionObject & err)
        {
            std::cerr << "ExceptionObject caught!" << std::endl;
            std::cerr << err << std::endl;
            throw exception();
        }
    }

}


int main(int argc, char ** argv)
{
    string HOME = "/gpfs0/santagoGrp/data/WFU_prior_CTC_data/CTC-0015/102_081747/";
	string fname_in = HOME + "mask.img";
	string fname_out = HOME + "mask.matched.vtk"; 
	string fname_oc_findings = "/gpfs0/santagoGrp/data/WFU_prior_CTC_data/WFU_VC_Results_unknown_fp.csv";
    unsigned int firstline_oc_finding = 2;
	string dir_fname_in;
    string fname_feature_polyps = HOME + "ExtractedPolyps.csv";
    unsigned int firstline_feature_polyps = 1;
    string fname_feature_voxels = HOME + "ExtractedVoxels.csv";
    unsigned int firstline_feature_voxels = 2;

	// fname_in must include absolute path
	if (fname_in.find_last_of('/') != string::npos)
		dir_fname_in = fname_in.substr(0, fname_in.find_last_of('/'));
	else
	{
		// TODO
		//   print usage
		std::cerr << "input filename must include ansolute path" << std::endl;
		return EXIT_FAILURE;
	}
	
	
    vector< vector<string> >  vec_vec;

    // read oc_findings (CSV file)
    vector<CTCFinding>  vec_finding;

    try {
        ctc::CTCCSVReader  csv_reader;
        vec_vec = csv_reader.parse(fname_oc_findings.c_str(),
        						   firstline_oc_finding);

        vector< vector<string> >::const_iterator  it_vec;
        for (it_vec = vec_vec.begin(); it_vec != vec_vec.end(); ++it_vec)
        {

            CTCFinding finding(*it_vec);
            if (finding.dir.compare(dir_fname_in) != 0)
                continue;  // discard OC findings for other volumes

            vec_finding.push_back(finding);
        }
    }
    catch (std::exception & e)
    {
        // TODO: log
        std::cerr << "parse CSV file error." << std::endl;
        return EXIT_FAILURE;
    }

    // read feature polyp file (CSV file)
    CTCPolypSet  polypset;

    try {
        CTCCSVReader  csv_reader;
        vec_vec = csv_reader.parse(fname_feature_polyps.c_str(),
        						   firstline_feature_polyps);

        vector< vector<string> >::const_iterator  it_vec;
        for (it_vec = vec_vec.begin(); it_vec != vec_vec.end(); ++it_vec)
        {
            CTCPolyp  polyp(*it_vec);
            polypset.insert(polyp);
        }
    }
    catch (std::exception & e)
    {
        // TODO: log
        std::cerr << "parse CSV file error." << std::endl;
        return EXIT_FAILURE;
    }

    // read feature voxel file (CSV file)
    CTCVoxelSet  voxelset;

    try {
        CTCCSVReader  csv_reader;
        vec_vec = csv_reader.parse(fname_feature_voxels.c_str(),
        						   firstline_feature_voxels);

        CTCPolyp * p_polyp = NULL;
        vector< vector<string> >::const_iterator  it_vec;
        for (it_vec = vec_vec.begin(); it_vec != vec_vec.end(); ++it_vec)
        {
            CTCVoxel  voxel(*it_vec);
            // TODO:
            // this is to omit duplicated line in voxel feature file because of
            // a bug in the writer. After fix that bug, remove this line.
            if (voxelset.find(voxel.idx))
            {
            	//std::cout << voxel.idx << std::endl;
            	continue;
            }
            
            CTCVoxel * p_voxel = voxelset.insert(voxel);
            if (!p_voxel)
            	throw exception();

            if (!p_polyp || p_polyp->pid != voxel.pid)
                p_polyp = polypset.find(voxel.pid);

            if (p_polyp)
            {
                p_voxel->polyp = p_polyp;
                p_polyp->vec_voxel.push_back(p_voxel);

                // calculate the min/max index for each candidate polyp
                if (p_polyp->min_idx[0] > voxel.idx[0])
                    p_polyp->min_idx[0] = voxel.idx[0];
                if (p_polyp->min_idx[1] > voxel.idx[1])
                    p_polyp->min_idx[1] = voxel.idx[1];
                if (p_polyp->min_idx[2] > voxel.idx[2])
                    p_polyp->min_idx[2] = voxel.idx[2];
                if (p_polyp->max_idx[0] < voxel.idx[0])
                    p_polyp->max_idx[0] = voxel.idx[0];
                if (p_polyp->max_idx[1] < voxel.idx[1])
                    p_polyp->max_idx[1] = voxel.idx[1];
                if (p_polyp->max_idx[2] < voxel.idx[2])
                    p_polyp->max_idx[2] = voxel.idx[2];
            }
            else
            {
                // TODO:
                // voxel havn't correponding polyp to contain it. in-consistent
                // between feature_polyp and feature_voxel files.
                throw exception();
            }
        }
    }
    catch (std::exception & e)
    {
        // TODO: log
        std::cerr << "parse CSV file error." << std::endl;
        return EXIT_FAILURE;
    }

	// assert cross polyp-voxel pointers
	CTCPolypSet::iterator it1;
	for (it1 = polypset.begin(); it1 != polypset.end(); ++it1)
	{
		vector<CTCVoxel *>::iterator  it2;
		for (it2 = it1->second.vec_voxel.begin(); it2 != it1->second.vec_voxel.end(); ++it2)
		{
			CTCVoxel * p_voxel = *it2;
			assert(p_voxel == voxelset.find(p_voxel->idx));
			assert(p_voxel->polyp == &(it1->second));
		}
	}

    matching(vec_finding, polypset, voxelset, fname_in.c_str(), fname_out.c_str());
    
    std::cout << "return" << std::endl;
    
	return EXIT_SUCCESS;
}



