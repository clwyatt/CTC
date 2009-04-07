/*
 * CSVReader.hpp
 *
 *  Created on: Oct 8, 2008
 *      Author: haxu
 */

#ifndef CSVREADER_HPP_
#define CSVREADER_HPP_

#include <string>
#include <vector>
#include "boost/any.hpp"
#include "boost/shared_ptr.hpp"

using namespace std;
using namespace boost;

class CSVReader {
public:
  /**
   * @param fname Filename
   * @param fmt Specify column format in CSV file (comma delimited, e.g.,
   *             "s,s,d,f,s")
   *              s -- string
   *              f -- decimal floating point
   *              d -- signed decimal integer
   */
  CSVReader(string fname_, string fmt_);
  virtual ~CSVReader();

  int parseFile();

  unsigned int getNumRow() const {
    return _rowNum;
  }

  unsigned int getNumColumn() const {
    return _colNum;
  }

  bool isParsed() const {
    return _parseSuccess;
  }

  /**
   * Get column vector.
   *
   * @param idxCol_ Index of column. (0 based).
   *
   * @return A const reference to column vector<string>.
   */
  const vector<string> & getStringColumn(unsigned int idxCol_) {
    return * any_cast< shared_ptr< vector<string> > > (_bag.at(idxCol_));
  }

  /**
   * Get column vector.
   *
   * @param idxCol_ Index of column. (0 based).
   *
   * @return A const reference of column vector<double>.
   */
  const vector<double> & getDoubleColumn(unsigned int idxCol_) {
    return * any_cast< shared_ptr< vector<double> > > (_bag.at(idxCol_));
  }

  /**
   * Get column vector.
   *
   * @param idxCol_ Index of column. (0 based).
   *
   * @return A const reference of column vector<int>.
   */
  const vector<int> & getIntColumn(unsigned int idxCol_) {
    return * any_cast< shared_ptr< vector<int> > > (_bag.at(idxCol_));
  }

protected:
  CSVReader() { };

protected:
  string _fname;
  string _colFormat;
  bool _parseSuccess;
  unsigned int _rowNum;
  unsigned int _colNum;
  vector<any> _bag;

};

#endif /* CSVREADER_HPP_ */
