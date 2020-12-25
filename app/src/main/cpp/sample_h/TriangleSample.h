//
// Created by chenyilyang on 12/25/20.
//

#ifndef MYNATIAVEAPPLICATION_TRIANGLESAMPLE_H
#define MYNATIAVEAPPLICATION_TRIANGLESAMPLE_H

#include "SampleBase.h"
class TriangleSample : public SampleBase{
public:
    TriangleSample();
    virtual ~TriangleSample();
    virtual void LoadImage(NativeImage * pImage);
    virtual void Init();
    virtual void Draw(int screenW, int screenH);
    virtual void Destroy();
};


#endif //MYNATIAVEAPPLICATION_TRIANGLESAMPLE_H
