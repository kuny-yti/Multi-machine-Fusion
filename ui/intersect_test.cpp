#include "intersect_test.h"
#include <QVector>

///
/// \brief intersect
/// \param inout 源矩形，当相交时输出相交的位置
/// \param in   目标矩形
/// \return true 发生相交，否则未相交
///
bool intersect_test::intersect(QRectF &inout, const QRectF &in)
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
bool intersect_test::inside(const QRectF &src,const QRectF &dst)
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
int intersect_test::intersect_dir(const QRectF &src,const QRectF &dst)
{
    bool isOK = false;
    int dir = 0;
    QPointF center_s = src.center ();
    QPointF center_d = dst.center ();

    if (((center_s.y ()+0.005) > center_d.y ()) &&
        ((center_s.y ()-0.005) < center_d.y ())
            && !isOK)
    {
        if (center_s.x () < center_d.x ())//right
        {
            isOK = true;
            dir = 1 << 1;
        }
        else if (center_s.x () > center_d.x ())//left
        {
            isOK = true;
            dir = 1;
        }
    }
    if (((center_s.x ()+0.005) > center_d.x ()) &&
        ((center_s.x ()-0.005) < center_d.x ())
            && !isOK)
    {
        if (center_s.y () < center_d.y ())//bottom
        {
            isOK = true;
            dir = 1 << 3;
        }
        else if (center_s.y () > center_d.y ())//top
        {
            isOK = true;
            dir = 1 << 2;
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

BlendEdge intersect_test::intersect_h( QRectF &inout, const QVector<QRectF> in,
                                        const uint src_id, const uint id)
{
    BlendEdge edge;
    memset(&edge, 0, sizeof(BlendEdge));
    for (int i = 0; i < in.size (); i++)
    {
        QRectF out = inout;
        if (intersect(out, in[i]))///矩形是否相交
        {
            BlendArea fus;
            memset(&fus, 0, sizeof(BlendArea));
            fus.device_id =  idTools(id, i);//相交的id
            fus.area[0] = out.x();//相交区域
            fus.area[1] = out.y();//相交区域
            fus.area[2] = out.width();//相交区域
            fus.area[3] = out.height();//相交区域

            fus.ratio[0] = out.width() / inout.width();
            fus.ratio[1] = out.height() / inout.height();//相交区域所占比例

            int dir = intersect_dir(inout, out);//相交的方向
            edge.dir |=  dir;
            if (dir)
            {
                edge.device_id = src_id;//当前的源id
                edge.coord[0] = inout.x();//当前的源区域
                edge.coord[1] = inout.y();//当前的源区域
                edge.coord[2] = inout.width();//当前的源区域
                edge.coord[3] = inout.height();//当前的源区域

                switch (dir)
                {
                    case Blend_Left:
                    if ((edge.edge[0].device_id > fus.device_id) ||
                            (edge.edge[0].device_id == 0))
                        //当左边已经存储了相交信息则进行对先后顺序的过滤
                    {
                        edge.edge[0] = fus;
                    }
                    break;
                    case Blend_Right:
                    if ((edge.edge[1].device_id < fus.device_id) ||
                            (edge.edge[1].device_id == 0))
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

BlendEdge intersect_test::intersect_v(QRectF &inout, const QVector<QRectF> in,
                                       const uint src_id, const uint id)
{
    BlendEdge edge;
    memset(&edge, 0, sizeof(BlendEdge));

    for (int i = 0; i < in.size (); i++)
    {
        QRectF out = inout;
        if (intersect(out, in[i]))///矩形是否相交
        {
            BlendArea fus;
            memset(&fus, 0, sizeof(BlendArea));
            fus.device_id = idTools(i, id);
            fus.area[0] = out.x();//相交区域
            fus.area[1] = out.y();//相交区域
            fus.area[2] = out.width();//相交区域
            fus.area[3] = out.height();//相交区域

            fus.ratio[0] = out.width() / inout.width();
            fus.ratio[1] = out.height() / inout.height();//相交区域所占比例

            int dir = intersect_dir(inout, out);//相交的方向
            edge.dir |= dir;
            if (dir)
            {
                edge.device_id = src_id;//当前的源id
                edge.coord[0] = inout.x();//当前的源区域
                edge.coord[1] = inout.y();//当前的源区域
                edge.coord[2] = inout.width();//当前的源区域
                edge.coord[3] = inout.height();//当前的源区域

                switch (dir)
                {
                    case Blend_Top:
                    if ((edge.edge[2].device_id > fus.device_id) ||
                            (edge.edge[2].device_id == 0))
                    {
                       edge.edge[2] = fus;
                    }
                    break;
                    case Blend_Bottom:
                    if ((edge.edge[3].device_id < fus.device_id) ||
                            (edge.edge[3].device_id == 0))
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

BlendEdge intersect_test::intersect(const int i, const int j,
                                    const QVector<QVector<QRectF> > &in)
{
    BlendEdge tem;
    memset(&tem, 0, sizeof(BlendEdge));

    BlendEdge h, v;
    QRectF input = in[i][j];//当前要计算的矩形
    QVector<QRectF> h_input = in[i];//取横向全部矩形

    int src_id = idTools(i,j);//计算源矩形id
    h = intersect_h(input, h_input, src_id, i);//取横向计算

    QVector<QRectF> vol;
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

    if (h.device_id == src_id)//若取得的id和当前的id相同则将相交数据保存
    {
        if (h.dir & Blend_Left)
        {
            tem.device_id = h.device_id;
            tem.dir = tem.dir | h.dir;
            tem.coord[0] = h.coord[0];
            tem.coord[1] = h.coord[1];
            tem.coord[2] = h.coord[2];
            tem.coord[3] = h.coord[3];
            tem.edge[0] = h.edge[0];
        }
        if (h.dir & Blend_Right)
        {
            tem.device_id = h.device_id;
            tem.dir = (tem.dir | h.dir);
            tem.coord[0] = h.coord[0];
            tem.coord[1] = h.coord[1];
            tem.coord[2] = h.coord[2];
            tem.coord[3] = h.coord[3];
            tem.edge[1] = h.edge[1];
        }
    }
    if (v.device_id == src_id)
    {
        if (v.dir & Blend_Top)
        {
            tem.device_id = v.device_id;
            tem.dir = tem.dir | v.dir;
            tem.coord[0] = v.coord[0];
            tem.coord[1] = v.coord[1];
            tem.coord[2] = v.coord[2];
            tem.coord[3] = v.coord[3];
            tem.edge[2] = v.edge[2];
        }
        if (v.dir & Blend_Bottom)
        {
            tem.device_id = v.device_id;
            tem.dir = (tem.dir | v.dir);
            tem.coord[0] = v.coord[0];
            tem.coord[1] = v.coord[1];
            tem.coord[2] = v.coord[2];
            tem.coord[3] = v.coord[3];
            tem.edge[3] = v.edge[3];
        }
    }
    return tem;
}


///根据矩形位置计算融合方向，融合尺寸，纹理坐标
QVector<QVector<BlendEdge> > intersect_test::intersect(const QVector<QVector<QRectF> > &in)
{
    //! 本函数的思路
    //! 1.循环容器对数据进行取出，和所有矩形进行相交检测
    //!
    QVector<QVector<BlendEdge> > arr;
    for (int i = 0; i < in.size (); i++)
    {
        QVector<BlendEdge> a;
        for (int j = 0; j < in[i].size (); j++)
        {
            a  << intersect(i, j, in);
        }
        arr << a;
    }
    return arr;
}


