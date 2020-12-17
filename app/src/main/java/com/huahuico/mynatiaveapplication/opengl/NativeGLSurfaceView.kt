package com.huahuico.mynatiaveapplication.opengl

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import java.lang.IllegalArgumentException

class NativeGLSurfaceView : GLSurfaceView, ScaleGestureDetector.OnScaleGestureListener {
    companion object {
        val IMAGE_FORMAT_RGBA = 0x01
        val IMAGE_FORMAT_NV21 = 0x02
        val IMAGE_FORMAT_NV12 = 0x03
        val IMAGE_FORMAT_Y420 = 0x04
    }
    val TOUCH_SCALE_FACTOR = 180.0F / 320.0F
    var previousY = 0.0F
    var previousX = 0.0F
    var angleX = 0
    var angleY = 0
    var ratioWidth = 0
    var ratioHeight = 0
    var preScale = 1.0F
    var curScale = 1.0F
    var lastMultiTouchTime = 0L
    lateinit var scaleGestureDetector: ScaleGestureDetector
    lateinit var glRender: NativeGLRender

    constructor(context: Context?, glRender: NativeGLRender, attrs: AttributeSet? = null) : super(context, attrs) {
        this.glRender = glRender
        setEGLContextClientVersion(2)
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(this.glRender)
        renderMode = RENDERMODE_WHEN_DIRTY
        context?.let {
            scaleGestureDetector = ScaleGestureDetector(it, this)
        }
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        event?.let {
            if (it.pointerCount == 1) {
                consumeTouchEvent(event)
                val currentTimeMillis = System.currentTimeMillis()
                if (currentTimeMillis - lastMultiTouchTime > 200) {
                    val y = it.y
                    val x = it.x
                    when(it.action) {
                        MotionEvent.ACTION_MOVE -> {
                            val dy = y - previousY
                            val dx = x - previousX
                            angleY = (dy * TOUCH_SCALE_FACTOR).toInt()
                            angleX = (dx * TOUCH_SCALE_FACTOR).toInt()
                        }
                    }
                    previousX = x
                    previousY = y
                }
                it
            } else {
                scaleGestureDetector.onTouchEvent(event)
            }
        }
        return true
    }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val measureW = MeasureSpec.getSize(widthMeasureSpec)
        val measureH = MeasureSpec.getSize(heightMeasureSpec)
        if (0 == ratioWidth || 0 == ratioHeight) {
            setMeasuredDimension(measureW, measureH)
        } else {
            if (measureW < measureH * ratioWidth / ratioHeight) {
                setMeasuredDimension(measureW, measureW * ratioHeight / ratioWidth)
            } else {
                setMeasuredDimension(measureH * ratioWidth / ratioHeight, measureH)
            }
        }
    }

    open fun setAspectRatio(width : Int, height : Int) {
        Log.d("chenyl", "setAspectRatio() called with : width = [$width], heithg = [$height]")
        if (width < 0 || height < 0) {
            throw IllegalArgumentException("Size cannot be negative.")
        }
        ratioWidth = width
        ratioHeight = height
        requestLayout()
    }

    override fun onScale(detector: ScaleGestureDetector?): Boolean {
        //do scale
        return false
    }

    override fun onScaleBegin(detector: ScaleGestureDetector?): Boolean {
        return true
    }

    override fun onScaleEnd(detector: ScaleGestureDetector?) {
        preScale = curScale
        lastMultiTouchTime = System.currentTimeMillis()
    }
    private fun consumeTouchEvent(event : MotionEvent) {
        dealClickEvent(event)
        var touchX = -1F
        var touchY = -1F
        when(event.action) {
            MotionEvent.ACTION_MOVE -> {
                touchX = event.x
                touchY = event.y
            }
            MotionEvent.ACTION_UP -> {
                touchX = -1F
                touchY = -1F
            }
            MotionEvent.ACTION_CANCEL -> {
                touchX = -1F
                touchY = -1F
            }
        }
        //slide
//        when(glRender.sampleType) {

//        }
        //click
        when(event.action) {
            MotionEvent.ACTION_MOVE -> {}
            MotionEvent.ACTION_UP -> {}
            MotionEvent.ACTION_CANCEL -> {}
        }
    }
    private fun dealClickEvent(event : MotionEvent) {
        var touchX = -1F
        var touchY = -1F
        when(event.action) {
            MotionEvent.ACTION_MOVE ->{
                touchX = event.x
                touchY = event.y
                //click
//                when(glRender.sampleType) {

//                }
            }
        }
    }
}