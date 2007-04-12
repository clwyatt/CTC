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
  std::string mrn;
  std::string study;
  std::string series;
  double x, y, z;
  double size;
  char true_false;
};
typedef std::list<EntryType> DBType;

void ExtractHistogramFeature(std::ofstream & out, const EntryType & e);

void ExtractOrientedVolumeFeature(std::ofstream & out, const EntryType & e);

void SingleScaleEigenstructureFeature(std::ofstream & out, const EntryType & e);

#endif
