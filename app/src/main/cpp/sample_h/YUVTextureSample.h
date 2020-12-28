//
// Created by chenyilyang on 12/28/20.
//

#ifndef MYNATIAVEAPPLICATION_YUVTEXTURESAMPLE_H
#define MYNATIAVEAPPLICATION_YUVTEXTURESAMPLE_H
#include "../include/SampleBase.h"

class YUVTextureSample : public SampleBase{
private:
    GLuint m_yTextureId;
    GLuint m_uvTextureId;
    GLint m_ySamplerLoc;
    GLint m_uvSamplerLoc;
    NativeImage m_RenderImage;
public:
    YUVTextureSample() {
        m_yTextureId = GL_NONE;
        m_uvTextureId = GL_NONE;
        m_ySamplerLoc = GL_NONE;
        m_uvSamplerLoc = GL_NONE;
    }
    virtual ~YUVTextureSample() {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
    }
    virtual void LoadImage(NativeImage * pImage);
    virtual void Init();
    virtual void Draw(int screenW, int screenH);
    virtual void Destroy();
};


#endif //MYNATIAVEAPPLICATION_YUVTEXTURESAMPLE_H
