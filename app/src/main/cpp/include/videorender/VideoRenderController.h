//
// Created by Macro on 2020/6/8.
//

#ifndef JJPLAYER_VIDEORENDERCONTROLLER_H
#define JJPLAYER_VIDEORENDERCONTROLLER_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <egl/egl_core.h>
#include "es_manager.h"

extern "C"{
#include <libavutil/frame.h>
};
static char* OUTPUT_VIEW_VERTEX_SHADER =

        "attribute vec4 position;    \n"
        "attribute vec2 texcoord;   \n"
        "uniform mat4 modelViewProjectionMatrix;\n"
        "uniform mat4 rotateMatrix;\n"
        "varying vec2 v_texcoord;     \n"
        "void main(void)               \n"
        "{                            \n"
        "   gl_Position =modelViewProjectionMatrix*rotateMatrix*position;  \n"
        "   v_texcoord = texcoord.xy;  \n"
        "}                            \n";


static char* OUTPUT_VIEW_FRAG_SHADER =
        "varying highp vec2 v_texcoord;\n"
        "uniform sampler2D s_texture_y;\n"
        "uniform sampler2D s_texture_u;\n"
        "uniform sampler2D s_texture_v;\n"

        "void main() {\n"
            "highp float y = texture2D(s_texture_y, v_texcoord).r; \n"
            "highp float u = texture2D(s_texture_u, v_texcoord).r - 0.5;\n"
            "highp float v = texture2D(s_texture_v, v_texcoord).r - 0.5;\n"
            "highp float r = y + 1.402 * v;\n"
            "highp float g = y - 0.344 * u - 0.714 * v;\n"
            "highp float b = y + 1.772 * u;\n"
            "gl_FragColor = vec4(r,g,b,1.0);\n"
        "}          \n";
//todo surface view 渲染的控制类
class VideoRenderController {
private:
    ESManager *mEsManager;
    EGLSurface _surface;
    int screenwidth;
    int screenheight;
    GLint mSamplers[3];
    GLuint mTextures[3];
    GLint mVertextCoordsLoc;
    GLint mTextCoordsLoc;
    GLint mMatrix;
    GLint mRotateMatrix;
    GLfloat mVertices[8]={
            -1.0f ,-1.0f,
            1.0f ,-1.0f,
            -1.0f , 1.0f,
            1.0f ,1.0f
    };
    GLfloat mTextCoords[8]=
            {
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    1.0f, 0.0f
            };
    //y u v数据缓存
    unsigned char * y;
    unsigned char * u;
    unsigned char * v;
public:
    VideoRenderController();
    GLint loadShaderAndPro();
    void mat4f_LoadOrtho(float left, float right, float bottom, float top, float near, float far, float* mout);
    int setFrame(AVFrame *avFrame);
    GLint prepareRender();
    void render(AVFrame *avFrame);
    ~VideoRenderController();
    void initEGL(ANativeWindow *nativeWindow);
    EglCore* eglCore;

};


#endif //JJPLAYER_VIDEORENDERCONTROLLER_H
