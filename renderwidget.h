#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QVector3D>

class RenderWidget : public QGLWidget
{
    Q_OBJECT

public:
    RenderWidget(QWidget *parent = 0);
    ~RenderWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    bool loadFile(const QString &fileName);

public slots:
    void setXPos(double pos);
    void setYPos(double pos);
    void setZPos(double pos);

    void cameraForward(double amount);

    void setPointRenderCount(int);

signals:
    void xPosChanged(double pos);
    void yPosChanged(double pos);
    void zPosChanged(double pos);

    void pointCountChanged(int count);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *e);

    void keyPressEvent(QKeyEvent *);

protected:
    bool parsePoints(const QString &filePathName);
    bool parseLine(const QString& line, QVector3D &ret, double &extrudeAmount, QString &retCode);
    bool doLinePoints();

    void drawAxes(const QVector3D &origin, float length);
    void drawGrid(int majorSpacing);

    double cameraDistance() const;
    QVector3D cameraDir() const;

private slots:
    void showPoints();
    void showCodes();

    void cameraReset();

private:

    QPoint m_LastPressPos;

    struct LineStrip {
        QVector<QVector3D> Points;
        bool isExtruded;
    };

    QList<LineStrip> m_StripList;
    int m_PointCount;

    int m_RenderPointCount;

    QVector<QVector4D> m_Points;

    QVector3D m_MinPoint;
    QVector3D m_MaxPoint;
    QVector3D m_PointsCentre;

    int m_CamRotX;
    int m_CamRotY;
    int m_CamRotZ;
    QVector3D m_CamPos;
    QVector3D m_CamLookAt;

    QMap<QString, int> m_CodeInfo;
};

#endif
