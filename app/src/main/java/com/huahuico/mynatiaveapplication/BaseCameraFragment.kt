package com.huahuico.mynatiaveapplication

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.View
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import com.huahuico.mynatiaveapplication.rxpermissions.RxPermissions
import com.huahuico.mynatiaveapplication.rxpermissions.RxPermissionsFragment
import com.huahuico.mynatiaveapplication.util.Rx3Common
import kotlinx.android.synthetic.main.cam_shift_fragment.*
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.MainScope
import kotlinx.coroutines.launch
import java.lang.IllegalStateException
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import kotlin.math.abs
import kotlin.math.max
import kotlin.math.min

abstract class BaseCameraFragment : RxPermissionsFragment(), CoroutineScope by MainScope() {
    private var displayId = -1
    private var lensFacing = CameraSelector.LENS_FACING_BACK
    private var cameraProvider : ProcessCameraProvider? = null
    private var preview : Preview? = null
    private var camera : Camera? = null
    protected lateinit var cameraExecutor : ExecutorService
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        initialize()
    }
    private fun initialize() {
        RxPermissions(requireActivity())
            .request(android.Manifest.permission.CAMERA)
            .compose(Rx3Common.Obs_io_main())
            .to(Rx3Common.autoDispose(viewLifecycleOwner))
            .subscribe {
                if (it) {
                    cameraPreviewView.post {
                        displayId = cameraPreviewView.display.displayId
                        setupCamera()
                    }
                } else {
                    Log.d(tag, "APP has no Camera Permission!")
                }
        }
//        launch {
//
//        }
    }

    fun setupCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(requireContext())
        cameraProviderFuture.addListener({
            cameraProvider = cameraProviderFuture.get()
            lensFacing = when {
                hasBackCamera() -> CameraSelector.LENS_FACING_BACK
                hasFrontCamera() -> CameraSelector.LENS_FACING_FRONT
                else -> throw IllegalStateException("Back and Front Camera are unavailable")
            }
            bindCameraUseCase()
        }, ContextCompat.getMainExecutor(requireContext()))
    }

    private fun hasBackCamera() : Boolean {
        return cameraProvider?.hasCamera(CameraSelector.DEFAULT_BACK_CAMERA)?:false
    }
    private fun hasFrontCamera() : Boolean {
        return cameraProvider?.hasCamera(CameraSelector.DEFAULT_FRONT_CAMERA)?:false
    }
    private fun bindCameraUseCase() {
        val metrics = DisplayMetrics().also { cameraPreviewView.display.getRealMetrics(it) }
        Log.d(tag, "Screen metrics: ${metrics.widthPixels} x ${metrics.heightPixels}")
        val screenAspectRatio = aspectRatio(metrics.widthPixels, metrics.heightPixels)
        Log.d(tag, "Preview aspect ratio: $screenAspectRatio")
        val rotation = cameraPreviewView.display.rotation
        val cameraProvider = cameraProvider?: throw IllegalStateException("Camera initialization failed")
        val cameraSelector = CameraSelector.Builder().requireLensFacing(lensFacing).build()
        preview = Preview.Builder().setTargetAspectRatio(screenAspectRatio).setTargetRotation(rotation).build()
        //Must unbind the use-case before rebinding them
        cameraProvider.unbindAll()
        try {
            cameraExecutor = Executors.newSingleThreadExecutor()
            camera = cameraProvider.bindToLifecycle(this, cameraSelector, preview, onCameraSetup(screenAspectRatio, rotation, cameraExecutor))
            preview?.setSurfaceProvider(cameraPreviewView.surfaceProvider)
        }catch (e: Exception) {
            Log.e(tag, "Use case bind failed")
        }
    }
    private fun aspectRatio(width : Int, height : Int) : Int {
        val previewRatio = max(width, height).toDouble() / min(width, height)
        if (abs(previewRatio - RATIO_4_3_VALUE) <= abs(previewRatio - RATIO_16_9_VALUE)) {
            return AspectRatio.RATIO_4_3
        }
        return AspectRatio.RATIO_16_9
    }

    abstract fun onCameraSetup(screenAspectRation : Int, rotation: Int, cameraAnalysisExecutor: ExecutorService) : ImageAnalysis

    companion object {
        const val RATIO_4_3_VALUE = 4.0 / 3.0
        const val RATIO_16_9_VALUE = 16.0 / 9.0
    }

    override fun onDestroyView() {
        super.onDestroyView()
        cameraExecutor.shutdown()
    }
}