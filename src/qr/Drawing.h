#ifndef __QR_DRAWING_H__
#define __QR_DRAWING_H__

#include "config.h"
#include <boost/noncopyable.hpp>
#include <QList>
#include "Label.h"
#include "Edge.h"
#include "Hatch.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Drawing
// -------------------------------------------------------------------------- //
  class Drawing: private boost::noncopyable {
  public:
    Drawing() {}

    Drawing(const QList<Edge*>& edges, const QList<Label*>& labels, const QList<Hatch*>& hatches): mEdges(edges), mLabels(labels), mHatches(hatches) {}

    const QList<Edge*>& edges() const {
      return mEdges;
    }

    void addEdge(Edge* segment) {
      mEdges.push_back(segment);
    }

    void setEdges(const QList<Edge*>& segments) {
      mEdges = segments;
    }

    const QList<Label*>& labels() const {
      return mLabels;
    }

    void addLabel(Label* label) {
      mLabels.push_back(label);
    }

    void setLabels(const QList<Label*>& labels) {
      mLabels = labels;
    }

    const QList<Hatch*>& hatches() const {
      return mHatches;
    }

    void addHatch(Hatch* hatch) {
      mHatches.push_back(hatch);
    }

    void setHatches(QList<Hatch*>& hatches) {
      mHatches = hatches;
    }

  private:
    QList<Edge*> mEdges;
    QList<Label*> mLabels;
    QList<Hatch*> mHatches;
  };

} // namespace qr


#endif // __QR_DRAWING_H__
