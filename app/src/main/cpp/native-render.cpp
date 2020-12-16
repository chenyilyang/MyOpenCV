//
// Created by chenyilyang on 12/15/20.
//

#include "include/native-render.h"
#ifdef __cplusplus
extern "C" {
#endif
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1init(JNIEnv *env,
                                                                                    jobject thiz) {
        LOGCATD("native opengl init");
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1uninit(JNIEnv *env,
                                                                                      jobject thiz) {
        LOGCATD("native opengl uninit");
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetParamsInt(JNIEnv *env,
                                                                                            jobject thiz,
                                                                                            jint param_type,
                                                                                            jint value0,
                                                                                            jint value1) {
        LOGCATD("native opengl setParamInt param_type = %d, value0 = %d, value1 = %d", param_type, value0, value1);
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnDrawFrame(JNIEnv *env,
                                                                                           jobject thiz) {
        // TODO: implement native_OnDrawFrame()
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnSurfaceChanged(
            JNIEnv *env, jobject thiz, jint width, jint height) {
        // TODO: implement native_OnSurfaceChanged()
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnSurfaceCreated(
            JNIEnv *env, jobject thiz) {
        // TODO: implement native_OnSurfaceCreated()
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetImageDataWithIndex(
            JNIEnv *env, jobject thiz, jint index, jint format, jint width, jint height,
            jbyteArray bytes) {
        // TODO: implement native_SetImageDataWithIndex()
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetImageData(JNIEnv *env,
                                                                                            jobject thiz,
                                                                                            jint format,
                                                                                            jint width,
                                                                                            jint height,
                                                                                            jbyteArray bytes) {
        // TODO: implement native_SetImageData()
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1UpdateTransformMatrix(
            JNIEnv *env, jobject thiz, jfloat rotate_x, jfloat rotate_y, jfloat scale_x,
            jfloat scale_y) {
        // TODO: implement native_UpdateTransformMatrix()
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetParamsFloat(
            JNIEnv *env, jobject thiz, jint param_type, jfloat value0, jfloat value1) {
        // TODO: implement native_SetParamsFloat()
    }
#ifdef __cplusplus
}
#endif