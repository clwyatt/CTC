/********************************************************
File: Dicom.hh
Abstract: The IMTK class for reading individual DICOM images. 
Created: 11/16/01
Author: Chris L. Wyatt

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2002/07/31 20:19:53 $
*********************************************************/

//prevent recursive includes
#ifndef _Dicom_hh
#define _Dicom_hh 1

#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>

#include "Vector.hh"

#define HAVE_CONFIG_H 1

#include <osconfig.h>
#include <dctk.h>

#undef HAVE_CONFIG_H 

/** The IMTK class for reading individual DICOM images.

This class encapsulates the reading of individual
DICOM files using the DCMTK software as a base.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.2 $
*/

namespace IMTK{

class Dicom
{
private:
	DcmObject *dcmObj;

protected:

public:
	/// Default Constructor
	Dicom();
	
	/// Copy Constructor
	Dicom(const Dicom & D);
	
	/// Destructor
	~Dicom();
	
	/**
	 * Open the DICOM file.
	 * 
	 *@param theFile the filename to open
	 */
	bool open(const std::string &theFile);

	/**
	 * Close the DICOM file.
	 * 
	 */
	void close();
	
	/**
	 * Get the Patient coordinates of (0,0)
	 * in this slice.
	 *
	 */
	bool getOrigin(float & xo, float &yo, float &zo);

	/**
	 * Get the direction cosines for row and col
	 * of this slice.
	 *
	 */
	bool getOrientation(IMTK::Vector<float> &rowv, IMTK::Vector<float> &colv);

	/**
	 * Get the image dimensions
	 * 
	 */
	bool getDims(unsigned short &xdim, unsigned short &ydim);

	/**
	 * Get the image pixel size
	 * 
	 */
	bool getPxlSize(float &dx, float &dy);

	/**
	 * Get the image data
	 * This must be preallocated space.
	 * 
	 */
	bool getData(unsigned short *data, unsigned int length);
	
	/**
	 * Get the patient name
	 * This is a list of first middle last DR etc name
	 */
	bool getPatientName(std::list< std::string > &name);
	
	/**
	 * Get the patient id
	 */
	bool getPatientID(std::string &id);
	
	/**
	 * Get the study id
	 */
	bool getStudyID(std::string &id);
	
	/**
	 * Get the series number
	 */
	bool getSeriesNumber(long &num);

	/**
	 * Get the instance number
	 */
	bool getInstanceNumber(long &num);

	/**
	 * Get the echo time for MR data
	 */
	bool getMRI_TE(double &te);

	/**
	 * Get the repetition time for MR data
	 */
	bool getMRI_TR(double &tr);

	/**
	 * Get the inversion time for MR data
	 */
	bool getMRI_TI(double &ti);

	/**
	 * Get the echo number
	 */
	bool getMRI_EchoNumber(long &en);

	/**
	 * Get the contrast agent
	 */
	bool getContrast(std::string &contrast);

};
}

#endif
