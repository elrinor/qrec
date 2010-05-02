#include "DxfReader.h"
#include <algorithm> /* for std::swap() */
#include <dxflib/dl_creationinterface.h>
#include <dxflib/dl_dxf.h>
#include "Drawing.h"
#include "Edge.h"
#include "Hatch.h"
#include "Label.h"
#include "Utility.h"

namespace qr {
// -------------------------------------------------------------------------- //
// DxfReader::DxfCreationInterface
// -------------------------------------------------------------------------- //
  class DxfReader::DxfCreationInterface: public DL_CreationInterface {
  public:
    DxfCreationInterface(Drawing* drawing): mDrawing(drawing) {}

  private:
    QList<Edge*> breakCircle(double x, double y, double radius) {
      QList<Edge*> result;
      for(int i = 0; i < 4; i++) {
        result.push_back(
          new Edge(
            Edge::Arc(), 
            Vector2d(x, y), 
            Vector2d(radius, 0.0),
            Vector2d(0.0, radius), 
            i * M_PI / 2, 
            M_PI / 2, 
            color(), 
            penStyle()
          )
        );
      }
      return result;
    }

    QList<Edge*> breakArc(double x, double y, double radius, double angle1, double angle2) {
      QList<Edge*> result;

      /* Normalize angles. */
      double startAngle = angle1;
      double spanAngle = angle2 - angle1;
      while(spanAngle < 0)
        spanAngle += 2 * M_PI;
      while(spanAngle > 2 * M_PI)
        spanAngle -= 2 * M_PI;
      while(startAngle < 0)
        startAngle += 2 * M_PI;
      while(startAngle > 2 * M_PI)
        startAngle -= 2 * M_PI;

      /* Break arc into smaller components. */
      double endAngle = startAngle + spanAngle;
      double currentAngle = startAngle;
      for(double midAngle = 0; midAngle < 4 * M_PI && midAngle < endAngle; midAngle += M_PI / 2) {
        if(currentAngle < midAngle && endAngle > midAngle) {
          result.push_back(
            new Edge(
              Edge::Arc(), 
              Vector2d(x, y), 
              Vector2d(radius, 0.0), 
              Vector2d(0.0, radius), 
              currentAngle, 
              midAngle - currentAngle, 
              color(), 
              penStyle()
            )
          );
          currentAngle = midAngle;
        }
      }

      /* Add the last component if needed. */
      if(currentAngle < endAngle) {
        result.push_back(
          new Edge(
            Edge::Arc(), 
            Vector2d(x, y), 
            Vector2d(radius, 0.0), 
            Vector2d(0.0, radius), 
            currentAngle, 
            endAngle - currentAngle, 
            color(), 
            penStyle()
          )
        );
      }

      return result;
    }

    QColor color() const {
      int colorCode = attributes.getColor();
      
      if(colorCode <= 0 || colorCode >= 256)
        return QColor(Qt::white);

      return QColor(
        static_cast<int>(dxfColors[colorCode][0] * 255),
        static_cast<int>(dxfColors[colorCode][1] * 255),
        static_cast<int>(dxfColors[colorCode][2] * 255)
      );
    }

    Qt::PenStyle penStyle() const {
      std::string lineType = attributes.getLineType();
      if(lineType == "DASHDOT")
        return Qt::DashDotLine;
      else if(lineType == "DASHEDX2" || lineType == "DASHED" || lineType == "DASHED2")
        return Qt::DashLine;
      else if(lineType == "ByLayer" || lineType == "CONTINUOUS")
        return Qt::SolidLine;
      else
        Unreachable();
    }

    virtual void addLine(const DL_LineData& data) {
      mDrawing->addEdge(new Edge(Edge::Line(), Vector2d(data.x1, data.y1), Vector2d(data.x2, data.y2), color(), penStyle()));
    }

    virtual void addArc(const DL_ArcData& data) {
      double angle1 = data.angle1;
      double angle2 = data.angle2;
      while(angle2 < angle1)
        angle2 += 2 * M_PI;
      foreach(Edge* segment, breakArc(data.cx, data.cy, data.radius, angle1, angle2))
        mDrawing->addEdge(segment);
    }

    virtual void addCircle(const DL_CircleData& data) {
      foreach(Edge* segment, breakCircle(data.cx, data.cy, data.radius))
        mDrawing->addEdge(segment);
    }

    virtual void addText(const DL_TextData& data) {
      mDrawing->addLabel(new Label(Vector2d(data.ipx, data.ipy), QString::fromLocal8Bit(data.text.c_str()), QFont("Arial", data.height), color()));
    }

    virtual void addMText(const DL_MTextData& data) {
      mDrawing->addLabel(new Label(Vector2d(data.ipx, data.ipy), QString::fromLocal8Bit(data.text.c_str()), QFont("Arial", data.height), color()));
    }

    virtual void addHatch(const DL_HatchData& /*data*/) {
      mDrawing->addHatch(new Hatch(QBrush(color(), Qt::BDiagPattern)));
      /* TODO: parse data. */
    }

    virtual void addHatchLoop(const DL_HatchLoopData& /*data*/) {
      /* TODO */
    }

    virtual void addHatchEdge(const DL_HatchEdgeData& data) {
      Edge* segment = NULL;
      Hatch* hatch = mDrawing->hatches().back();
      if(data.type == 1) {
        segment = new Edge(Edge::Line(), Vector2d(data.x1, data.y1), Vector2d(data.x2, data.y2), color(), penStyle());
        segment->setHatch(hatch);
        hatch->addSegment(segment);
        mDrawing->addEdge(segment);
      } else if(data.type == 2) {
        double angle1 = data.angle1, angle2 = data.angle2;
        if(!data.ccw)
          std::swap(angle1, angle2);
        foreach(Edge* segment, breakArc(data.cx, data.cy, data.radius, angle1, angle2)) {
          segment->setHatch(hatch);
          hatch->addSegment(segment);
          mDrawing->addEdge(segment);
        }
      } else {
        Unreachable();
      }
    }

    virtual void endEntity() {
      /* Skip. */
    }

    virtual void addMTextChunk(const char* /*text*/) {}

    virtual void addEllipse2d(const DL_Ellipse2dData& /*data*/) {}
    virtual void addPoint(const DL_PointData& /*data*/) {}

    virtual void addLayer(const DL_LayerData& /*data*/) {}
    virtual void addBlock(const DL_BlockData& /*data*/) {}
    virtual void endBlock() {}
    virtual void addPolyline(const DL_PolylineData& /*data*/) {}
    virtual void addVertex(const DL_VertexData& /*data*/) {}
    virtual void addSpline(const DL_SplineData& /*data*/) {}
    virtual void addControlPoint(const DL_ControlPointData& /*data*/) {}
    virtual void addKnot(const DL_KnotData& /*data*/) {}
    virtual void addInsert(const DL_InsertData& /*data*/) {}
    virtual void addTrace(const DL_TraceData& /*data*/) {}
    virtual void add3dFace(const DL_3dFaceData& /*data*/) {}
    virtual void addSolid(const DL_SolidData& /*data*/) {}
    virtual void addDimAlign(const DL_DimensionData& /*data*/, const DL_DimAlignedData& /*edata*/) {}
    virtual void addDimLinear(const DL_DimensionData& /*data*/, const DL_DimLinearData& /*edata*/) {}
    virtual void addDimRadial(const DL_DimensionData& /*data*/, const DL_DimRadialData& /*edata*/) {}
    virtual void addDimDiametric(const DL_DimensionData& /*data*/, const DL_DimDiametricData& /*edata*/) {}
    virtual void addDimAngular(const DL_DimensionData& /*data*/, const DL_DimAngularData& /*edata*/) {}
    virtual void addDimAngular3P(const DL_DimensionData& /*data*/, const DL_DimAngular3PData& /*edata*/) {}
    virtual void addDimOrdinate(const DL_DimensionData& /*data*/, const DL_DimOrdinateData& /*edata*/) {}
    virtual void addLeader(const DL_LeaderData& /*data*/) {}
    virtual void addLeaderVertex(const DL_LeaderVertexData& /*data*/) {}
    virtual void addImage(const DL_ImageData& /*data*/) {}
    virtual void linkImage(const DL_ImageDefData& /*data*/) {}
    virtual void addComment(const char* /*comment*/) {}
    virtual void setVariableVector(const char* /*key*/, double /*v1*/, double /*v2*/, double /*v3*/, int /*code*/) {}
    virtual void setVariableString(const char* /*key*/, const char* /*value*/, int /*code*/) {}
    virtual void setVariableInt(const char* /*key*/, int /*value*/, int /*code*/) {}
    virtual void setVariableDouble(const char* /*key*/, double /*value*/, int /*code*/) {}
    virtual void endSequence() {}

  private:
    Drawing* mDrawing;
  };


// -------------------------------------------------------------------------- //
// DxfReader
// -------------------------------------------------------------------------- //
  DxfReader::DxfReader(QIODevice& source, Drawing* drawing) {
    mData = source.readAll();
    mDrawing = drawing;
  }

  void DxfReader::operator() () {
    std::auto_ptr<DL_Dxf> reader(new DL_Dxf());
    DxfCreationInterface creationInterface(mDrawing);
    std::stringstream stringStream(std::string(mData.data()));

    if(!reader->in(stringStream, &creationInterface))
      throw std::runtime_error("Invalid DXF file format"); // TODO: exception class? 
  }

} // namespace qr


