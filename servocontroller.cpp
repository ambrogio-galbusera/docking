#include "servocontroller.h"

#define SERVO_MIN  600 /*mS*/
#define SERVO_MAX 2400 /*mS*/

ServoController::ServoController()
{
    initServos();
    mContPos = 0;
}

#ifdef RPI
std::unique_ptr <RCOutput> get_rcout()
{
    if (get_navio_version() == NAVIO2)
    {
        auto ptr = std::unique_ptr <RCOutput>{ new RCOutput_Navio2() };
        return ptr;
    } else
    {
        auto ptr = std::unique_ptr <RCOutput>{ new RCOutput_Navio() };
        return ptr;
    }
}
#endif

void ServoController::initServos()
{
#ifdef RPI
    mPWM = get_rcout();

    if (check_apm()) {
        return;
    }
#endif

    for (int i=0; i<2; i++)
    {
        mServoStatus[i].channel = i;
        mServoStatus[i].target = 0;
        mServoStatus[i].direction = 0;
        mServoStatus[i].current = 0;
        mServoStatus[i].step = 0;
        mServoStatus[i].contStart = 0;
        mServoStatus[i].contEnd = 0;
        mServoStatus[i].continous = false;

        int ch = mServoStatus[i].channel;

#ifdef RPI
        printf("Initializing channel %d\n", ch);
        if( !(mPWM->initialize(ch)) ) {
            fprintf(stderr, "Unable to initialize servo %d\n", i);
        }

        printf("Setting frequency\n");
        mPWM->set_frequency(ch, 50);

        printf("Enabling channel\n");
        if ( !(mPWM->enable(ch)) ) {
            fprintf(stderr, "Unable to enable servo %d\n", i);
        }
#endif
    }

    connect(&mServoTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    mServoTimer.start(1000);
}

void ServoController::moveServo(int i, int angle, int durationMs)
{
    //printf("<%d> Move servo %d, %d\n", i, angle, durationMs);

    int target = angleToPWM(angle);
    mServoStatus[i].target = target;
    mServoStatus[i].continous = false;

    if (mServoStatus[i].current < target)
    {
        mServoStatus[i].step = (target - mServoStatus[i].current) / qMax((durationMs / mServoTimer.interval()), 1);
        mServoStatus[i].direction = 1;
    }
    else
    {
        mServoStatus[i].step = (mServoStatus[i].current - target) / qMax((durationMs / mServoTimer.interval()), 1);
        mServoStatus[i].direction = -1;
    }
}

void ServoController::moveServo(int i, int start, int end, int durationMs, bool restart)
{
    //printf("<%d> Move servo %d -> %d, duration %d, restart %d\n", i, start, end, durationMs, restart);

    mServoStatus[i].contStart = angleToPWM(start);
    mServoStatus[i].contEnd = angleToPWM(end);

    if (!restart && mServoStatus[i].continous)
        return;

    mServoStatus[i].continous = true;

    if (mServoStatus[i].contStart < mServoStatus[i].contEnd)
    {
        mServoStatus[i].step = (mServoStatus[i].contEnd - mServoStatus[i].contStart); // / qMax((durationMs / mServoTimer.interval()), 1);
        mServoStatus[i].direction = 1;
    }
    else
    {
        mServoStatus[i].step = (mServoStatus[i].contStart - mServoStatus[i].contEnd); // / qMax((durationMs / mServoTimer.interval()), 1);
        mServoStatus[i].direction = -1;
    }

    mServoStatus[i].target = mServoStatus[i].contStart;
}

int ServoController::angleToPWM(int angle)
{
    int pwm = SERVO_MIN + (angle * (SERVO_MAX-SERVO_MIN) / SERVO_MAX_ANGLE);
    //printf("Angle %d -> PWN %d\n", angle, pwm);
    return pwm;
}

void ServoController::timeout()
{
    for (int i=0; i<2; i++)
    {
        /*
        printf("<%d> current: %d, target: %d, cont: %d, direction: %d, contStart: %d, contEnd: %d, step: %d\n",
               i,
               mServoStatus[i].current,
               mServoStatus[i].target,
               mServoStatus[i].continous,
               mServoStatus[i].direction,
               mServoStatus[i].contStart,
               mServoStatus[i].contEnd,
               mServoStatus[i].step);
        */
 
        if (mServoStatus[i].current != mServoStatus[i].target)
        {
            mServoStatus[i].current += (mServoStatus[i].direction * mServoStatus[i].step);

            if (mServoStatus[i].direction < 0)
            {
                if (mServoStatus[i].current < mServoStatus[i].target)
                    mServoStatus[i].current = mServoStatus[i].target;
            }

            if (mServoStatus[i].direction > 0)
            {
                if (mServoStatus[i].current > mServoStatus[i].target)
                    mServoStatus[i].current = mServoStatus[i].target;
            }

#ifdef RPI
            //printf("Channel %d: setting DC to %d\n", mServoStatus[i].channel, mServoStatus[i].current);
            mPWM->set_duty_cycle(mServoStatus[i].channel, mServoStatus[i].current);
#endif
        }

        if (mServoStatus[i].current == mServoStatus[i].target)
        {
            emit servoPositioned(i);

            if (mServoStatus[i].continous)
            {
                if (mContPos == 0)
                    mServoStatus[i].target = mServoStatus[i].contEnd;
                else
                    mServoStatus[i].target = mServoStatus[i].contStart;

                mServoStatus[i].direction = (mServoStatus[i].current < mServoStatus[i].target)? +1 : -1;
            }
        }
    }

    mContPos = (mContPos + 1) % 2;
}

