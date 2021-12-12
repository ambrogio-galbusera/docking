#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include <QTimer>

#ifdef RPI
#include <memory>
#include <Navio2/PWM.h>
#include <Navio+/RCOutput_Navio.h>
#include <Navio2/RCOutput_Navio2.h>
#include <Common/Util.h>
#endif

#define SERVO_LEFT  0
#define SERVO_RIGHT 1
#define SERVO_MIN_ANGLE   0 /*grad*/
#define SERVO_MAX_ANGLE 180 /*grad*/

class ServoController: public QObject
{
    Q_OBJECT

public:
    ServoController();

    void moveServo(int i, int angle, int durationMs);
    void moveServo(int i, int start, int end, int durationMs, bool restart=false);

signals:
    void servoPositioned(int i);

private slots:
    void timeout();

private:
    void initServos();
    int angleToPWM(int angle);

    struct ServoStatus {
        int channel;
        int target;
        int current;
        int step;
        int direction;

        int contStart;
        int contEnd;
        bool continous;
    };

    int mContPos;
    ServoStatus mServoStatus[2];
    QTimer mServoTimer;
#ifdef RPI
    std::unique_ptr <RCOutput> mPWM;
#endif
};

#endif // SERVOCONTROLLER_H
