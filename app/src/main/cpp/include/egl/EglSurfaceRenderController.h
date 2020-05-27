//
// Created by Macro on 2020/5/27.
//

#ifndef JJPLAYER_EGLSURFACERENDERCONTROLLER_H
#define JJPLAYER_EGLSURFACERENDERCONTROLLER_H

#include <EglSurfaceRender.h>
#include <android/native_window.h>

/**
 * 视频渲染控制类
 */
class EglSurfaceRenderController {

private:
    EglSurfaceRender render;
    ANativeWindow *nativeWindow;


public:
    void init();





};


#endif //JJPLAYER_EGLSURFACERENDERCONTROLLER_H
