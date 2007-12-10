/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __CTCVOXELSET_H__
#define __CTCVOXELSET_H__

#include <sstream>
#include <map>

#include "ctcVoxel.h"
#include "ctcCTCImage.h"

namespace ctc {

    /**
     * Holds a set of CTCVoxel objects.
     *
     * TODO:
     * <li/>Embed std::map<> rather than derive from std::map<>.
     */
    class CTCVoxelSet : public map<string, CTCVoxel>
    {
    public:
        /**
         * Insert a CTCVoxel object.
         * @param voxel  CTCVoxel object.
         * @return pointer to the inserted CTCVoxel object. NULL if failed.
         */
        CTCVoxel * insert(const CTCVoxel & voxel)
        {
            ostringstream  oss_key;
            oss_key << voxel.idx[0] << "." 
                    << voxel.idx[1] << "."
                    << voxel.idx[2];
            pair<super::iterator, bool>  p =
                    super::insert(make_pair(oss_key.str(), voxel));

            return p.second? &(p.first->second) : NULL;
        }

        /** Find the voxel object through its index. */
        CTCVoxel * find(BinaryImageType::IndexType idx)
        {
            ostringstream  oss_key;
            oss_key << idx[0] << "." << idx[1] << "." << idx[2];
            CTCVoxelSet::iterator  it = super::find(oss_key.str());
            if (it != super::end())
               return &(it->second);
            else
               return NULL; 
        }

        /** Find a voxel object through its index. */
        CTCVoxel * find(unsigned int x, unsigned int y, unsigned int z)
        {
            ostringstream  oss_key;
            oss_key << x << "." << y << "." << z;
            
            CTCVoxelSet::iterator  it = super::find(oss_key.str());
            if (it != super::end())
               return &(it->second);
            else
               return NULL; 
        }
        
    private:
        typedef map<string, CTCVoxel> super;
    };

}

#endif

