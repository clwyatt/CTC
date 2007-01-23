/********************************************************
File: Colon.hh
Abstract:  A class represents colon data which includes surface 
           mesh, volume data and lesion information.
Created: 04/17/2002
Author: Hong Li

Last Revision  
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:28 $
*********************************************************/

#ifndef _Colon_hh
#define _Colon_hh 1
class Colon;

#include <slist>

#include "Data.hh"
#include "mvLesion.hh"
#include "Mesh_Features.hh"
#include "Volume_ext.hh"

/**
* A class  represents colon data 
*  A class  represents colon data which includes surface 
*  mesh, volume data and lesion information.
*/	   
class Colon : public IMTK::Data
{
private:
	///Volume data of the colon
	Volume_ext<unsigned short> * pvolume;
	///flag for grouping process
	int LesionGrouped;
	///flag for filtering process
	int LesionFiltered;
public:
	///mesh surface data of the colon
	Mesh_Features * plumen;
	
	///List of detected lesions 
	std::slist< mvLesion *> lesionList;

	///a constructor
	Colon()
	{
		LesionGrouped = 0;
		LesionFiltered = 0;
	}
	
	///a constructor with mesh data and volume data
	Colon (Mesh_Features *m, Volume_ext<unsigned short> *v)
	{
		plumen = m;
		pvolume = v;
		LesionGrouped = 0;
		LesionFiltered = 0;	
	}
		
	///a constructor with mesh data and volume data
	Colon (Colon & c) {
	//copy not implemented	
	
	}
	
	///Get the volume data
	Volume_ext<unsigned short> * Get_Volume(){
		return pvolume;
		}
		
	///Get the mesh data		
	Mesh_Features *Get_Lumen(){
		return plumen;
		}
	/**
	*Group the lesion
	*first stage of polyp detection
	*/	
	int Group_Lesion(const int16_t min_ithickness, const int32_t min_size, const int32_t min_area,
                               const int32_t use_area, const float min_con);
	
	/**
	*Filter the lesion
	*second stage of polyp detection. Remove false positive detetions
	*/	
	int Filter_Lesion();
};
	
#endif	
			
