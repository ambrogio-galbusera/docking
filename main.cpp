#include "mainwindow.h"
#include <QApplication>

int CameraIndex = -1;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc > 1)
        CameraIndex = atoi(argv[1]);

    MainWindow w;
    w.show();

    return a.exec();
}
