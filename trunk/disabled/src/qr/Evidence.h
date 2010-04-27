#ifndef __QR_EVIDENCE_H__
#define __QR_EVIDENCE_H__

#include "config.h"
#include <set>

namespace qr {
// -------------------------------------------------------------------------- //
// Evidence
// -------------------------------------------------------------------------- //
  template<class T>
  class Evidence {
  public:
    Evidence(const std::set<T>& set, double belief): mSet(set), mBelief(belief) {}

    const std::set<T>& set() const {
      return mSet;
    }

    double belief() const {
      return mBelief;
    }

    bool operator< (const )

  private:
    std::set<T> mSet;
    double mBelief;
  };

} // namespace qr

#endif // __QR_EVIDENCE_H__
