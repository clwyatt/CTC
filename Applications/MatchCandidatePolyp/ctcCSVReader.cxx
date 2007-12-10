/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include "ctcCSVReader.h"

namespace ctc {

vector< vector< string > > & 
CTCCSVReader::parse(const char *fname, 
					unsigned int idx_line_from)
              throw (exception)
{
	ifstream ifs(fname);
	istringstream iss;
	char str_buff[_buff_length + 2];
	int idx_line = 0;
	
    // reset the number of columns for a new CSV file
    _num_columns = _INIT_NUM_COLUMNS;

	while (ifs)
	{
		ifs.getline(str_buff, _buff_length);
		if (++idx_line < idx_line_from)
			continue;
		
		if (ifs.eof())
		{
			ifs.clear();
			break;
		}

		iss.clear();
		iss.str(str_buff);
        if (iss.str().length() > _buff_length)
        {
            //TODO: log
            throw exception();
        }

		if (iss.str().find_first_not_of(" \r") == string::npos)  //'\r':DOS file
        {
            continue;  // omit blank line
        }
		else
        {
            try {
                _vec_vec_str.push_back(this->parse_line(iss.str().c_str()));
            }
            catch (exception & e)
            {
                // TODO: log
                throw;
            }
        }
	}

	if (ifs.fail())
	{
        // TODO: log
        throw exception();
	}
	
	return _vec_vec_str;
}


vector< string > CTCCSVReader::parse_line(const char * line) throw (exception)
{
	vector< string > vec_str;  // return value
	istringstream iss(line);
	char str_buff[_buff_length + 1];
	int idx_column = 0;

    if (iss.str().length() > _buff_length)
        // TODO: log
        throw exception();

	while (!iss.fail() && !iss.eof())
	{
		iss.get(str_buff, _buff_length, _DELIM);

		// STL istream set failbit if read count equals 0. In a CSV file, a 
		// NULL field is not a failure(e.g., "0,1,,3"). So, we clear failbit 
		// if read count is 0, otherwise, it's a real failure
		if (iss.fail())
		{
			if (iss.gcount() == 0)
				iss.clear(iss.rdstate() & ~ios_base::failbit);
			else
				break;  // exit while loop
		}
		
		vec_str.push_back(str_buff);
		
		if (!iss.eof())
			iss.ignore(1, _DELIM);  // ignore the delim
	}

	// different lines should have the same number of columns
	if (_num_columns == _INIT_NUM_COLUMNS)
        _num_columns = idx_column;
    else if (idx_column != _num_columns)
        throw exception();

	if (iss.fail())
        // TODO: log
		throw exception();
	
	return vec_str; 
}

}

