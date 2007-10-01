/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __MAT4Converter_h
#define __MAT4Converter_h

#include "ctcFeatureExtraction.h"

using namespace ctc;

typedef std::vector<AssociatedFeatureList>         GrowableRegionType;
typedef std::vector<GrowableRegionType>            RegionCollectorType;

void MAT4FeatureVector(RegionCollectorType a);

#endif
