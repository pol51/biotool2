#include "point.h"

#include <QtOpenGL/QGLWidget>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

Point::Point() :
  QPointF()
{
}

Point::Point(const QPointF &point) :
  QPointF(point)
{
}

Point::Point(const qreal &x, const qreal &y) :
  QPointF(x, y)
{
}

void Point::draw() const
{
  glVertex3f(x(), y(), 0.f);
}

void Point::save(QDomDocument &doc, QDomElement &parentNode) const
{
  QDomElement PointNode = doc.createElement("point");
  parentNode.appendChild(PointNode);
  PointNode.setAttribute("x", x());
  PointNode.setAttribute("y", y());
}

bool Point::load(QDomElement &node)
{
  bool OkX = false, OkY = false;
  double X = node.attribute("x").toDouble(&OkX);
  double Y = node.attribute("y").toDouble(&OkY);
  if (OkX && OkY)
  {
    setX(X);
    setY(Y);

    return true;
  }
  return false;
}
