#include "mainwindow.h"
#include "ui_mainwindow.h"

#define WIDTH           800
#define HEIGHT          480
#define ERROR_X_MM      100

#define CROSS_TOP       10
#define CROSS_LEG_W     10
#define CROSS_LEG_H     50
#define CROSS_ARC_W     CROSS_LEG_W

#define PLATE_WIDTH_MM      360.0
#define PLATE_HEIGHT_MM     110.0
#define PLATE_RATIO         (PLATE_WIDTH_MM / PLATE_HEIGHT_MM)
#define PLATE_DISTANCE_MM   1000
#define PLATE_WIDTH_PIXELS  200 /* pixels of plate when placed at 1m */
#define DISTANCE_MAX_MM     600.0
#define DISTANCE_MIN_MM     50.0
#define DISTANCE_HYST_MM    50.0

extern int CameraIndex;

const QString STATUS_TEXTS[] =
{
    "No Car",
    "Car Detected",
    "Docking",
    "Precision docking",
    "Parked",
    "Undocking"
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mPlateX(0),
    mPlateY(0),
    mDistanceX(0),
    mDistanceY(0),
    mPlateWidth(0),
    mPlateHeight(0),
    mStatus(Status_NoCar),
    mSonarReadsCounter(0),
    mDetectionRetries(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);

    ui->lblHDist->setStyleSheet("QLabel { color : #00ff21; }");
    ui->lblVDist->setStyleSheet("QLabel { color : #00ff21; }");

    initCrossPath();

    mCursorPen = QPen(QColor(00, 0xff, 0x21), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    mCursorThinPen = QPen(QColor(00, 0xff, 0x21), 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    mCrossPen = QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    mBlackPen = QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    mNormalBrush = QBrush(QColor(0x40, 0x40, 0x40));
    mParkedBrush = QBrush(QColor(0xff, 0x51, 0x44));

    mMeasureIndex = 0;
    for (int i=0; i<MEASURES_MAX; i++)
    {
        mLefts[i] = WIDTH/2;
        mWidths[i] = 0;
    }

    mThread = new VideoThread(CameraIndex);
    connect(mThread, SIGNAL(imageReady()), this, SLOT(imageReady()), Qt::QueuedConnection);
    connect(mThread, SIGNAL(plateDetected(double, double, double, double)), this, SLOT(plateDetected(double, double, double, double)), Qt::QueuedConnection);
    connect(mThread, SIGNAL(noPlateDetected()), this, SLOT(noPlateDetected()), Qt::QueuedConnection);

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeout()));

    mThread->start();

#ifdef HAS_SONAR
    mTimer.start(5);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::imageReady()
{
    repaint(600, 360, 200, 120);
}

void MainWindow::noPlateDetected()
{
    mDetectionRetries ++;
    if (mDetectionRetries >= 3)
    {
        mStatus = Status_NoCar;
        repaint();
    }
}

void MainWindow::plateDetected(double x, double y, double w, double h)
{
    mDetectionRetries = 0;

    double imageWidth = mThread->image().size().width();
    mLefts[mMeasureIndex] = x;
    mWidths[mMeasureIndex] = w;
    mMeasureIndex = (mMeasureIndex + 1) % MEASURES_MAX;

    mPlateX = ((avgLeft() + avgWidth()/2) * WIDTH) / imageWidth;

    if (mStatus == Status_NoCar)
        mStatus = Status_Docking;

    if (mStatus == Status_Docking)
    {
        double f = (PLATE_WIDTH_PIXELS * PLATE_DISTANCE_MM) / PLATE_WIDTH_MM;
        mDistanceY = (PLATE_WIDTH_MM * f) / w;
        mPlateY = CROSS_TOP + ((double)mDistanceY / DISTANCE_MAX_MM) * HEIGHT;
    }

    double centerX = avgLeft() + avgWidth()/2;
    double deltaX = centerX - imageWidth/2;
    mDistanceX  = (deltaX * PLATE_WIDTH_MM) / w;

    mPlateWidth = w;
    mPlateHeight = h;

    repaint();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    delete mThread;
    event->accept();
}

void MainWindow::initCrossPath()
{
#if 0
    /* 0 */ mCrossPath.moveTo(400.0-CROSS_LEG_W/2, CROSS_TOP+CROSS_ARC_W);
    /* 1 */ mCrossPath.arcTo(400.0-CROSS_LEG_W/2, CROSS_TOP, CROSS_ARC_W, CROSS_ARC_W, 180.0, -180.0);
    /* 2 */ mCrossPath.lineTo(400.0+CROSS_LEG_W/2, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H-CROSS_LEG_W/2));

    /* 3 */ mCrossPath.lineTo(400.0+CROSS_LEG_H, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H-CROSS_LEG_W/2));
    /* 4 */ mCrossPath.arcTo(400.0+CROSS_LEG_H, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H-CROSS_LEG_W/2), CROSS_ARC_W, CROSS_ARC_W, 90.0, -180.0);
    /* 5 */ mCrossPath.lineTo(400.0+CROSS_LEG_W/2, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H+CROSS_LEG_W/2));

    /* 6 */ mCrossPath.lineTo(400.0+CROSS_LEG_W/2, CROSS_TOP+(CROSS_ARC_W+2*CROSS_LEG_H));
    /* 7 */ mCrossPath.arcTo(400.0-CROSS_LEG_W/2, CROSS_TOP+(CROSS_ARC_W+2*CROSS_LEG_H), CROSS_ARC_W, CROSS_ARC_W, 0.0, -180.0);
    /* 8 */ mCrossPath.lineTo(400.0-CROSS_LEG_W/2, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H+CROSS_LEG_W/2));

    /* 9 */ mCrossPath.lineTo(400.0-CROSS_LEG_H, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H+CROSS_LEG_W/2));
    /*10 */ mCrossPath.arcTo(400.0-CROSS_LEG_H-CROSS_ARC_W, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H-CROSS_LEG_W/2), CROSS_ARC_W, CROSS_ARC_W, 270.0, -180.0);
    /*11 */ mCrossPath.lineTo(400.0-CROSS_LEG_W/2, CROSS_TOP+(CROSS_ARC_W+CROSS_LEG_H-CROSS_LEG_W/2));

    mCrossPath.closeSubpath();
#endif

    /* 0 */ mCrossPath.moveTo(400.0-CROSS_LEG_H, CROSS_TOP-(CROSS_LEG_W/2));
    /* 1 */ mCrossPath.lineTo(400.0+CROSS_LEG_H, CROSS_TOP-(CROSS_LEG_W/2));
    /* 2 */ mCrossPath.arcTo(400.0+CROSS_LEG_H, CROSS_TOP-(CROSS_LEG_W/2), CROSS_ARC_W, CROSS_ARC_W, 90.0, -180.0);
    /* 3 */ mCrossPath.lineTo(400.0+CROSS_LEG_W/2, CROSS_TOP+(CROSS_LEG_W/2));

    /* 4 */ mCrossPath.lineTo(400.0+CROSS_LEG_W/2, CROSS_TOP+CROSS_LEG_H);
    /* 5 */ mCrossPath.arcTo(400.0-CROSS_LEG_W/2, CROSS_TOP+CROSS_LEG_H, CROSS_ARC_W, CROSS_ARC_W, 0.0, -180.0);
    /* 6 */ mCrossPath.lineTo(400.0-CROSS_LEG_W/2, CROSS_TOP+CROSS_LEG_W/2);

    /* 7 */ mCrossPath.lineTo(400.0-CROSS_LEG_H, CROSS_TOP+CROSS_LEG_W/2);
    /* 8 */ mCrossPath.arcTo(400.0-CROSS_LEG_H-CROSS_ARC_W, CROSS_TOP-CROSS_LEG_W/2, CROSS_ARC_W, CROSS_ARC_W, 270.0, -180.0);

    mCrossPath.closeSubpath();
}

void MainWindow::drawCross(QPainter& painter)
{
    painter.setPen(mCrossPen);
    painter.drawPath(mCrossPath);
}

void MainWindow::drawCursor(QPainter& painter, int x, int y)
{
    painter.setPen(mCursorPen);
    painter.drawLine(x, 0, x, HEIGHT);

    painter.setPen(mCursorThinPen);

    int l = HEIGHT / 11;
    int ty = l;
    for (int i=1; i<=10; i++)
    {
        painter.drawLine(x-5, ty, x+5, ty);
        ty += l;
    }

    painter.setPen(mCursorPen);
    painter.drawLine(0, y, 800, y);

    painter.setPen(mCursorThinPen);

    l = WIDTH / 11;
    int tx = l;
    for (int i=1; i<=10; i++)
    {
        painter.drawLine(tx, y-5, tx, y+5);
        tx += l;
    }
}

void MainWindow::updateHDist(int dist)
{
    double x = qAbs(dist / 1000.0);
    if (dist < -ERROR_X_MM)
    {
        ui->lblHDist->setText(QString("%1 >").arg(x, 3, 'f', 2));
    }
    else if (dist > ERROR_X_MM)
    {
        ui->lblHDist->setText(QString("< %1").arg(x, 3, 'f', 2));
    }
    else
    {
        ui->lblHDist->setText(QString("%1").arg(x, 3, 'f', 2));
    }
}

void MainWindow::updateVDist(int dist)
{
    double x = (double)dist / 1000.0;
    ui->lblVDist->setText(QString("%1").arg(x, 3, 'f', 2));
}

void MainWindow::updateServos(int distX, int distY)
{
    if (mStatus == Status_Parked)
    {
        mServos.moveServo(SERVO_LEFT, SERVO_MAX_ANGLE-0, 0);
        mServos.moveServo(SERVO_RIGHT, SERVO_MIN_ANGLE+0, 0);
    }
    else if (mStatus == Status_Undocking)
    {
        mServos.moveServo(SERVO_LEFT, SERVO_MAX_ANGLE-0, SERVO_MAX_ANGLE-30, 0);
        mServos.moveServo(SERVO_RIGHT, SERVO_MIN_ANGLE+0, SERVO_MIN_ANGLE+30, 0);
    }
    else if ((mStatus != Status_Docking) && (mStatus != Status_SonarDocking))
    {
        mServos.moveServo(SERVO_LEFT, SERVO_MAX_ANGLE-110, 0);
        mServos.moveServo(SERVO_RIGHT, SERVO_MIN_ANGLE+110, 0);
    }
    else
    {
        if (distX < -ERROR_X_MM)
        {
            mServos.moveServo(SERVO_RIGHT, SERVO_MIN_ANGLE+90, 0);
            mServos.moveServo(SERVO_LEFT, SERVO_MAX_ANGLE-20, SERVO_MAX_ANGLE-100, 1000);
        }
        else if (distX > ERROR_X_MM)
        {
            mServos.moveServo(SERVO_LEFT, SERVO_MAX_ANGLE-90, 0);
            mServos.moveServo(SERVO_RIGHT, SERVO_MIN_ANGLE+20, SERVO_MIN_ANGLE+100, 1000);
        }
        else
        {
            mServos.moveServo(SERVO_LEFT, SERVO_MAX_ANGLE-20, SERVO_MAX_ANGLE-100, 1000, false);
            mServos.moveServo(SERVO_RIGHT, SERVO_MIN_ANGLE+20, SERVO_MIN_ANGLE+100, 1000, false);
        }
    }
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(0,0,WIDTH, HEIGHT, mStatus == Status_Parked? mParkedBrush : mNormalBrush);

    drawCross(painter);
    if (mStatus != Status_NoCar)
    {
        drawCursor(painter, mPlateX, mPlateY);

        updateHDist(mDistanceX);
        updateVDist(mDistanceY);
    }

    updateServos(mDistanceX, mDistanceY);

    painter.drawImage(QRect(600, 360, 200, 120), mThread->image());

    painter.setPen(mBlackPen);
    painter.drawText(600, 355, QString("%1 x %2").arg(mPlateWidth).arg(mPlateHeight));
    painter.drawText(700, 355, QString("%1").arg(STATUS_TEXTS[mStatus]));
}

void MainWindow::timeout()
{
    float d = mSonar.distance();

    if (mStatus == Status_Docking)
    {
        if (d < DISTANCE_MAX_MM)
        {
            mStatus = Status_SonarDocking;
            mDistanceY = d;
            mPlateY = CROSS_TOP + ((double)mDistanceY / DISTANCE_MAX_MM) * HEIGHT;
        }
    }
    else if (mStatus == Status_SonarDocking)
    {
        mDistanceY = d;
        mPlateY = CROSS_TOP + ((double)mDistanceY / DISTANCE_MAX_MM) * HEIGHT;
        if (d < DISTANCE_MIN_MM)
            mStatus = Status_Parked;
    }
    else if (mStatus == Status_Parked)
    {
        mDistanceY = d;
        mPlateY = CROSS_TOP + ((double)mDistanceY / DISTANCE_MAX_MM) * HEIGHT;

        if (d > DISTANCE_MIN_MM+DISTANCE_HYST_MM)
            mStatus = Status_Undocking;
    }

    // update every 100 ms
    mSonarReadsCounter = (mSonarReadsCounter + 1) % (20 / mTimer.interval());

    if (mSonarReadsCounter == 0)
    {
        if (mStatus == Status_Docking)
            printf("Distance: %f mm\n", d);
        repaint();
    }
}

int MainWindow::avgLeft()
{
    int avg = 0;
    for (int i=0; i<MEASURES_MAX; i++)
        avg += mLefts[i];

    return avg / MEASURES_MAX;
}

int MainWindow::avgWidth()
{
    int avg = 0;
    for (int i=0; i<MEASURES_MAX; i++)
        avg += mWidths[i];

    return avg / MEASURES_MAX;
}
