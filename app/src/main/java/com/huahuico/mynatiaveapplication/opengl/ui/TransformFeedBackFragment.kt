package com.huahuico.mynatiaveapplication.opengl.ui

import android.graphics.Bitmap
import com.huahuico.mynatiaveapplication.opengl.SampleSahderType

class TransformFeedBackFragment : SampleImageBaseFragment() {
    override fun init() {
        super.init()
        glRender.setParamInt(SampleSahderType.SAMPLE_SHADER_TYPE_NORMAL, SampleSahderType.SAMPLE_SHADER_TYPE_TRANSFORM_FEEDBACK, 0)
    }
    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("temple.jpg")
    }
}