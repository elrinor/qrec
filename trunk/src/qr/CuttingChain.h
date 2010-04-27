#ifndef __QR_CUTTING_CHAIN_H__
#define __QR_CUTTING_CHAIN_H__

#include "config.h"
#include <boost/noncopyable.hpp>
#include <QString>
#include <QList>
#include "Primitive.h"

namespace qr {
  class Edge;

// -------------------------------------------------------------------------- //
// CuttingChain
// -------------------------------------------------------------------------- //
  class CuttingChain: public Primitive, private boost::noncopyable {
  public:
    CuttingChain(const QString& name): mName(name) {}

    const QString& name() const {
      return mName;
    }

    const QList<Edge*>& edges() const {
      return mEdges;
    }

    Edge* edge(int index) const {
      return mEdges[index];
    }

    void addEdge(Edge* edge) {
      assert(edge->role() == Edge::CUTTING);

      if(!mEdges.empty()) {
        assert(edge->isExtension<0>(mEdges.back(), 1.0e-6) && mEdges.back()->isExtension<1>(edge, 1.0e-6)); /* TODO: EPS */
        mEdges.back()->addExtension<1>(edge);
        edge->addExtension<0>(mEdges.back());
      }
      mEdges.push_back(edge);
    }

  private:
    QList<Edge*> mEdges;
    QString mName;
  };

} // namespace qr

#endif // __QR_CUTTING_CHAIN_H__
