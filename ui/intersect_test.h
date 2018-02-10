#ifndef INTERSECT_TEST_H
#define INTERSECT_TEST_H

#include <QRectF>
#include "ui_define.h"

class intersect_test
{
private:
    bool intersect(QRectF &inout,const QRectF &in);

public:
    bool inside(const QRectF &src,const QRectF &dst);
    int intersect_dir(const QRectF &src,const QRectF &dst);
    BlendEdge intersect_h( QRectF &inout, const QVector<QRectF> in,
                            const uint src_id, const uint id);
    BlendEdge intersect_v( QRectF &inout, const QVector<QRectF>  in,
                            const uint src_id, const uint id);

    BlendEdge intersect(const int i, const int j,
                        const QVector<QVector<QRectF> > &in);
    QVector<QVector<BlendEdge> > intersect(const QVector<QVector<QRectF> > &in);
};
#endif // INTERSECT_TEST_H
