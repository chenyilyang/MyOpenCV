//
// Created by mac on 12/29/20.
//

#ifndef MYNATIAVEAPPLICATION_VAOVBOSAMPLE_H
#define MYNATIAVEAPPLICATION_VAOVBOSAMPLE_H

#include "SampleBase.h"
class VAOVBOSample : public SampleBase{
private:
    GLuint m_VaoId;
    GLuint m_VboIds[2];
public:
    VAOVBOSample();
    virtual ~VAOVBOSample();
    virtual void LoadImage(NativeImage * pImage);
    virtual void Init();
    virtual void Draw(int screenW, int screenH);
    virtual void Destroy();
};


#endif //MYNATIAVEAPPLICATION_VAOVBOSAMPLE_H
