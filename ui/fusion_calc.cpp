#include "fusion_calc.h"


///
/// \brief intersect
/// \param inout 源矩形，当相交时输出相交的位置
/// \param in   目标矩形
/// \return true 发生相交，否则未相交
///
bool fusion_calc::intersect(QRectF &inout,const QRectF &in)
{
    QRectF src = inout;

    if(src != in)///是否相同
    {
        if (src.intersects(in))///是否相交
        {
            inout = src.intersected(in);///取相交区域
            return true;
        }
    }
    return false;
}

///
/// \brief inside
/// \param src 源矩形
/// \param dst 目标矩形
/// \return  当目标矩形在源矩形内部时返回true,否则返回false
///
bool fusion_calc::inside(const QRectF &src,const QRectF &dst)
{
    if (src.contains (dst))
    {
        return true;
    }
    return false;
}

///
/// \brief intersect_dir
/// \param src 源矩形
/// \param dst 目标矩形(相交的矩形)
/// \return 返回目标矩形和那边相交
///
FusionDir fusion_calc::intersect_dir(const QRectF &src,const QRectF &dst)
{
    bool isOK = false;
    FusionDir dir = FUSION_NO;
    QPointF center_s = src.center ();
    QPointF center_d = dst.center ();

    if (((center_s.y ()+0.005) > center_d.y ()) &&
        ((center_s.y ()-0.005) < center_d.y ())
            && !isOK)
    {
        if (center_s.x () < center_d.x ())//right
        {
            isOK = true;
            dir = FUSION_RIGHT;
        }
        else if (center_s.x () > center_d.x ())//left
        {
            isOK = true;
            dir = FUSION_LEFT;
        }
    }
    if (((center_s.x ()+0.005) > center_d.x ()) &&
        ((center_s.x ()-0.005) < center_d.x ())
            && !isOK)
    {
        if (center_s.y () < center_d.y ())//bottom
        {
            isOK = true;
            dir = FUSION_BOTTOM;
        }
        else if (center_s.y () > center_d.y ())//top
        {
            isOK = true;
            dir = FUSION_TOP;
        }
    }
    /*if ((center_s.x () < center_d.x ()) && !isOK)
    {
        if (center_s.y () < center_d.y ())//right bottom
        {
            isOK = true;
            dir = FUSION_BOTTOM_RIGHT;
        }
        else if (center_s.y () > center_d.y ())// right top
        {
            isOK = true;
            dir = FUSION_TOP_RIGHT;
        }
    }
    if ((center_s.x () > center_d.x ()) && !isOK)
    {
        if (center_s.y () < center_d.y ())//left bottom
        {
            isOK = true;
            dir = FUSION_BOTTOM_LEFT;
        }
        else if (center_s.y () > center_d.y ())// left top
        {
            isOK = true;
            dir = FUSION_TOP_LEFT;
        }
    }
    if ((center_s.y () < center_d.y ()) && !isOK)
    {
        if (center_s.x () > center_d.x ())//bottom left
        {
            isOK = true;
            dir = FUSION_BOTTOM_LEFT;
        }
        else if (center_s.x () < center_d.x ()) //bottom right
        {
            isOK = true;
            dir = FUSION_BOTTOM_RIGHT;
        }
    }
    if ((center_s.y () > center_d.y ()) && !isOK)
    {
        if (center_s.x () > center_d.x ())//top left
        {
            isOK = true;
            dir = FUSION_TOP_LEFT;
        }
        else if (center_s.x () < center_d.x ()) //top right
        {
            isOK = true;
            dir = FUSION_TOP_RIGHT;
        }
    }*/
    return dir;
}

EdgeFusion fusion_calc::intersect_h( QRectF &inout, const arrayRectF in, const uint src_id, const uint id)
{
    EdgeFusion edge;
    memset(&edge, 0, sizeof(EdgeFusion));
    for (int i = 0; i < in.size (); i++)
    {
        QRectF out = inout;
        if (intersect(out, in[i]))///矩形是否相交
        {
            FusionArea fus;
            memset(&fus, 0, sizeof(FusionArea));
            fus.id = (id << 8)&0xff00;
            fus.id |= i&0xff;//相交的id
            fus.area = out;//相交区域

            fus.ratio = QSizeF(out.width() / inout.width(),
                               out.height() / inout.height());//相交区域所占比例
            FusionDir dir = intersect_dir(inout, out);//相交的方向
            edge.dir = (FusionDir)(edge.dir | dir);
            if (dir)
            {
                edge.id = src_id;//当前的源id
                edge.coord = inout;//当前的源区域
                switch (dir)
                {
                    case FUSION_LEFT:
                    if ((edge.edge[0].id > fus.id) || (edge.edge[0].id == 0))
                        //当左边已经存储了相交信息则进行对先后顺序的过滤
                    {
                        edge.edge[0] = fus;
                    }
                    break;
                    case FUSION_RIGHT:
                    if ((edge.edge[1].id < fus.id) || (edge.edge[1].id == 0))
                    {
                        edge.edge[1] = fus;
                    }
                    break;
                    default:
                    break;
                }
            }
        }
    }
    return edge;
}

EdgeFusion fusion_calc::intersect_v( QRectF &inout, const arrayRectF in, const uint src_id, const uint id)
{
    EdgeFusion edge;
    memset(&edge, 0, sizeof(EdgeFusion));

    for (int i = 0; i < in.size (); i++)
    {
        QRectF out = inout;
        if (intersect(out, in[i]))///矩形是否相交
        {
            FusionArea fus;
            memset(&fus, 0, sizeof(FusionArea));
            fus.id = (i << 8)&0xff00;
            fus.id |= id&0xff;//相交的id
            fus.area = out;//相交区域

            fus.ratio = QSizeF(out.width() / inout.width(),
                               out.height() / inout.height());//相交区域所占比例
            FusionDir dir = intersect_dir(inout, out);//相交的方向
            edge.dir =(FusionDir)(edge.dir | dir);
            if (dir)
            {
                edge.id = src_id;//当前的源id
                edge.coord = inout;//当前的源区域
                switch (dir)
                {
                    case FUSION_TOP:
                    if ((edge.edge[2].id > fus.id) || (edge.edge[2].id == 0))
                    {
                       edge.edge[2] = fus;
                    }
                    break;
                    case FUSION_BOTTOM:
                    if ((edge.edge[3].id < fus.id) || (edge.edge[3].id == 0))
                    {
                        edge.edge[3] = fus;
                    }
                    break;
                    default:
                    break;
                }
            }
        }
    }
    return edge;
}

///检测矩形相交位置并计算融合信息

EdgeFusion fusion_calc::intersect(const int i, const int j, const arrayRectFtwo in)
{
    EdgeFusion tem;
    memset(&tem, 0, sizeof(EdgeFusion));

    EdgeFusion h, v;
    QRectF input = in[i][j];//当前要计算的矩形
    arrayRectF h_input = in[i];//取横向全部矩形

    uint src_id = (i << 8) &0xff00;//计算源矩形id
    src_id |=  j&0xff;

    h = intersect_h(input, h_input, src_id, i);//取横向计算

    arrayRectF vol;
    for (int k = 0; k < in.size (); k++)//取出当前矩形所在的列
    {
        for (int a = 0; a < in[k].size (); a++)
        {
            if (a == j)
            {
                vol << in[k][a];
            }
        }
    }
    input = in[i][j];//当前要计算的矩形
    v = intersect_v(input, vol, src_id, j);//取横向

    if (h.id == src_id)//若取得的id和当前的id相同则将相交数据保存
    {
        if ((h.dir & FUSION_LEFT) != FUSION_NO)
        {
            tem.id = h.id;
            tem.dir = (FusionDir)(tem.dir | h.dir);
            tem.coord = h.coord;
            tem.edge[0] = h.edge[0];
        }
        if ((h.dir & FUSION_RIGHT) != FUSION_NO)
        {
            tem.id = h.id;
            tem.dir = (FusionDir)(tem.dir | h.dir);
            tem.coord = h.coord;
            tem.edge[1] = h.edge[1];
        }
    }
    if (v.id == src_id)
    {

        if ((v.dir & FUSION_TOP) != FUSION_NO)
        {
            tem.id = v.id;
            tem.dir = (FusionDir)(tem.dir | v.dir);
            tem.coord = v.coord;
            tem.edge[2] = v.edge[2];
        }
        if ((v.dir & FUSION_BOTTOM) != FUSION_NO)
        {
            tem.id = v.id;
            tem.dir = (FusionDir)(tem.dir | v.dir);
            tem.coord = v.coord;
            tem.edge[3] = v.edge[3];
        }
    }
    return tem;
}


///根据矩形位置计算融合方向，融合尺寸，纹理坐标
arrayEdgeTwo fusion_calc::intersect(const arrayRectFtwo in)
{
    //! 本函数的思路
    //! 1.循环容器对数据进行取出，和所有矩形进行相交检测
    //!
    arrayEdgeTwo arr;
    for (int i = 0; i < in.size (); i++)
    {
        arrayEdge a;
        for (int j = 0; j < in[i].size (); j++)
        {
            a  << intersect(i, j, in);
        }
        arr << a;
    }
    return arr;
}

