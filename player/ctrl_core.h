#ifndef CTRL_CORE_H
#define CTRL_CORE_H

#include <QObject>

class ctrl_core : public QObject
{
    Q_OBJECT
public:
    explicit ctrl_core(QObject *parent = 0);

signals:

public slots:
    //void open(const QString &filepath);
    //void close();


};

#endif // CTRL_CORE_H
