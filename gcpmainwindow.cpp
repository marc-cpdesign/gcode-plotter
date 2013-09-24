#include <QtGui>

#include <QDoubleSpinBox>
#include <QSlider>

#include "renderwidget.h"
#include "gcpmainwindow.h"

GCPMainWindow::GCPMainWindow()
{
    setAcceptDrops(true);

    m_pRenderWindow = new RenderWidget;

    m_pEditXPos = createEdit();
    m_pEditYPos = createEdit();
    m_pEditZPos = createEdit();

    connect(m_pEditXPos, SIGNAL(valueChanged(double)), m_pRenderWindow, SLOT(setXPos(double)));
    connect(m_pRenderWindow, SIGNAL(xPosChanged(double)), m_pEditXPos, SLOT(setValue(double)));
    connect(m_pEditYPos, SIGNAL(valueChanged(double)), m_pRenderWindow, SLOT(setYPos(double)));
    connect(m_pRenderWindow, SIGNAL(yPosChanged(double)), m_pEditYPos, SLOT(setValue(double)));
    connect(m_pEditZPos, SIGNAL(valueChanged(double)), m_pRenderWindow, SLOT(setZPos(double)));
    connect(m_pRenderWindow, SIGNAL(zPosChanged(double)), m_pEditZPos, SLOT(setValue(double)));

    QVBoxLayout *pAreaLayout = new QVBoxLayout;
    pAreaLayout->addWidget(m_pRenderWindow);

    QHBoxLayout *pViewposLayout = new QHBoxLayout;
    pViewposLayout->addWidget(m_pEditXPos);
    pViewposLayout->addWidget(m_pEditYPos);
    pViewposLayout->addWidget(m_pEditZPos);

    pAreaLayout->addLayout(pViewposLayout);

    m_pPointsSlider = new QSlider;
    m_pPointsSlider->setMinimum(0);
    m_pPointsSlider->setMaximum(0);
    m_pPointsSlider->setOrientation(Qt::Vertical);
    m_pPointsSlider->setInvertedControls(true);

    connect(m_pRenderWindow, SIGNAL(pointCountChanged(int)),
            this, SLOT(onModelPointsChanged(int)));
    connect(m_pPointsSlider, SIGNAL(valueChanged(int)),
            m_pRenderWindow, SLOT(setPointRenderCount(int)));

    QHBoxLayout *pLayout = new QHBoxLayout;
    pLayout->addLayout(pAreaLayout);
    pLayout->addWidget(m_pPointsSlider);
    setLayout(pLayout);

    setWindowTitle(tr("GCode Plotter"));

    m_pRenderWindow->loadFile("large_fan_duct.gcode");
}

void GCPMainWindow::onModelPointsChanged(int count)
{
    m_pPointsSlider->setMinimum(0);
    m_pPointsSlider->setMaximum(count);
    m_pPointsSlider->setValue(count);
}

QDoubleSpinBox *GCPMainWindow::createEdit()
{
    QDoubleSpinBox *pRet = new QDoubleSpinBox;
    pRet->setMinimum(-10000);
    pRet->setMaximum(10000);
    pRet->setDecimals(3);
    return pRet;
}

void GCPMainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void GCPMainWindow::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        QList<QUrl> urls = e->mimeData()->urls();

        Q_FOREACH(const QUrl &u, urls) {
            QString fileName = u.toLocalFile();

            doLoadFile(fileName);
        }
    }
}

void GCPMainWindow::mouseReleaseEvent(QMouseEvent *e)
{

    if (e->button() == Qt::RightButton) {
        QMenu m;
        m.addAction(tr("Animate"), this, SLOT(startAnimate()));

        m.exec(e->globalPos());
    }
}

bool GCPMainWindow::doLoadFile(const QString &filePathName)
{
    if (!m_pRenderWindow->loadFile(filePathName)) {
        return false;
    }
    return true;
}

#include <QPropertyAnimation>
void GCPMainWindow::startAnimate()
{
    QPropertyAnimation *pAnim = new QPropertyAnimation(m_pPointsSlider, "value");
    pAnim->setStartValue(m_pPointsSlider->minimum());
    pAnim->setEndValue(m_pPointsSlider->maximum());
    pAnim->setDuration(10 * 1000);
    pAnim->start(QAbstractAnimation::DeleteWhenStopped);
}
