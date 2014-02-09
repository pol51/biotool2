#ifndef __DATACTRL_H__
#define __DATACTRL_H__

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "point.h"

class DataCtrl : public QObject
{
  Q_OBJECT

  public:
    enum EMode
    {
      eModeView,
      eModeEdit,
    };

  public:
    DataCtrl(QObject *parent = NULL);
    virtual ~DataCtrl() {}

    void addPoint(const QPointF &point);

    void draw() const;

    bool isSaved() const { return saved; }

    void setImageRealWidth(const qreal &width) { _imageRealWidth = width; }
    const qreal &imageRealWidth() const { return _imageRealWidth; }

  public slots:
    void removeLastPoint();

    void clear();

    void save(const QString &filename);
    void load(const QString &filename);
    void exportCsv(const QString &filename);

    void setCurrentMode(const EMode mode) { cntMode = mode; }
    EMode currentMode() const { return cntMode; }

  signals:
    void countChanged(int);

  protected:
    bool saved;
    qreal _imageRealWidth;
    EMode cntMode;
    QVector<Point> points;
};

#endif
