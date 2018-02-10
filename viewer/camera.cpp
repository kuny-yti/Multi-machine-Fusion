#include "camera.h"
#include <QVector3D>
#include <qmath.h>
#include <QQuaternion>
#include <QMatrix4x4>

class camera_p
{
public:
    camera_p():
        projectionType(Perspective),
        fieldOfView(0.0f),
        nearPlane(5.0f),
        farPlane(1000.0f),
        viewSize(1.0f, 1.0f),
        minViewSize(0.0001f, 0.0001f),
        screenRotation(0),
        eye(0.0f, 0.0f, 10.0f),
        upVector(0.0f, 1.0f, 0.0f),
        center(0.0f, 0.0f, 0.0f),
        viewVector(0.0f, 0.0f, -10.0f),
        eyeSeparation(0.0f),
        motionAdjustment(0.0f, 0.0f, 1.0f),
        adjustForAspectRatio(true)
  {
  }

    ProjectionType projectionType;
    float fieldOfView;
    float nearPlane;
    float farPlane;
    QSizeF viewSize;
    QSizeF minViewSize;
    int screenRotation;
    QVector3D eye;
    QVector3D upVector;
    QVector3D center;
    QVector3D viewVector;
    float eyeSeparation;
    QVector3D motionAdjustment;
    QQuaternion motionQuaternion;
    bool adjustForAspectRatio;
};

camera::camera(QObject *parent):
    QObject(parent),
    d_ptr(new camera_p)
{
}
camera::~camera()
{
    delete d_ptr;
}

ProjectionType camera::type_projection() const
{
    return d_ptr->projectionType;
}

void camera::type_projection(ProjectionType value)
{
    if (d_ptr->projectionType != value)
    {
        d_ptr->projectionType = value;
        emit changed_projection();
    }
}

float camera::field_view() const
{
    return d_ptr->fieldOfView;
}

void camera::field_view(float angle)
{
    if (d_ptr->fieldOfView != angle)
    {
        d_ptr->fieldOfView = angle;
        emit changed_projection();
    }
}

float camera::plane_near() const
{
    return d_ptr->nearPlane;
}

void camera::plane_near(float value)
{
    if (d_ptr->nearPlane != value)
    {
        d_ptr->nearPlane = value;
        emit changed_projection();
    }
}

float camera::plane_far() const
{
    return d_ptr->farPlane;
}

void camera::plane_far(float value)
{
    if (d_ptr->farPlane != value)
    {
        d_ptr->farPlane = value;
        emit changed_projection();
    }
}

QSizeF camera::size_view() const
{
    return d_ptr->viewSize;
}

void camera::size_view(const QSizeF& size)
{

    QSizeF sz(size);
    if (qAbs(sz.width()) < d_ptr->minViewSize.width())
    {
        if (sz.width() >= 0.0f)
            sz.setWidth(d_ptr->minViewSize.width());
        else
            sz.setWidth(-d_ptr->minViewSize.width());
    }
    if (qAbs(sz.height()) < d_ptr->minViewSize.height())
    {
        if (sz.height() >= 0.0f)
            sz.setHeight(d_ptr->minViewSize.height());
        else
            sz.setHeight(-d_ptr->minViewSize.height());
    }
    if (d_ptr->viewSize != sz)
    {
        d_ptr->viewSize = sz;
        emit changed_projection();
    }
}

QSizeF camera::size_view_min() const
{
    return d_ptr->minViewSize;
}

void camera::size_view_min(const QSizeF& size)
{
    if (d_ptr->minViewSize != size)
    {
        d_ptr->minViewSize = size;
        emit changed_projection();
    }
}

int camera::rotation_screen() const
{
    return d_ptr->screenRotation;
}

void camera::rotation_screen(int angle)
{

    if (d_ptr->screenRotation != angle)
    {
        d_ptr->screenRotation = angle;
        emit changed_projection();
    }
}

QVector3D camera::eye() const
{

    return d_ptr->eye;
}

void camera::eye(const QVector3D& vertex)
{

    if (d_ptr->eye != vertex)
    {
        d_ptr->eye = vertex;
        d_ptr->viewVector = d_ptr->center - d_ptr->eye;
        emit changed_view();
    }
}

void camera::eye_translate(float x, float y, float z)
{

    d_ptr->eye += translation(x, y, z);
    d_ptr->viewVector = d_ptr->center - d_ptr->eye;
    emit changed_view();
}

QVector3D camera::upVector() const
{

    return d_ptr->upVector;
}

void camera::upVector(const QVector3D& vector)
{

    if (d_ptr->upVector != vector)
    {
        d_ptr->upVector = vector;
        emit changed_view();
    }
}

QVector3D camera::center() const
{

    return d_ptr->center;
}

void camera::center(const QVector3D& vertex)
{

    if (d_ptr->center != vertex)
    {
        d_ptr->center = vertex;
        d_ptr->viewVector = d_ptr->center - d_ptr->eye;
        emit changed_view();
    }
}

void camera::center_translate(float x, float y, float z)
{

    d_ptr->center += translation(x, y, z);
    d_ptr->viewVector = d_ptr->center - d_ptr->eye;
    emit changed_view();
}

float camera::eye_separation() const
{

    return d_ptr->eyeSeparation;
}

void camera::eye_separation(float value)
{

    if (d_ptr->eyeSeparation != value)
    {
        d_ptr->eyeSeparation = value;
        emit changed_view();
    }
}

QVector3D camera::adjustment_motion() const
{

    return d_ptr->motionAdjustment;
}
void camera::adjustment_motion(const QVector3D& vector)
{

    if (d_ptr->motionAdjustment != vector)
    {
        d_ptr->motionAdjustment = vector;
        if (vector.x() == 0.0f && vector.y() == 0.0f)
        {
            // If the vector is centered, then don't perform any rotations.
            d_ptr->motionQuaternion = QQuaternion();
        } else {
            // Determine the pan and tilt angles from the vector.
            QVector3D view = -vector.normalized();
            if (view.z() < 0.0f)
                view = -view;
            float xangle = asinf(view.x()) * 180.0f / M_PI;
            float yangle = asinf(-view.y()) * 180.0f / M_PI;

            // Construct the pan and tilt quaternions.
            if (qFuzzyIsNull(xangle))
                d_ptr->motionQuaternion = tilt(yangle);
            else if (qFuzzyIsNull(yangle))
                d_ptr->motionQuaternion = pan(xangle);
            else
                d_ptr->motionQuaternion = tilt(yangle) * pan(xangle);
        }
        emit changed_view();
    }
}

bool camera::aspect_ratio_adjust() const
{

    return d_ptr->adjustForAspectRatio;
}

void camera::aspect_ratio_adjust(bool value)
{

    if (d_ptr->adjustForAspectRatio != value)
    {
        d_ptr->adjustForAspectRatio = value;
        emit changed_view();
    }
}

QQuaternion camera::tilt(float angle) const
{

    QVector3D side = QVector3D::crossProduct(d_ptr->viewVector, d_ptr->upVector);
    return QQuaternion::fromAxisAndAngle(side, angle);
}

QQuaternion camera::pan(float angle) const
{

    return QQuaternion::fromAxisAndAngle(d_ptr->upVector, angle);
}

QQuaternion camera::roll(float angle) const
{

    return QQuaternion::fromAxisAndAngle(d_ptr->viewVector, angle);
}

void camera::eye_rotate(const QQuaternion& q)
{

    d_ptr->upVector = q.rotatedVector(d_ptr->upVector);
    d_ptr->viewVector = q.rotatedVector(d_ptr->viewVector);
    d_ptr->center = d_ptr->eye + d_ptr->viewVector;
    emit changed_view();
}

void camera::center_rotate(const QQuaternion& q)
{

    d_ptr->upVector = q.rotatedVector(d_ptr->upVector);
    d_ptr->viewVector = q.rotatedVector(d_ptr->viewVector);
    d_ptr->eye = d_ptr->center - d_ptr->viewVector;
    emit changed_view();
}

QVector3D camera::translation(float x, float y, float z) const
{

    QVector3D vector(0.0f, 0.0f, 0.0f);
    if (x != 0.0f)
        vector += QVector3D::normal(d_ptr->viewVector, d_ptr->upVector) * x;
    if (y != 0.0f)
        vector += d_ptr->upVector.normalized() * y;
    if (z != 0.0f)
        vector += d_ptr->viewVector.normalized() * z;
    return vector;
}

QMatrix4x4 camera::matrix_projection(float aspectRatio) const
{

    QMatrix4x4 m;
    if (!d_ptr->adjustForAspectRatio)
        aspectRatio = 1.0f;
    if (d_ptr->screenRotation != 0) {
        m.rotate(float(d_ptr->screenRotation), 0.0f, 0.0f, 1.0f);
        if (d_ptr->screenRotation == 90 || d_ptr->screenRotation == 270) {
            if (aspectRatio != 0.0f)
                aspectRatio = 1.0f / aspectRatio;
        }
    }
    if (d_ptr->projectionType == Perspective && d_ptr->fieldOfView != 0.0f) {
        m.perspective(d_ptr->fieldOfView, aspectRatio,
                      d_ptr->nearPlane, d_ptr->farPlane);
    } else {
        float halfWidth = d_ptr->viewSize.width() / 2.0f;
        float halfHeight = d_ptr->viewSize.height() / 2.0f;
        if (aspectRatio > 1.0f) {
            halfWidth *= aspectRatio;
        } else if (aspectRatio > 0.0f && aspectRatio < 1.0f) {
            halfHeight /= aspectRatio;
        }
        if (d_ptr->projectionType == Perspective) {
            m.frustum(-halfWidth, halfWidth, -halfHeight, halfHeight,
                      d_ptr->nearPlane, d_ptr->farPlane);
        } else {
            m.ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                    d_ptr->nearPlane, d_ptr->farPlane);
        }
    }
    return m;
}

QMatrix4x4 camera::matrix_model_view(Eye eye) const
{

    QMatrix4x4 m;
    QVector3D adjust;
    if (eye == EYE_LEFT)
        adjust = translation(-d_ptr->eyeSeparation / 2.0f, 0.0f, 0.0f);
    else if (eye == EYE_RIGHT)
        adjust = translation(d_ptr->eyeSeparation / 2.0f, 0.0f, 0.0f);
    if (d_ptr->motionQuaternion.isIdentity()) {
        m.lookAt(d_ptr->eye + adjust, d_ptr->center, d_ptr->upVector);
    }
    else
    {
        QVector3D up = d_ptr->motionQuaternion.rotatedVector(d_ptr->upVector);
        QVector3D view = d_ptr->motionQuaternion.rotatedVector(d_ptr->viewVector);
        m.lookAt(d_ptr->center - view + adjust, d_ptr->center, up);
    }
    return m;
}

QVector3D camera::map_point
    (const QPoint& point, float aspectRatio, const QSize& viewportSize) const
{
    // Rotate the co-ordinate system to account for the screen rotation.
    int x = point.x();
    int y = point.y();
    int width = viewportSize.width();
    int height = viewportSize.height();
    if (!d_ptr->adjustForAspectRatio)
        aspectRatio = 1.0f;
    if (d_ptr->screenRotation == 90)
    {
        if (aspectRatio != 0.0f)
            aspectRatio = 1.0f / aspectRatio;
        qSwap(x, y);
        qSwap(width, height);
        y = height - 1 - y;
    }
    else if (d_ptr->screenRotation == 180)
    {
        x = width - 1 - x;
        y = height - 1 - y;
    }
    else if (d_ptr->screenRotation == 270)
    {
        if (aspectRatio != 0.0f)
            aspectRatio = 1.0f / aspectRatio;
        qSwap(x, y);
        qSwap(width, height);
    }

    // Determine the relative distance from the middle of the screen.
    // After this xrel and yrel are typically between -1.0 and +1.0
    // (unless the point was outside the viewport).  The yrel is
    // flipped upside down to account for the incoming co-ordinate
    // being left-handed, but the world being right-handed.
    float xrel, yrel;
    if (width)
        xrel = ((float(x * 2)) - float(width)) / float(width);
    else
        xrel = 0.0f;
    if (height)
        yrel = -((float(y * 2)) - float(height)) / float(height);
    else
        yrel = 0.0f;

    // Reverse the projection and return the point in world co-ordinates.
    QMatrix4x4 m = matrix_projection(aspectRatio);
    QMatrix4x4 invm = m.inverted();
    return invm.map(QVector3D(xrel, yrel, -1.0f));
}

void camera::center_tilt_pan_roll
    (float tiltAngle, float panAngle, float rollAngle,
     RotateOrder order)
{
    switch (order) {
    case TiltPanRoll:
        center_rotate(roll(rollAngle) * pan(panAngle) * tilt(tiltAngle));
        break;
    case TiltRollPan:
        center_rotate(pan(panAngle) * roll(rollAngle) * tilt(tiltAngle));
        break;
    case PanTiltRoll:
        center_rotate(roll(rollAngle) * tilt(tiltAngle) * pan(panAngle));
        break;
    case PanRollTilt:
        center_rotate(tilt(tiltAngle) * roll(rollAngle) * pan(panAngle));
        break;
    case RollTiltPan:
        center_rotate(pan(panAngle) * tilt(tiltAngle) * roll(rollAngle));
        break;
    case RollPanTilt:
        center_rotate(tilt(tiltAngle) * pan(panAngle) * roll(rollAngle));
        break;
    }
}

void camera::eye_tilt_pan_roll
    (float tiltAngle, float panAngle, float rollAngle,
     RotateOrder order)
{
    switch (order) {
    case TiltPanRoll:
        eye_rotate(roll(rollAngle) * pan(panAngle) * tilt(tiltAngle));
        break;
    case TiltRollPan:
        eye_rotate(pan(panAngle) * roll(rollAngle) * tilt(tiltAngle));
        break;
    case PanTiltRoll:
        eye_rotate(roll(rollAngle) * tilt(tiltAngle) * pan(panAngle));
        break;
    case PanRollTilt:
        eye_rotate(tilt(tiltAngle) * roll(rollAngle) * pan(panAngle));
        break;
    case RollTiltPan:
        eye_rotate(pan(panAngle) * tilt(tiltAngle) * roll(rollAngle));
        break;
    case RollPanTilt:
        eye_rotate(tilt(tiltAngle) * pan(panAngle) * roll(rollAngle));
        break;
    }
}

camera *camera::clone(QObject *parent) const
{

    camera *copy = new camera(parent);
    copy->d_func()->projectionType = d_ptr->projectionType;
    copy->d_func()->fieldOfView = d_ptr->fieldOfView;
    copy->d_func()->nearPlane = d_ptr->nearPlane;
    copy->d_func()->farPlane = d_ptr->farPlane;
    copy->d_func()->viewSize = d_ptr->viewSize;
    copy->d_func()->minViewSize = d_ptr->minViewSize;
    copy->d_func()->screenRotation = d_ptr->screenRotation;
    copy->d_func()->eye = d_ptr->eye;
    copy->d_func()->upVector = d_ptr->upVector;
    copy->d_func()->center = d_ptr->center;
    copy->d_func()->viewVector = d_ptr->viewVector;
    copy->d_func()->eyeSeparation = d_ptr->eyeSeparation;
    copy->d_func()->motionAdjustment = d_ptr->motionAdjustment;
    copy->d_func()->adjustForAspectRatio = d_ptr->adjustForAspectRatio;
    return copy;
}
