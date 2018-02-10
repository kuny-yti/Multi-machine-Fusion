#include "ray_pick.h"
#include "qmath.h"
#include <QSize>

ray_pick::ray_pick(const QVector3D &rayStart, const QVector3D &rayEnd)
{
    start = rayStart;
    end = rayEnd;
}

QVector3D ray_pick::Normalise(const QVector3D &v) const
{
    QVector3D out;
    double l = 1 / sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
    out = v;
    out *= l;
    return out;
}

QVector3D ray_pick::delivery(const QVector3D &U, const QVector3D &V)const
{
    QVector3D ret;
    ret.setX((U.y() * V.z()) - (U.z() * V.y()));
    ret.setY((U.z() * V.x()) - (U.x() * V.z()));
    ret.setZ((U.x() * V.y()) - (U.y() * V.x()));
    return ret;
}

QVector3D ray_pick::CalcPlaneNormal(const QVector3D &P1,const QVector3D &P2, const QVector3D &P3)const
{
   QVector3D V1, V2;
   QVector3D result;

   V1 = P2 - P1;
   V2 = P3 - P1;

   result = delivery(V1 , V2);

   result = Normalise(result);

   return result;
}

ray_pick::LineEqv::LineEqv(const QVector3D &m0, const QVector3D &s)
{
    //默认1、3方程
    a1 = s.y();
    b1 = s.z() - s.x();
    c1 = -s.y();
    d1 = (s.y() * (m0.z() - m0.x())) + (m0.y() * (s.x() - s.z()));

    a2 = s.z();
    b2 = a2;
    c2 = -s.y() - s.x();
    d2 = (m0.z() * (s.x() + s.y())) - (s.z() * (m0.x() + m0.y()));

    //若有一个不可用，则把它改为2方程
    if (((a1 == 0) & (b1 == 0)) & (c1 == 0))
    {
        a1 = s.y() + s.z();
        b1 = -s.x();
        c1 = b1;
        d1 = (s.x() * (m0.y() + m0.z())) - (m0.x() * (s.y() + s.z()));
    }
    else if (((a2 == 0) & (b2 == 0)) & (c2 == 0))
    {
        a2 = s.y() + s.z();
        b2 = -s.x();
        c2 = b2;
        d2 = (s.x() * (m0.y() + m0.z())) - (m0.x() * (s.y() + s.z()));
    }
}

void ray_pick::LineEqv::TestPoint(const QVector3D &p, double &res1, double &res2)
{
    res1 = a1 * p.x() + b1 * p.y() + c1 * p.z() + d1;
    res2 = a2 * p.x() + b2 * p.y() + c2 * p.z() + d2;
}

ray_pick::TriEqaResult::TriEqaResult(const LineEqv &a, const PlaneEqv &c)
{
    x = 0.0; y = 0.0; z = 0.0;
    hasSolution = false;

    double d1, d2;
    if ((a.a1 == 0) | (a.a2 == 0) | (a.b1 == 0) | (a.b2 == 0) |
        (a.c1 == 0) | (a.c2 == 0) | (c.a == 0) | (c.b == 0) | (c.c == 0))
    {
        bool xS = false, yS = false, zS = false;
        //系数为零构成2元一次方程组
        //1,2
        if ((a.a1 == 0) & (a.a2 == 0))
        {
            if (SolveDEqv(a.b1, a.c1, a.d1, a.b2, a.c2, a.d2, y, z))
            {
                yS = true;
                zS = true;

                if (c.a != 0)
                {
                    //a3x+b3y+c3z+d3=0
                    //x=(-d3-c3z-b3y)/a3
                    x = (-c.d - c.c * z - c.b * y) / c.a;
                    xS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        if ((a.b1 == 0) & (a.b2 == 0))
        {
            if (SolveDEqv(a.a1, a.c1, a.d1, a.a2, a.c2, a.d2, x, z))
            {
                xS = true;
                zS = true;

                if (c.b != 0)
                {
                    //a3x+b3y+c3z+d3=0
                    //y=(-d3-c3z-a3x)/b3
                    y = (-c.d - c.c * z - c.a * x) / c.b;
                    yS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        if ((a.c1 == 0) & (a.c2 == 0))
        {
            if (SolveDEqv(a.a1, a.b1, a.d1, a.a2, a.b2, a.d2, x, y))
            {
                xS = true;
                yS = true;

                if (c.c != 0)
                {
                    //a3x+b3y+c3z+d3=0
                    //z=(-d3-b3y-a3x)/c3
                    z = (-c.d - c.b * y - c.a * x) / c.c;
                    zS = true;
                    hasSolution = true;
                    return;
                }

            }
        }

        //------------
        //2,3
        if ((a.a2 == 0) & (c.a == 0))
        {
            if (SolveDEqv(a.b2, a.c2, a.d2, c.b, c.c, c.d, y, z))
            {
                yS = true;
                zS = true;
                if (a.a1 != 0)
                {
                    //a1x+b1y+c1z+d1=0
                    //x=(-d1-c1z-b1y)/a1
                    x = (-a.d1 - a.c1 * z - a.b1 * y) / a.a1;
                    xS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        if ((a.b2 == 0) & (c.b == 0))
        {
            if (SolveDEqv(a.a2, a.c2, a.d2, c.a, c.c, c.d, x, z))
            {
                xS = true;
                zS = true;

                if (a.b1 != 0)
                {
                    //a1x+b1y+c1z+d1=0
                    //y=(-d1-c1z-a1x)/b1
                    y = (-a.d1 - a.c1 * z - a.a1 * x) / a.b1;
                    yS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        if ((a.c2 == 0) & (c.c == 0))
        {
            if (SolveDEqv(a.a2, a.b2, a.d2, c.a, c.b, c.d, x, y))
            {
                xS = true;
                yS = true;

                if (a.c1 != 0)
                {
                    //a1x+b1y+c1z+d1=0
                    //z=(-d1-b1y-a1x)/c1
                    z = (-a.d1 - a.b1 * y - a.a1 * x) / a.c1;
                    zS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        //------------
        //1,3
        if ((a.a1 == 0) & (c.a == 0))
        {
            if (SolveDEqv(a.b1, a.c1, a.d1, c.b, c.c, c.d, y, z))
            {
                yS = true;
                zS = true;

                if (a.a2 != 0)
                {
                    //a2x+b2y+c2z+d2=0
                    //x=(-d2-c2z-b2y)/a2
                    x = (-a.d2 - a.c2 * z - a.b2 * y) / a.a2;
                    xS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        if ((a.b1 == 0) & (c.b == 0))
        {
            if (SolveDEqv(a.a1, a.c1, a.d1, c.a, c.c, c.d, x, z))
            {
                xS = true;
                zS = true;

                if (a.b2 != 0)
                {
                    //a2x+b2y+c2z+d2=0
                    //y=(-d2-c2z-a2x)/b2
                    y = (-a.d2 - a.c2 * z - a.a2 * x) / a.b2;
                    yS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        if ((a.c1 == 0) & (c.c == 0))
        {
            if (SolveDEqv(a.a1, a.b1, a.d1, c.a, c.b, c.d, x, y))
            {
                xS = true;
                yS = true;

                if (a.c2 != 0)
                {
                    //a2x+b2y+c2z+d2=0
                    //z=(-d2-b2y-a2x)/c2
                    z = (-a.d2 - a.b2 * y - a.a2 * x) / a.c2;
                    zS = true;
                    hasSolution = true;
                    return;
                }
            }
        }

        //-------------------------
        //c
        if ((c.c == 0) & (c.b == 0) & (c.a != 0))
        {
            x = -c.d / c.a;
            xS = true;

            //根据另外两个求解
            if (SolveDEqv(a.b1, a.c1, a.a1 * x + a.d1, a.b2, a.c2, a.a2 * x + a.d2, y, z))
            {
                yS = true;
                zS = true;
                hasSolution = true;
                return;
            }
        }
        if ((c.a == 0) & (c.c == 0) & (c.b != 0))
        {
            y = -c.d / c.b;
            yS = true;
            if (SolveDEqv(a.a1, a.c1, a.b1 * y + a.d1, a.a2, a.c2, a.b2 * y + a.d2, x, z))
            {
                xS = true;
                zS = true;
                hasSolution = true;
                return;
            }
        }
        if ((c.a == 0) & (c.b == 0) & (c.c != 0))
        {
            z = -c.d / c.c;
            zS = true;
            if (SolveDEqv(a.a1, a.b1, a.c1 * z + a.d1, a.a2, a.b2, a.c2 * z + a.d2, x,  y))
            {
                xS = true;
                yS = true;
                hasSolution = true;
                return;
            }
        }

        //a1
        if ((a.b1 == 0) & (a.c1 == 0) & (a.a1 != 0) & (!xS))
        {
            x = -a.d1 / a.a1;
            xS = true;
            if (SolveDEqv(c.b, c.c, c.a * x + c.d, a.b2, a.c2, a.a2 * x + a.d2, y, z))
            {
                yS = true;
                zS = true;
                hasSolution = true;
                return;
            }
        }
        if ((a.a1 == 0) & (a.c1 == 0) & (a.b1 != 0) & (!yS))
        {
            y = -a.d1 / a.b1;
            yS = true;
            if (SolveDEqv(c.a, c.c, c.b * y + c.d, a.a2, a.c2, a.b2 * y + a.d2, x, z))
            {
                xS = true;
                zS = true;
                hasSolution = true;
                return;
            }
        }
        if ((a.a1 == 0) & (a.b1 == 0) & (a.c1 != 0) & (!zS))
        {
            z = -a.d1 / a.c1;
            zS = true;
            if (SolveDEqv(c.a, c.b, c.c * z + c.d, a.a2, a.b2, a.c2 * z + a.d2, x, y))
            {
                xS = true;
                yS = true;
                hasSolution = true;
                return;
            }
        }

        //a2
        if ((a.b2 == 0) & (a.c2 == 0) & (a.a2 != 0) & (!xS))
        {
            x = -a.d2 / a.a2;
            xS = true;
            if (SolveDEqv(a.b1, a.c1, a.a1 * x + a.d1, c.b, c.c, c.a * x + c.d, y, z))
            {
                yS = true;
                zS = true;
                hasSolution = true;
                return;
            }
        }
        if ((a.a2 == 0) & (a.c2 == 0) & (a.b2 != 0) & (!yS))
        {
            y = -a.d2 / a.b2;
            yS = true;
            if (SolveDEqv(a.a1, a.c1, a.b1 * y + a.d1, c.a, c.c, c.b * y + c.d, x, z))
            {
                xS = true;
                zS = true;
                hasSolution = true;
                return;
            }
        }
        if ((a.a2 == 0) & (a.b2 == 0) & (a.c2 != 0) & (!zS))
        {
            z = -a.d2 / a.c2;
            zS = true;
            if (SolveDEqv(a.a1, a.b1, a.c1 * z + a.d1, c.a, c.b, c.c * z + c.d, x, y))
            {
                xS = true;
                yS = true;
                hasSolution = true;
                return;
            }
        }

        //两项
        //c
        if ((c.a == 0) & (c.b != 0) & (c.c != 0))
        {
            if (yS & !zS)
            {
                zS = true;
                //by + cz =-d
                z = (-c.d - c.b * y) / c.c;
            }
            if (zS & !yS)
            {
                yS = true;
                //by + cz =-d
                y = (-c.d - c.c * z) / c.b;
            }
        }
        if ((c.a != 0) & (c.b == 0) & (c.c != 0))
        {
            if (xS & !zS)
            {
                zS = true;
                //ax + cz =-d
                z = (-c.d - c.a * x) / c.c;
            }
            if (zS & !xS)
            {
                xS = true;
                //ax + cz =-d
                x = (-c.d - c.c * z) / c.a;
            }
        }
        if ((c.a != 0) & (c.b != 0) & (c.c == 0))
        {
            if (xS & !yS)
            {
                yS = true;
                //ax + by =-d
                y = (-c.d - c.a * x) / c.b;
            }
            if (yS & !xS)
            {
                xS = true;
                //ax + by =-d
                x = (-c.d - c.b * y) / c.a;
            }
        }

        //a1
        if ((a.a1 == 0) & (a.b1 != 0) & (a.c1 != 0))
        {
            if (yS & !zS)
            {
                zS = true;
                //by + cz =-d
                z = (-a.d1 - a.b1 * y) / a.c1;
            }
            if (zS & !yS)
            {
                yS = true;
                //by + cz =-d
                y = (-a.d1 - a.c1 * z) / a.b1;
            }
        }
        if ((a.a1 != 0) & (a.b1 == 0) & (a.c1 != 0))
        {
            if (xS & !zS)
            {
                zS = true;
                //ax + cz =-d
                z = (-a.d1 - a.a1 * x) / a.c1;
            }
            if (zS & !xS)
            {
                xS = true;
                //ax + cz =-d
                x = (-a.d1 - a.c1 * z) / a.a1;
            }
        }
        if ((a.a1 != 0) & (a.b1 != 0) & (a.c1 == 0))
        {
            if (xS & !yS)
            {
                yS = true;
                //ax + by =-d
                y = (-a.d1 - a.a1 * x) / a.b1;
            }
            if (yS & !xS)
            {
                xS = true;
                //ax + by =-d
                x = (-a.d1 - a.b1 * y) / a.a1;
            }
        }

        //a2
        if ((a.a2 == 0) & (a.b2 != 0) & (a.c2 != 0))
        {
            if (yS & !zS)
            {
                zS = true;
                //by + cz =-d
                z = (-a.d2 - a.b2 * y) / a.c2;
            }
            if (zS & !yS)
            {
                yS = true;
                //by + cz =-d
                y = (-a.d2 - a.c2 * z) / a.b2;
            }
        }

        if ((a.a2 != 0) & (a.b2 == 0) & (a.c2 != 0))
        {
            if (xS & !zS)
            {
                zS = true;
                //ax + cz =-d
                z = (-a.d2 - a.a2 * x) / a.c2;
            }
            if (zS & !xS)
            {
                xS = true;
                //ax + cz =-d
                x = (-a.d2 - a.c2 * z) / a.a2;
            }
        }

        if ((a.a2 != 0) & (a.b2 != 0) & (a.c2 == 0))
        {
            if (xS & !yS)
            {
                yS = true;
                //ax + by =-d
                y = (-a.d2 - a.a2 * x) / a.b2;
            }
            if (yS & !xS)
            {
                xS = true;
                //ax + by =-d
                x = (-a.d2 - a.b2 * y) / a.a2;
            }
        }
        hasSolution = xS & yS & zS;

        if (!hasSolution)
        {

            d1 = (c.a * a.b2 - a.a2 * c.b) * (a.b1 * a.c2 - a.b2 * a.c1) - (a.b2 * c.c - c.b * a.c2) * (a.a2 * a.b1 - a.a1 * a.b2);
            //for y z
            d2 = (a.a2 * c.b - c.a * a.b2) * (a.a1 * a.c2 - a.a2 * a.c1) - (a.a2 * c.c - c.a * a.c2) * (a.a1 * a.b2 - a.a2 * a.b1);
            if ((d1 == 0) | (d2 == 0))
            {
                hasSolution = false;
                x = 0.0;
                y = 0.0;
                z = 0.0;
            }
            else
            {
                hasSolution = true;
                x = ((a.b2 * c.c - c.b * a.c2) * (a.b1 * a.d2 - a.d1 * a.b2) - (a.b1 * a.c2 - a.b2 * a.c1) * (a.b2 * c.d - a.d2 * c.b)) / d1;

                y = ((a.a2 * c.c - c.a * a.c2) * (a.a1 * a.d2 - a.a2 * a.d1) - (a.a1 * a.c2 - a.a2 * a.c1) * (a.a2 * c.d - c.a * a.d2)) / d2;
                z = ((a.a1 * a.b2 - a.a2 * a.b1) * (a.a2 * c.d - c.a * a.d2) - (a.a2 * c.b - c.a * a.b2) * (a.a1 * a.d2 - a.a2 * a.d1)) / d2;
            }

        }
    }
    else
    {
        d1 = (c.a * a.b2 - a.a2 * c.b) * (a.b1 * a.c2 - a.b2 * a.c1) - (a.b2 * c.c - c.b * a.c2) * (a.a2 * a.b1 - a.a1 * a.b2);
        //for y z
        d2 = (a.a2 * c.b - c.a * a.b2) * (a.a1 * a.c2 - a.a2 * a.c1) - (a.a2 * c.c - c.a * a.c2) * (a.a1 * a.b2 - a.a2 * a.b1);
        if ((d1 == 0) | (d2 == 0))
        {
            hasSolution = false;
            x = 0.0;
            y = 0.0;
            z = 0.0;
        }
        else
        {
            hasSolution = true;
            x = ((a.b2 * c.c - c.b * a.c2) * (a.b1 * a.d2 - a.d1 * a.b2) - (a.b1 * a.c2 - a.b2 * a.c1) * (a.b2 * c.d - a.d2 * c.b)) / d1;

            y = ((a.a2 * c.c - c.a * a.c2) * (a.a1 * a.d2 - a.a2 * a.d1) - (a.a1 * a.c2 - a.a2 * a.c1) * (a.a2 * c.d - c.a * a.d2)) / d2;
            z = ((a.a1 * a.b2 - a.a2 * a.b1) * (a.a2 * c.d - c.a * a.d2) - (a.a2 * c.b - c.a * a.b2) * (a.a1 * a.d2 - a.a2 * a.d1)) / d2;
        }
    }
}

bool ray_pick::TriEqaResult::SolveDEqv(double a1, double b1, double c1, double a2, double b2, double c2,
                                       double &sx,  double &sy)
{
    double d = a1 * b2 - a2 * b1;
    if (d == 0)
    { return false; }
    else
    {
        sx = (b1 * c2 - b2 * c1) / d;
        sy = (a2 * c1 - a1 * c2) / d;
        return true;
    }
}

bool ray_pick::IsInTrg(const QVector3D &vtxA, const QVector3D &vtxB, const QVector3D &vtxC,
                       const TriEqaResult &pHit)
{
    LineEqv l1, l2, l3;

    l1 = LineEqv(vtxA, vtxB - vtxA);
    l2 = LineEqv(vtxC, vtxA - vtxC);
    l3 = LineEqv(vtxB, vtxC - vtxB);

    double res1[3] = {0}, res2[3] = {0};

    QVector3D pos = QVector3D(pHit.x, pHit.y, pHit.z);

    l1.TestPoint(pos, res1[0], res2[0]);
    l2.TestPoint(pos, res1[1], res2[1]);
    l3.TestPoint(pos, res1[2], res2[2]);

    int i;
    int ca = 0, cm = 0;
    for (i = 0; i <= 2; i++)
    {
        if (res1[i] < 0) { cm++; }
        if (res1[i] >= 0) { ca++; }

    }

    if ((ca == 3) | (cm == 3))
    {
        ca = 0;
        cm = 0;
        for (i = 0; i <= 2; i++)
        {
            if (res2[i] < 0) { cm++; }
            if (res2[i] >= 0) { ca++; }
        }

        if ((ca == 3) | (cm == 3))
        {
            return true;
        }
        else
        {
            ca = 0; cm = 0;
            for (i = 0; i <= 2; i++)
            {
                if (res2[i] <= 0) { cm++; }
                if (res2[i] > 0) { ca++; }
            }
            if ((ca == 3) | (cm == 3))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        ca = 0; cm = 0;
        for (i = 0; i <= 2; i++)
        {
            if (res1[i] <= 0) { cm++; }
            if (res1[i] > 0) { ca++; }

        }

        if ((ca == 3) | (cm == 3))
        {
            ca = 0;
            cm = 0;
            for (i = 0; i <= 2; i++)
            {
                if (res2[i] < 0) { cm++; }
                if (res2[i] >= 0) { ca++; }
            }

            if ((ca == 3) | (cm == 3))
            {
                return true;
            }
            else
            {
                ca = 0; cm = 0;
                for (i = 0; i <= 2; i++)
                {
                    if (res2[i] <= 0) { cm++; }
                    if (res2[i] > 0) { ca++; }
                }
                if ((ca == 3) | (cm == 3))
                {
                    return true;
                }
                else
                { return false; }
            }
        }
        else
        { return false; }
    }
}

bool ray_pick::intersect(const QVector3D &vtxA, const QVector3D &vtxB, const QVector3D &vtxC,
                         QVector3D &pos)
{
    PlaneEqv curFace = PlaneEqv(vtxA, CalcPlaneNormal(vtxA, vtxB, vtxC));
    QVector3D ori = end - start;
    LineEqv curSpd = LineEqv(start, ori);

    TriEqaResult pHit = TriEqaResult(curSpd, curFace);

    if (!pHit.hasSolution) { return false; }

    pos = QVector3D((float)pHit.x, (float)pHit.y, (float)pHit.z);

    QVector3D tr = (pos - start) * ori;
    if ((tr.x() > 0) && (tr.y() > 0) &&(tr.z() > 0))
    {
        return IsInTrg(vtxA, vtxB, vtxC, pHit);
    }
    return false;
}

bool ray_pick::pick (QVector3D &pos, const QVector3D &rayStart, const QVector3D &rayEnd,
                            const QSizeF rect)
{
    QVector3D vtxA1, vtxB1, vtxC1;
    QVector3D vtxA2, vtxB2, vtxC2;
    QVector3D vtxA3, vtxB3, vtxC3;
    float sw = (float)rect.width()/2.;
    float sh = (float)rect.height()/2.;

    vtxA1 = QVector3D(pos.x() -sw, pos.y() -sh, pos.z());
    vtxB1 = QVector3D(pos.x() +sw, pos.y() -sh, pos.z());
    vtxC1 = QVector3D(pos.x() , pos.y() +sh, pos.z());

    vtxA2 = QVector3D(pos.x() , pos.y() -sh, pos.z() -sh);
    vtxB2 = QVector3D(pos.x() , pos.y() -sh, pos.z() +sh);
    vtxC2 = QVector3D(pos.x() , pos.y() +sh, pos.z() );

    vtxA3 = QVector3D(pos.x() -sh, pos.y(), pos.z() -sh);
    vtxB3 = QVector3D(pos.x() -sh, pos.y(), pos.z() +sh);
    vtxC3 = QVector3D(pos.x() +sh, pos.y(), pos.z() );

    ray_pick a(rayStart, rayEnd);
    bool ba = a.intersect(vtxA1, vtxB1, vtxC1, pos);
    bool bb = a.intersect(vtxA2, vtxB2, vtxC2, pos);
    bool bc = a.intersect(vtxA3, vtxB3, vtxC3, pos);

    return ba | bb | bc;
}
