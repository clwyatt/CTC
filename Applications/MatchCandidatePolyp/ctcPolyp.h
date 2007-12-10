/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __CTCPOLYP_H__
#define __CTCPOLYP_H__

#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <climits>

#include "ctcVoxel.h"
#include "ctcCTCImage.h"

using namespace std;

namespace ctc {

    class CTCVoxel;

    /** Class holds candidate polyp. */
    class CTCPolyp
    {
    public:
        string  pid;
        BinaryImageType::PointType  coord;
        float  mean_SI, max_SI, min_SI, var_SI, skew_SI, kurt_SI, contrast_SI;
        float  mean_CV, max_CV, min_CV, var_CV, skew_CV, kurt_CV, contrast_CV;
        BinaryImageType::IndexType  min_idx;
        BinaryImageType::IndexType  max_idx;
        bool  tp_fp;
        vector<CTCVoxel *>  vec_voxel;
        
        /** Default constructor. */
        CTCPolyp() { _init_min_max_idx(); }

        /** Constructor from a vector of string. */
        CTCPolyp(const vector<string> & vec_str)
        {
            int idx = 0;
            istringstream iss_str2num;

            vector<string>::const_iterator  iter_str;
            for (iter_str = vec_str.begin(); 
                 iter_str != vec_str.end();
                 ++iter_str, ++idx)
            {
                switch (idx)
                {
                case 0:
                    this->pid = *iter_str;
                    break;
                case 1:
                    // TODO: 
                    //   test case: convert a char to coord
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->coord[0]))
                        throw exception();
                    break;
                case 2:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->coord[1]))
                        throw exception();
                    break;
                case 3:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->coord[2]))
                        throw exception();
                    break;
                case 4:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->mean_SI))
                        throw exception();
                    break;
                case 5:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->max_SI))
                        throw exception();
                    break;
                case 6:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->min_SI))
                        throw exception();
                    break;
                case 7:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->var_SI))
                        throw exception();
                    break;
                case 8:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->skew_SI))
                        throw exception();
                    break;
                case 9:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->kurt_SI))
                        throw exception();
                    break;
                case 10:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->contrast_SI))
                        throw exception();
                    break;
                case 11:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->mean_CV))
                        throw exception();
                    break;
                case 12:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->max_CV))
                        throw exception();
                    break;
                case 13:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->min_CV))
                        throw exception();
                    break;
                case 14:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->var_CV))
                        throw exception();
                    break;
                case 15:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->skew_CV))
                        throw exception();
                    break;
                case 16:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->kurt_CV))
                        throw exception();
                    break;
                case 17:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->contrast_CV))
                        throw exception();
                    break;
                default:
                    throw exception();
                    break;
                }
            }

            _init_min_max_idx();
        }

    protected:
        void _init_min_max_idx() 
        {
            //the indexvaluetype in itk is long. 
            min_idx[0] = min_idx[1] = min_idx[2] = LONG_MAX;
            max_idx[0] = max_idx[1] = max_idx[2] = LONG_MIN;
        }

    };

}

#endif

