//
// Created by mac on 12/30/20.
//

#ifndef MYNATIAVEAPPLICATION_COORDSYSTEMSAMPLE_H
#define MYNATIAVEAPPLICATION_COORDSYSTEMSAMPLE_H
#include "../include/SampleBase.h"
#include <detail/type_mat4x4.hpp>

class CoordSystemSample : public SampleBase{
private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3] = {GL_NONE};
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;
    int m_AngleX, m_AngleY;
    float m_ScaleX, m_ScaleY;
public:
    CoordSystemSample();
    virtual ~CoordSystemSample();
    virtual void LoadImage(NativeImage * pImage);
    virtual void Init();
    virtual void Draw(int screenW, int screenH);
    virtual void Destroy();
    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);
};


#endif //MYNATIAVEAPPLICATION_COORDSYSTEMSAMPLE_H
