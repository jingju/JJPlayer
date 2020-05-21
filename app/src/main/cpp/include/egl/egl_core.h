//
// Created by Macro on 2019-12-02.
//

#ifndef CAMERAPREVIEW_EGL_CORE_H
#define CAMERAPREVIEW_EGL_CORE_H

#include "pthread.h"
#include "android/native_window.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "KHR/khrplatform.h"
#include "CommonTools.h"

/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1
/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2
/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
/// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8

typedef EGLBoolean (EGLAPIENTRYP PFNEGLPRESENTATIONTIMEANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);


class EglCore {

public:
    EglCore();
    virtual ~EglCore();
    bool init();
    bool init(EGLContext sharedContext);
    bool initWithSharedContext();

    EGLSurface creatWindowSurface(ANativeWindow * _window);
    EGLSurface creatOffScreenSurface(int width , int height);

    bool makeCurrent(EGLSurface eglSurface);
    void downCurrent();


    bool swapBuffers(EGLSurface eglSurface);
    int querySurface(EGLSurface eglSurface,int what);


    void setPresentationTime(EGLSurface eglSurface,khronos_stime_nanoseconds_t ncsec);

    void releaseSurface(EGLSurface eglSurface);
    void release();


    EGLContext getContext();
    EGLDisplay getDisplay();
    EGLConfig  getConfigure();

private:

    EGLConfig config;
    EGLContext context;
    EGLDisplay display;

    PFNEGLPRESENTATIONTIMEANDROIDPROC pfneglPresentationTimeANDROID;

};


#endif //CAMERAPREVIEW_EGL_CORE_H
