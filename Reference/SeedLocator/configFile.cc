/********************************************************
File: configFile.cc

see configFile.hh for documentation
*********************************************************/

#include "configFile.hh"

#define CONFIG_DELIMITER (char)('=')
#define CONFIG_CONTINUATION (char)('\\')
#define CONFIG_COMMENT (char)('#')

using namespace std;

void read_parameter(ifstream &ifs, string parameter, list<string> &value, int &error){

        string line, tempP, rhs;
        string::iterator si;
        bool delimiterFound = false;
        bool valueEmpty = false;
        bool parameterFound = false;
        bool continueFound = false;
        
        error = 0;

	// start at begining of file
	ifs.seekg(0);
			     
	// check if stream is valid
	if(!ifs.good()){
		error = 1;
		return;
	}
                
        while( (getline(ifs, line)) && (!parameterFound) ){
                //cerr << "Line: " << line << endl;
                
                // ignore if line is empty
                if( line.empty() ) continue;
                
                // search for delimeter, storing possible parameter
                // and ignoring blanks
                si = line.begin();
                
                // skip this line if it is a comment
                if( *si == CONFIG_COMMENT ) continue;
                
                delimiterFound = false;
                tempP.erase(tempP.begin(), tempP.end());
                while( (si != line.end()) && (!delimiterFound) ){
                        if( *si != CONFIG_DELIMITER ){
                                if(*si != (char)(' ')) tempP += *si;
                        }
                        else{
                                delimiterFound = true;
                        }
                        //cerr << "Examining LHS " << *si << endl;
                        si++;
                }
                // check delimiter was found
                if(!delimiterFound){
                        if( *si == CONFIG_DELIMITER )
                                valueEmpty = true;
                        else{
                                //cerr << "No delimeter was found!" << endl;
                                error = 2;
                                break;
                        }
                }
                
                // check possible parameter string non-empty
                if( tempP.empty() ){
                        //cerr << "Parameter String empty!" << endl;
                        error = 2;
                        break;
                }
                // compare tempP to parameter
                //cerr << "Left Hand Side is " << tempP << endl;
                parameterFound = tempP.compare(parameter) ? false : true;
                
                // if parameter found get right hand side
                // else continue through file
                if(!valueEmpty){
                        // continue with current line
                        continueFound = true;
                        while(continueFound){
                                // now only continue if CONFIG_CONTINUATION present
                                continueFound = false;
                                rhs.erase(rhs.begin(), rhs.end());
                                while( si != line.end() ){
                                        if( *si != CONFIG_CONTINUATION ){
                                                if(*si != (char)(' ')) rhs += *si;
                                        }
                                        else{
                                                continueFound = true;
                                                break;
                                        }
                                        //cerr << "Examining RHS " << *si << endl;
                                        si++;
                                }
                                if(parameterFound) value.push_back(rhs);
                                if(continueFound){
                                        //start over at next line
                                        if(!getline(ifs, line)){
                                                error = 2;
                                                break;
                                        }
                                
                                        si = line.begin();
                                        // skip this line if it is a comment
                                        while( *si == CONFIG_COMMENT ){
                                                if(!getline(ifs, line)){
                                                        error = 2;
                                                        break;
                                                }
                                                si = line.begin();
                                        }
                                        if(error) break;
                                }
                        }
                }
                
        }
        
        if(!parameterFound && error == 0)
                error = 3;

};
