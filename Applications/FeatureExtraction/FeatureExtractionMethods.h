/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __FeatureExtractionMethods_h
#define __FeatureExtractionMethods_h

#include <fstream>
#include <list>
#include <string>

struct EntryType
{
  int id;
  double x, y, z;
};
typedef std::list<EntryType> DBType;

#endif
