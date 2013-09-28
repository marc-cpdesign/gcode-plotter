#include <QtOpenGL>
#include <QTextBrowser>

#include <math.h>
#include <GL/glu.h>

#include "renderwidget.h"

RenderWidget::RenderWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    setFocusPolicy(Qt::StrongFocus);

    m_CamRotX = 0;
    m_CamRotY = 0;
    m_CamRotZ = 0;

    m_CamPos = QVector3D(100, -100, 50);
}

RenderWidget::~RenderWidget()
{
}

QSize RenderWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize RenderWidget::sizeHint() const
{
    return QSize(640, 480);
}

bool RenderWidget::loadFile(const QString &fileName)
{
    bool ok = parsePoints(fileName);
    if (ok) {
        m_CamLookAt = m_PointsCentre;

        emit xPosChanged(m_CamLookAt.x());
        emit yPosChanged(m_CamLookAt.y());
        emit zPosChanged(m_CamLookAt.z());

        emit pointCountChanged(m_PointCount);
    }

    return ok;
}

void RenderWidget::setXPos(double pos)
{
    if (pos != m_CamLookAt.x()) {
        m_CamLookAt.setX(pos);
        emit xPosChanged(pos);
        updateGL();
    }
}

void RenderWidget::setYPos(double pos)
{
    if (pos != m_CamLookAt.y()) {
        m_CamLookAt.setY(pos);
        emit yPosChanged(pos);
        updateGL();
    }
}

void RenderWidget::setZPos(double pos)
{
    if (pos != m_CamLookAt.z()) {
        m_CamLookAt.setZ(pos);
        emit zPosChanged(pos);
        updateGL();
    }
}

void RenderWidget::cameraForward(double amount)
{
    m_CamPos += cameraDir() * amount;
    updateGL();
}

void RenderWidget::setPointRenderCount(int count)
{
    m_RenderPointCount = count;
    updateGL();
}

void RenderWidget::initializeGL()
{
    qglClearColor(QColor(0x20,0x23,0x25));

    glEnable(GL_DEPTH_TEST);
 //   glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderWidget::drawAxes(const QVector3D &origin, float axisLength)
{
    float alpha = 0.6;
    glBegin(GL_LINE_STRIP);
    glColor4f(1.0, 0, 0, alpha);
    glVertex3f(origin.x(), origin.y(), origin.z());
    glColor4f(1.0, 0, 0, alpha);
    glVertex3f(origin.x() + axisLength, origin.y(), origin.z());
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor4f(0, 1, 0, alpha);
    glVertex3f(origin.x(), origin.y(), origin.z());
    glColor4f(0, 1, 0, alpha);
    glVertex3f(origin.x(), origin.y() + axisLength, origin.z());
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor4f(0, 0, 1, alpha);
    glVertex3f(origin.x(), origin.y(), origin.z());
    glColor4f(0, 0, 1, alpha);
    glVertex3f(origin.x(), origin.y(), origin.z() + axisLength);
    glEnd();
}

void RenderWidget::drawGrid(int majorSpacing)
{
    const float alpha = 0.3;
    const float extent = 1000;

    glBegin(GL_LINES);
    for (int x =0; x <= (int)extent; x+= majorSpacing) {
    glColor4f(0.4, 0.4, 0.4, alpha);
        glVertex3f(x, 0, 0);
        glVertex3f(x, extent, 0);
    }
    for (int y =0; y <= (int)extent; y += majorSpacing) {
        glVertex3f(0, y, 0);
        glVertex3f(extent, y, 0);
    }
    glEnd();
}

double RenderWidget::cameraDistance() const
{
    return qAbs( QVector3D(m_CamPos - m_CamLookAt).length());
}

QVector3D RenderWidget::cameraDir() const
{
    QVector3D viewDir =  m_CamLookAt - m_CamPos;
    return viewDir.normalized();
}

void RenderWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    gluLookAt(m_CamPos.x(), m_CamPos.y(), m_CamPos.z(),
              m_CamLookAt.x(), m_CamLookAt.y(), m_CamLookAt.z(),
              0, 0, 1);

    glTranslatef(m_CamLookAt.x(), m_CamLookAt.y(), m_CamLookAt.z());
    glRotatef(m_CamRotX / 16.0, 1.0, 0.0, 0.0);
    glRotatef(m_CamRotY / 16.0, 0.0, 1.0, 0.0);
    glRotatef(m_CamRotZ / 16.0, 0.0, 0.0, 1.0);
    glTranslatef(-m_CamLookAt.x(), -m_CamLookAt.y(), -m_CamLookAt.z());

    drawAxes(QVector3D(0,0,0), 1000);

    doLinePoints();

    drawGrid(10);

    glDisable(GL_DEPTH_TEST);
    drawAxes(m_CamLookAt, 10);
}

void RenderWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    //glOrtho(-50.0, +50.0, -50.0, +50.0, 0.1, 10000.0);

    float aspect = float(width) / float(height);
    gluPerspective(60, aspect, 0.1, 1000.0);

#endif
    glMatrixMode(GL_MODELVIEW);
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    m_LastPressPos = event->pos();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_LastPressPos.isNull()) {
        return;
    }

    int dx = event->x() - m_LastPressPos.x();
    int dy = event->y() - m_LastPressPos.y();

    if (event->buttons() & Qt::LeftButton) {
        m_CamRotZ += 8 * dx;
        m_CamRotX -= 8 * dy;

        updateGL();
    }

    m_LastPressPos = event->pos();
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if ((e->button() == Qt::RightButton)
            && (e->modifiers() & Qt::ShiftModifier)) {
        QMenu m;
        m.addAction(tr("Reset camera"), this, SLOT(cameraReset()));
        m.addSeparator();
        m.addAction(tr("Dump points"), this, SLOT(showPoints()));
        m.addAction(tr("Dump codes"), this, SLOT(showCodes()));

        m.exec(e->globalPos());
    }

    m_LastPressPos = QPoint();
}

void RenderWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Plus:
    case Qt::Key_Equal: {
        if (cameraDistance() > 10) {
            cameraForward(10);
        } else {
            cameraForward(1);
        }
        break;
    }
    case Qt::Key_Minus: {
        if (cameraDistance() > 10) {
            cameraForward(-10);
        } else {
            cameraForward(-1);
        }
        break;
    }
    case Qt::Key_Up: {
        setZPos(m_CamLookAt.z() +  1);
        break;
    }
    case Qt::Key_Down: {
        setZPos(m_CamLookAt.z() - 1);
        break;
    }
    case Qt::Key_Right: {
        setXPos(m_CamLookAt.x() + 1);
        break;
    }
    case Qt::Key_Left: {
        setXPos(m_CamLookAt.x() - 1);
        break;
    }
    case Qt::Key_PageUp: {
        setYPos(m_CamLookAt.y() + 1);
        break;
    }
    case Qt::Key_PageDown: {
        setYPos(m_CamLookAt.y() - 1);
        break;
    }

    default:
        return;
    }
}

bool RenderWidget::parseLine(const QString &line,
                         QVector3D &ret,
                         double &extrudeAmount,
                         QString &retCode)
{
    QString l = line;
    int firstComment = l.indexOf(";", 0);
    if (firstComment > 0) {
        l.truncate(firstComment);
    }

    QStringList sl = l.split(" ", QString::SkipEmptyParts);

    if (sl.size() != 0) {
        if (sl[0].startsWith(';')) {
            retCode = ";";
        } else {
            retCode = sl[0];
        }
    }

    Q_FOREACH(QString s, sl) {
        char c = s[0].toAscii();
        s.remove(0, 1);

        bool ok = true;
        switch (c)
        {
        case 'G':
            break;
        case 'E': {
            extrudeAmount = s.toDouble(&ok);
            if (extrudeAmount < 0) {
                extrudeAmount = 0.0;
            }
            break;
        }
        case 'X':
            ret.setX(s.toDouble(&ok));
            break;
        case 'Y':
            ret.setY(s.toDouble(&ok));
            break;
        case 'Z':
            ret.setZ(s.toDouble(&ok));
            break;
        default:
            break;
        }
        if (!ok) {
            return false;
        }
    }

    return true;
}

static void adjustMaxPoint(QVector3D &vMax, const QVector3D &v1)
{
    if (vMax.x() < v1.x()) {
        vMax.setX(v1.x());
    }
    if (vMax.y() < v1.y()) {
        vMax.setY(v1.y());
    }
    if (vMax.z() < v1.z()) {
        vMax.setZ(v1.z());
    }
}

static void adjustMinPoint(QVector3D &vMin, const QVector3D &v1)
{
    if (vMin.x() > v1.x()) {
        vMin.setX(v1.x());
    }
    if (vMin.y() > v1.y()) {
        vMin.setY(v1.y());
    }
    if (vMin.z() > v1.z()) {
        vMin.setZ(v1.z());
    }
}

bool RenderWidget::parsePoints(const QString &filePathName)
{
    m_Points.clear();
    m_CodeInfo.clear();

    m_StripList.clear();
    m_PointCount = 0;

    QFile f(filePathName);

    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }
    m_MinPoint = QVector3D(10000, 10000, 10000);
    m_MaxPoint = QVector3D(-10000, -10000, -10000);

    const QVector3D origin = QVector3D(0, 0, 0);

    double currAmount = 0.0;

    QVector3D point = origin;
    while (!f.atEnd()) {
        QString codeString;
        double extrudeAmount = currAmount;
        QString line = f.readLine().trimmed();
        if (line.isEmpty() || line.startsWith(";")) {
            continue;
        }

        QVector3D lastPoint = point;

        if (!parseLine(line, point, extrudeAmount, codeString)) {
            return false;
        }

        m_CodeInfo[codeString.trimmed()] ++;

        bool isExtruding = extrudeAmount > currAmount;

        bool newStrip = m_StripList.isEmpty() || (isExtruding != m_StripList.last().isExtruded);

        if (newStrip) {
            LineStrip ls;
            ls.isExtruded = isExtruding;
            m_StripList.append(ls);
            if (isExtruding) {
                m_StripList.back().Points.append(lastPoint);
                ++m_PointCount;
            }
        }

        LineStrip &rStrip = m_StripList.back();
        rStrip.Points += point;

        if (isExtruding) {
            ++m_PointCount;
            adjustMinPoint(m_MinPoint, point);
            adjustMaxPoint(m_MaxPoint, point);
        }

        if (newStrip) {
            if (!m_Points.isEmpty()) {
                QVector4D v = m_Points.last();
                v.setW(isExtruding ? 1.0 : 0.0);
                m_Points += v;
            }
        }
        m_Points += QVector4D(point, isExtruding ? 1.0 : 0.0);

        currAmount = extrudeAmount;
    }

    m_PointsCentre = (m_MinPoint + m_MaxPoint) / 2;

    return true;
}

bool RenderWidget::doLinePoints()
{
    int pointsTotal = m_PointCount;
    int count = 0;

    Q_FOREACH(const LineStrip &exStrip, m_StripList) {

        if (exStrip.isExtruded) {
            const float minCol = 0.3f;
            const QVector<QVector3D> &points = exStrip.Points;

            glBegin(GL_LINE_STRIP);

            for (int ii = 0; ii < points.size(); ++ii) {

                const QVector3D &point = points.at(ii);

                float c = static_cast<float>(count) / static_cast<float>(pointsTotal);
                c *= (1 - minCol);
                c += minCol;

                glColor3f(c, c, c);
                glVertex3f(point.x(), point.y(), point.z());

                if (count >= m_RenderPointCount) {
                    glEnd();
                    return true;
                }

                ++count;
            }

            glEnd();
        }
    }

    return true;
}

static QString vec3DToString(const QVector3D &v)
{
    return QString("%1, %2, %3")
            .arg(v.x(), 10, 'f', 4)
            .arg(v.y(), 10, 'f', 4)
            .arg(v.x(), 10, 'f', 4);
}

static QString vec4DToString(const QVector4D &v)
{
    return QString("%1, %2, %3")
            .arg(v.x(), 10, 'f', 4)
            .arg(v.y(), 10, 'f', 4)
            .arg(v.x(), 10, 'f', 4);
}

void RenderWidget::showPoints()
{
    QTextBrowser *pBrowser = new QTextBrowser;
    pBrowser->setAttribute(Qt::WA_DeleteOnClose);

    QStringList sl;

    Q_FOREACH(const QVector4D &point, m_Points) {
        sl += vec4DToString(point);
    }

    pBrowser->setPlainText(sl.join("\n"));

    pBrowser->show();
}

void RenderWidget::showCodes()
{
    QTextBrowser *pBrowser = new QTextBrowser;
    pBrowser->setAttribute(Qt::WA_DeleteOnClose);

    QString s;

    for (QMap<QString,int>::iterator it = m_CodeInfo.begin(); it != m_CodeInfo.end(); ++it) {
        s += QString("%1  : %2 \n")
                .arg(it.key(), 16, QLatin1Char(' '))
                .arg(it.value());
    }

    pBrowser->setPlainText(s);
    pBrowser->show();
}

void RenderWidget::cameraReset()
{
    setXPos(m_PointsCentre.x());
    setYPos(m_PointsCentre.y());
    setZPos(m_PointsCentre.z());

    m_CamRotX = 0;
    m_CamRotY = 0;
    m_CamRotZ = 0;

    updateGL();
}
