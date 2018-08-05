#include"enterthreedwidget.h"

#include<QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    EnterThreeDWidget w;
    w.resize(400,300);
    w.show();

    return app.exec();
}
