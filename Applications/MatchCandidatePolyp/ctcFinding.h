/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __CTCFINDING_H__
#define __CTCFINDING_H__

#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <climits>

#include "ctcCTCImage.h"

using namespace std;

namespace ctc {

	/** data structure holds polyp finding in OC. */
	class CTCFinding
	{
	public:
		string dir; 
        BinaryImageType::PointType  coord;
		string loc;
		float size;
		string reading;
		string path;
		string grouped;
		bool tp_fp;

        /** Constructor from a vector of string. */
        CTCFinding(const vector<string> & vec_str)
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
                    this->dir = *iter_str;
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
                    this->loc = *iter_str;
                    break;
                case 5:
                	iss_str2num.clear();
                    iss_str2num.str(*iter_str);
                    if (!(iss_str2num >> this->size))
                        throw exception();
                    break;
                case 6:
                    this->reading = *iter_str;
                    break;
                case 7:
                    this->path = *iter_str;
                    break;
                case 8:
                    this->grouped = *iter_str;
                    break;
                case 9:
                {
                    string s(iter_str->substr(0, 1));
                    
                    // "T" or "t" == 1, "F" or "f" == 0
                    if (s.compare("T") == 0 || s.compare("t") == 0)
                        this->tp_fp = true;
                    else if (s.compare("F") == 0 || s.compare("f") == 0)
                        this->tp_fp = false;
                    else
                        throw exception();

                    break;
                }
                default:
                    throw exception();
                    break;
                }
            }
        }
	};
}

#endif

