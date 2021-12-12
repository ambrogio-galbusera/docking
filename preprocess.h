// Preprocess.h

#ifndef PREPROCESS_H
#define PREPROCESS_H

#include "opencv.hpp"

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Size GAUSSIAN_SMOOTH_FILTER_SIZE = cv::Size(5, 5);
const int ADAPTIVE_THRESH_BLOCK_SIZE = 7; //19;
const int ADAPTIVE_THRESH_WEIGHT = 9;

// function prototypes ////////////////////////////////////////////////////////////////////////////
class Preprocess
{
public:
    static void preprocess(cv::Mat &imgOriginal, cv::Mat &imgGrayscale, cv::Mat &imgThresh);
    static cv::Mat extractValue(cv::Mat &imgOriginal);
    static cv::Mat maximizeContrast(cv::Mat &imgGrayscale);
};

#endif	// PREPROCESS_H
