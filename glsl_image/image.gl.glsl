
varying vec2 v_texcoord_media;
varying vec2 v_texcoord_multi1;
varying vec2 v_texcoord_multi2;

uniform mat4 color_matrix;
uniform vec4 color_primary;

uniform sampler2D tex_src;
uniform sampler2D tex_dst_1;
uniform sampler2D tex_dst_2;

uniform int tex_dst_state;

vec3 adjust_color(vec3 rgb)
{
    vec4 rgbw = color_matrix * vec4(rgb, 1.0);
    float w = max(rgbw.w, 0.0001);
    return clamp(rgbw.rgb / w, 0.0, 1.0);
}

vec4 texture_mulit()
{
    vec4 col, col1;
    vec4 outCol = vec4(1,1,1,1);

    if ((tex_dst_state & (1<<1)) != 0)
    {
       col =  texture2D(tex_dst_1, v_texcoord_multi1);
       outCol = outCol *col;
    }
    if ((tex_dst_state & (1 << 2)) != 0)
    {
       col1 =  texture2D(tex_dst_2, v_texcoord_multi2);
       outCol = outCol * col1;
    }
    if (tex_dst_state == 0)
    {
        return vec4(1, 1, 1, 1);
    }
    return outCol;
}

void main(void)
{
    vec4 col_media, col_mulit;
    vec3 out_media;
    col_media = texture2D(tex_src, v_texcoord_media);
    out_media = adjust_color(col_media.rgb);

    col_media = vec4(out_media, 1.0) *
            vec4(color_primary.x, color_primary.y, color_primary.z, color_primary.w);
    col_mulit = texture_mulit();

    gl_FragColor = col_media * col_mulit;
}

