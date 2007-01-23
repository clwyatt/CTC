/********************************************************
File: configFile.hh
Abstract: I/O functions for generalized configuration file
Created: 11/21/2000 
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

#include <iostream>
#include <fstream>
#include <list>
#include <string>

/*****************************************************************
Abstract: Function to read a generalized configuration
          file.
         
NOTES:
Format is param = value with possibility of lists using
continuation \ .

Spaces in the parameters are ignored.

Blank lines are ignored unless they are in a list, in which case
a "Malformed Parameter Line" error is generated. However comment
lines may be included and are ignored as always.

ERROR CODES:
error = 0 indicates success
error = 1 indicates file io error
error = 2 indicates malformed parameter line
error = 3 indicates no such parameter in file

*****************************************************************/
void read_parameter(std::ifstream &ifs, std::string parameter, std::list<std::string> &value, int &error);
