#ifndef SONAR_H
#define SONAR_H

#include <QObject>
#ifdef RPI
#include <memory>
#include <Common/Util.h>
#include <Navio2/ADC_Navio2.h>
#include <Navio+/ADC_Navio.h>
#endif

#define READOUTS_MAX	5

class Sonar: public QObject
{
    Q_OBJECT

public:
    Sonar();
    int distance();

private:
#ifdef RPI
    std::unique_ptr <ADC> mADC;
#endif
    int mIndex;
    float mReadouts[READOUTS_MAX];
};

#endif // SONAR_H
