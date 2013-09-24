#ifndef GCPMAINWINDOW_H
#define GCPMAINWINDOW_H

#include <QWidget>

class QDoubleSpinBox;
class QSlider;
class RenderWidget;

class GCPMainWindow : public QWidget
{
    Q_OBJECT

public:
    GCPMainWindow();

public slots:
    void onModelPointsChanged(int count);


protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);

    void mouseReleaseEvent(QMouseEvent *e);

private:
    bool doLoadFile(const QString &filePathName);

private slots:
    void startAnimate();

private:
    QDoubleSpinBox *createEdit();

private:
    QDoubleSpinBox *m_pEditXPos;
    QDoubleSpinBox *m_pEditYPos;
    QDoubleSpinBox *m_pEditZPos;

    QSlider *m_pPointsSlider;

    RenderWidget *m_pRenderWindow;
};

#endif
