#ifndef __QR_EVIDENCE_SET_H__
#define __QR_EVIDENCE_SET_H__

#include "config.h"
#include <set>
#include <map>
#include <algorithm> /* for std::set_intersection */
#include <iterator> /* for std::inserter */

namespace qr {
// -------------------------------------------------------------------------- //
// EvidenceSet
// -------------------------------------------------------------------------- //
  template<class T>
  class EvidenceSet {
    typedef std::map<std::set<T>, double> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::value_type value_type;
  public:
    EvidenceSet() {
      mEvidence[std::set<T>()] = 0;
    }

    void addEvidence(const std::set<T>& set, double evidence) {
      assert(mEvidence.find(set) == mEvidence.end());

      mEvidence[set] = evidence;
    }

    void combine(EvidenceSet& other) {
      std::map<std::set<T>, double> newEvidence;
      foreach(const value_type& pair, mEvidence)
        newEvidence[pair.first] = 0;
      foreach(const value_type& pair, other.mEvidence)
        newEvidence[pair.first] = 0;
      foreach(const value_type& aPair, mEvidence)
        foreach(const value_type& bPair, other.mEvidence)
          newEvidence[intersect(aPair.first, bPair.first)] = 0;

      double k = 0;
      foreach(const value_type& aPair, mEvidence)
        foreach(const value_type& bPair, other.mEvidence)
          if(intersect(aPair.first, bPair.first).empty())
            k += mEvidence[aPair.first] * other.mEvidence[bPair.first];

      foreach(const value_type& pair, newEvidence) {
        double sum = 0;
        if(!pair.first.empty())
          foreach(const value_type& aPair, mEvidence)
            foreach(const value_type& bPair, other.mEvidence)
              if(intersect(aPair.first, bPair.first) == pair.first)
                sum += mEvidence[aPair.first] * other.mEvidence[bPair.first];
        newEvidence[pair.first] = 1 / (1 - k) * sum;
      }

      mEvidence = newEvidence;
    }

    double evidence(const std::set<T>& set) const {
      const_iterator pos = mEvidence.find(set);
      if(pos == mEvidence.end())
        return 0;
      else
        return pos->second;
    }

  private:
    static std::set<T> intersect(const std::set<T>& a, const std::set<T>& b) {
      std::set<T> result;
      set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.end()));
      return result;
    }

    container_type mEvidence;
  };

} // namespace qr

#endif // __QR_EVIDENCE_SET_H__
