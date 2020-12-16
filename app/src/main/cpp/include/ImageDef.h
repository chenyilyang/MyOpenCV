//
// Created by chenyilyang on 12/16/20.
//

#ifndef MYNATIAVEAPPLICATION_IMAGEDEF_H
#define MYNATIAVEAPPLICATION_IMAGEDEF_H
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "sys/stat.h"
#include "stdint.h"
#include "LogUtil.h"

#define IMAGE_FORMAT_RGBA               0x01
#define IMAGE_FORMAT_NV21               0x02
#define IMAGE_FORMAT_NV12               0x03
#define IMAGE_FORMAT_I420               0x04

#define IMAGE_FORMAT_RGBA_EXT           "RGB32"
#define IMAGE_FORMAT_NV21_EXT           "NV21"
#define IMAGE_FORMAT_NV12_EXT           "NV12"
#define IMAGE_FORMAT_I420_EXT           "I420"

typedef struct _tag_NativeRectF{
    float left;
    float top;
    float right;
    float bottom;
    _tag_NativeRectF() {
        left = top = right = bottom = 0.0f;
    }
} RectF;
typedef struct _tag_NativeImage {
    int width;
    int height;
    int format;
    uint8_t * ppPlane[3];
    _tag_NativeImage() {
        width = height = format = 0;
        ppPlane[0] = nullptr;
        ppPlane[1] = nullptr;
        ppPlane[2] = nullptr;
    }
} NativeImage;
class NativeImageUtil {
    public:
        static void AllocNativeImage(NativeImage * pImage) {
            if (pImage->width == 0 || pImage->height == 0) return;
            switch (pImage->format) {
                case IMAGE_FORMAT_RGBA:
                    pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 4));
                    break;
                case IMAGE_FORMAT_NV21:
                case IMAGE_FORMAT_NV12:
                    pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
                    pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
                    break;
                case IMAGE_FORMAT_I420:
                    pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
                    pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
                    pImage->ppPlane[2] = pImage->ppPlane[1] + pImage->width * (pImage->height >> 2);
                    break;
                default:
                    LOGCATE("NativeImageUtil::AllocNativeImage do not support the format, Format = %d", pImage->format);
            }
        }
        static void FreeNativeImage(NativeImage * pImage) {
            if (pImage == nullptr || pImage->ppPlane[0] == nullptr) return;
            free(pImage->ppPlane[0]);
            pImage->ppPlane[0] = nullptr;
            pImage->ppPlane[1] = nullptr;
            pImage->ppPlane[2] = nullptr;
        }
        static void CopyNativeImage(NativeImage * pSrcImg, NativeImage * pDstImg) {
            if (pSrcImg == nullptr || pSrcImg->ppPlane[0] == nullptr) return;
            if (pSrcImg->width != pDstImg->width || pSrcImg->height != pDstImg->height || pSrcImg->format != pDstImg->format) return;
            if (pDstImg->ppPlane[0] == nullptr) AllocNativeImage(pDstImg);
            switch (pSrcImg->format) {
                case IMAGE_FORMAT_I420:
                case IMAGE_FORMAT_NV21:
                case IMAGE_FORMAT_NV12:
                    memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pDstImg->height * 1.5);
                    break;
                case IMAGE_FORMAT_RGBA:
                    memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pDstImg->height * 4);
                    break;
                default:
                    LOGCATE("NativeImageUtil::CopyNativeImage do not support the format, Format = %d", pSrcImg->format);
                    break;
            }
        }
};
#endif //MYNATIAVEAPPLICATION_IMAGEDEF_H
