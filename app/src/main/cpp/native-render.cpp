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
        GLRenderContext::GetInstance();
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1uninit(JNIEnv *env,
                                                                                      jobject thiz) {
        LOGCATD("native opengl uninit");
        GLRenderContext::DestroyInstance();
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetParamsInt(JNIEnv *env,
                                                                                            jobject thiz,
                                                                                            jint param_type,
                                                                                            jint value0,
                                                                                            jint value1) {
        LOGCATD("native opengl setParamInt param_type = %d, value0 = %d, value1 = %d", param_type, value0, value1);
        GLRenderContext::GetInstance()->SetParamsInt(param_type, value0, value1);
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnDrawFrame(JNIEnv *env,
                                                                                           jobject thiz) {
        GLRenderContext::GetInstance()->OnDrawFrame();
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnSurfaceChanged(
            JNIEnv *env, jobject thiz, jint width, jint height) {
        GLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnSurfaceCreated(
            JNIEnv *env, jobject thiz) {
        GLRenderContext::GetInstance()->OnSurfaceCreated();
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetImageDataWithIndex(
            JNIEnv *env, jobject thiz, jint index, jint format, jint width, jint height,
            jbyteArray bytes) {
        int len = env->GetArrayLength(bytes);
        uint8_t * buf = new uint8_t [len];
        env->GetByteArrayRegion(bytes, 0, len, reinterpret_cast<jbyte*>(buf));
        GLRenderContext::GetInstance()->SetImageDataWithIndex(index, format, width, height, buf);
        delete [] buf;
        env->DeleteLocalRef(bytes);
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetImageData(JNIEnv *env,
                                                                                            jobject thiz,
                                                                                            jint format,
                                                                                            jint width,
                                                                                            jint height,
                                                                                            jbyteArray bytes) {
        int len = env->GetArrayLength(bytes);
        uint8_t * buf = new uint8_t [len];
        env->GetByteArrayRegion(bytes, 0, len, reinterpret_cast<jbyte*>(buf));
        GLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
        delete [] buf;
        env->DeleteLocalRef(bytes);
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1UpdateTransformMatrix(
            JNIEnv *env, jobject thiz, jfloat rotate_x, jfloat rotate_y, jfloat scale_x,
            jfloat scale_y) {
        GLRenderContext::GetInstance()->UpdateTransformMatrix(rotate_x, rotate_y, scale_x, scale_y);
    }
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetParamsFloat(
            JNIEnv *env, jobject thiz, jint param_type, jfloat value0, jfloat value1) {
        GLRenderContext::GetInstance()->SetParansFloat(param_type, value0, value1);
    }
#ifdef __cplusplus
}
#endif