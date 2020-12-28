package com.huahuico.mynatiaveapplication.opengl.ui

import android.graphics.Bitmap
import com.huahuico.mynatiaveapplication.opengl.NativeGLSurfaceView.Companion.IMAGE_FORMAT_NV21
import com.huahuico.mynatiaveapplication.opengl.SampleSahderType

class YUVRenderingFragment : SampleImageBaseFragment() {
    override fun init() {
        super.init()
        glRender.setParamInt(SampleSahderType.SAMPLE_SHADER_TYPE_NORMAL, SampleSahderType.SAMPLE_SHADER_TYPE_YUVTEXTURE, 0)
    }
    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("temple.jpg")
    }
    override fun renderingDstImages(bitmaps: Array<Bitmap>) {
        init()
        val inputStream = resources.assets.open("YUV_Image_840x1074.NV21")
        val buffer = ByteArray(inputStream.available())
        inputStream.read(buffer)
        glRender.setImageData(IMAGE_FORMAT_NV21, 840, 1074, buffer)
    }
}