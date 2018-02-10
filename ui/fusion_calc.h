#ifndef FUSION_ADJUST_H
#define FUSION_ADJUST_H
#include "type_define.h"

class fusion_calc
{
public:
    bool intersect(QRectF &inout,const QRectF &in);
    bool inside(const QRectF &src,const QRectF &dst);
    FusionDir intersect_dir(const QRectF &src,const QRectF &dst);
    EdgeFusion intersect_h( QRectF &inout, const arrayRectF in,
                            const uint src_id, const uint id);
    EdgeFusion intersect_v( QRectF &inout, const arrayRectF in,
                            const uint src_id, const uint id);
    EdgeFusion intersect(const int i, const int j,
                          const arrayRectFtwo in);
    arrayEdgeTwo intersect(const arrayRectFtwo in);
};

#endif // FUSION_ADJUST_H
