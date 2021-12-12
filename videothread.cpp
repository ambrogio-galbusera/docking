#include <QDebug>
#include "videothread.h"
#include "detectplates.h"
#include "detectchars.h"
#include "possibleplate.h"
#include "globals.h"

// comparison function object
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

VideoThread::VideoThread(int camera)
    : mCameraIdx(camera)
    , mRunning(false)
    , mStopping(false)
{
}

VideoThread::~VideoThread()
{
    mStopping = true;
    while (mRunning)
        QThread::msleep(100);

    if(mVideo.isOpened())
    {
        mVideo.release();
    }
}

void VideoThread::run()
{
#if 0
    if(!mVideo.open(0))
    {
        emit error("Camera Error",
                   "Make sure you entered a correct camera index,"
                   "<br>or that the camera is not being accessed by another program!");
        return;
    }

    mRunning = true;

    cv::Mat frame;
    while(mVideo.isOpened() && (!mStopping))
    {
        mVideo >> frame;
        if(!frame.empty())
        {
            QImage qimg(frame.data,
                        frame.cols,
                        frame.rows,
                        frame.step,
                        QImage::Format_RGB888);

            mPixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );
            mPreview->fitInView(&mPixmap, Qt::KeepAspectRatio);
        }
    }

    mVideo.release();
#endif
#if 0
    mRunning = true;

    while (!mStopping)
    {
        cv::Mat img = cv::imread("/home/user/Downloads/image13.png",cv::IMREAD_COLOR);
        //cv::resize(img, img, cv::Size(200,120) );

        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        cv::Mat filtered;
        cv::bilateralFilter(gray, filtered, 13, 15, 15);

        cv::Mat edged;
        cv::Canny(filtered, edged, 30, 200) ;
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(edged, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        // sort contours by size
        std::sort(contours.begin(), contours.end(), compareContourAreas);

        // grab contours
        std::vector<cv::Point> biggestContour = contours[contours.size()-1];
        std::vector<cv::Point> smallestContour = contours[0];

        std::vector<cv::Point> plateContour;
        int contourIdx = -1;

        for (int i=contours.size()-1; i>=0; i--)
        {
            std::vector<cv::Point> contour = contours[i];

            double peri = cv::arcLength(contour, true);

            std::vector<cv::Point> approxCurve;
            cv::approxPolyDP(contour, approxCurve, 0.018 * peri, true);

            if (approxCurve.size() == 4)
            {
                plateContour = approxCurve;
                contourIdx = i;
                break;
            }
        }

        if (contourIdx != -1)
        {
            cv::Scalar colors[3];
            colors[0] = cv::Scalar(255, 0, 0);
            colors[1] = cv::Scalar(0, 255, 0);
            colors[2] = cv::Scalar(0, 0, 255);
            cv::drawContours(img, contours, contourIdx, colors[0]);
        }

        cv::Mat mask = cv::Mat::zeros(filtered.size(), CV_8U);
        cv::Mat maskedImg;
        cv::bitwise_and(img, img, maskedImg, mask);

        mImage = QImage((uchar*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
        mMaskedImage = QImage((uchar*) maskedImg.data, maskedImg.cols, maskedImg.rows, maskedImg.step, QImage::Format_RGB888);
        emit imageReady();


        QThread::sleep(1);
    }

    mRunning = false;
#endif

    mRunning = true;
    DetectChars::loadKNNDataAndTrainKNN();

    while (!mStopping)
    {
        cv::Mat img;

        if ((mCameraIdx != -1) && (!mVideo.isOpened()))
        {
            mVideo.open(mCameraIdx);
            //mVideo.set(cv::CAP_PROP_FRAME_WIDTH, 800);
            //mVideo.set(cv::CAP_PROP_FRAME_HEIGHT, 600); 
        }

        if (mVideo.isOpened())
            mVideo >> img;
        else
            img = cv::imread("image1.png",cv::IMREAD_COLOR);

        std::vector<PossiblePlate> vectorOfPossiblePlates = DetectPlates::detectPlatesInScene(img);
        vectorOfPossiblePlates = DetectChars::detectCharsInPlates(vectorOfPossiblePlates);

        cv::Point2f p2fRectPoints[4];

        /*
        //if (vectorOfPossiblePlates.size() > 0)
        for (int i=0; i<vectorOfPossiblePlates.size(); i++)
        {
            PossiblePlate plate = vectorOfPossiblePlates.at(i);

            plate.rrLocationOfPlateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect

            for (int i = 0; i < 4; i++) {                                       // draw 4 red lines
               cv::line(img, p2fRectPoints[i], p2fRectPoints[(i + 1) % 4], SCALAR_RED, 2);
            }
        }
        */

        std::sort(vectorOfPossiblePlates.begin(), vectorOfPossiblePlates.end(), PossiblePlate::sortDescendingByNumberOfChars);

        if (vectorOfPossiblePlates.size() > 0)
        {
            // suppose the plate with the most recognized chars (the first plate in sorted by string length descending order) is the actual plate
            PossiblePlate licPlate = vectorOfPossiblePlates.front();

            if (licPlate.strChars.length() == 0) {                                                      // if no chars were found in the plate
               std::cout << std::endl << "no characters were detected" << std::endl << std::endl;      // show message
               emit noPlateDetected();
            }
            else
            {
                licPlate.rrLocationOfPlateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect
                for (int i = 0; i < 4; i++) {                                       // draw 4 red lines
                    //std::cout << p2fRectPoints[i].x << ";" << p2fRectPoints[i].y << std::endl;
                    cv::line(img, p2fRectPoints[i], p2fRectPoints[(i + 1) % 4], SCALAR_BLUE, 2);
                }

                QRectF r = makeRect(p2fRectPoints);
                emit plateDetected(img.size().width-r.x()-r.width(), r.y(), r.width(), r.height());
            }
        }

        mImage = QImage((uchar*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
        emit imageReady();

        QThread::msleep(100);
    }

    mRunning = false;
}

QRectF VideoThread::makeRect(cv::Point2f* points)
{
    double top, bottom;
    double right, left;

    right = left = points[0].x;
    top = bottom = points[0].y;

    for (int i=1; i<4; i++)
    {
        if (points[i].y < top)
            top = points[i].y;
        if (points[i].y > bottom)
            bottom = points[i].y;

        if (points[i].x < left)
            left = points[i].x;
        if (points[i].x > right)
            right = points[i].x;
    }

    return QRectF(left, top, right-left, bottom-top);
}
