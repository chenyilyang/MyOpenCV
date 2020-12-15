#include <jni.h>
#include <string>
#include <iostream>
#include <streambuf>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <android/log.h>
using namespace cv;
using namespace std;

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