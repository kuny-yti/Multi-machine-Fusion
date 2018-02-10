#ifndef RANDOM_NUMBER_H
#define RANDOM_NUMBER_H

#include <qglobal.h>
#include <QString>

class random_number
{
public:
    random_number();
    void
    init_genrand(unsigned long s);

    void
    init_by_array(unsigned long init_key[], int key_length);

    unsigned long
    genrand_int32(void);

    long
    genrand_int31(void);

    double
    genrand_real1(void);

    double
    genrand_real2(void);

    double
    genrand_real3(void);

    double
    genrand_res53(void);

    quint64 genrand_int64();
    QString genrand_str(const int len);
};

#endif // RANDOM_NUMBER_H
