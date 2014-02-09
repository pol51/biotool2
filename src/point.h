#ifndef __POINT_H__
#define __POINT_H__

#include <QtCore/QPointF>

class QDomDocument;
class QDomElement;

class Point : public QPointF
{
  public:
    Point();
    Point(const QPointF& point);
    Point(const qreal &x, const qreal &y);

    void draw() const;
    void save(QDomDocument &doc, QDomElement &parentNode) const;
    bool load(QDomElement &node);
};

#endif
