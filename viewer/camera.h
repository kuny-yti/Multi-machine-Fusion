#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include "type_define.h"

class camera_p;
class camera: public QObject
{
    Q_OBJECT
private:
    camera_p *d_ptr;

    camera_p *d_func() const { return d_ptr; }
public:

public:
    camera(QObject *parent = 0);
    ~camera();

    ProjectionType type_projection() const;
    void type_projection(ProjectionType value);

    float field_view() const;///视野
    void field_view(float angle);

    float plane_near() const;///近剪裁面(默认5)
    void plane_near(float value);

    float plane_far() const;///远剪裁面(默认1000)
    void plane_far(float value);

    QSizeF size_view() const;///观看尺寸(默认为(2,2))
    void size_view(const QSizeF& size);

    QSizeF size_view_min() const;///近剪裁面的最小值(默认0.0001,0.0001)
    void size_view_min(const QSizeF& size);

    int rotation_screen() const;///屏幕旋转角度
    void rotation_screen(int angle);

    QVector3D eye() const;///眼睛位置
    void eye(const QVector3D& vertex);

    QVector3D upVector() const;///朝向 默认(0,1,0)
    void upVector(const QVector3D& vector);

    QVector3D center() const;///观看中心位置
    void center(const QVector3D& vertex);

    float eye_separation() const;///两眼中的中点，用于立体图形的分离
    void eye_separation(float value);

    QVector3D adjustment_motion() const;
    void adjustment_motion(const QVector3D& vector);

    bool aspect_ratio_adjust() const;
    void aspect_ratio_adjust(bool value);

    QQuaternion tilt(float angle) const;///倾斜角度
    QQuaternion pan(float angle) const;///平移视图
    QQuaternion roll(float angle) const;///滚动视图

    void eye_rotate(const QQuaternion& q);///根据眼睛位置旋转
    void center_rotate(const QQuaternion& q);///根据中心位置旋转

    QVector3D translation(float x, float y, float z) const;///返回平移向量

    QMatrix4x4 matrix_projection(float aspectRatio) const;
    QMatrix4x4 matrix_model_view(Eye eye = EYE_NO) const;

    QVector3D map_point
        (const QPoint& point, float aspectRatio,
         const QSize& viewportSize) const;

    camera *clone(QObject *parent = 0) const;

public Q_SLOTS:
    void eye_translate(float x, float y, float z);///平移眼睛位置
    void center_translate(float x, float y, float z);///平移中心位置

    void center_tilt_pan_roll(float tiltAngle, float panAngle, float rollAngle,
                           RotateOrder order = TiltPanRoll);
    void eye_tilt_pan_roll(float tiltAngle, float panAngle, float rollAngle,
                        RotateOrder order = TiltPanRoll);

Q_SIGNALS:
    void changed_projection();
    void changed_view();
};

#endif // CAMERA_H
