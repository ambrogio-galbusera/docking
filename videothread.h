#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QThread>

#include "opencv2/opencv.hpp"

class VideoThread: public QThread
{
    Q_OBJECT

public:
    VideoThread(int camera);
    virtual ~VideoThread();

    const QImage& image() { return mImage; }
    const QImage& maskedImage() { return mMaskedImage; }

signals:
    void error(QString title, QString txt);
    void imageReady();
    void plateDetected(double x, double y, double w, double h);
    void noPlateDetected();

protected:
    void run();

private:
    QRectF makeRect(cv::Point2f* points);

    int mCameraIdx;
    QPixmap mPixmap;
    cv::VideoCapture mVideo;
    QImage mImage;
    QImage mMaskedImage;
    bool mRunning;
    bool mStopping;
};

#endif // VIDEOTHREAD_H
