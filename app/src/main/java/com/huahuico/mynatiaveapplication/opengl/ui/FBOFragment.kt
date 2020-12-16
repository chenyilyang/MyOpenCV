package com.huahuico.mynatiaveapplication.opengl.ui

import android.graphics.Bitmap
import android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.RelativeLayout
import com.huahuico.mynatiaveapplication.BaseFragment
import com.huahuico.mynatiaveapplication.R
import com.huahuico.mynatiaveapplication.opengl.NativeGLRender
import com.huahuico.mynatiaveapplication.opengl.NativeGLSurfaceView
import com.huahuico.mynatiaveapplication.opengl.NativeGLSurfaceView.Companion.IMAGE_FORMAT_RGBA
import com.huahuico.mynatiaveapplication.opengl.SahderType
import kotlinx.android.synthetic.main.fragment_f_b_o.*
import java.nio.ByteBuffer

class FBOFragment : BaseFragment() {
    lateinit var nativeGLSurfaceView : NativeGLSurfaceView
    lateinit var glRender : NativeGLRender
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? = inflater.inflate(R.layout.fragment_f_b_o, container, false)

    override fun doSomething(bitmaps: Array<Bitmap>) : Array<Bitmap> {
        return Array<Bitmap>(bitmaps.size) {
            bitmaps[it]
        }
    }

    private fun init() {
        glRender = NativeGLRender()
        glRender.init()
        val lp = RelativeLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.MATCH_PARENT
        )
        lp.addRule(RelativeLayout.CENTER_IN_PARENT)
        nativeGLSurfaceView = NativeGLSurfaceView(context, glRender)
        rootView.addView(nativeGLSurfaceView, lp)
        nativeGLSurfaceView.renderMode = RENDERMODE_WHEN_DIRTY
        if (rootView.width != nativeGLSurfaceView.width || rootView.height != nativeGLSurfaceView.height) {
            nativeGLSurfaceView.setAspectRatio(rootView.width, rootView.height)
        }
        glRender.setParamInt(SahderType.SHADER_TYPE_NORMAL, SahderType.SHADER_TYPE_FBO, 0)
    }

    override fun onDestroy() {
        super.onDestroy()
        glRender.unInit()
    }

    override fun renderingDstImages(bitmaps: Array<Bitmap>) {
        init()
        for (bitmap in bitmaps) {
            val bytes = bitmap.byteCount
            val buf = ByteBuffer.allocate(bytes)
            bitmap.copyPixelsToBuffer(buf)
            glRender.setImageData(IMAGE_FORMAT_RGBA, bitmap.width, bitmap.height, buf.array())
        }
    }

    override fun renderingSrcImages(bitmaps: Array<Bitmap>) {
    }

    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("cat.jpg")
    }
}