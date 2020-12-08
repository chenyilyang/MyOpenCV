package com.huahuico.mynatiaveapplication.ui.mat

import android.graphics.Bitmap
import androidx.lifecycle.ViewModelProviders
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.huahuico.mynatiaveapplication.BaseFragment
import com.huahuico.mynatiaveapplication.Native
import com.huahuico.mynatiaveapplication.R
import kotlinx.android.synthetic.main.fragment_src_dst.*

class MatFragment : BaseFragment() {

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_src_dst, container, false)
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)
    }

    override fun doSomething(bitmaps: Array<Bitmap>): Array<Bitmap> {
        return Array<Bitmap>(bitmaps.size) {
            Native.mat(bitmaps[0])
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