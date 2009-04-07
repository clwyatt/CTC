/*
 * VCPredict.hpp
 *
 *  Created on: Oct 11, 2008
 *      Author: haxu
 */

#ifndef VCPREDICT_HPP_
#define VCPREDICT_HPP_

#include <vector>
#include <string>
#include <sstream>
#include <functional>

using namespace std;

class VCPredict {
public:
  VCPredict() : _patId(""),  _studyId(""),_polypId(""), _target(-1) { }
  virtual ~VCPredict() { }
  VCPredict(string locId_,
            string polypId_,
            int target_,
            double prob_0_,
            double prob_1_) {
    string::size_type pos1 = locId_.find('/');
    string::size_type pos2 = locId_.rfind('/');

    if (pos1 < 0 || pos2 < 0 || pos1 >= pos2)
      throw "VC Exception: invalid location id.";
    _patId = locId_.substr(0, pos1);
    _studyId = locId_.substr(pos1 + 1, pos2 - pos1 - 1);

    _polypId = polypId_;
    _target = target_;
    _probs.push_back(prob_0_);
    _probs.push_back(prob_1_);
  }

  string getPatientId() const {
    return _patId;
  }

  string getStudyId() const {
    return _studyId;
  }

  string getPolypId() const {
    return _polypId;
  }

  /**
   * Get true class (gold standard)
   */
  int getTarget() const {
    return _target;
  }

  /**
   * Get prediction probability.
   */
  vector<double> getPredProb() const {
    return _probs;
  }

  /**
   * Get prediction.
   *
   * @return -1: error, otherwise, return the predict class index (0 based).
   */
  int getPredictClassIndex() const {
    if (_probs.size() != 2)
      return -1;
    if (_probs.at(0) > _probs.at(1))
      return 0;
    else
      return 1;
  }

protected:
  string _patId;
  string _studyId;
  string _polypId;
  int _target;
  vector<double> _probs;
};

class VCPredict_op_less : public binary_function<VCPredict, VCPredict, bool> {
public:
  bool operator()(const VCPredict & a, const VCPredict & b) {
    if (a.getPatientId() < b.getPatientId())
      return true;
    else if (a.getPatientId() > b.getPatientId())
      return false;
    else {
      istringstream issaSID(a.getStudyId()), issbSID(b.getStudyId());
      int iaSID, ibSID;

      if (issaSID >> iaSID && issbSID >> ibSID) {
        if (iaSID < ibSID)
          return true;
        else if (iaSID > ibSID)
          return false;
        else {
          istringstream issaPID(a.getPolypId()), issbPID(b.getPolypId());
          int iaPID, ibPID;

          if (issaPID >> iaPID && issbPID >> ibPID)
            return iaPID < ibPID;
          else
            return a.getPolypId() < b.getPolypId();
        }
      }
      else {
        if (a.getStudyId() < b.getStudyId())
          return true;
        else if (a.getStudyId() > b.getStudyId())
          return false;
        else {
          istringstream issaPID(a.getPolypId()), issbPID(b.getPolypId());
          int iaPID, ibPID;

          if (issaPID >> iaPID && issbPID >> ibPID)
            return iaPID < ibPID;
          else
            return a.getPolypId() < b.getPolypId();
        }
      }
    }
  }
};

#endif /* VCPREDICT_HPP_ */
