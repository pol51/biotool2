#include "dataCtrl.h"

#include <QtOpenGL/QGLContext>
#include <QtXml/QDomDocument>
#include <QtCore/QFile>

DataCtrl::DataCtrl(QObject *parent):
  QObject(parent), saved(true), _imageRealWidth(1.), cntMode(eModeView)
{
}

void DataCtrl::addPoint(const QPointF &point)
{
  points.append(point);
  emit countChanged(points.count());
  saved = false;
}

void DataCtrl::draw() const
{
  glPointSize(10.);
  glColor3f(.0f, .0f, .0f);
  glBegin(GL_POINTS);
    foreach(const Point &Point, points) Point.draw();
  glEnd();

  glPointSize(4.);
  glColor3f(1.f, 1.f, 1.f);
  glBegin(GL_POINTS);
    foreach(const Point &Point, points) Point.draw();
  glEnd();
}

void DataCtrl::removeLastPoint()
{
  if (points.count())
  {
    points.pop_back();
    emit countChanged(points.count());
    saved = false;
  }
}

void DataCtrl::clear()
{
  points.clear();
  emit countChanged(0);
  saved = true;
}

void DataCtrl::save(const QString &filename)
{
  QDomDocument Doc("document");
  QDomElement Root = Doc.createElement("document");
  Doc.appendChild(Root);

  QDomElement Points = Doc.createElement("points");
  Points.setAttribute("imageRealWidth", _imageRealWidth);
  Root.appendChild(Points);
  foreach(const Point &_point, points) _point.save(Doc, Points);

  QString FileName(filename);
  if (!FileName.endsWith(".xml"))
    FileName.append(".xml");
  QFile File(FileName);
  if (File.open(QFile::WriteOnly))
  {
    File.write(Doc.toString(2).toUtf8());
    File.close();
  }

  saved = true;
}

void DataCtrl::exportCsv(const QString &filename)
{
  QByteArray CSV;

  if (points.count() > 1)
    foreach(const Point &PointA, points)
    {
      qreal MinimalDistance(4.);
      foreach(const Point &PointB, points)
        if (&PointA != &PointB)
        {
          qreal Distance(QLineF(PointA, PointB).length());
          if (Distance < MinimalDistance)
            MinimalDistance = Distance;
        }

      CSV.append(QString("%1\n").arg(QString::number(MinimalDistance * _imageRealWidth / 2.)));
    }
  else if (points.count() == 1)
    CSV.append(QString("%1\n").arg(0.));

  QString FileName(filename);
  if (!FileName.endsWith(".csv"))
    FileName.append(".csv");
  QFile File(FileName);
  if (File.open(QFile::WriteOnly))
  {
    File.write(CSV);
    File.close();
  }
}

void DataCtrl::load(const QString &filename)
{
  QDomDocument Doc("document");
  QFile File(filename);
  if (!File.open(QIODevice::ReadOnly))
    return;

  if (!Doc.setContent(&File))
  {
    File.close();
    return;
  }
  File.close();

  clear();

  QDomElement Element = Doc.documentElement().firstChildElement();
  while (!Element.isNull())
  {
    if (Element.tagName() == "points")
    {
      bool ImageRealWidthOk;
      qreal ImageRealWidth = Element.attribute("imageRealWidth").toDouble(&ImageRealWidthOk);
      if (ImageRealWidthOk)
        _imageRealWidth = ImageRealWidth;

      QDomElement PointElement = Element.firstChildElement("point");
      while (!PointElement.isNull())
      {
        Point LoadedPoint;
        if (LoadedPoint.load(PointElement))
          points.push_back(LoadedPoint);
        PointElement = PointElement.nextSiblingElement("point");
      }
    }
    Element = Element.nextSiblingElement();
  }

  emit countChanged(points.count());

  saved = true;
}
