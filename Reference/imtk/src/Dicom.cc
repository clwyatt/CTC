/********************************************************
File: Dicom.cc
Abstract: Implementation for Dicom

See header (Dicom.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2002/07/31 20:19:53 $
*********************************************************/

#include "Dicom.hh"

#include <fstream>
#include <Subscript.hh>

IMTK::Dicom::Dicom()
{
	dcmObj = NULL;
}

IMTK::Dicom::Dicom(const Dicom & D)
{
	// should never do this????
}

IMTK::Dicom::~Dicom()
{
	close();
}

bool IMTK::Dicom::open(const std::string &theFile)
{

	// if dcmObj exisits, close it first
	if(dcmObj != NULL) close();

	DcmFileFormat *dcmFile = new DcmFileFormat();
	dcmObj = dcmFile;
	OFCondition cond = dcmFile->loadFile(theFile.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength, OFFalse);

	if (! cond.good()){
	  close();
	  return false;
	}

	dcmFile->loadAllDataIntoMemory();

	// everything is OK if we got here
	return true;
}

void IMTK::Dicom::close()
{
	if(dcmObj != NULL)
		delete dcmObj;

	dcmObj = NULL; //reset so it can be reused
}

bool IMTK::Dicom::getOrigin(float & xo, float &yo, float &zo)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0020, 0x0032);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		unsigned long n = stack.card();
		
		DcmDecimalString *dobj = static_cast<DcmDecimalString *> ( stack.top() );

		if( dobj->getVM() != 3){
			cerr << "3 coords expected for origin" << endl;
			cerr << "n = " << n << " , vm = " << dobj->getVM() << endl;
			return false;
		}

		double dxo, dyo, dzo;
		dobj->getFloat64(dxo, 0);
		dobj->getFloat64(dyo, 1);
		dobj->getFloat64(dzo, 2);

		//clog << dxo <<  " " << dyo <<  " " << dzo << endl;  
		
		xo = (float) dxo;
		yo = (float) dyo;
		zo = (float) dzo;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;

}

bool IMTK::Dicom::getOrientation(IMTK::Vector<float> &rowv, IMTK::Vector<float> &colv)
{
	assert(dcmObj != NULL);

	// initialize vectors
	IMTK::Subscript<1> bounds;
	bounds[0] = 3;
	rowv.setBounds(bounds);
	colv.setBounds(bounds);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0020, 0x0037);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		unsigned long n = stack.card();
		
		DcmDecimalString *dobj = static_cast<DcmDecimalString *> ( stack.top() );

		if( dobj->getVM() != 6){
			cerr << "6 direction cosines expected for orientation" << endl;
			cerr << "n = " << n << " , vm = " << dobj->getVM() << endl;
			return false;
		}

		double dxo, dyo, dzo;
		dobj->getFloat64(dxo, 0);
		dobj->getFloat64(dyo, 1);
		dobj->getFloat64(dzo, 2);
		rowv[0] = dxo;
		rowv[1] = dyo;
		rowv[2] = dzo;
		//clog << dxo <<  " " << dyo <<  " " << dzo << endl;
		dobj->getFloat64(dxo, 3);
		dobj->getFloat64(dyo, 4);
		dobj->getFloat64(dzo, 5);
		colv[0] = dxo;
		colv[1] = dyo;
		colv[2] = dzo;
		//clog << dxo <<  " " << dyo <<  " " << dzo << endl;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;

}


bool IMTK::Dicom::getDims(unsigned short &xdim, unsigned short &ydim)
{
	assert(dcmObj != NULL);

	// rows
	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0028, 0x0010);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmUnsignedShort *dobj = static_cast<DcmUnsignedShort *> ( stack.top() );

		if( dobj->getVM() != 1){
			cerr << "1 value expected for rows" << endl;
			return false;
		}

		dobj->getUint16(xdim, 0);

		//clog << "xdim = " << xdim << endl;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// cols
	searchKey.set(0x0028, 0x0011);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmUnsignedShort *dobj = static_cast<DcmUnsignedShort *> ( stack.top() );

		if( dobj->getVM() != 1){
			cerr << "1 value expected for cols" << endl;
			return false;
		}

		dobj->getUint16(ydim, 0);

		//clog << "ydim = " << ydim << endl;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;
}

bool IMTK::Dicom::getPxlSize(float &dx, float &dy)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0028, 0x0030);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		unsigned long n = stack.card();

		double tempdx, tempdy;

		DcmDecimalString *dobj = static_cast<DcmDecimalString *> ( stack.top() );

		if( (dobj->getVM() != 2) ){
			cerr << "2 values expected for pixel spacing" << endl;
			cerr << "n = " << n << " , vm = " << dobj->getVM() << endl;
			return false;
		}

		
		dobj->getFloat64(tempdx, 0);		
		dobj->getFloat64(tempdy, 1);

		dx = (float) tempdx;
		dy = (float) tempdy;

		//clog << "dx = " << dx << endl;
		//clog << "dy = " << dy << endl;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;

}

bool IMTK::Dicom::getData(unsigned short *data, unsigned int length)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x7fe0, 0x0010);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmOtherByteOtherWord *dobj = static_cast<DcmOtherByteOtherWord *> ( stack.top() );

		unsigned short * tempd;
		dobj->getUint16Array(tempd);

		//clog << "Length = " << dobj->getLength() << endl;

		if( 2*length != dobj->getLength() ){
			cerr << "PixelData length does not match allocated space." << endl;
			return false;
		}
		else{
			swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, data, dobj->getLength(), sizeof(Uint16));
			for (unsigned int i = 0; i < length; i++)
				data[i] = tempd[i];
		}
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;
}


bool IMTK::Dicom::getPatientName(std::list< std::string > &name)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0010, 0x0010);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmPersonName *dobj = static_cast<DcmPersonName *> ( stack.top() );

		for(int i = 0; i < dobj->getVM(); i++){
			OFString str;
			dobj->getOFString(str, i);
			name.push_back( std::string( str.data() ) );
		}
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// split name at ^ into different strings in list
	// i.e. Last^First^Middle becomes 3 strings
	
	
	// if we got here, everything is OK
	return true;
}

bool IMTK::Dicom::getPatientID(std::string &id)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0010, 0x0020);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmLongString *dobj = static_cast<DcmLongString *> ( stack.top() );

		OFString str;
		dobj->getOFString(str, 0);
		id = str.data();
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}
	
	
	// if we got here, everything is OK
	return true;
};

bool IMTK::Dicom::getStudyID(std::string &id)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0020, 0x0010);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmShortString *dobj = static_cast<DcmShortString *> ( stack.top() );

		OFString str;
		dobj->getOFString(str, 0);
		id = str.data();
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}
	
	
	// if we got here, everything is OK
	return true;
};


bool IMTK::Dicom::getSeriesNumber(long &num)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0020, 0x0011);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmIntegerString *dobj = static_cast<DcmIntegerString *> ( stack.top() );

		dobj->getSint32(num, 0);
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}
	
	
	// if we got here, everything is OK
	return true;

};

bool IMTK::Dicom::getInstanceNumber(long &num)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0020, 0x0013);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmIntegerString *dobj = static_cast<DcmIntegerString *> ( stack.top() );

		dobj->getSint32(num, 0);
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}
	
	
	// if we got here, everything is OK
	return true;

};


bool IMTK::Dicom::getMRI_TE(double &te)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0018, 0x0081);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		unsigned long n = stack.card();

		DcmDecimalString *dobj = static_cast<DcmDecimalString *> ( stack.top() );

		if( dobj->getVM() != 1){
			cerr << "1 values expected for echo time" << endl;
			cerr << "n = " << n << " , vm = " << dobj->getVM() << endl;
			return false;
		}

		dobj->getFloat64(te, 0);

		//clog << "te = " << te << endl;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;

};


bool IMTK::Dicom::getMRI_TR(double &tr)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0018, 0x0080);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		unsigned long n = stack.card();

		DcmDecimalString *dobj = static_cast<DcmDecimalString *> ( stack.top() );

		if( dobj->getVM() != 1){
			cerr << "1 values expected for echo time" << endl;
			cerr << "n = " << n << " , vm = " << dobj->getVM() << endl;
			return false;
		}

		dobj->getFloat64(tr, 0);

		//clog << "ti = " << ti << endl;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;

};

bool IMTK::Dicom::getMRI_TI(double &ti)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0018, 0x0082);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		unsigned long n = stack.card();

		DcmDecimalString *dobj = static_cast<DcmDecimalString *> ( stack.top() );

		if( dobj->getVM() != 1){
			cerr << "1 values expected for echo time" << endl;
			cerr << "n = " << n << " , vm = " << dobj->getVM() << endl;
			return false;
		}

		dobj->getFloat64(ti, 0);

		//clog << "ti = " << ti << endl;

	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}

	// if we got here, everything is OK
	return true;

};

bool  IMTK::Dicom::getMRI_EchoNumber(long &en)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0018, 0x0086);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmIntegerString *dobj = static_cast<DcmIntegerString *> ( stack.top() );

		dobj->getSint32(en, 0);
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}
	
	
	// if we got here, everything is OK
	return true;
};

bool IMTK::Dicom::getContrast(std::string &contrast)
{
	assert(dcmObj != NULL);

	DcmStack stack;
	DcmTagKey searchKey;
	searchKey.set(0x0018, 0x0010);
	if(dcmObj->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal){

		DcmLongString *dobj = static_cast<DcmLongString *> ( stack.top() );

		OFString str;
		dobj->getOFString(str, 0);
		contrast = str.data();
	}
	else{
	  //cerr << "Group, Element not found" << endl;
		return false;
	}
	
	
	// if we got here, everything is OK
	return true;


};
