package com.huahuico.mynatiaveapplication.ui.erod

import android.graphics.Bitmap
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.huahuico.mynatiaveapplication.BaseFragment
import com.huahuico.mynatiaveapplication.Native
import com.huahuico.mynatiaveapplication.R
import kotlinx.android.synthetic.main.fragment_src_dst.*

class ErodeFragment() : BaseFragment() {

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? = inflater.inflate(R.layout.fragment_src_dst, container, false)

    override fun doSomething(bitmaps: Array<Bitmap>) : Array<Bitmap> {
        return Array<Bitmap>(bitmaps.size) {
            Native.erode(bitmaps[it])
        }
    }

    override fun renderingDstImages(bitmaps: Array<Bitmap>) {
        dstImage.setImageBitmap(bitmaps[0])
    }

    override fun renderingSrcImages(bitmaps: Array<Bitmap>) {
        srcImage.setImageBitmap(bitmaps[0])
    }

    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("cat.jpg")
    }
}