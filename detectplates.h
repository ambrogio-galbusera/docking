#ifndef DETECTPLATES_H
#define DETECTPLATES_H

#include <QList>
#include "possibleplate.h"
#include "possiblechar.h"
#include "opencv2/opencv.hpp"

// global constants ///////////////////////////////////////////////////////////////////////////////
const double PLATE_WIDTH_PADDING_FACTOR = 1.3;
const double PLATE_HEIGHT_PADDING_FACTOR = 1.5;

class DetectPlates
{
public:
    DetectPlates();

    static std::vector<PossiblePlate> detectPlatesInScene(cv::Mat &imgOriginalScene);

private:
    static std::vector<PossibleChar> findPossibleCharsInScene(cv::Mat &imgThresh);
    static PossiblePlate extractPlate(cv::Mat &imgOriginal, std::vector<PossibleChar> &vectorOfMatchingChars);
};

#endif // DETECTPLATES_H
