// PossiblePlate.h

#ifndef POSSIBLE_PLATE_H
#define POSSIBLE_PLATE_H

#include <QString>
#include "opencv2/opencv.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
class PossiblePlate {
public:
    PossiblePlate();

    // member variables ///////////////////////////////////////////////////////////////////////////
    cv::Mat imgPlate;
    cv::Mat imgGrayscale;
    cv::Mat imgThresh;

    cv::RotatedRect rrLocationOfPlateInScene;

    std::string strChars;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static bool sortDescendingByNumberOfChars(const PossiblePlate &ppLeft, const PossiblePlate &ppRight) {
        return(ppLeft.strChars.length() > ppRight.strChars.length());
    }

};

#endif
