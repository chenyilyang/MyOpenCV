package com.huahuico.mynatiaveapplication.ui.offscreenrendering

import android.graphics.Bitmap
import android.opengl.GLES30
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.huahuico.mynatiaveapplication.BaseFragment
import com.huahuico.mynatiaveapplication.Native
import com.huahuico.mynatiaveapplication.R
import kotlinx.android.synthetic.main.fragment_src_dst.*
import java.nio.IntBuffer

class OffscreenRenderingFragment : BaseFragment() {
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? = inflater.inflate(R.layout.fragment_src_dst, container, false)

    override fun doSomething(bitmaps: Array<Bitmap>) : Array<Bitmap> {
        return Array<Bitmap>(bitmaps.size) {
            bitmaps[it]
//            Native.laplacian(bitmaps[it])
        }
    }
    fun convertABGRtoARGB(pixels: IntArray) {
        var p: Int
        var r: Int
        var g: Int
        var b: Int
        var a: Int
        for (i in pixels.indices) {
            p = pixels[i]
            a = p shr 24 and 0xFF // get pixel bytes in ARGB order
            b = p shr 16 and 0xFF
            g = p shr 8 and 0xFF
            r = p shr 0 and 0xFF
            pixels[i] = a shl 24 or (r shl 16) or (g shl 8) or (b shl 0)
        }
    }
    override fun renderingDstImages(bitmaps: Array<Bitmap>) {
        val RGBABuffer = IntBuffer.allocate(bitmaps[0].width * bitmaps[0].height)
//        Native.offscreenRendering(512, 512)
        if (Native.openglOffscreen(bitmaps[0]) < 0) return
        RGBABuffer.position(0)
        GLES30.glReadPixels(
            0,
            0,
            bitmaps[0].width,
            bitmaps[0].height,
            GLES30.GL_RGBA,
            GLES30.GL_UNSIGNED_BYTE,
            RGBABuffer
        )
        val modelData = RGBABuffer.array()
        convertABGRtoARGB(modelData)
        val modelBitmap = Bitmap.createBitmap(
            modelData,
            bitmaps[0].width,
            bitmaps[0].height,
            Bitmap.Config.ARGB_8888
        )
        dstImage.setImageBitmap(modelBitmap)
        Native.releaseOffscreen()
//        startActivity(Intent(requireActivity(), FrameBufferObjectActivity::class.java))
    }

    override fun renderingSrcImages(bitmaps: Array<Bitmap>) {
        srcImage.setImageBitmap(bitmaps[0])
    }

    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("cat.jpg")
    }
}