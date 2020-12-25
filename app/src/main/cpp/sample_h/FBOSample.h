//
// Created by chenyilyang on 12/17/20.
//

#ifndef MYNATIAVEAPPLICATION_FBOSAMPLE_H
#define MYNATIAVEAPPLICATION_FBOSAMPLE_H

#include "SampleBase.h"
class FBOSample : public SampleBase {
public:
    FBOSample();
    virtual ~FBOSample();
    virtual void LoadImage(NativeImage * pImage);
    virtual void Init();
    virtual void Draw(int screenW, int screenH);
    virtual void Destroy();
    bool CreateFrameBufferObj();
private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2];
    GLuint m_VboIds[4];
    GLuint m_SamplerLoc;
    NativeImage m_RenderImage;
    GLuint m_FboProgramObj;
    GLuint m_FboVertexShader;
    GLuint m_FboFragmentShader;
    GLuint m_FboSamplerLoc;
};


#endif //MYNATIAVEAPPLICATION_FBOSAMPLE_H
