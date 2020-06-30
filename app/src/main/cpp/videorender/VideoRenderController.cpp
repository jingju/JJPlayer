//
// Created by Macro on 2020/6/8.
//

#include "VideoRenderController.h"

VideoRenderController::VideoRenderController() {
    eglCore = new EglCore;
    mEsManager= new ESManager;
}

VideoRenderController::~VideoRenderController() {
    delete eglCore;
    delete mEsManager;
}


/**
 * 加载 shader 和 program
 */
GLint VideoRenderController::loadShaderAndPro() {
    if(!mEsManager->loadShaderAndlinkProgram(OUTPUT_VIEW_VERTEX_SHADER,OUTPUT_VIEW_FRAG_SHADER)){
        return FALSE;
    }
    return TRUE;

}

/**
 * 准备渲染解码后的视频
 * @return
 */
GLint VideoRenderController::prepareRender() {
    if(loadShaderAndPro()){
        mSamplers[0] = glGetUniformLocation(mEsManager->programObject, "s_texture_y");
        mSamplers[1] = glGetUniformLocation(mEsManager->programObject, "s_texture_u");
        mSamplers[2] = glGetUniformLocation(mEsManager->programObject, "s_texture_v");
        mMatrix = glGetUniformLocation(mEsManager->programObject, "modelViewProjectionMatrix");
        mRotateMatrix = glGetUniformLocation(mEsManager->programObject, "rotateMatrix");
        mVertextCoordsLoc= glGetAttribLocation(mEsManager->programObject, "position");
        mTextCoordsLoc= glGetAttribLocation(mEsManager->programObject, "texcoord");
        return TRUE;
    }
    return FALSE;
}

/**
 * 渲染视频
 */
void VideoRenderController::render(AVFrame *avFrame) {
    EGLint width = 0;
    EGLint height = 0;
    eglQuerySurface(eglCore->getDisplay(),_surface, EGL_WIDTH, &width);
    eglQuerySurface(eglCore->getDisplay(),_surface, EGL_HEIGHT, &height);
    //todo 这里的尺寸还是不对，应该用AVFrame的尺寸。
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(mEsManager->programObject);
    setFrame(avFrame);

    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, mTextures[i]);
        glUniform1i(mSamplers[i], i);
    }

    GLfloat modelviewProj[16];
    mat4f_LoadOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, modelviewProj);
    glUniformMatrix4fv(mMatrix, 1, GL_FALSE, modelviewProj);

    //逆时针旋转90度。
    GLfloat  rotateMatrix[16]={
            0,-1,0,0,
            1,0,0,0,
            0,0,1,0,
            0,0,0,1
    };
    glUniformMatrix4fv(mRotateMatrix, 1, GL_FALSE, rotateMatrix);

    glVertexAttribPointer(mVertextCoordsLoc, 2, GL_FLOAT, 0, 0, mVertices);
    glEnableVertexAttribArray(mVertextCoordsLoc);
    glVertexAttribPointer(mTextCoordsLoc, 2, GL_FLOAT, 0, 0, mTextCoords);
    glEnableVertexAttribArray(mTextCoordsLoc);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    eglCore->swapBuffers(_surface);
}

void VideoRenderController::mat4f_LoadOrtho(float left, float right, float bottom, float top,
                                            float near, float far, float *mout) {
    float r_l = right - left;
    float t_b = top - bottom;
    float f_n = far - near;
    float tx = - (right + left) / (right - left);
    float ty = - (top + bottom) / (top - bottom);
    float tz = - (far + near) / (far - near);

    mout[0] = 2.0f / r_l;
    mout[1] = 0.0f;
    mout[2] = 0.0f;
    mout[3] = 0.0f;

    mout[4] = 0.0f;
    mout[5] = 2.0f / t_b;
    mout[6] = 0.0f;
    mout[7] = 0.0f;

    mout[8] = 0.0f;
    mout[9] = 0.0f;
    mout[10] = -2.0f / f_n;
    mout[11] = 0.0f;

    mout[12] = tx;
    mout[13] = ty;
    mout[14] = tz;
    mout[15] = 1.0f;

}

int VideoRenderController::setFrame(AVFrame *avFrame) {
    int w = avFrame->width;
    int h = avFrame->height;

    if(y == nullptr){
        y = new unsigned char[w*h];
    }
    if(u == nullptr){
        u = new unsigned char[w*h/2];
    }

    if(v == nullptr){
        v = new unsigned char[w*h/2];
    }

    y=avFrame->data[0];
    u=avFrame->data[1];
    v=avFrame->data[2];

//    int l1 = avFrame->linesize[0];
//    int l2 = avFrame->linesize[1];
//
//    for(int i= 0 ; i < h ; i++)
//    {
//        memcpy(y + w*i,avFrame->data[0] + l1* i, sizeof( unsigned char)*w);
//    }
//
//    for(int i= 0 ; i < h/2 ; i++)
//    {
//        memcpy(u + w*i,avFrame->data[1] +  l2 * i, sizeof( unsigned char)*w);
//    }
//
//    for(int i= 0 ; i < h/2 ; i++)
//    {
//        memcpy(v + w*i,avFrame->data[1] +  l2 * i, sizeof( unsigned char)*w);
//    }


    if (0 == mTextures[0])
    {
        glGenTextures(3, mTextures);
    }


    const  int widths[3]  = { avFrame->width, avFrame->width / 2, avFrame->width / 2 };
    const  int heights[3] = { avFrame->height, avFrame->height / 2, avFrame->height / 2 };

    unsigned char *pixels[3]={y,u,v};

    for (int i = 0; i < 3; ++i)
    {

        glBindTexture(GL_TEXTURE_2D, mTextures[i]);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_LUMINANCE,
                     widths[i],
                     heights[i],
                     0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE,
                     pixels[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    return 0;

}
void VideoRenderController::initEGL(ANativeWindow *window) {
    //todo 把surface相关都封装到egl_core里面。
    _surface=eglCore->creatWindowSurface(window);
    eglCore->makeCurrent(_surface);
}



