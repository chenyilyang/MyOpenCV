package com.huahuico.mynatiaveapplication

class NativeRender {
    companion object {
        init {
            System.loadLibrary("native-opengl")
        }
        val SAMPLE_TYPE = 200
        val SAMPLE_TYPE_SET_TOUCH_LOC = SAMPLE_TYPE + 999
        val SAMPLE_TYPE_SET_GRAVITY_XY = SAMPLE_TYPE + 999
    }
    open external fun native_init()
    open external fun native_uninit()
    open external fun native_SetParamsInt(paramType : Int, value0 : Int, value1 : Int)
    open external fun native_SetParamsFloat(paramType: Int, value0: Float, value1 : Float)
    open external fun native_UpdateTransformMatrix(rotateX : Float, rotateY : Float, scaleX : Float, scaleY : Float)
    open external fun native_SetImageData(format : Int, width : Int, height : Int, bytes : ByteArray)
    open external fun native_SetImageDataWithIndex(index : Int, format: Int, width: Int, height: Int, bytes: ByteArray)
    open external fun native_OnSurfaceCreated()
    open external fun native_OnSurfaceChanged(width : Int, height: Int)
    open external fun native_OnDrawFrame()
}