package com.huahuico.mynatiaveapplication.ui.camshift

import android.graphics.Bitmap
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.activity.result.ActivityResultCallback
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.ActivityResultRegistry
import androidx.activity.result.contract.ActivityResultContract
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.lifecycle.ViewModelProviders
import com.huahuico.mynatiaveapplication.BaseCameraFragment
import com.huahuico.mynatiaveapplication.Native
import com.huahuico.mynatiaveapplication.databinding.CamShiftFragmentBinding
import com.huahuico.mynatiaveapplication.util.*
import java.util.concurrent.ExecutorService

class CamShiftFragment : BaseCameraFragment() {

    private lateinit var viewModel: CamShiftViewModel
    private lateinit var mHandler : Handler
    private lateinit var imageAnalysis: ImageAnalysis

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _binding = CamShiftFragmentBinding.inflate(inflater, container, false)
        return _binding?.root
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)
        viewModel = ViewModelProviders.of(this).get(CamShiftViewModel::class.java)
        mHandler = Handler(Looper.getMainLooper()) {
            if (it.what == 100) {
                val bitmap = it.data["bitmap"] as Bitmap?
                if (bitmap != null && _binding != null) {
                    _binding?.analysisImageView?.setImageBitmap(bitmap)
                } else {
                    mHandler.removeMessages(100)
                }
            }
            true
        }

    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
    }

    private class PlanesAnalyser : ImageAnalysis.Analyzer {
        override fun analyze(image: ImageProxy) {

        }
    }

    override fun onCameraSetup(
        screenAspectRation: Int,
        rotation: Int,
        cameraAnalysisExecutor: ExecutorService
    ): ImageAnalysis {
        return ImageAnalysis.Builder()
            .setTargetAspectRatio(screenAspectRation)
            .setTargetRotation(rotation)
            .build()
            .also { imageAnalysis -> imageAnalysis.setAnalyzer(cameraAnalysisExecutor, { image ->
//                val imageData = getBytesFromImage(
//                    image.planes[0],
//                    image.planes[1],
//                    image.planes[2]
//                )
//                if (buf == null) {
//                    buf = ByteArray(imageData.data.size)
//                }
//                val rw = if (rotation % 180 == 0) imageData.width else imageData.height
//                val rh = if (rotation % 180 == 0) imageData.height else imageData.width // rotated
//                rotateSP(imageData.data, buf!!, imageData.width, imageData.height, rotation)
//                imageData.data = buf!!
//                imageData.width = rw
//                imageData.height = rh
                val bitmap = Native.camShift(image)
//                val bitmap = convertToBitmap(imageData.data, imageData.width, imageData.height, rotation)
                Log.d("chenyl", "rotation = $rotation")
                val msg = mHandler.obtainMessage()
                val bundle = Bundle()
                bundle.putParcelable("bitmap", bitmap)
                msg.what = 100
                msg.data = bundle
                mHandler.sendMessage(msg)
                image.close()
            })
            }
    }

//    override fun <I : Any?, O : Any?> prepareCall(
//        contract: ActivityResultContract<I, O>,
//        callback: ActivityResultCallback<O>
//    ): ActivityResultLauncher<I> {
//        TODO("Not yet implemented")
//    }
//
//    override fun <I : Any?, O : Any?> prepareCall(
//        contract: ActivityResultContract<I, O>,
//        registry: ActivityResultRegistry,
//        callback: ActivityResultCallback<O>
//    ): ActivityResultLauncher<I> {
//        TODO("Not yet implemented")
//    }

    var buf : ByteArray? = null
    private fun convertToBitmap(data: ByteArray, w: Int, h: Int, rotation : Int) : Bitmap {
        var time = System.currentTimeMillis()
        if (buf == null) {
            buf = ByteArray(data.size)
        }
        val rw = if (rotation % 180 == 0) w else h
        val rh = if (rotation % 180 == 0) h else w // rotated
//        rotateP(data, buf!!, w, h, rotation)
//        val b0: Bitmap = i420ToBitmap(buf!!, rw, rh)
//        rotateP(data, buf!!, w, h, rotation)
//        val b1: Bitmap = yv12ToBitmap(buf!!, rw, rh)
//        rotateSP(data, buf!!, w, h, rotation)
//        val b2: Bitmap = nv12ToBitmap(buf!!, rw, rh)
        rotateSP(data, buf!!, w, h, rotation)
        val b3: Bitmap = nv21ToBitmap(buf!!, rw, rh)
        time = System.currentTimeMillis() - time
        Log.d("chenyl", "convert time: $time")
//        return b0
//        return b1
//        return b2
        return b3
    }
}