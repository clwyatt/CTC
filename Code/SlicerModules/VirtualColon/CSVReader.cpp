/*
 * CSVReader.cpp
 *
 *  Created on: Oct 11, 2008
 *      Author: haxu
 */

#include "CSVReader.hpp"

#include <fstream>
#include <iostream>
#include "boost/spirit/core.hpp"
#include "boost/spirit/utility/loops.hpp"
#include "boost/spirit/dynamic/stored_rule.hpp"

using namespace boost::spirit;

// constructor
CSVReader::CSVReader(string fname_, string colFormat_) :
  _fname(fname_), _colFormat(colFormat_), _parseSuccess(false),
  _rowNum(0), _colNum(0) {

}

// destructor
CSVReader::~CSVReader() {
  /*
  for (vector<any>::iterator it = _bag.begin();
       it != _bag.end();
       it++) {
    if (any_cast<vector<string> * >(&*it)) {
      delete *(any_cast<vector<string> * >(&*it));
      cout << "delete vector<string> *" << endl;
    }
    else if (any_cast<vector<double> * > (&*it)) {
      delete *(any_cast<vector<double> * >(&*it));
      cout << "delete vector<double> *" << endl;
    }
    else if (any_cast<vector<int> * >(&*it)) {
      delete *(any_cast<vector<int> * >(&*it));
      cout << "delete vector<int> *" << endl;
    }
  }
  */
}

//
int CSVReader::parseFile() {
  // construct boost::spirit parse rule
  stored_rule<> ruleLine;

  shared_ptr< vector<string> > vecString;
  shared_ptr< vector<int> > vecInt;
  shared_ptr< vector<double> > vecDouble;

  // build rule for the first column
  string::const_iterator strit = _colFormat.begin();
  if (strit != _colFormat.end()) {
    if (*strit == 's') {
      vecString.reset(new vector<string>());
      _bag.push_back(vecString);
      ruleLine = repeat_p(1, more)[~ch_p(',')][push_back_a(*vecString)];
    }
    else if (*strit == 'f') {
      vecDouble.reset(new vector<double>());
      _bag.push_back(vecDouble);
      ruleLine = real_p[push_back_a(*vecDouble)];
    }
    else if (*strit == 'd') {
      vecInt.reset(new vector<int>());
      _bag.push_back(vecInt);
      ruleLine = int_p[push_back_a(*vecInt)];
    }
    else if (*strit == ',')
      ruleLine = ch_p(',');
    else
      return -1;  // err: invalid format string

    if (*strit != ',') {
      _colNum++;
    }
  }

  // build rule for the rest of line
  while (++strit != _colFormat.end()) {
    if (*strit == 's') {
      vecString.reset(new vector<string>());
      _bag.push_back(vecString);
      ruleLine = ruleLine.copy() >>
                 repeat_p(1, more)[~ch_p(',')][push_back_a(*vecString)];
    }
    else if (*strit == 'f') {
      vecDouble.reset(new vector<double>());
      _bag.push_back(vecDouble);
      ruleLine = ruleLine.copy() >> real_p[push_back_a(*vecDouble)];
    }
    else if (*strit == 'd') {
      vecInt.reset(new vector<int>());
      _bag.push_back(vecInt);
      ruleLine = ruleLine.copy() >> int_p[push_back_a(*vecInt)];
    }
    else if (*strit == ',')
      ruleLine = ruleLine.copy() >> ch_p(',');
    else
      return -1;  // err: invalid format string

    if (*strit != ',') {
      _colNum++;
    }
  }

  // parse file
  ifstream ifs(_fname.c_str());
  if (ifs.fail())
    return -1;  // err: open file failed

  string line;
  while (ifs.good()) {
    getline(ifs, line);
    if (ifs.eof() && line.length() == 0)  // read nothing before hit EOF
      break;

    parse_info<> info = parse(line.c_str(), ruleLine);
    if (!info.full) {
      return -1;  // err: parse error
    }
    else {
      /*
      cout << _rowNum + 1 << ":\t";
      for (unsigned int i = 0; i < _colNum; i++) {
        if (_colFmt.at(i) == 's') {
          vecString = any_cast< shared_ptr< vector<string> > >(_bag.at(i));
          cout << vecString->at(_rowNum) << ",";
        }
        else if (_colFmt.at(i) == 'f') {
          vecDouble = any_cast< shared_ptr< vector<double> > >(_bag.at(i));
          cout << vecDouble->at(_rowNum) << ",";
        }
        else if (_colFmt.at(i) == 'd') {
          vecInt = any_cast< shared_ptr < vector<int> > >(_bag.at(i));
          cout << vecInt->at(_rowNum) << ",";
        }
        else
          return -1;  // err: unknown col format
      }
      cout << endl;
      */
      _rowNum++;
    }
  }

  _parseSuccess = true;
  return 0;
}
