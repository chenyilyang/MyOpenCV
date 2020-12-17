//
// Created by chenyilyang on 12/15/20.
//
#ifndef MYNATIAVEAPPLICATION_NATIVE_RENDER_H
#define MYNATIAVEAPPLICATION_NATIVE_RENDER_H
#include <jni.h>
#include "LogUtil.h"
#include "EGLRender.h"
#include "GLRenderContext.h"
#ifdef __cplusplus
extern "C" {
#endif
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1init(
            JNIEnv *, jobject);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1uninit(
            JNIEnv *, jobject);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetParamsInt(
            JNIEnv *, jobject, jint, jint, jint);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnDrawFrame(
            JNIEnv *, jobject);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnSurfaceChanged(
            JNIEnv *, jobject, jint, jint);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1OnSurfaceCreated(
            JNIEnv *, jobject);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetImageDataWithIndex(
            JNIEnv *, jobject, jint, jint, jint, jint,
            jbyteArray);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetImageData(
            JNIEnv *, jobject, jint, jint, jint, jbyteArray);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1UpdateTransformMatrix(
            JNIEnv *, jobject, jfloat, jfloat, jfloat, jfloat);
    JNIEXPORT void JNICALL
    Java_com_huahuico_mynatiaveapplication_NativeRender_native_1SetParamsFloat(
            JNIEnv *, jobject, jint, jfloat, jfloat);
#ifdef __cplusplus
}
#endif
#endif //MYNATIAVEAPPLICATION_NATIVE_RENDER_H
