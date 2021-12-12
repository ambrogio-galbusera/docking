#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QCloseEvent>
#include <QMessageBox>
#include <QPainterPath>
#include <QTimer>
#include "videothread.h"
#include "servocontroller.h"
#include "sonar.h"

#include "opencv2/opencv.hpp"

#define HAS_SONAR
#define MEASURES_MAX	20

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent* event);

private slots:
    void imageReady();
    void plateDetected(double x, double y, double w, double h);
    void noPlateDetected();
    void timeout();

private:
    typedef enum {
        Status_NoCar,
        Status_CarDetected,
        Status_Docking,
        Status_SonarDocking,
        Status_Parked,
        Status_Undocking
    } AppStatusEnum;

    void initCrossPath();
    void drawCross(QPainter& painter);
    void drawCursor(QPainter& painter, int x, int y);
    void updateHDist(int dist);
    void updateVDist(int dist);
    void updateServos(int distX, int distY);

    int avgLeft();
    int avgWidth();

    Ui::MainWindow *ui;

    QPen mCursorPen;
    QPen mCursorThinPen;
    QPen mCrossPen;
    QPen mBlackPen;
    QBrush mNormalBrush;
    QBrush mParkedBrush;
    QPainterPath mCrossPath;
    QTimer mTimer;
    VideoThread* mThread;
    ServoController mServos;
    Sonar mSonar;
    int mPlateX;
    int mPlateY;
    int mDistanceX;
    int mDistanceY;
    int mPlateWidth;
    int mPlateHeight;
    int mLefts[MEASURES_MAX];
    int mWidths[MEASURES_MAX];
    int mMeasureIndex;

    AppStatusEnum mStatus;
    int mSonarReadsCounter;
    int mDetectionRetries;
};

#endif // MAINWINDOW_H
