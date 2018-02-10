#include "client_ui.h"
#include <QApplication>
#include "message.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    message log;
    log.object()->print_level(message::LOG_INFO);
    log.object()->print_debug(true);
    client_ui w;
    w.show();

    return a.exec();
}
