#ifndef __QR_DXF_READER_H__
#define __QR_DXF_READER_H__

#include "config.h"
#include <QByteArray>

class QIODevice;

namespace qr {
  class Drawing;

// -------------------------------------------------------------------------- //
// DxfReader
// -------------------------------------------------------------------------- //
  class DxfReader {
  public:
    DxfReader(QIODevice& source, Drawing* drawing);

    void operator() ();

  private:
    class DxfCreationInterface;

    QByteArray mData;
    Drawing* mDrawing;
  };

} // namespace qr

#endif // __QR_DXF_READER_H__
