//
// Created by chenyilyang on 12/17/20.
//

#ifndef MYNATIAVEAPPLICATION_GLRENDERCONTEXT_H
#define MYNATIAVEAPPLICATION_GLRENDERCONTEXT_H

#include "stdint.h"
#include <GLES3/gl3.h>
#include "LogUtil.h"
#include "SampleBase.h"

class GLRenderContext {
    GLRenderContext();
    ~GLRenderContext();

public:
    void SetImageData(int format, int width, int height, uint8_t * pData);

    void SetImageDataWithIndex(int index, int format, int width, int height, uint8_t * pData);

    void SetParamsInt(int paramType, int value0, int value1);

    void SetParansFloat(int paramType, float value0, float value1);

    void SetParamsShortArr(short * const pShortArr, int arrSize);

    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void OnSurfaceCreated();
    void OnSurfaceChanged(int width, int height);
    void OnDrawFrame();

    static GLRenderContext * GetInstance();
    static void DestroyInstance();

private:
    static GLRenderContext * m_pContext;
    SampleBase * m_pCurrSample;
    int m_ScreenW = 0, m_ScreenH = 0;
};


#endif //MYNATIAVEAPPLICATION_GLRENDERCONTEXT_H
