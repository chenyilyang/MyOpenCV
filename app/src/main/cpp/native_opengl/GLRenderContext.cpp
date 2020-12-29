//
// Created by chenyilyang on 12/17/20.
//

#include "../include/GLRenderContext.h"
#include "../sample_h/FBOSample.h"
#include "../sample_h/TextureMapSample.h"
#include "../sample_h/TriangleSample.h"
#include "../sample_h/YUVTextureSample.h"
#include "../sample_h/VAOVBOSample.h"

GLRenderContext * GLRenderContext::m_pContext = nullptr;
GLRenderContext::GLRenderContext() {
    m_pCurrSample = nullptr;
}
GLRenderContext::~GLRenderContext() {
    if (m_pCurrSample) {
        delete m_pCurrSample;
        m_pCurrSample = nullptr;
    }
}
void GLRenderContext::SetParamsInt(int paramType, int value0, int value1) {
    LOGCATD("GLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType, value0, value1);
    if (paramType == SAMPLE_SHADER_TYPE_NORMAL) {
        switch (value0) {
            //todo add sample shader type filter here
            case SAMPLE_SHADER_TYPE_TRIANGLE:
                m_pCurrSample = new TriangleSample();
                break;
            case SAMPLE_SHADER_TYPE_FBO:
                m_pCurrSample = new FBOSample();
                break;
            case SAMPLE_SHADER_TYPE_TEXTUREMAP:
                m_pCurrSample = new TextureMapSample();
                break;
            case SAMPLE_SHADER_TYPE_YUVTEXTURE:
                m_pCurrSample = new YUVTextureSample();
                break;
            case SAMPLE_SHADER_TYPE_VAOVBO:
                m_pCurrSample = new VAOVBOSample();
                break;
        }
    }
    if (m_pCurrSample != nullptr) LOGCATD("GLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType, value0, value1);
}

void GLRenderContext::SetParansFloat(int paramType, float value0, float value1) {
    LOGCATD("GLRenderContext::SetParansFloat paramType = %d, value0 = %f, value1 = %f", paramType, value0, value1);
    if (m_pCurrSample) {
        switch (paramType) {
            case SAMPLE_TYPE_KEY_SET_TOUCH_LOC:
                m_pCurrSample->SetTouchLocation(value0, value1);
                break;
            case SAMPLE_TYPE_KEY_SET_GRAVITY_XY:
                m_pCurrSample->SetGravityXY(value0, value1);
                break;
        }
    }
}

void GLRenderContext::SetParamsShortArr(short *const pShortArr, int arrSize) {
    LOGCATD("GLRenderContext::SetParamsShortArr pShortArr = %p, arrSize = %d", pShortArr, arrSize);
    if (m_pCurrSample) m_pCurrSample->LoadShortArrData(pShortArr, arrSize);
}
void GLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    LOGCATD("GLRenderContext::UpdateTransformMatrix [rotateX, rotateY, scaleX, scaleY] = [%f, %f, %f, %f]", rotateX, rotateY, scaleX, scaleY);
    if (m_pCurrSample) m_pCurrSample->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
}
void GLRenderContext::SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData) {
    LOGCATD("GLRenderContext::SetImageDataWithIndex index = %d, format = %d, width = %d, height = %d, pData = %p", index, format, width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }
    if (m_pCurrSample) m_pCurrSample->LoadMultiImageWithIndex(index, &nativeImage);
}
void GLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData) {
    LOGCATD("GLRenderContext::SetImageData format = %d, width = %d, height = %d, pData = %p", format, width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }
    if (m_pCurrSample) m_pCurrSample->LoadImage(&nativeImage);
}
void GLRenderContext::OnSurfaceCreated() {
    LOGCATD("GLRenderContext::OnSurfaceCreated");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
void GLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGCATD("GLRenderContext::OnSurfaceChanged [w,h] = [%d,%d]", width, height);
    glViewport(0, 0, width, height);
    m_ScreenH = height;
    m_ScreenW = width;
}
void GLRenderContext::OnDrawFrame() {
    LOGCATD("GLRenderContext::OnDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    if (m_pCurrSample) {
        m_pCurrSample->Init();
        m_pCurrSample->Draw(m_ScreenW, m_ScreenH);
    }
}
GLRenderContext * GLRenderContext::GetInstance() {
    LOGCATD("GLRenderContext::GetInstance");
    if (m_pContext == nullptr) m_pContext = new GLRenderContext();
    return m_pContext;
}
void GLRenderContext::DestroyInstance() {
    LOGCATD("GLRenderContext::DestroyInstance");
    if (m_pContext) {
        delete m_pContext;
        m_pContext = nullptr;
    }
}