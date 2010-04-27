#ifndef __QR_CUTTING_CHAIN_H__
#define __QR_CUTTING_CHAIN_H__

#include "config.h"
#include <boost/noncopyable.hpp>
#include <QString>
#include <QList>

namespace qr {
  class Edge;

// -------------------------------------------------------------------------- //
// CuttingChain
// -------------------------------------------------------------------------- //
  class CuttingChain: private boost::noncopyable {
  public:
    CuttingChain(const QString& name): mName(name), mView(NULL) {}

    const QString& name() const {
      return mName;
    }

    const QList<Edge*>& segments() const {
      return mSegments;
    }

    Edge* segment(int index) const {
      return mSegments[index];
    }

    void addSegment(Edge* segment) {
      assert(segment->role() == Edge::CUTTING);

      if(!mSegments.empty()) {
        assert(segment->isExtension<0>(mSegments.back(), 1.0e-6) && mSegments.back()->isExtension<1>(segment, 1.0e-6)); /* TODO: EPS */
        mSegments.back()->addExtension<1>(segment);
        segment->addExtension<0>(mSegments.back());
      }
      mSegments.push_back(segment);
    }

    View* view() const {
      return mView;
    }

    void setView(View* view) {
      assert(mView == NULL);
      mView = view;
    }

  private:
    QList<Edge*> mSegments;
    QString mName;
    View* mView;
  };

} // namespace qr

#endif // __QR_CUTTING_CHAIN_H__
