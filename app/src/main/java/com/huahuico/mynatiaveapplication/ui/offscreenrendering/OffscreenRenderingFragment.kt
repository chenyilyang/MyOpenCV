package com.huahuico.mynatiaveapplication.ui.offscreenrendering

import android.graphics.Bitmap
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.SeekBar
import com.huahuico.mynatiaveapplication.BaseFragment
import com.huahuico.mynatiaveapplication.Native
import com.huahuico.mynatiaveapplication.databinding.FragmentSrcDstBinding
import java.nio.IntBuffer

class OffscreenRenderingFragment : BaseFragment() {
    private lateinit var RGBABuffer : IntBuffer
    private var width = 0
    private var height = 0
    private var contrastLocation = -1
    private var bitmap : Bitmap? = null
    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? {
        _bindingSrcDst = FragmentSrcDstBinding.inflate(inflater, container, false)
        return _bindingSrcDst?.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        _bindingSrcDst?.seekBar?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                seekBar?.let { seekBarContrast ->
                    val contrast = seekBarContrast.progress * 1.0f / 100.0f * 4.0f
                    bitmap?.let { changeContrast(it, contrast) }
                }
            }
        })
    }

    override fun doSomething(bitmaps: Array<Bitmap>) : Array<Bitmap> {
        return Array<Bitmap>(bitmaps.size) {
            bitmaps[it]
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
        bitmap = bitmaps[0]
        bitmap?.let {
            width = it.width
            height = it.height
            if (Native.openglOffscreen(width, height) < 0) return
            RGBABuffer = IntBuffer.allocate(width * height)
            changeContrast(bitmaps[0], 1.0f)
        }
    }
    private fun changeContrast(bitmap : Bitmap, contrast : Float) {
        val modelBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        Native.contrastFilter(bitmap, modelBitmap, contrast)
        _bindingSrcDst?.dstImage?.setImageBitmap(modelBitmap)
    }

    override fun renderingSrcImages(bitmaps: Array<Bitmap>) {
        _bindingSrcDst?.srcImage?.setImageBitmap(bitmaps[0])
    }

    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("weixuancat.jpg")
    }

    override fun onDestroy() {
        Native.releaseOffscreen()
        super.onDestroy()
    }
}

