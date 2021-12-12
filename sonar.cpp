#include "sonar.h"

#define SONAR_ADC_VMAX      3300.0
#define SONAR_ADC_OFFSET    50.0
#define SONAR_K             (1.0 / 10.0) /* 1.0 mV / 10 mm */
#define SONAR_MAX_RANGE_MM  5000.0


#ifdef RPI
std::unique_ptr <ADC> get_converter()
{
    if (get_navio_version() == NAVIO2)
    {
        auto ptr = std::unique_ptr <ADC>{ new ADC_Navio2() };
        return ptr;
    } else
    {
        auto ptr = std::unique_ptr <ADC>{ new ADC_Navio() };
        return ptr;
    }

}
#endif

Sonar::Sonar()
{
#ifdef RPI
    mADC = get_converter();
#endif
  
   for (int i=0; i<READOUTS_MAX; i++)
       mReadouts[i] = SONAR_ADC_VMAX;

   mIndex = 0;
}

int Sonar::distance()
{
    float adc = 0;
#ifdef RPI
    adc = mADC->read(2);
    //printf("ADC reading: %f\n", adc);
#endif
    
    mReadouts[mIndex] = adc;
    mIndex = (mIndex + 1) % READOUTS_MAX;

    float avg = 0;
    for (int i=0; i<READOUTS_MAX; i++)
        avg += mReadouts[i];

    avg = avg / READOUTS_MAX;

    avg = avg - SONAR_ADC_OFFSET;
    if (avg < 0) avg = 0;

    int dist = (int)(avg / SONAR_K );
    printf("ADC: %f, Distance: %d\n", avg, dist);

    return dist;
}
