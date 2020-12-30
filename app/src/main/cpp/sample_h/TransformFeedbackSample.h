//
// Created by mac on 12/30/20.
//

#ifndef MYNATIAVEAPPLICATION_TRANSFORMFEEDBACKSAMPLE_H
#define MYNATIAVEAPPLICATION_TRANSFORMFEEDBACKSAMPLE_H
#include "../include/SampleBase.h"

class TransformFeedbackSample : public SampleBase {
private:
    GLuint m_ImageTextureId;
    GLuint m_VaoId;
    GLuint m_VboIds[2] = {GL_NONE};
    GLint m_SamplerLoc;
    GLuint m_TransFeedbackObjId;
    GLuint m_TransFeedbackBuffId;
    NativeImage m_RenderImage;
public:
    TransformFeedbackSample();
    virtual ~TransformFeedbackSample();
    virtual void LoadImage(NativeImage * pImage);
    virtual void Init();
    virtual void Draw(int screenW, int screenH);
    virtual void Destroy();
};


#endif //MYNATIAVEAPPLICATION_TRANSFORMFEEDBACKSAMPLE_H
