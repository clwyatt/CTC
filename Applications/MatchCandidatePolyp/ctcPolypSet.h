/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __CTCPOLYPSET_H__
#define __CTCPOLYPSET_H__

#include <string>
#include <map>

#include "ctcPolyp.h"

using namespace std;

namespace ctc {

    /**
     * Holds a set of CTCPolyp objects.
     *
     * TODO:
     * <li/>Embed std::map<> rather than derive from std::map<>.
     */
    class CTCPolypSet : public map<string, CTCPolyp>
    {
    public:
        /**
         * Insert CTCPolyp object. 
         * @param polyp  CTCPolyp object.
         * @return pointer to an inserted CTCPolyp object. NULL if failed.
         */
        CTCPolyp * insert(const CTCPolyp & polyp)
        {
            pair<CTCPolypSet::iterator, bool> p = 
                    super::insert(make_pair(polyp.pid, polyp));

           return p.second? &(p.first->second) : NULL;
        }

        /**
         * Find teh CTCPolyp obj through the polyp id.
         * @param pid  polyp id.
         * @return pointer to the CTCPolyp obj. NULL if failed.
         */
        CTCPolyp * find(string pid)
        {
            CTCPolypSet::iterator  it = super::find(pid);
            if (it != this->end())
                return &(it->second);
            else
                return NULL;
        }

    private:
        typedef map<string, CTCPolyp> super;
    };

}

#endif

