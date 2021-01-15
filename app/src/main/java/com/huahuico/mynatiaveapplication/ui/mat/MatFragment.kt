package com.huahuico.mynatiaveapplication.ui.mat

import android.graphics.Bitmap
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.huahuico.mynatiaveapplication.BaseFragment
import com.huahuico.mynatiaveapplication.Native
import com.huahuico.mynatiaveapplication.databinding.FragmentSrcDstBinding

class MatFragment : BaseFragment() {

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _bindingSrcDst = FragmentSrcDstBinding.inflate(inflater, container, false)
        return _bindingSrcDst?.root
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
        _bindingSrcDst?.dstImage?.setImageBitmap(bitmaps[0])
    }

    override fun renderingSrcImages(bitmaps: Array<Bitmap>) {
        _bindingSrcDst?.srcImage?.setImageBitmap(bitmaps[0])
    }

    override fun getAssetsImageNames(): Array<String> {
        return arrayOf("cat.jpg")
    }
}