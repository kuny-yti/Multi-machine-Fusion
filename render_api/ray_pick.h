#ifndef RAY_PICK_H
#define RAY_PICK_H

#include <QVector3D>
#include <QSizeF>

class ray_pick
{
    struct PlaneEqv
    {
        double a, b, c, d;

        /// <summary>
        /// 点法式方程
        /// </summary>
        PlaneEqv(QVector3D m0, QVector3D n)
        {
            a = n.x(); b = n.y(); c = n.z();
            d = -n.x() * m0.x() - n.y() * m0.y() - n.z() * m0.z();
        }

    };
    struct LineEqv
    {
        double a1, b1, c1, d1;
        double a2, b2, c2, d2;

        //点向式方程
        LineEqv(){}
        LineEqv(const QVector3D &m0, const QVector3D & s);

        void TestPoint(const QVector3D & p, double &res1, double &res2);
    };

    struct TriEqaResult
    {
        double x, y, z;
        bool hasSolution;

        TriEqaResult(const LineEqv &a, const PlaneEqv &c);
        bool SolveDEqv(double a1, double b1, double c1, double a2, double b2, double c2,
                       double &sx,  double &sy);

    };

    /// <summary>
    /// 转为单位向量
    /// </summary>
    QVector3D Normalise(const QVector3D &v)const;

    //向量积
    QVector3D delivery(const QVector3D &U, const QVector3D &V) const;
    QVector3D CalcPlaneNormal(const QVector3D &P1,const QVector3D &P2, const QVector3D &P3)const;

    bool IsInTrg(const QVector3D &vtxA, const QVector3D &vtxB,const QVector3D &vtxC,
                 const TriEqaResult &pHit);

    QVector3D start;
    QVector3D end;
public:
    ray_pick(const QVector3D &rayStart, const QVector3D &rayEnd);

    bool intersect(const QVector3D &vtxA, const QVector3D &vtxB, const QVector3D &vtxC,
                   QVector3D &pos);

    static bool pick (QVector3D &pos, const QVector3D &rayStart, const QVector3D &rayEnd,
                      const QSizeF rect = QSizeF(0.02,0.02));
};

#endif // RAY_PICK_H
