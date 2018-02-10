

varying vec2 v_texcoord_media;
varying vec2 v_texcoord_multi1;
varying vec2 v_texcoord_multi2;

void main()
{
    v_texcoord_media = gl_MultiTexCoord0.st;
    v_texcoord_multi1 = gl_MultiTexCoord1.st;
    v_texcoord_multi2 = gl_MultiTexCoord2.st;

    gl_Position = ftransform();
}
