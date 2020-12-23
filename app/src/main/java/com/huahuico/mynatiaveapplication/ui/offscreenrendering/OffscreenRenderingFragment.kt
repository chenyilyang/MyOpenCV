package com.huahuico.mynatiaveapplication.ui.offscreenrendering

import android.content.Intent
import android.graphics.Bitmap
import android.opengl.GLES20
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

    override fun renderingDstImages(bitmaps: Array<Bitmap>) {
        /*val RGBABuffer = IntBuffer.allocate(512 * 512)
        Native.offscreenRendering(512, 512)
        RGBABuffer.position(0)
        GLES20.glReadPixels(0, 0, 512, 512, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, RGBABuffer)
        val modelData = RGBABuffer.array()
        val ArData = IntArray(modelData.size)
        var offset1: Int
        var offset2: Int
        for (i in 0..511) {
            offset1 = i * 512
            offset2 = (512 - i - 1) * 512
            for (j in 0..511) {
                val texturePixel = modelData[offset1 + j]
                val blue = texturePixel shr 16 and 0xff
                val red = texturePixel shl 16 and 0x00ff0000
                val pixel = texturePixel and -0xff0100 or red or blue
                ArData[offset2 + j] = pixel
            }
        }
        val modelBitmap = Bitmap.createBitmap(ArData, 512, 512, Bitmap.Config.ARGB_8888)
        dstImage.setImageBitmap(modelBitmap)
        Native.releaseOffscreen()*/

        startActivity(Intent(requireActivity(), FrameBufferObjectActivity::class.java))
    }

    override fun renderingSrcImages(bitmaps: Array<Bitmap>) {
        srcImage.setImageBitmap(bitmaps[0])
    }

    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("cat.jpg")
    }
}