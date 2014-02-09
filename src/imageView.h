#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__

#include <QtOpenGL/QGLWidget>

#include <QtCore/QTimer>

#include "dataCtrl.h"

class ImageView : public QGLWidget
{
  Q_OBJECT

  public:
    ImageView(QWidget *parent = NULL);
    ~ImageView();

    void changeMode(DataCtrl::EMode mode) { dataCtrl->setCurrentMode(mode); onMoveDecal = false; }
    void setImageRealWidth(const qreal &width) { dataCtrl->setImageRealWidth(width / ratioWidthPerHeght); }
    void setImageRealHeight(const qreal &height) { dataCtrl->setImageRealWidth(height); }

    DataCtrl& data() { return *dataCtrl; }
    const DataCtrl& data() const { return *dataCtrl; }

    qreal imageRealWidth()  const { return dataCtrl->imageRealWidth() * ratioWidthPerHeght; }
    qreal imageRealHeight() const { return dataCtrl->imageRealWidth(); }

  public slots:
    void doZoomIn()     { if (zoom > 1) --zoom; }
    void doZoomOut()    { if (zoom < 11) ++zoom; }
    void doResetView()  { zoom = 10; xDecal = yDecal = 0.; resizeGL(width(), height()); }

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

  protected slots:
    void doChangeImage(const QImage &image);
    void doCloseImage();

  protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

  protected:
    bool onMoveDecal;
    int zoom;
    DataCtrl *dataCtrl;
    GLuint imageTexId;
    GLfloat xDecal, yDecal;
    qreal ratioWidthPerHeght;
    QTimer refreshTimer;
    QPoint lastMousePos;
};

#endif
