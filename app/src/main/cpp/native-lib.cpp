#include <jni.h>
#include <string>
#include <iostream>
#include <streambuf>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

using namespace cv;
using namespace std;
#define GO_CHECK_GL_ERROR() cout << "CHECK_GL_ERROR " <<  __FUNCTION__ << "glGetError = " << glGetError() << ", line = " <<  __LINE__ << endl

class MyStreamBuf : public std::streambuf {
    enum {
        BUFFER_SIZE = 255,
    };

public :
    MyStreamBuf() {
        buffer_[BUFFER_SIZE] = '\n';
        setp(buffer_, buffer_ + BUFFER_SIZE - 1);
    }
    ~MyStreamBuf() override {
        sync();
    }
protected :
    int_type overflow(int_type c) override {
        if (c != EOF) {
            *pptr() = c;
            pbump(1);
        }
        flush_buffer();
        return c;
    }

    int sync() override {
        flush_buffer();
        return 0;
    }
private :
    int flush_buffer() {
        int len = int(pptr() -pbase());
        if (len <= 0) return 0;
        if (len <= BUFFER_SIZE) buffer_[len] = '\0';
#ifdef ANDROID
        android_LogPriority t = ANDROID_LOG_INFO;
        __android_log_write(t, "native_log", buffer_);
#else
        printf("%s", buffer_);
#endif
        pbump(-len);
        return len;
    }
private :
    char buffer_[BUFFER_SIZE + 1];
};
extern "C"
void rotate90(Mat srcImage, Mat dstImage, int width, int height) {
    //顺时针旋转90度
    Point2f center(width / 2, height / 2);
    Mat rotateM = getRotationMatrix2D(center, -90, 1.0);
    //进行仿射变换
    warpAffine(srcImage, dstImage, rotateM, Size(width, height));
    circle(dstImage, center, 2, Scalar(255, 0, 0));
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_huahuico_mynatiaveapplication_Native_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_convertToGray(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jintArray bitmap,
                                                                           jint width,
                                                                           jint height) {
    jint* cbuf;
    Mat gray_img;
    Mat ret_img;
    cbuf = env->GetIntArrayElements(bitmap, JNI_FALSE);
    Mat inp_img(height, width, CV_8UC4, (unsigned char *)cbuf);
    cvtColor(inp_img, gray_img, CV_BGRA2GRAY);
    cvtColor(gray_img, ret_img, CV_GRAY2BGRA);
    int size = width * height;
    jintArray result = env->NewIntArray(size);
    uchar *ptr = ret_img.data;
    env->SetIntArrayRegion(result, 0, size, (const jint *) ptr);
    env->ReleaseIntArrayElements(bitmap, cbuf, 0);
    return result;
}
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_convertToErode(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jintArray bitmap,
                                                                           jint width,
                                                                           jint height) {
    jint * cbuf;
    cbuf = env-> GetIntArrayElements(bitmap, JNI_FALSE);
    Mat srcImage(height, width, CV_8UC4, (unsigned char *)cbuf);
    //指定形状和尺寸的结构元素（内核矩阵）
    Mat erodElement = getStructuringElement(MORPH_RECT, Size(15, 15));
    Mat dstImage;
    erode(srcImage, dstImage, erodElement);
    jintArray result = env->NewIntArray(width * height);
    uchar * ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, width * height, (const jint *)ptr);
    env->ReleaseIntArrayElements(bitmap, cbuf, 0);
    return result;
}
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_convertToBlur(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jintArray bitmap,
                                                                           jint width,
                                                                           jint height) {
    jint * cbuf;
    cbuf = env->GetIntArrayElements(bitmap, JNI_FALSE);
    Mat srcImage(height, width, CV_8UC4, (unsigned char *)cbuf);
    Mat dstImage;
    blur(srcImage, dstImage, Size(7, 7));
    jintArray result = env->NewIntArray(width * height);
    uchar * ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, width * height, (const jint *)ptr);
    env->ReleaseIntArrayElements(bitmap, cbuf, 0);
    return result;
}
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_convertToCanny(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jintArray bitmap,
                                                                            jint width,
                                                                            jint height) {
    jint * cbuf;
    cbuf = env->GetIntArrayElements(bitmap, JNI_FALSE);
    Mat srcImage(height, width, CV_8UC4, (unsigned char *)cbuf);
    Mat dstImage, edge, grayImage, binaryImage, blurImage;
    //1。转换为灰度图
    cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
    //2。使用3x3内核来降噪
//    blur(grayImage, blurImage, Size(3, 3));
    threshold(grayImage, binaryImage, 0, 255, THRESH_BINARY + THRESH_OTSU);
    //3。运行Canny算子
    Canny(grayImage, edge, 80, 100, 3);
    //We fill a dst image with zeros (meaning the image is completely black).
    dstImage.create(Size(width, height), grayImage.type());
    dstImage = Scalar::all(0);
    /**
     * Finally, we will use the function cv::Mat::copyTo
     * to map only the areas of the image that are identified
     * as edges (on a black background). cv::Mat::copyTo copy
     * the src image onto dst. However, it will only
     * copy the pixels in the locations where they have non-zero values.
     * Since the output of the Canny detector is the edge contours on a black background,
     * the resulting dst will be black in all the area but the detected edges.
     */
    binaryImage.copyTo(dstImage, edge);
    cvtColor(dstImage, dstImage, COLOR_GRAY2BGRA);
    jintArray result = env->NewIntArray(width * height);
    uchar * ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, width * height, (const jint *)ptr);
    env->ReleaseIntArrayElements(bitmap, cbuf, 0);
    return result;
}extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_convertYUVToRGBA_1NV12_121(JNIEnv *env,
                                                                                        jobject thiz,
                                                                                        jbyteArray channel_y,
                                                                                        jbyteArray channel_u,
                                                                                        jbyteArray channel_v,
                                                                                        jint width,
                                                                                        jint height) {
    jbyte * bbufY, * bbufU, * bbufV;
    Mat dstImage;
    bbufY = env->GetByteArrayElements(channel_y, JNI_FALSE);
    bbufU = env->GetByteArrayElements(channel_u, JNI_FALSE);
    bbufV = env->GetByteArrayElements(channel_v, JNI_FALSE);
    Mat yMat(height, width, CV_8UC1, (unsigned char *)bbufY);
    Mat uvMat1(height / 2, width / 2, CV_8UC2, (unsigned char *)bbufU);
    Mat uvMat2(height / 2, width / 2, CV_8UC2, (unsigned char *)bbufV);
    long addressDiff = &uvMat1.data - &uvMat2.data;
    if (addressDiff == 12L) {
//        cvtColorTwoPlane(yMat, uvMat1, dstImage, COLOR_YUV2RGBA_NV12);
        cvtColorTwoPlane(yMat, uvMat2, dstImage, COLOR_YUV2BGRA_NV21);
    } else if (addressDiff == 21L) {
        cvtColorTwoPlane(yMat, uvMat1, dstImage, COLOR_YUV2BGRA_NV12);
    } else {
        dstImage.create(Size(width, height), CV_8UC4);
        dstImage = Scalar::all(255);
    }
    rotate90(dstImage, dstImage, width, height);
    jintArray result = env->NewIntArray(width * height);
    uchar * ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, width * height, (const jint *)ptr);
    env->ReleaseByteArrayElements(channel_y, bbufY, 0);
    env->ReleaseByteArrayElements(channel_u, bbufU, 0);
    env->ReleaseByteArrayElements(channel_v, bbufV, 0);
    return result;
}extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_convertYUVToRGBA_1I420(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jbyteArray channel_yuv,
                                                                                    jint width,
                                                                                    jint height) {
    jbyte * bbufYUV;
    Mat dstImage;
    bbufYUV = env->GetByteArrayElements(channel_yuv, JNI_FALSE);
    Mat yuvImage(height + height / 2, width, CV_8UC1, (unsigned char *)bbufYUV);
    cvtColor(yuvImage, dstImage, COLOR_YUV2RGBA_I420);
    rotate90(dstImage, dstImage, width, height);
    jintArray result = env->NewIntArray(width * height);
    uchar * ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, width * height, (const jint *)ptr);
    env->ReleaseByteArrayElements(channel_yuv, bbufYUV, 0);
    return result;
}

bool backprojMode = false;
bool selectObject = true;
int trackObject = 0;
bool showHist = false;
Rect selection;
int vmin = 10, vmax = 200, smin = 30;
Rect trackWindow;
int hsize = 16;
float hranges [] = {0, 180};
const float * phranges = hranges;
Mat hsv, hue, mask, hist, histimg = Mat::zeros(Size(200, 320), CV_8UC3), backproj;
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_camShift(JNIEnv *env, jobject thiz,
                                                                      jbyteArray channel_y,
                                                                      jbyteArray channel_u,
                                                                      jbyteArray channel_v,
                                                                      jint width, jint height) {
    jbyte * bbufY, * bbufU, * bbufV;
    Mat dstImage;
    bbufY = env->GetByteArrayElements(channel_y, JNI_FALSE);
    bbufU = env->GetByteArrayElements(channel_u, JNI_FALSE);
    bbufV = env->GetByteArrayElements(channel_v, JNI_FALSE);
    Mat yMat(height, width, CV_8UC1, (unsigned char *)bbufY);
    Mat uvMat1(height / 2, width / 2, CV_8UC2, (unsigned char *)bbufU);
    Mat uvMat2(height / 2, width / 2, CV_8UC2, (unsigned char *)bbufV);
    long addressDiff = &uvMat1.data - &uvMat2.data;
    if (addressDiff == 12L) {
//        cvtColorTwoPlane(yMat, uvMat1, dstImage, COLOR_YUV2RGBA_NV12);
        cvtColorTwoPlane(yMat, uvMat2, dstImage, COLOR_YUV2BGRA_NV21);
    } else if (addressDiff == 21L) {
        cvtColorTwoPlane(yMat, uvMat1, dstImage, COLOR_YUV2BGRA_NV12);
    } else {
        dstImage.create(Size(width, height), CV_8UC4);
        dstImage = Scalar::all(255);
    }
    rotate90(dstImage, dstImage, width, height);

    //彩色对象追踪
    cvtColor(dstImage, hsv, COLOR_RGB2HSV);
    if (selectObject) {
        selection.x = width / 2;
        selection.y = height / 2;
        selection.width = 50;
        selection.height = 50;
        selection &= Rect(0, 0, dstImage.cols, dstImage.rows);
    }
    if (trackObject) {
        int _vmin = vmin, _vmax = vmax;
        inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)), Scalar(180, 256, MAX(_vmin,_vmax)), mask);
        int ch[] = {0, 0};
        hue.create(hsv.size(), hsv.depth());
        mixChannels(&hsv, 1, &hue, 1, ch, 1);
        if (trackObject == 0) {
            Mat roi(hue, selection), maskroi(mask, selection);
            calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
            normalize(hist, hist, 0, 255, NORM_MINMAX);
            trackWindow = selection;
            trackObject = 1;
            histimg = Scalar::all(0);
            int binW = histimg.cols / hsize;
            Mat buf(1, hsize, CV_8UC3);
            for (int i = 0; i < hsize; ++i) {
                buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i * 180./hsize), 255, 255);
            }
            cvtColor(buf, buf, COLOR_HSV2BGR);
            for (int i = 0; i < hsize; ++i) {
                int val = saturate_cast<int>(hist.at<float>(i) * histimg.rows / 255);
                rectangle(histimg, Point(i * binW, histimg.rows), Point((i + 1) * binW, histimg.rows - val), Scalar(buf.at<Vec3b>(i)), -1, 8);
            }
        }
        calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
        backproj &= mask;
        RotatedRect trackBox = CamShift(backproj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
        if (trackWindow.area() <= 1) {
            int cols = backproj.cols, rows = backproj.rows, r= (MIN(cols, rows) + 5) / 6;
            trackWindow = Rect(trackWindow.x - r, trackWindow.y - r, trackWindow.x + r, trackWindow.y + r) & Rect(0, 0, cols, rows);
        }
        if (backprojMode) {
            cvtColor(backproj, dstImage, COLOR_GRAY2BGR);
        }
        ellipse(dstImage, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
    }
    if (selectObject && selection.width > 0 && selection.height > 0) {
        Mat roi(dstImage, selection);
        bitwise_not(roi, roi);
    }
    //彩色对象追踪
    jintArray result = env->NewIntArray(width * height);
    uchar * ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, width * height, (const jint *)ptr);
    env->ReleaseByteArrayElements(channel_y, bbufY, 0);
    env->ReleaseByteArrayElements(channel_u, bbufU, 0);
    env->ReleaseByteArrayElements(channel_v, bbufV, 0);
    return result;
}extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_convertLaplacian(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jintArray bitmap,
                                                                              jint width,
                                                                              jint height) {
    jint* cbuf;
    Mat imageEnhance, grayImage;
    Mat dstImage;
    cbuf = env->GetIntArrayElements(bitmap, JNI_FALSE);
    Mat inp_img(height, width, CV_8UC4, (unsigned char *)cbuf);
//    smooth()

//    Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
//    filter2D(inp_img, imageEnhance, inp_img.depth(), kernel);
//    dstImage = inp_img + imageEnhance;

    cvtColor(inp_img, grayImage, CV_BGRA2GRAY);
    threshold(grayImage, dstImage, 220, 255, THRESH_BINARY + THRESH_OTSU);

    GaussianBlur(dstImage, imageEnhance, Size(3, 3), 0, 0);
    addWeighted(dstImage, 2, imageEnhance, -1 , 0, dstImage);

    cvtColor(dstImage, dstImage, CV_GRAY2BGRA);
    jintArray result = env->NewIntArray(width * height);
    uchar *ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, width * height, (const jint *) ptr);
    env->ReleaseIntArrayElements(bitmap, cbuf, 0);
    return result;
}

MyStreamBuf g_MyStreamBuf;
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_mat(JNIEnv *env, jobject thiz,
                                                                 jintArray bitmap, jint width,
                                                                 jint height) {

    std::cout.rdbuf(&g_MyStreamBuf);
    jint * cbuf;
    cbuf = env->GetIntArrayElements(bitmap, JNI_FALSE);
    Mat srcImage(height, width, CV_8UC4, (unsigned char *)cbuf);
//    Mat dstImage(srcImage, Rect(0, 0, 100, 100));
    Mat dstImage(srcImage,Range::all(), Range(100, 300));
    Mat E = Mat::eye(Size(4, 4), CV_64F);
    cout<< "R(Python format) = \n" << format(E, Formatter::FMT_PYTHON) << ";" << endl;
//    randu(dstImage, Scalar::all(0), Scalar::all(255));
    ellipse(dstImage, Point_<int>(150, height / 2), Size2i(50, 50), 30, 0, 360, Scalar_<double>(255, 129, 0), 2, 8);

    dstImage = Mat(Size2i(width, height), srcImage.type(), Scalar(0, 0, 0, 255));
//    Mat rookImage = Mat::zeros(Size2i(width, height), CV_8UC3);
    ellipse(dstImage, Point_<int>(width / 2, height / 2), Size2i(width / 4, width / 16), 90, 0, 360, Scalar_<double>(255, 129, 0), 2, 8);
    ellipse(dstImage, Point_<int>(width / 2, height / 2), Size2i(width / 4, width / 16), 0, 0, 360, Scalar_<double>(255, 129, 0), 2, 8);
    ellipse(dstImage, Point_<int>(width / 2, height / 2), Size2i(width / 4, width / 16), 45, 0, 360, Scalar_<double>(255, 129, 0), 2, 8);
    ellipse(dstImage, Point_<int>(width / 2, height / 2), Size2i(width / 4, width / 16), -45, 0, 360, Scalar_<double>(255, 129, 0), 2, 8);
    circle(dstImage, Point(width / 2, height / 2), width / 32, Scalar(0, 0, 255), -1, 8);
    jintArray result = env->NewIntArray(height * width);
    uchar * ptr = dstImage.data;
    env->SetIntArrayRegion(result, 0, height * width, (const jint *)ptr);
    env->ReleaseIntArrayElements(bitmap, cbuf, 0);
    return result;
}

static EGLConfig eglConf;
static EGLSurface eglSurface;
static EGLContext eglCtx;
static EGLDisplay eglDisplay;
static GLuint m_TextureId;
static GLint m_ProgramObj;
const char vShaderStr [] =
        "#version 300 es                                    \n"
        "layout(location = 0) in vec4 a_position;           \n"
        "layout(location = 1) in vec2 a_texCoord;           \n"
        "out vec2 v_texCoord;                               \n"
        "void main() {                                      \n"
        "   gl_Position = a_position;                       \n"
        "   v_texCoord = a_texCoord;                        \n"
        "}                                                  \n";
//normal fragment shader
const char fShaderStr [] =
        "#version 300 es                                                                                \n"
"precision mediump float;                                                                               \n"
        "in vec2 v_texCoord;                                                                            \n"
        "layout(location = 0) out vec4 outColor;                                                        \n"
        "uniform sampler2D s_TextureMap;                                                                \n"
        "uniform lowp float contrast;                                                                   \n"
        "void main() {                                                                                  \n"
        "   lowp vec4 textureColor = texture(s_TextureMap, v_texCoord);                                 \n"
        "   outColor = vec4(((textureColor.rgb - vec3(0.5)) * contrast + vec3(0.5)), textureColor.w);   \n"
        "}                                                                                              \n";

const GLfloat verticesCoords[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
};
const GLfloat textureCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
};
GLint m_SampleLoc;
GLuint m_VertexShader;
GLuint m_FragmentSahder;
GLint m_ContrastLoc;
GLfloat m_ContrastValue = 1.0f;
extern "C"
JNIEXPORT void JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_releaseOffscreen(JNIEnv *env,
                                                                              jobject thiz) {
    glDeleteProgram(m_ProgramObj);
    glDeleteTextures(1, &m_TextureId);
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisplay, eglCtx);
    eglDestroySurface(eglDisplay, eglSurface);
    eglTerminate(eglDisplay);
    eglDisplay = EGL_NO_DISPLAY;
    eglSurface = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;
}

void initEGLEnv(int width, int height) {
    const EGLint confAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,//very important!
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,//if you need the alpha channel
            EGL_DEPTH_SIZE, 8,//if you need the depth buffer
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,//very important!
            EGL_NONE
    };
    const EGLint surfaceAttr[] = {
            EGL_WIDTH, (EGLint)width,
            EGL_HEIGHT, (EGLint)height,
            EGL_NONE
    };
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == EGL_NO_DISPLAY) {
        cout << "Unable to open connection to local windowing system" << endl;
    }
    if (!eglInitialize(eglDisplay, &eglMajVers, &eglMinVers)) {
        cout << "Unable to initialize EGL, Handle and recover" << endl;
    }
    cout << "EGL init with version " << eglMajVers << "." << eglMinVers << endl;
    if (!eglChooseConfig(eglDisplay, confAttr, &eglConf, 1, &numConfigs)) {
        cout << "Some configs are wrong" << endl;
    }
    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConf, surfaceAttr);
    if (eglSurface == EGL_NO_SURFACE) {
        switch (eglGetError()) {
            case EGL_BAD_ALLOC:
                cout << "Not enough resources available" << endl;
                break;
            case EGL_BAD_CONFIG:
                cout << "provided EGLConfig is invalid" << endl;
                break;
            case EGL_BAD_PARAMETER:
                cout << "Provided EGL_WIDTH, EGL_HEIGHT is invalid" << endl;
                break;
            case EGL_BAD_MATCH:
                cout << "Check window and EGLConfig attributes" << endl;
                break;
        }
    }
    eglCtx = eglCreateContext(eglDisplay, eglConf, EGL_NO_CONTEXT, ctxAttr);
    if (eglCtx == EGL_NO_CONTEXT) {
        EGLint error = eglGetError();
        if (error == EGL_BAD_CONFIG) {
            cout << "EGL_BAD_CONFIG" << endl;
        }
    }
    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglCtx)) {
        cout << "Make current failed" << endl;
    }
    // end of standard gl context setup
}
void checkGLError(const char *pGLOperation) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        cout << "GLUtils::CheckGLError GL Operation " << pGLOperation << "() glError 0x" << error << endl;
    }
}
GLuint loadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = 0;
    shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char * buf = (char *) malloc((size_t)infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, nullptr, buf);
                    cout << "GLUtils::LoadShader Could not compile shader" << shaderType << "\n  " << buf << endl;
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}
GLuint createProgram(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle) {
    GLuint program = 0;
    vertexShaderHandle = loadShader(GL_VERTEX_SHADER, pVertexShaderSource);
    if (!vertexShaderHandle) return program;
    fragShaderHandle = loadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
    if (!fragShaderHandle) return program;
    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShaderHandle);
        checkGLError("glAttachVertexShader");
        glAttachShader(program, fragShaderHandle);
        checkGLError("glAttachFragmentShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        glDetachShader(program, vertexShaderHandle);
        glDeleteShader(vertexShaderHandle);
        vertexShaderHandle = 0;

        glDetachShader(program, fragShaderHandle);
        glDeleteShader(fragShaderHandle);
        fragShaderHandle = 0;

        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char * buf = (char *) malloc((size_t)bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, nullptr, buf);
                    cout << "GLUtils::CreateProgram Could not link program:" << buf << endl;
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    cout << "GLUtils::CreateProgram program = " << program << endl;
    return program;
}
void createRenderBuffer(int width, int height) {
    GLuint fboId = 0;
    GLuint renderBufferWidth = width;
    GLuint renderBufferHeight = height;

    // create a framebuffer object
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    cout << glGetError() << endl;
    GLuint renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    cout << glGetError() << endl;
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_RGB565,//only support RGBA565??
                          renderBufferWidth,
                          renderBufferHeight);
    cout << glGetError() << endl;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER,
                              renderBuffer);
    cout << glGetError() << endl;
    GLuint depthRenderbuffer;
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,     renderBufferWidth, renderBufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        cout << "Problem with OpenGL framebuffer after specifying color render buffer: \n" << status << endl;
    } else {
        cout << "FBO creation success" << endl;
    }
}

void createTextureBuffer(int width, int height) {
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    m_ProgramObj = createProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentSahder);
    if (m_ProgramObj) {
        m_SampleLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        m_ContrastLoc = glGetUniformLocation(m_ProgramObj, "contrast");
    } else {
        cout << "TextureMapSampler::Init create program fail" << endl;
    }
}
void drawTextureBuffer(int width, int height, void * pixels) {
    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR();
    glUseProgram(m_ProgramObj);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), verticesCoords);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), textureCoords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    GO_CHECK_GL_ERROR();
    //bind the rgba map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SampleLoc, 0);
    glUniform1f(m_ContrastLoc, m_ContrastValue);
    GO_CHECK_GL_ERROR();
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 0.0, 0.0, 1.0);
    GLushort indices [] = {0, 1, 2, 1, 3, 2};
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    GO_CHECK_GL_ERROR();

    // check the output format
    // This is critical to knowing what surface format just got created
    // ES only supports 5-6-5 and other limited formats and the driver
    // might have picked another format
//    GLint format = 0, type = 0;
//    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
//    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
    // commit the clear to the offscreen surface
    eglSwapBuffers(eglDisplay, eglSurface);
}
extern "C" JNIEXPORT jint JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_openglOffscreen(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint width, jint height) {
    std::cout.rdbuf(&g_MyStreamBuf);
    initEGLEnv(width, height);
    createRenderBuffer(width, height);
    createTextureBuffer(width, height);
    return 0;
}extern "C"
JNIEXPORT void JNICALL
Java_com_huahuico_mynatiaveapplication_Native_00024Companion_contrastFilter(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jobject bitmap,
                                                                            jobject dstBitmap,
                                                                            jfloat contrast) {
    m_ContrastValue = (GLfloat) contrast;
    void * srcpixels = 0;
    void * dstpixels = 0;
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) return;
    if (AndroidBitmap_lockPixels(env, bitmap, &srcpixels) < 0) return;
    if (AndroidBitmap_lockPixels(env, dstBitmap, &dstpixels) < 0) return;
    drawTextureBuffer(info.width, info.height, srcpixels);
    glReadPixels(0, 0, info.width, info.height, GL_RGBA, GL_UNSIGNED_BYTE, dstpixels);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, dstBitmap);
}