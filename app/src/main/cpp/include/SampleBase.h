//
// Created by chenyilyang on 12/17/20.
//

#ifndef MYNATIAVEAPPLICATION_SAMPLEBASE_H
#define MYNATIAVEAPPLICATION_SAMPLEBASE_H
#include "SyncLock.h"
#include <GLES3/gl3.h>
#include "ImageDef.h"

#define MATH_PI 3.1415926535897932384626433832802

#define SAMPLE_SHADER_TYPE_NORMAL 0
#define SAMPLE_SHADER_TYPE_TRIANGLE 1
#define SAMPLE_SHADER_TYPE_TEXTUREMAP 2
#define SAMPLE_SHADER_TYPE_FBO 3


#define SAMPLE_TYPE_KEY_SET_TOUCH_LOC SAMPLE_SHADER_TYPE_NORMAL + 999
#define SAMPLE_TYPE_KEY_SET_GRAVITY_XY SAMPLE_SHADER_TYPE_NORMAL + 1000

class SampleBase {
protected:
    GLuint m_VertexShader;
    GLuint m_FragmentSahder;
    GLuint m_ProgramObj;
    SyncLock m_Lock;
    int m_SurfaceWidth, m_SurfaceHeight;
public:
    SampleBase() {
        m_ProgramObj = GL_NONE;
        m_VertexShader = GL_NONE;
        m_FragmentSahder = GL_NONE;

        m_SurfaceHeight = 0;
        m_SurfaceWidth = 0;
    }
    virtual ~SampleBase() {

    }
    virtual void LoadImage(NativeImage * pImage) {};
    virtual void LoadMultiImageWithIndex(int index, NativeImage * pImage) {};
    virtual void LoadShortArrData(short * const pShortArr, int arrSize) {};
    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {};
    virtual void SetTouchLocation(float x, float y) {};
    virtual void SetGravityXY(float x, float y) {};
    virtual void Init() = 0;
    virtual void Draw(int screenW, int screenH) = 0;
    virtual void Destroy() = 0;
};
#endif //MYNATIAVEAPPLICATION_SAMPLEBASE_H
