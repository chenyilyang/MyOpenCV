package com.huahuico.mynatiaveapplication.opengl.ui

import com.huahuico.mynatiaveapplication.opengl.SampleSahderType

class TextureMapFragment : SampleImageBaseFragment() {
    override fun init() {
        super.init()
        glRender.setParamInt(SampleSahderType.SAMPLE_SHADER_TYPE_NORMAL, SampleSahderType.SAMPLE_SHADER_TYPE_TEXTUREMAP, 0)
    }
    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("temple.jpg")
    }
}