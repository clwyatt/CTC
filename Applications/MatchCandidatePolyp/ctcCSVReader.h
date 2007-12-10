/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception>

using namespace std;

namespace ctc
{
	class CTCCSVReader
	{
	public:
		/**
		 * Default constructor.
		 * @param max_line_length
		 * 
		 * If the length of a line is larger than <code>max_line_length</code>, 
		 * the parse operation will throw exception.
		 */
		CTCCSVReader(unsigned int max_line_length = 1024)
        {
			_num_columns = _INIT_NUM_COLUMNS;
			_buff_length = max_line_length; 
		}
		
		/** Destructor. */
		~CTCCSVReader() { }

		/**
		 * Parse a CSV file.
		 * @param fname  file name.
		 * @param idx_line_from  Parse from the n-th line (1 based)
		 */ 
		vector< vector< string > > & parse(const char * fname,
										   unsigned int idx_line_from = 1)
                                          throw (exception);

		/** 
		 * Parse one line in a CSV file.
		 * @param line  A line in a CSV file. (NO '\n' at the end)
		 */
		vector<string>  parse_line(const char* line) throw (exception);
		
	protected:
		int _num_columns;  /// number of columns in the CSV file
		unsigned int _buff_length;	/// max length of lines in the CSV file
		static const int _INIT_NUM_COLUMNS = -1;
		static const char _DELIM = ',';
		
		/**
		 * Hold the content of the CSV file. Each line corresponds to a vector 
		 * of string in this data structure.
		 */
		vector< vector< string > >  _vec_vec_str;
    };

}
