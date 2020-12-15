package com.huahuico.mynatiaveapplication.opengl

import android.opengl.GLSurfaceView
import android.util.Log
import com.huahuico.mynatiaveapplication.NativeRender
import com.huahuico.mynatiaveapplication.NativeRender.Companion.SAMPLE_TYPE_SET_GRAVITY_XY
import com.huahuico.mynatiaveapplication.NativeRender.Companion.SAMPLE_TYPE_SET_TOUCH_LOC
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class NativeGLRender : GLSurfaceView.Renderer {
    val nativeRender : NativeRender = NativeRender()
    var sampleType : Int = 0
    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        nativeRender.native_OnSurfaceCreated()
        Log.d("chenyl", "onSurfaceCreated() call with GL_VERSION = [${gl?.glGetString(GL10.GL_VERSION)}")
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        nativeRender.native_OnSurfaceChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        nativeRender.native_OnDrawFrame()
    }

    open fun init() {
        nativeRender.native_init()
    }
    open fun unInit() {
        nativeRender.native_uninit()
    }
    open fun setParamInt(paramType : Int, value0 : Int, value1 : Int) {
        if (paramType == sampleType) {
            sampleType = value0
        }
        nativeRender.native_SetParamsInt(paramType, value0, value1)
    }
    open fun setTouchLoc(x : Float, y : Float) {
        nativeRender.native_SetParamsFloat(SAMPLE_TYPE_SET_TOUCH_LOC, x, y)
    }
    open fun setGravityXY(x : Float, y: Float) {
        nativeRender.native_SetParamsFloat(SAMPLE_TYPE_SET_GRAVITY_XY, x, y)
    }
    open fun setImageData(format : Int, width : Int, height : Int, bytes: ByteArray) {
        nativeRender.native_SetImageData(format, width, height, bytes)
    }
    open fun setImageData(index : Int, format : Int, width : Int, height : Int, bytes: ByteArray) {
        nativeRender.native_SetImageDataWithIndex(index, format, width, height, bytes)
    }
    open fun updateTransformMatrix(rotateX : Float, rotateY : Float, scaleX : Float, scaleY : Float) {
        nativeRender.native_UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY)
    }
}