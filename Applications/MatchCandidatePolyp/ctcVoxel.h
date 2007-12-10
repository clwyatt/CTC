/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __CTCVOXEL_H__
#define __CTCVOXEL_H__

#include <string>
#include <vector>
#include <sstream>
#include <exception>

#include "ctcPolyp.h"
#include "ctcCTCImage.h"

using namespace std;

namespace ctc {
    class CTCPolyp;

    /** Data structure holds voxel in candidate polyp.  */
    class CTCVoxel
    {
    public:
        string  pid;
        BinaryImageType::PointType  coord;
        BinaryImageType::IndexType  idx;
        float SI;
        float CV;
        bool  tp_fp;
        CTCPolyp *  polyp;

        /** Constructor from a vector of string.  */
        CTCVoxel(const vector<string> & vec_str)
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
                    if (!(iss_str2num >> this->idx[0]))
                        throw exception();
                    break;
                case 5:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->idx[1]))
                        throw exception();
                    break;
                case 6:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->idx[2]))
                        throw exception();
                    break;
                case 7:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->SI))
                        throw exception();
                    break;
                case 8:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->CV))
                        throw exception();
                    break;
                default:
                    throw exception();
                    break;
                }
            }
        }
    };

}

#endif

