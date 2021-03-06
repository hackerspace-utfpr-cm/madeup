#include "MadeupCanvas.h"
#include <iostream>

using std::string;

/* ------------------------------------------------------------------------- */

MadeupCanvas::MadeupCanvas(QWidget *parent) :
  QOpenGLWidget(parent),
  auto_rotate_timer(this) {
  /* setAttribute(Qt::WA_TranslucentBackground); */
  /* setAttribute(Qt::WA_NoSystemBackground, false); */
  /* QSurfaceFormat format; */
  /* format.setVersion(3, 2); */
  /* format.setProfile(QSurfaceFormat::CoreProfile); */
  /* format.setDepthBufferSize(24); */

  // Really need QSurfaceFormat::setDefaultFormat(format)?
  // https://forum.qt.io/topic/67280/create-a-core-profile-opengl-app/3
 
  // Multisampling seems to mess up glReadPixels. TODO: fix this by blitting
  // the FBO to a non-multisampled FBO and reading from that.
  /* format.setSamples(4); */

  // These don't seem to be necessary. Defaults?
  /* format.setRedBufferSize(8); */
  /* format.setGreenBufferSize(8); */
  /* format.setBlueBufferSize(8); */
  /* format.setAlphaBufferSize(8); */

  /* setFormat(format); */

  std::cout << "format().hasAlpha(): " << format().hasAlpha() << std::endl;

  connect(&auto_rotate_timer, &QTimer::timeout, [=]() {
    makeCurrent();
    renderer.autoRotate();
    update();
  });
}

/* ------------------------------------------------------------------------- */

void MadeupCanvas::paintGL() {
  renderer.render();
}

/* ------------------------------------------------------------------------- */

void MadeupCanvas::initializeGL() {
  initializeOpenGLFunctions();
  renderer.initializeGL();
}

/* ------------------------------------------------------------------------- */

void MadeupCanvas::resizeGL(int width, int height) {
  renderer.resize(width, height);
  update();
}

/* ------------------------------------------------------------------------- */

MadeupRenderer &MadeupCanvas::getRenderer() {
  return renderer;
}

/* ------------------------------------------------------------------------- */

void MadeupCanvas::mousePressEvent(QMouseEvent *event) {
  makeCurrent();
  auto_rotate_timer.stop();
  renderer.leftMouseDownAt(event->x(), height() - event->y());
  update();
}

/* ------------------------------------------------------------------------- */

void MadeupCanvas::mouseMoveEvent(QMouseEvent *event) {
  makeCurrent();
  renderer.leftMouseDraggedTo(event->x(), height() - event->y());
  update();
}

/* ------------------------------------------------------------------------- */

void MadeupCanvas::mouseReleaseEvent(QMouseEvent *event) {
  makeCurrent();
  float inertia = renderer.leftMouseUpAt(event->x(), height() - event->y());
  if (inertia > 3) {
    auto_rotate_timer.start(33);
  }
  update();
}

/* ------------------------------------------------------------------------- */

void MadeupCanvas::wheelEvent(QWheelEvent *event) {
  makeCurrent();
  renderer.scroll(event->angleDelta().y());
  update();
}

/* ------------------------------------------------------------------------- */

