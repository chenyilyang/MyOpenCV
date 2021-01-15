package com.huahuico.mynatiaveapplication.opengl.ui
import android.graphics.Bitmap
import android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.RelativeLayout
import com.huahuico.mynatiaveapplication.BaseFragment
import com.huahuico.mynatiaveapplication.databinding.FragmentImageBinding
import com.huahuico.mynatiaveapplication.opengl.NativeGLRender
import com.huahuico.mynatiaveapplication.opengl.NativeGLSurfaceView
import com.huahuico.mynatiaveapplication.opengl.NativeGLSurfaceView.Companion.IMAGE_FORMAT_RGBA
import java.nio.ByteBuffer

abstract class SampleImageBaseFragment : BaseFragment() {
    lateinit var nativeGLSurfaceView : NativeGLSurfaceView
    lateinit var glRender : NativeGLRender
    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? {
        _bindingImage = FragmentImageBinding.inflate(inflater, container, false)
        return _bindingImage?.root
    }

    override fun doSomething(bitmaps: Array<Bitmap>) : Array<Bitmap> {
        return Array<Bitmap>(bitmaps.size) {
            bitmaps[it]
        }
    }

    protected open fun init() {
        glRender = NativeGLRender()
        glRender.init()
        val lp = RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
        )
        lp.addRule(RelativeLayout.CENTER_IN_PARENT)
        nativeGLSurfaceView = NativeGLSurfaceView(context, glRender)
        _bindingImage?.let {
            it.rootView.addView(nativeGLSurfaceView, lp)
            nativeGLSurfaceView.renderMode = RENDERMODE_WHEN_DIRTY
            if (it.rootView.width != nativeGLSurfaceView.width || it.rootView.height != nativeGLSurfaceView.height) {
                nativeGLSurfaceView.setAspectRatio(it.rootView.width, it.rootView.height)
            }
        }
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
}