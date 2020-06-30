//
// Created by Macro on 2019-12-17.
//

#ifndef CAMERAPREVIEW_SHADER_MANAGER_H
#define CAMERAPREVIEW_SHADER_MANAGER_H


#include <GLES3/gl3.h>
#include <CommonTools.h>


#define FALSE 0
#define TRUE  1

class ESManager {

private:
    GLuint vertexShader;
    GLuint fragmentShader;
public:
    GLuint programObject;
public:
    GLuint loadShader(GLenum shaderType, const char* pSource);

    GLint linkProgram(GLuint vertexShader,GLuint fragmentShader);


    GLint loadShaderAndlinkProgram(const char* vSource,const char* fSource);

};


#endif //CAMERAPREVIEW_SHADER_MANAGER_H
