#ifndef POLYLINE_H
#define POLYLINE_H
#include "type_define.h"
#include <QQueue>

class polyline
{
public:
    polyline();
    polyline(const QVector<QVector3D>& verts);

    static polyline from_rectangle(const QRect& rect);

    /// remove all the points
    void clear();

    /// add a vertex
    void append( const QVector3D& p );
    void append( float x, float y, float z=0 );
    void appends( const  QVector<QVector3D>& verts );
    void appends(const QVector3D* verts, int numverts);

    // adds a straight line to the polyline
    void lineTo(const QVector3D & to ){ append(to); }
    void lineTo(float x, float y, float z=0)
    {
        append(x,y,z);
    }

    void insert(const QVector3D &p, int index);
    void insert(float x, float y, float z, int index);

    // adds an arc to the polyline
    // if the arc doesn't start at the same point
    // the last vertex finished a straight line will
    // be created to join both

    void arc(const QVector3D & center, float radiusX, float radiusY, float angleBegin, float angleEnd, bool clockwise, int circleResolution = 20);

    void arc(const QVector3D & center, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20)
    {
        arc(center, radiusX,  radiusY,  angleBegin,  angleEnd, true,  circleResolution);
    }
    void arc(float x, float y, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20)
    {
        arc(QVector3D(x, y,0), radiusX, radiusY, angleBegin, angleEnd, true, circleResolution);
    }
    void arc(float x, float y, float z, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20)
    {
        arc(QVector3D(x, y, z), radiusX, radiusY, angleBegin, angleEnd, true, circleResolution);
    }
    void arc_negative(const QVector3D & center, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20)
    {
        arc(center, radiusX, radiusY, angleBegin, angleEnd, false, circleResolution);
    }
    void arc_negative(float x, float y, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20)
    {
        arc(QVector3D(x,y, 0), radiusX, radiusY, angleBegin, angleEnd, false, circleResolution);
    }
    void arc_negative(float x, float y, float z, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20)
    {
        arc(QVector3D(x, y, z), radiusX, radiusY, angleBegin, angleEnd, false, circleResolution);
    }


    // catmull-rom curve
    void curveTo( const QVector3D & to, int curveResolution = 20 );
    void curveTo(float x, float y, float z = 0,  int curveResolution = 20 )
    {
        curveTo(QVector3D(x,y,z),curveResolution);
    }

    /// cubic bezier
    void bezierTo( const QVector3D & cp1, const QVector3D & cp2, const QVector3D & to, int curveResolution = 20);
    void bezierTo(float cx1, float cy1, float cx2, float cy2, float x, float y, int curveResolution = 20){
        bezierTo(QVector3D(cx1,cy1,0),QVector3D(cx2,cy2,0),QVector3D(x,y,0),curveResolution);
    }
    void bezierTo(float cx1, float cy1, float cz1, float cx2, float cy2, float cz2, float x, float y, float z, int curveResolution = 20)
    {
        bezierTo(QVector3D(cx1,cy1,cz1),QVector3D(cx2,cy2,cz2),QVector3D(x,y,z),curveResolution);
    }

    /// quadratic bezier (lower resolution than cubic)
    void quad_bezierTo(float cx1, float cy1, float cz1, float cx2, float cy2, float cz2, float x, float y, float z, int curveResolution = 20);
    void quad_bezierTo(  const QVector3D & p1, const QVector3D & p2,const QVector3D & p3,  int curveResolution = 20 )
    {
        quad_bezierTo(p1.x(),p1.y(),p1.z(),
                      p2.x(),p2.y(),p2.z(),
                      p3.x(),p3.y(),p3.z(),curveResolution);
    }
    void quad_bezierTo(float cx1, float cy1, float cx2, float cy2, float x, float y, int curveResolution = 20)
    {
        quad_bezierTo(cx1,cy1,0,cx2,cy2,0,x,y,0,curveResolution);
    }

    polyline get_smoothed(int smoothingSize, float smoothingShape = 0) const;

    // resample a polyline based on the distance between the points
    polyline get_resampled_by_spacing(float spacing) const;

    // resample a polyline based on the total point count
    polyline get_resampled_by_count(int count) const;

    // get the bounding box of a polyline
    QRect get_bounding_box() const;

    // find the closest point 'target' on a polyline
    // optionally pass a pointer to/address of an unsigned int to get the index of the closest vertex
    QVector3D get_closest(const QVector3D& target, unsigned int* nearestIndex = NULL) const;

    // check wheteher a point is inside the area enclosed by the polyline
    static bool inside(float x, float y, const polyline & _polyline);
    static bool inside(const QVector3D & p, const polyline & _polyline);

    // non-static versions
    bool inside(float x, float y) const;
    bool inside(const QVector3D & p) const;

    void simplify(float tolerance=0.3f);

    /// points vector access
    size_t size() const;
    const QVector3D& operator[] (int index) const;
    QVector3D& operator[] (int index);
    void resize(size_t size);

    /// closed
    void set_closed( bool tf );
    bool isClosed() const;
    void close();

    bool hasChanged();
    void flag_hasChanged();

    QVector<QVector3D> & get_vertices();
    const QVector<QVector3D> & get_vertices() const;

    float get_perimeter() const;
    float get_area() const;
    QVector3D get_centroid2D() const;

    void draw();

    // used for calculating the normals
    void set_right_vector(QVector3D v = QVector3D(0, 0, -1));
    QVector3D get_right_vector() const;


    // get (interpolated) index at given length along the path
    // includes info on percentage along segment, e.g. ret=5.75 => 75% along the path between 5th and 6th points
    float get_index_length(float f) const;

    // get (interpolated) index at given percentage along the path
    // includes info on percentage along segment, e.g. ret=5.75 => 75% along the path between 5th and 6th points
    float get_index_percent(float f) const;

    // get length along path at index
    float get_length_index(int index) const;

    // get length along path at interpolated index (e.g. f=5.75 => 75% along the path between 5th and 6th points)
    float get_length_index_interpolated(float findex) const;

    // get point long the path at a given length (e.g. f=150 => 150 units along the path)
    QVector3D get_point_length(float f) const;

    // get point along the path at a given percentage (e.g. f=0.25 => 25% along the path)
    QVector3D get_point_percent(float f) const;

    // get point along the path at interpolated index (e.g. f=5.75 => 75% along the path between 5th and 6th points)
    QVector3D get_point_index_interpolated(float findex) const;

    // get angle (degrees) at index
    float get_angle_index(int index) const;

    // get angle (degrees) at interpolated index (interpolated between neighboring indices)
    float get_angle_index_interpolated(float findex) const;

    // get rotation vector at index (magnitude is sin of angle)
    QVector3D get_rotation_index(int index) const;

    // get rotation vector at interpolated index (interpolated between neighboring indices) (magnitude is sin of angle)
    QVector3D get_rotation_index_interpolated(float findex) const;

    // get tangent vector at index
    QVector3D get_tangent_index(int index) const;

    // get tangent vector at interpolated index (interpolated between neighboring indices)
    QVector3D get_tangent_index_interpolated(float findex) const;

    // get normal vector at index
    QVector3D get_normal_index(int index) const;

    // get normal vector at interpolated index (interpolated between neighboring indices)
    QVector3D get_normal_index_interpolated(float findex) const;

    // get wrapped index depending on whether poly is closed or not
    int get_wrapped_index(int index) const;

private:
    void set_circle_resolution(int res);
    float wrap_angle(float angleRad);

    QVector<QVector3D> points;
    QVector3D rightVector;

    // cache
    mutable QVector<float> lengths;    // cumulative lengths, stored per point (lengths[n] is the distance to the n'th point, zero based)
    mutable QVector<QVector3D> tangents;   // tangent at vertex, stored per point
    mutable QVector<QVector3D> normals;    //
    mutable QVector<QVector3D> rotations;   // rotation between adjacent segments, stored per point (cross product)
    mutable QVector<float> angles;    // angle (degrees) between adjacent segments, stored per point (asin(cross product))
    mutable QVector3D centroid2D;
    mutable float area;


    QQueue <QVector3D> curveVertices;
    QVector<QVector3D> circlePoints;

    bool bClosed;
    bool bHasChanged;   // public API has access to this
    mutable bool bCacheIsDirty;   // used only internally, no public API to read

    void update_cache(bool bForceUpdate = false) const;

    // given an interpolated index (e.g. 5.75) return neighboring indices and interolation factor (e.g. 5, 6, 0.75)
    void get_interpolation_params(float findex, int &i1, int &i2, float &t) const;

    void calc_data(int index, QVector3D &tangent, float &angle, QVector3D &rotation, QVector3D &normal) const;
};
#endif // POLYLINE_H
