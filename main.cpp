
#include <QApplication>
#include "main_dispatch.h"
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ctrl_core control_core;
    main_dispatch *core = new main_dispatch(&control_core);
    core->start();
    a.exec();
    delete core;
    return 0;
}
