//
// Created by chenyilyang on 12/17/20.
//

#ifndef MYNATIAVEAPPLICATION_TEXTUREMAPSAMPLE_H
#define MYNATIAVEAPPLICATION_TEXTUREMAPSAMPLE_H

#include "SampleBase.h"
class TextureMapSample : public SampleBase{
public:
    TextureMapSample();
    virtual ~TextureMapSample();
    void LoadImage(NativeImage *pImage);
    virtual void Init();
    virtual void Draw(int screenW, int screenH);
    virtual void Destroy();
private:
    GLuint m_TextureId;
    GLint m_SampleLoc;
    NativeImage m_RenderImage;
};
#endif //MYNATIAVEAPPLICATION_TEXTUREMAPSAMPLE_H
