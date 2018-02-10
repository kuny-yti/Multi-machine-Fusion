#include "polyline.h"
#include <cmath>

//----------------------------------------------------------
polyline::polyline(){
    set_right_vector();
    clear();
}

//----------------------------------------------------------
polyline::polyline(const QVector<QVector3D>& verts)
{
    set_right_vector();
    clear();
    appends(verts);
}

//----------------------------------------------------------
polyline polyline::from_rectangle(const QRect& rect)
{
    polyline _polyline;
    _polyline.append(QVector3D(rect.x(), rect.y(),0));
    _polyline.append(rect.x()+rect.width(), rect.y());
    _polyline.append(QVector3D(rect.x()+rect.width(),
                               rect.y()+rect.height(),0));
    _polyline.append(rect.x(), rect.y()+rect.height());
    _polyline.close();
    return _polyline;
}

//----------------------------------------------------------
void polyline::clear()
{
    set_closed(false);
    points.clear();
    curveVertices.clear();
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::append(const QVector3D& p)
{
    curveVertices.clear();
    points.push_back(p);
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::append(float x, float y, float z)
{
    curveVertices.clear();
    append(QVector3D(x,y,z));
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::appends(const QVector<QVector3D>& verts)
{
    curveVertices.clear();
    points += verts;
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::appends(const QVector3D* verts, int numverts)
{
    curveVertices.clear();
    for (int i=0; i < numverts; i++)
    {
       points.append(verts[i]);
    }
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::insert(const QVector3D &p, int index)
{
    curveVertices.clear();
    points.insert(points.begin()+index, p);
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::insert(float x, float y, float z, int index)
{
    insert(QVector3D(x, y, z), index);
}


//----------------------------------------------------------
size_t polyline::size() const
{
    return points.size();
}

//----------------------------------------------------------
const QVector3D& polyline::operator[] (int index) const
{
    return points[index];
}

//----------------------------------------------------------
QVector3D& polyline::operator[] (int index)
{
    flag_hasChanged();
    return points[index];
}

//----------------------------------------------------------
void polyline::resize(size_t size)
{
    points.resize(size);
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::set_closed( bool tf )
{
    bClosed = tf;
    flag_hasChanged();
}

//----------------------------------------------------------
bool polyline::isClosed() const
{
    return bClosed;
}

//----------------------------------------------------------
void polyline::close()
{
    set_closed(true);
}

//----------------------------------------------------------
bool polyline::hasChanged()
{
    if(bHasChanged){
        bHasChanged=false;
        return true;
    }else{
        return false;
    }
}

//----------------------------------------------------------
void polyline::flag_hasChanged()
{
    bHasChanged = true;
    bCacheIsDirty = true;
}

//----------------------------------------------------------
QVector<QVector3D> & polyline::get_vertices()
{
    flag_hasChanged();
    return points;
}

//----------------------------------------------------------
const QVector<QVector3D> & polyline::get_vertices() const
{
    return points;
}


//----------------------------------------------------------
void polyline::set_circle_resolution(int res)
{
    if (res > 1 && res != (int)circlePoints.size())
    {
        circlePoints.resize(res);

        float angle = 0.0f;
        const float angleAdder = M_TWO_PI / (float)res;
        for (int i = 0; i < res; i++)
        {
            circlePoints[i].setX(cos(angle));
            circlePoints[i].setY(sin(angle));
            circlePoints[i].setZ(0.0f);
            angle += angleAdder;
        }
    }
}

//----------------------------------------------------------
// wraps any radian angle -FLT_MAX to +FLT_MAX into 0->2PI range.
// TODO, make angle treatment consistent across all functions
// should always be radians?  or should this take degrees?
// used internally, so perhaps not as important
float polyline::wrap_angle(float angleRadians)
{
    return wrap(angleRadians, 0.0f, TWO_PI);
}

//----------------------------------------------------------
void polyline::bezierTo( const QVector3D & cp1, const QVector3D & cp2, const QVector3D & to, int curveResolution ){
    // if, and only if poly vertices has points, we can make a bezier
    // from the last point
    curveVertices.clear();

    // the resolultion with which we computer this bezier
    // is arbitrary, can we possibly make it dynamic?

    if (size() > 0)
    {
        float x0 = points[size()-1].x();
        float y0 = points[size()-1].y();
        float z0 = points[size()-1].z();

        float   ax, bx, cx;
        float   ay, by, cy;
        float   az, bz, cz;
        float   t, t2, t3;
        float   x, y, z;

        // polynomial coefficients
        cx = 3.0f * (cp1.x() - x0);
        bx = 3.0f * (cp2.x() - cp1.x()) - cx;
        ax = to.x() - x0 - cx - bx;

        cy = 3.0f * (cp1.y() - y0);
        by = 3.0f * (cp2.y() - cp1.y()) - cy;
        ay = to.y() - y0 - cy - by;

        cz = 3.0f * (cp1.z() - z0);
        bz = 3.0f * (cp2.z() - cp1.z()) - cz;
        az = to.z() - z0 - cz - bz;

        for (int i = 0; i < curveResolution; i++)
        {
            t 	=  (float)i / (float)(curveResolution-1);
            t2 = t * t;
            t3 = t2 * t;
            x = (ax * t3) + (bx * t2) + (cx * t) + x0;
            y = (ay * t3) + (by * t2) + (cy * t) + y0;
            z = (az * t3) + (bz * t2) + (cz * t) + z0;
            points.push_back(QVector3D(x,y,z));
        }
    }
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::quad_bezierTo(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, int curveResolution)
{
    curveVertices.clear();
    for(int i=0; i <= curveResolution; i++)
    {
        double t = (double)i / (double)(curveResolution);
        double a = (1.0 - t)*(1.0 - t);
        double b = 2.0 * t * (1.0 - t);
        double c = t*t;
        double x = a * x1 + b * x2 + c * x3;
        double y = a * y1 + b * y2 + c * y3;
        double z = a * z1 + b * z2 + c * z3;
        points.push_back(QVector3D(x, y, z));
    }
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::curveTo( const QVector3D & to, int curveResolution )
{

    curveVertices.push_back(to);

    if (curveVertices.size() == 4){

        float x0 = curveVertices[0].x();
        float y0 = curveVertices[0].y();
        float z0 = curveVertices[0].z();
        float x1 = curveVertices[1].x();
        float y1 = curveVertices[1].y();
        float z1 = curveVertices[1].z();
        float x2 = curveVertices[2].x();
        float y2 = curveVertices[2].y();
        float z2 = curveVertices[2].z();
        float x3 = curveVertices[3].x();
        float y3 = curveVertices[3].y();
        float z3 = curveVertices[3].z();

        float t,t2,t3;
        float x,y,z;

        for (int i = 0; i < curveResolution; i++)
        {

            t 	=  (float)i / (float)(curveResolution-1);
            t2 	= t * t;
            t3 	= t2 * t;

            x = 0.5f * ( ( 2.0f * x1 ) +
                        ( -x0 + x2 ) * t +
                        ( 2.0f * x0 - 5.0f * x1 + 4 * x2 - x3 ) * t2 +
                        ( -x0 + 3.0f * x1 - 3.0f * x2 + x3 ) * t3 );

            y = 0.5f * ( ( 2.0f * y1 ) +
                        ( -y0 + y2 ) * t +
                        ( 2.0f * y0 - 5.0f * y1 + 4 * y2 - y3 ) * t2 +
                        ( -y0 + 3.0f * y1 - 3.0f * y2 + y3 ) * t3 );

            z = 0.5f * ( ( 2.0f * z1 ) +
                        ( -z0 + z2 ) * t +
                        ( 2.0f * z0 - 5.0f * z1 + 4 * z2 - z3 ) * t2 +
                        ( -z0 + 3.0f * z1 - 3.0f * z2 + z3 ) * t3 );

            points.push_back(QVector3D(x,y,z));
        }
        curveVertices.pop_front();
    }
    flag_hasChanged();
}

//----------------------------------------------------------
void polyline::arc(const QVector3D & center, float radiusX, float radiusY, float angleBegin, float angleEnd, bool clockwise, int circleResolution)
{

    if(circleResolution<=1) circleResolution=2;
    set_circle_resolution(circleResolution);
    points.reserve(points.size()+circleResolution);

    const float epsilon = 0.0001f;

    const size_t nCirclePoints = circlePoints.size();
    float segmentArcSize  = M_TWO_PI / (float)nCirclePoints;

    // convert angles to radians and wrap them into the range 0-M_TWO_PI and
    float angleBeginRad = wrap_angle(deg_rad(angleBegin));
    float angleEndRad =   wrap_angle(deg_rad(angleEnd));

    while(angleBeginRad >= angleEndRad) angleEndRad += M_TWO_PI;

    // determine the directional angle delta
    float d = clockwise ? angleEndRad - angleBeginRad : angleBeginRad - angleEndRad;
    // find the shortest angle delta, clockwise delta direction yeilds POSITIVE values
    float deltaAngle = atan2(sin(d),cos(d));

    // establish the remaining angle that we have to work through
    float remainingAngle = deltaAngle;

    // if the delta angle is in the CCW direction OR the start and stop angles are
    // effectively the same adjust the remaining angle to be a be a full rotation
    if(deltaAngle < 0 || abs(deltaAngle) < epsilon) remainingAngle += M_TWO_PI;

    QVector3D radii(radiusX,radiusY, 0);
    QVector3D point;

    int currentLUTIndex = 0;
    bool isFirstPoint = true; // special case for the first point

    while(remainingAngle > 0)
    {
        if(isFirstPoint)
        {
            // TODO: should this be the exact point on the circle or
            // should it be an intersecting point on the line that connects two
            // surrounding LUT points?
            //
            // get the EXACT first point requested (for points that
            // don't fall precisely on a LUT entry)
            point = QVector3D(cos(angleBeginRad),sin(angleBeginRad), 0);
            // set up the get any in between points from the LUT
            float ratio = angleBeginRad / M_TWO_PI * (float)nCirclePoints;
            currentLUTIndex = clockwise ? (int)ceil(ratio) : (int)floor(ratio);
            float lutAngleAtIndex = currentLUTIndex * segmentArcSize;
            // the angle between the beginning angle and the next angle in the LUT table
            float d = clockwise ? (lutAngleAtIndex - angleBeginRad) : (angleBeginRad - lutAngleAtIndex);
            float firstPointDelta = atan2(sin(d),cos(d)); // negative is in the clockwise direction

            // if the are "equal", get the next one CCW
            if(abs(firstPointDelta) < epsilon) {
                currentLUTIndex = clockwise ? (currentLUTIndex + 1) : (currentLUTIndex - 1);
                firstPointDelta = segmentArcSize; // we start at the next lut point
            }

            // start counting from the offset
            remainingAngle -= firstPointDelta;
            isFirstPoint = false;
        } else {
            point = QVector3D(circlePoints[currentLUTIndex].x(),circlePoints[currentLUTIndex].y(),0);
            if(clockwise) {
                currentLUTIndex++; // go to the next LUT point
                remainingAngle -= segmentArcSize; // account for next point
                // if the angle overshoots, then the while loop will fail next time
            } else {
                currentLUTIndex--; // go to the next LUT point
                remainingAngle -= segmentArcSize; // account for next point
                // if the angle overshoots, then the while loop will fail next time
            }
        }

        // keep the current lut index in range
        if(clockwise) {
            currentLUTIndex = currentLUTIndex % nCirclePoints;
        } else {
            if(currentLUTIndex < 0) currentLUTIndex = nCirclePoints + currentLUTIndex;
        }

        // add the point to the poly line
        point = point * radii + center;
        points.push_back(point);

        // if the next LUT point moves us past the end angle then
        // add a a point a the exact end angle and call it finished
        if(remainingAngle < epsilon)
        {
            point = QVector3D(cos(angleEndRad),sin(angleEndRad), 0);
            point = point * radii + center;
            points.push_back(point);
            remainingAngle = 0; // call it finished, the next while loop test will fail
        }
    }
    flag_hasChanged();
}

//----------------------------------------------------------
float polyline::get_perimeter() const
{
    if(points.size() < 2) {
        return 0;
    } else {
        update_cache();
        return lengths.back();
    }
}

//----------------------------------------------------------
float polyline::get_area() const
{
    update_cache();
    return area;
}

//----------------------------------------------------------
QVector3D polyline::get_centroid2D() const
{
    update_cache();
    return centroid2D;
}

//----------------------------------------------------------
QRect polyline::get_bounding_box() const
{

    QRect box;
    for(size_t i = 0; i < size(); i++)
    {
        if(i == 0)
        {
            box = QRect(points[i].x(), points[i].y(),0,0);
        }
        else
        {
            float x0 = MIN(box.x(), points[i].x());
            float x1 = MAX(box.x()+box.width(), points[i].x());
            float y0 = MIN(box.y(), points[i].y());
            float y1 = MAX(box.y()+box.height(), points[i].y());
            float w = x1 - x0;
            float h = y1 - y0;
            box = QRect(x0, y0,w,h);
        }
    }
    return box;
}

//----------------------------------------------------------
polyline polyline::get_smoothed(int smoothingSize, float smoothingShape) const
{
    int n = size();
    smoothingSize = clamp(smoothingSize, 0, n);
    smoothingShape = clamp(smoothingShape, 0, 1);

    // precompute weights and normalization
    QVector<float> weights;
    weights.resize(smoothingSize);
    // side weights
    for(int i = 1; i < smoothingSize; i++)
    {
        float curWeight = map((float)i, 0., (float)smoothingSize, 1., (float)smoothingShape);
        weights[i] = curWeight;
    }

    // make a copy of this polyline
    polyline result = *this;

    for(int i = 0; i < n; i++)
    {
        float sum = 1; // center weight
        for(int j = 1; j < smoothingSize; j++) {
            QVector3D cur;
            int leftPosition = i - j;
            int rightPosition = i + j;
            if(leftPosition < 0 && bClosed)
            {
                leftPosition += n;
            }
            if(leftPosition >= 0)
            {
                cur += points[leftPosition];
                sum += weights[j];
            }
            if(rightPosition >= n && bClosed)
            {
                rightPosition -= n;
            }
            if(rightPosition < n)
            {
                cur += points[rightPosition];
                sum += weights[j];
            }
            result[i] += cur * weights[j];
        }
        result[i] /= sum;
    }

    return result;
}

//----------------------------------------------------------
polyline polyline::get_resampled_by_spacing(float spacing) const
{
    if(spacing==0 || size() == 0) return *this;
    polyline poly;
    float totalLength = get_perimeter();
    for(float f=0; f<totalLength; f += spacing)
    {
        poly.lineTo(get_point_length(f));
    }

    if(!isClosed())
    {
        if(poly.size() > 0) poly.points.back() = points.back();
        poly.set_closed(false);
    } else {
        poly.set_closed(true);
    }

    return poly;
}

//----------------------------------------------------------
polyline polyline::get_resampled_by_count(int count) const
{
    float perimeter = get_perimeter();
    if(count < 2)
    {
        qDebug () <<("polyline") << "get_resampled_by_count(): requested " << count <<" points, using minimum count of 2 ";
        count = 2;
    }
    return polyline::get_resampled_by_spacing(perimeter / (count-1));
}

QVector3D  crossed(const QVector3D& s, const QVector3D& vec )
{
    return QVector3D( s.y()*vec.z() - s.z()*vec.y(),
                   s.z()*vec.x() - s.x()*vec.z(),
                   s.x()*vec.y() - s.y()*vec.x() );
}

QVector3D interpolated(const QVector3D& s, const QVector3D& pnt, float p )
{
    return QVector3D( s.x()*(1-p) + pnt.x()*p,
                   s.y()*(1-p) + pnt.y()*p,
                   s.z()*(1-p) + pnt.z()*p );
}
float distance(const QVector3D& s, const QVector3D& pnt)
{
    float vx = s.x()-pnt.x();
    float vy = s.y()-pnt.y();
    float vz = s.z()-pnt.z();
    return (float)sqrt(vx*vx + vy*vy + vz*vz);
}
//----------------------------------------------------------
// http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
static QVector3D get_closest_util(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, float* normalizedPosition) {
    // if p1 is coincident with p2, there is no line
    if(p1 == p2) {
        if(normalizedPosition != NULL)
        {
            *normalizedPosition = 0;
        }
        return p1;
    }

    float u = (p3.x() - p1.x()) * (p2.x() - p1.x());
    u += (p3.y() - p1.y()) * (p2.y() - p1.y());
    // perfect place for fast inverse sqrt...
    float len = (p2 - p1).length();
    u /= (len * len);

    // clamp u
    if(u > 1) {
        u = 1;
    } else if(u < 0) {
        u = 0;
    }
    if(normalizedPosition != NULL) {
        *normalizedPosition = u;
    }

    return interpolated(p1, p2, u);
}

//----------------------------------------------------------
// a much faster but less accurate version would check distances to vertices first,
// which assumes vertices are evenly spaced
QVector3D polyline::get_closest(const QVector3D& target, unsigned int* nearestIndex) const {
    const polyline & polyline = *this;

    if(polyline.size() < 2)
    {
        if(nearestIndex != NULL)
        {
            nearestIndex = 0;
        }
        return target;
    }

    float distance = 0;
    QVector3D nearestPoint;
    unsigned int nearest = 0;
    float normalizedPosition = 0;
    unsigned int lastPosition = polyline.size() - 1;
    if(polyline.isClosed())
    {
        lastPosition++;
    }
    for(int i = 0; i < (int) lastPosition; i++) {
        bool repeatNext = i == (int) (polyline.size() - 1);

        const QVector3D& cur = polyline[i];
        const QVector3D& next = repeatNext ? polyline[0] : polyline[i + 1];

        float curNormalizedPosition = 0;
        QVector3D curNearestPoint = get_closest_util(cur, next, target, &curNormalizedPosition);
        float curDistance = ::distance(curNearestPoint, target);
        if(i == 0 || curDistance < distance)
        {
            distance = curDistance;
            nearest = i;
            nearestPoint = curNearestPoint;
            normalizedPosition = curNormalizedPosition;
        }
    }

    if(nearestIndex != NULL)
    {
        if(normalizedPosition > .5)
        {
            nearest++;
            if(nearest == polyline.size())
            {
                nearest = 0;
            }
        }
        *nearestIndex = nearest;
    }

    return nearestPoint;
}

//--------------------------------------------------
bool polyline::inside(const QVector3D & p, const polyline & _polyline)
{
    return polyline::inside(p.x(),p.y(),_polyline);
}

//--------------------------------------------------
bool polyline::inside(float x, float y, const polyline & _polyline)
{
    int counter = 0;
    int i;
    double xinters;
    QVector3D p1,p2;

    int N = _polyline.size();

    p1 = _polyline[0];
    for (i=1;i<=N;i++)
    {
        p2 = _polyline[i % N];
        if (y > MIN(p1.y(),p2.y()))
        {
            if (y <= MAX(p1.y(),p2.y()))
            {
                if (x <= MAX(p1.x(),p2.x()))
                {
                    if (p1.y() != p2.y())
                    {
                        xinters = (y-p1.y())*(p2.x()-p1.x())/(p2.y()-p1.y())+p1.x();
                        if (p1.x() == p2.x() || x <= xinters)
                            counter++;
                    }
                }
            }
        }
        p1 = p2;
    }

    if (counter % 2 == 0) return false;
    else return true;
}

//--------------------------------------------------
bool polyline::inside(float x, float y) const
{
    return polyline::inside(x, y, *this);

}

//--------------------------------------------------
bool polyline::inside(const QVector3D & p) const
{
    return polyline::inside(p, *this);
}

//This is for polygon/contour simplification - we use it to reduce the number of points needed in
//representing the letters as openGL shapes - will soon be moved to ofGraphics.cpp

// From: http://softsurfer.com/Archive/algorithm_0205/algorithm_0205.htm
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

typedef struct
{
    QVector3D P0;
    QVector3D P1;
}Segment;

// dot product (3D) which allows vector operations in arguments
#define dot(u,v)   ((u).x() * (v).x() + (u).y() * (v).y() + (u).z() * (v).z())
#define norm2(v)   dot(v,v)        // norm2 = squared length of vector
#define norm(v)    sqrt(norm2(v))  // norm = length of vector
#define d2(u,v)    norm2(u-v)      // distance squared = norm2 of difference
#define d(u,v)     norm(u-v)       // distance = norm of difference

//--------------------------------------------------
static void simplifyDP(float tol, QVector3D* v, int j, int k, int* mk )
{
    if (k <= j+1) // there is nothing to simplify
        return;

    // check for adequate approximation by segment S from v[j] to v[k]
    int     maxi	= j;          // index of vertex farthest from S
    float   maxd2	= 0;         // distance squared of farthest vertex
    float   tol2	= tol * tol;  // tolerance squared
    Segment S		= {v[j], v[k]};  // segment from v[j] to v[k]
    QVector3D u;
    u				= S.P1 - S.P0;   // segment direction vector
    double  cu		= dot(u,u);     // segment length squared

    // test each vertex v[i] for max distance from S
    // compute using the Feb 2001 Algorithm's dist_QVector3D_to_Segment()
    // Note: this works in any dimension (2D, 3D, ...)
    QVector3D  w;
    QVector3D   Pb;                // base of perpendicular from v[i] to S
    float  b, cw, dv2;        // dv2 = distance v[i] to S squared

    for (int i=j+1; i<k; i++)
    {
        // compute distance squared
        w = v[i] - S.P0;
        cw = dot(w,u);
        if ( cw <= 0 ) dv2 = d2(v[i], S.P0);
        else if ( cu <= cw ) dv2 = d2(v[i], S.P1);
        else {
            b = (float)(cw / cu);
            Pb = S.P0 + u*b;
            dv2 = d2(v[i], Pb);
        }
        // test with current max distance squared
        if (dv2 <= maxd2) continue;

        // v[i] is a new max vertex
        maxi = i;
        maxd2 = dv2;
    }
    if (maxd2 > tol2)        // error is worse than the tolerance
    {
        // split the polyline at the farthest vertex from S
        mk[maxi] = 1;      // mark v[maxi] for the simplified polyline
        // recursively simplify the two subpolylines at v[maxi]
        simplifyDP( tol, v, j, maxi, mk );  // polyline v[j] to v[maxi]
        simplifyDP( tol, v, maxi, k, mk );  // polyline v[maxi] to v[k]
    }
    // else the approximation is OK, so ignore intermediate vertices
    return;
}

//--------------------------------------------------
void polyline::simplify(float tol)
{
    if(points.size() < 2) return;

    int n = size();

    if(n == 0)
    {
        return;
    }

    QVector <QVector3D> sV;
    sV.resize(n);

    int    i, k, m, pv;            // misc counters
    float  tol2 = tol * tol;       // tolerance squared
    QVector<QVector3D> vt;
    QVector<int> mk;
    vt.resize(n);
    mk.resize(n);
    mk.fill(0);

    // STAGE 1.  Vertex Reduction within tolerance of prior vertex cluster
    vt[0] = points[0];              // start at the beginning
    for (i=k=1, pv=0; i<n; i++)
    {
        if (d2(points[i], points[pv]) < tol2) continue;

        vt[k++] = points[i];
        pv = i;
    }
    if (pv < n-1) vt[k++] = points[n-1];      // finish at the end

    // STAGE 2.  Douglas-Peucker polyline simplification
    mk[0] = mk[k-1] = 1;       // mark the first and last vertices
    simplifyDP( tol, &vt[0], 0, k-1, &mk[0] );

    // copy marked vertices to the output simplified polyline
    for (i=m=0; i<k; i++) {
        if (mk[i]) sV[m++] = vt[i];
    }

    //get rid of the unused points
    if( m < (int)sV.size() )
    {
        points.clear();
        for (int j = 0; j < m; j++)
        {
           points.append(sV.at(j));
        }
        //points.assign( sV.begin(),sV.begin()+m );
    }
    else
    {
        points = sV;
    }

}

//--------------------------------------------------
void polyline::draw()
{
    //ofGetCurrentRenderer()->draw(*this);
}

//--------------------------------------------------
void polyline::set_right_vector(QVector3D v)
{
    rightVector = v;
    flag_hasChanged();
}

//--------------------------------------------------
QVector3D polyline::get_right_vector() const
{
    return rightVector;
}


//--------------------------------------------------
float polyline::get_index_length(float length) const
{
    if(points.size() < 2) return 0;
    update_cache();

    float totalLength = get_perimeter();
    length = clamp(length, 0, totalLength);

    int lastPointIndex = isClosed() ? points.size() : points.size()-1;

    int i1 = clamp(floor(length / totalLength * lastPointIndex), 0, lengths.size()-2);   // start approximation here
    int leftLimit = 0;
    int rightLimit = lastPointIndex;

    float distAt1, distAt2;
    for(int iterations = 0; iterations < 32; iterations ++) {	// limit iterations
        i1 = clamp(i1, 0, lengths.size()-1);
        distAt1 = lengths[i1];
        if(distAt1 <= length) {         // if Length at i1 is less than desired Length (this is good)
            distAt2 = lengths[i1+1];
            if(distAt2 > length) {
                float t = map(length, distAt1, distAt2, 0, 1);
                return i1 + t;
            } else {
                leftLimit = i1;
            }
        } else {
            rightLimit = i1;
        }
        i1 = (leftLimit + rightLimit)/2;
    }
    return 0;
}


//--------------------------------------------------
float polyline::get_index_percent(float f) const
{
    return get_index_length(f * get_perimeter());
}

//--------------------------------------------------
float polyline::get_length_index(int index) const
{
    if(points.size() < 2) return 0;
    update_cache();
    return lengths[get_wrapped_index(index)];
}

//--------------------------------------------------
float polyline::get_length_index_interpolated(float findex) const
{
    if(points.size() < 2) return 0;
    update_cache();
    int i1, i2;
    float t;
    get_interpolation_params(findex, i1, i2, t);
    return lerp(get_length_index(i1), get_length_index(i2), t);
}


//--------------------------------------------------
QVector3D polyline::get_point_length(float f) const
{
    if(points.size() < 2) return QVector3D();
    update_cache();
    return get_point_index_interpolated(get_index_length(f));
}

//--------------------------------------------------
QVector3D polyline::get_point_percent(float f) const
{
    float length = get_perimeter();
    return get_point_length(f * length);
}


//--------------------------------------------------
QVector3D polyline::get_point_index_interpolated(float findex) const
{
    if(points.size() < 2) return QVector3D();
    int i1, i2;
    float t;
    get_interpolation_params(findex, i1, i2, t);
    QVector3D leftPoint(points[i1]);
    QVector3D rightPoint(points[i2]);
    return interpolated(leftPoint, rightPoint, t);
}


//--------------------------------------------------
float polyline::get_angle_index(int index) const
{
    if(points.size() < 2) return 0;
    update_cache();
    return angles[get_wrapped_index(index)];
}

//--------------------------------------------------
float polyline::get_angle_index_interpolated(float findex) const
{
    if(points.size() < 2) return 0;
    int i1, i2;
    float t;
    get_interpolation_params(findex, i1, i2, t);
    return lerp(get_angle_index(i1), get_angle_index(i2), t);
}

//--------------------------------------------------
QVector3D polyline::get_rotation_index(int index) const
{
    if(points.size() < 2) return QVector3D();
    update_cache();
    return rotations[get_wrapped_index(index)];
}

//--------------------------------------------------
QVector3D polyline::get_rotation_index_interpolated(float findex) const
{
    if(points.size() < 2) return QVector3D();
    int i1, i2;
    float t;
    get_interpolation_params(findex, i1, i2, t);
    return interpolated(get_rotation_index(i1), get_rotation_index(i2), t);
}

//--------------------------------------------------
QVector3D polyline::get_tangent_index(int index) const {
    if(points.size() < 2) return QVector3D();
    update_cache();
    return tangents[get_wrapped_index(index)];
}

//--------------------------------------------------
QVector3D polyline::get_tangent_index_interpolated(float findex) const
{
    if(points.size() < 2) return QVector3D();
    int i1, i2;
    float t;
    get_interpolation_params(findex, i1, i2, t);
    return interpolated(get_tangent_index(i1), get_tangent_index(i2), t);
}

//--------------------------------------------------
QVector3D polyline::get_normal_index(int index) const
{
    if(points.size() < 2) return QVector3D();
    update_cache();
    return normals[get_wrapped_index(index)];
}

//--------------------------------------------------
QVector3D polyline::get_normal_index_interpolated(float findex) const
{
    if(points.size() < 2) return QVector3D();
    int i1, i2;
    float t;
    get_interpolation_params(findex, i1, i2, t);
    return interpolated(get_normal_index(i1), get_normal_index(i2), t);
}


//--------------------------------------------------
void polyline::calc_data(int index, QVector3D &tangent, float &angle, QVector3D &rotation, QVector3D &normal) const {
    int i1 = get_wrapped_index(index - 1);
    int i2 = get_wrapped_index(index);
    int i3 = get_wrapped_index(index + 1);

    QVector3D p1(points[i1]);
    QVector3D p2(points[i2]);
    QVector3D p3(points[i3]);

    QVector3D v1(p1 - p2); // vector to previous point
    QVector3D v2(p3 - p2); // vector to next point
    v1.normalize();
    v2.normalize();

    tangent = (v2 - v1);
    tangent.normalize();

    rotation = crossed(v1,v2);
    angle = 180 - rad_deg(acos(clamp(v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z(), -1, 1)));

    normal = crossed(rightVector,tangent);
    normal.normalize();
}


//--------------------------------------------------
int polyline::get_wrapped_index(int index) const
{
    if(points.empty()) return 0;

    if(index < 0) return isClosed() ? (index + points.size()) % points.size() : 0;
    if(index > int(points.size())-1) return isClosed() ? index % points.size() : points.size() - 1;
    return index;
}

//--------------------------------------------------
void polyline::get_interpolation_params(float findex, int &i1, int &i2, float &t) const {
    i1 = floor(findex);
    t = findex - i1;
    i1 = get_wrapped_index(i1);
    i2 = get_wrapped_index(i1 + 1);
}

//--------------------------------------------------
void polyline::update_cache(bool bForceUpdate) const
{
    if(bCacheIsDirty || bForceUpdate)
    {
        lengths.clear();
        angles.clear();
        rotations.clear();
        normals.clear();
        tangents.clear();
        area = 0;
        centroid2D = QVector3D(0, 0, 0);
        bCacheIsDirty = false;

        if(points.size() < 2) return;

        // area
        for(int i=0;i<(int)points.size()-1;i++)
        {
            area += points[i].x() * points[i+1].y() - points[i+1].x() * points[i].y();
        }
        area += points[points.size()-1].x() * points[0].y() - points[0].x() * points[points.size()-1].y();
        area *= 0.5;


        // centroid
        // TODO: doesn't seem to work on all concave shapes
        for(int i=0;i<(int)points.size()-1;i++)
        {
            centroid2D.setX(centroid2D.x() + (points[i].x() + points[i+1].x()) * (points[i].x()*points[i+1].y() - points[i+1].x()*points[i].y()));
            centroid2D.setY(centroid2D.y() + (points[i].y() + points[i+1].y()) * (points[i].x()*points[i+1].y() - points[i+1].x()*points[i].y()));
        }
        centroid2D.setX(centroid2D.x() + (points[points.size()-1].x() + points[0].x()) * (points[points.size()-1].x()*points[0].y()
                - points[0].x()*points[points.size()-1].y()));
        centroid2D.setY(centroid2D.y() + (points[points.size()-1].y() + points[0].y()) * (points[points.size()-1].x()*points[0].y()
                - points[0].x()*points[points.size()-1].y()));

        centroid2D.setX(centroid2D.x() / (6*area));
        centroid2D.setY(centroid2D.y() / (6*area));


        // per vertex cache
        lengths.resize(points.size());
        tangents.resize(points.size());
        angles.resize(points.size());
        normals.resize(points.size());
        rotations.resize(points.size());

        float angle;
        QVector3D rotation;
        QVector3D normal;
        QVector3D tangent;

        float length = 0;
        for(int i=0; i<(int)points.size(); i++)
        {
            lengths[i] = length;

            calc_data(i, tangent, angle, rotation, normal);
            tangents[i] = tangent;
            angles[i] = angle;
            rotations[i] = rotation;
            normals[i] = normal;

            length += distance(points[i], points[get_wrapped_index(i + 1)]);
        }

        if(isClosed()) lengths.push_back(length);
    }
}

