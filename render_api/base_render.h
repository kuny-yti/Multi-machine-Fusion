#ifndef BASE_RENDER_H
#define BASE_RENDER_H

#include "type_define.h"
#include "polyline.h"

class base_render
{
public:
    ~base_render();
    virtual const QString & type()=0;

    virtual void update()=0;

    virtual void draw(polyline & poly)=0;
    virtual void draw(ofPath & shape)=0;
    virtual void draw(ofMesh & vertexData, bool useColors, bool useTextures, bool useNormals)=0;
    virtual void draw(ofMesh & vertexData, ofPolyRenderMode renderType, bool useColors, bool useTextures, bool useNormals)=0;
    virtual void draw(of3dPrimitive& model, ofPolyRenderMode renderType)=0;
    virtual void draw(ofImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh)=0;
    virtual void draw(ofFloatImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh)=0;
    virtual void draw(ofShortImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh)=0;

    //--------------------------------------------
    // transformations
    virtual void push_view(){}
    virtual void pop_view(){}

    // setup matrices and viewport (upto you to push and pop view before and after)
    // if width or height are 0, assume windows dimensions (ofGetWidth(), ofGetHeight())
    // if nearDist or farDist are 0 assume defaults (calculated based on width / height)
    virtual void set_viewport(QRect viewport){}
    virtual void set_viewport(float x = 0, float y = 0, float width = 0, float height = 0){}
    virtual void set_perspective(float width = 0, float height = 0, float fov = 60, float nearDist = 0, float farDist = 0){}
    virtual void set_ortho(float width = 0, float height = 0, float nearDist = -1, float farDist = 1){}
    //virtual void setOrientation(ofOrientation orientation, bool vFlip){};
    virtual QRect get_current_viewport(){return QRect();}
    virtual QRect get_native_viewport(){return get_current_viewport();}
    virtual QRect get_viewport(){return QRect();}
    //virtual bool isVFlipped() const{return true;}

    //virtual void setCoordHandedness(ofHandednessType handedness){};
    //virtual ofHandednessType getCoordHandedness(){return OF_LEFT_HANDED;};

    //our openGL wrappers
    virtual void push_matrix(){}
    virtual void pop_matrix(){}
    virtual QMatrix4x4 get_current_matrix(MatrixMode matrixMode_)
    const { return QMatrix4x4();}

    virtual void translate(float x, float y, float z = 0){}
    virtual void translate(const QVector3D & p){}

    virtual void scale(float xAmnt, float yAmnt, float zAmnt = 1){}
    virtual void scale(const QVector3D & amnt){}

    virtual void rotate(float degrees, float vecX, float vecY, float vecZ){}
    virtual void rotate(const QVector4D & degrees){}
    virtual void rotateX(float degrees){}
    virtual void rotateY(float degrees){}
    virtual void rotateZ(float degrees){}
    virtual void rotate(float degrees){}

    virtual void set_matrix_mode(MatrixMode mode){}
    virtual void matrix_load_identity (void){}
    virtual void matrix_load (const QMatrix4x4 & m){}
    virtual void matrix_load (const float *m){}
    virtual void matrix_mult (const QMatrix4x4 & m){}
    virtual void matrix_mult (const float *m){}

    // screen coordinate things / default gl values
    virtual void setupGraphicDefaults(){}
    virtual void setupScreen(){}

    // drawing modes
    virtual void set_rect_mode(ofRectMode mode)=0;
    virtual ofRectMode get_rect_mode()=0;
    virtual void setFillMode(ofFillFlag fill)=0;
    virtual ofFillFlag getFillMode()=0;
    virtual void line_width(float lineWidth)=0;
    virtual void depth_test(bool depthTest)=0;
    virtual void blend_mode(ofBlendMode blendMode)=0;
    virtual void line_smoothing(bool smooth)=0;
    virtual void circle_resolution(int res){}
    virtual void enable_point_sprites(){}
    virtual void disable_point_sprites(){}
    virtual void enable_antia_aliasing(){}
    virtual void disable_anti_aliasing(){}

    // color options
    virtual void color(int r, int g, int b){} // 0-255
    virtual void color(int r, int g, int b, int a){} // 0-255
    virtual void color(const QColor & color){}
    virtual void color(int gray){} // new set a color as grayscale with one argument
    virtual void hex_color( int hexColor ){} // hex, like web 0xFF0033;

    // bg color
    virtual ofFloatColor & getBgColor()=0;
    virtual bool bClearBg(){return true;}
    virtual void background(const QColor & c){}
    virtual void background(float brightness){}
    virtual void background(int hexColor, float _a=255.0f){}
    virtual void background(int r, int g, int b, int a=255){}

    virtual void set_background_auto(bool bManual){}		// default is true

    virtual void clear(float r, float g, float b, float a=0){}
    virtual void clear(float brightness, float a=0){}
    virtual void clear_alpha(){}

    // drawing
    virtual void draw_line(float x1, float y1, float z1, float x2, float y2, float z2)=0;
    virtual void draw_rectangle(float x, float y, float z, float w, float h)=0;
    virtual void draw_triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)=0;
    virtual void draw_circle(float x, float y, float z, float radius)=0;
    virtual void draw_ellipse(float x, float y, float z, float width, float height)=0;
    virtual void draw_string(QString text, float x, float y, float z, ofDrawBitmapMode mode)=0;


    // returns true if the renderer can render curves without decomposing them
    virtual bool rendersPathPrimitives()=0;
};

#endif // BASE_RENDER_H
