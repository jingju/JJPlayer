//
// Created by Macro on 2019-12-17.
//

#include "es_manager.h"

#define LOG_TAG "esManager"

/**
 * 加载着色器源码
 * @param shaderType
 * @param pSource
 * @return
 */
GLuint ESManager::loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGI("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
            } else {
                LOGI( "Guessing at GL_INFO_LOG_LENGTH size\n");
                char* buf = (char*) malloc(0x1000);
                if (buf) {
                    glGetShaderInfoLog(shader, 0x1000, NULL, buf);
                    LOGI("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
            }
            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}



GLint ESManager::linkProgram(GLuint vertexShader,GLuint fragmentShader) {
    GLint linked;
    programObject=glCreateProgram();
    if(programObject==0){
        return FALSE;
    }

    glAttachShader(programObject,vertexShader);
    glAttachShader(programObject,fragmentShader);


    glLinkProgram(programObject);

    //check link status
    glGetProgramiv(programObject,GL_LINK_STATUS,&linked);

    if(!linked){
        GLint  infoLen=0;
        glGetProgramiv(programObject,GL_INFO_LOG_LENGTH,&infoLen);

        if(infoLen > 1){
            char* infoLog= static_cast<char *>(malloc(sizeof(char) * infoLen));
            glGetProgramInfoLog(programObject,infoLen,NULL,infoLog);
            LOGE("Error linking program:\n %s \n",infoLog);
            free(infoLog);
        }

        glDeleteProgram(programObject);
        return FALSE;
    }
    return TRUE;
}


GLint ESManager::loadShaderAndlinkProgram(const char *vSource, const char *fSource) {
    //加载和编译shaders
    vertexShader=loadShader(GL_VERTEX_SHADER,vSource);
    fragmentShader=loadShader(GL_FRAGMENT_SHADER,fSource);
    if(vertexShader==0 ||fragmentShader ==0){
        return FALSE;
    }
    if(!linkProgram(vertexShader,fragmentShader)){
        return FALSE;
    }
    return TRUE;
}


