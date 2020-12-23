package com.huahuico.mynatiaveapplication

import android.graphics.Bitmap
import android.graphics.ImageFormat
import androidx.camera.core.ImageProxy
import com.huahuico.mynatiaveapplication.util.YUVBytes

class Native {
    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
        public external fun offscreenRendering(width: Int, height: Int) : IntArray?
        public external fun releaseOffscreen()
        public external fun openglOffscreen(bitmap : Bitmap) : Int
        private external fun convertToGray(bitmap: IntArray?, width: Int, height: Int) : IntArray?
        private external fun convertToErode(bitmap: IntArray?, width: Int, height: Int) : IntArray?
        private external fun convertToBlur(bitmap: IntArray?, width: Int, height: Int) : IntArray?
        private external fun convertLaplacian(bitmap: IntArray?, width: Int, height: Int) : IntArray?
        private external fun convertToCanny(bitmap: IntArray?, width: Int, height: Int) : IntArray?
        private external fun mat(bitmap: IntArray?, width: Int, height: Int) : IntArray?
        private external fun convertYUVToRGBA_NV12_21(channelY: ByteArray?,channelU: ByteArray?,channelV: ByteArray?, width: Int, height: Int) : IntArray?
        private external fun camShift(channelY: ByteArray?,channelU: ByteArray?,channelV: ByteArray?, width: Int, height: Int) : IntArray?
        private external fun convertYUVToRGBA_I420(channelYUV: ByteArray?, width: Int, height: Int) : IntArray?
        fun loadGrayImage(bitmap: Bitmap) : Bitmap {
            val width = bitmap.width
            val height = bitmap.height
            var pixels : IntArray? = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            pixels = convertToGray(pixels, width, height)
            val dstBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            dstBitmap.setPixels(pixels, 0, width, 0,0, width, height)
            return dstBitmap;
        }

        fun erode(bitmap: Bitmap) : Bitmap {
            val width = bitmap.width
            val height = bitmap.height
            var pixels : IntArray? = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            pixels = convertToErode(pixels, width, height)
            val dstBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            dstBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
            return dstBitmap
        }

        fun blur(bitmap: Bitmap) : Bitmap {
            val width = bitmap.width
            val height = bitmap.height
            var pixels : IntArray? = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            pixels = convertToBlur(pixels, width, height)
            val dstBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            dstBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
            return dstBitmap
        }
        fun laplacian(bitmap: Bitmap) : Bitmap {
            val width = bitmap.width
            val height = bitmap.height
            var pixels : IntArray? = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            pixels = convertLaplacian(pixels, width, height)
            val dstBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            dstBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
            return dstBitmap
        }
        fun canny(bitmap: Bitmap) : Bitmap {
            val width = bitmap.width
            val height = bitmap.height
            var pixels : IntArray? = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            pixels = convertToCanny(pixels, width, height)
            val dstBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            dstBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
            return dstBitmap
        }
        fun mat(bitmap: Bitmap) : Bitmap {
            val width = bitmap.width
            val height = bitmap.height
            var pixels : IntArray? = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            pixels = mat(pixels, width, height)

            val dstBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            dstBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
            return dstBitmap
        }
        fun camShift(image: ImageProxy) : Bitmap {
            val width = image.width
            val height = image.height
            val yuvBytes = image.yuv2rgbBytes()
            val dstBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            if (yuvBytes != null && yuvBytes.chromaPixelStride == 2) {//色度，对比度交错
                val bytesY = ByteArray(yuvBytes.channelY.remaining())
                val bytesUV1 = ByteArray(yuvBytes.channelUV1.remaining())
                val bytesUV2 = ByteArray(yuvBytes.channelUV2.remaining())
                yuvBytes.channelY.get(bytesY)
                yuvBytes.channelUV1.get(bytesUV1)
                yuvBytes.channelUV2.get(bytesUV2)
//                val pixels = convertYUVToRGBA_NV12_21(bytesY, bytesUV1, bytesUV2, width, height)
                val pixels = camShift(bytesY, bytesUV1, bytesUV2, width, height)
                dstBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
            } else if (yuvBytes != null) {
                val pixels = convertYUVToRGBA_I420(yuvBytes.channelYUV, width, height)
                dstBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
            }
            return dstBitmap
        }

        /**
         * 参考：
         * https://github.com/opencv/opencv/blob/master/modules/java/generator/android-21/java/org/opencv/android/JavaCamera2View.java
         */
        private fun ImageProxy.yuv2rgbBytes() : YUVBytes? {
            if (format == ImageFormat.YUV_420_888 && planes.size == 3) {
                val chromaPixelStride = planes[1].pixelStride
                if (chromaPixelStride == 2) {//Chroma channels are interleaved//色度对比度交错
                    assert(planes[0].pixelStride == 1)
                    assert(planes[2].pixelStride == 2)
                    val yPlane = planes[0].buffer
                    val uvPlanne1 = planes[1].buffer
                    val uvPlanne2 = planes[2].buffer
                    return YUVBytes(yPlane, uvPlanne1, uvPlanne2, width, height)
                } else {//Chroma channels are not interleaved//色度对比度不交错
                    val yuvBytes = ByteArray(width * (height + height / 2))
                    val yPlane = planes[0].buffer
                    val uPlane = planes[1].buffer
                    val vPlane = planes[2].buffer
                    yPlane.get(yuvBytes, 0, width * height)
                    val chromaRowStride = planes[1].rowStride
                    val chromaRowPadding = chromaRowStride - width / 2
                    var offset = width * height
                    if (chromaRowPadding == 0) {
                        uPlane.get(yuvBytes, offset, width * height / 4)
                        offset += width / 4
                        vPlane.get(yuvBytes, offset, width * height / 4)
                    } else {
                        //We need to copy the channels row by row
                        for (i in 0 until height / 2) {
                            uPlane.get(yuvBytes, offset, width / 2)
                            offset += width / 2
                            if (i < height / 2 - 1) {
                                uPlane.position(uPlane.position() + chromaRowPadding)
                            }
                        }
                        for (i in 0 until height / 2) {
                            vPlane.get(yuvBytes, offset, width / 2)
                            offset += width / 2
                            if (i < height / 2 - 1) {
                                vPlane.position(vPlane.position() + chromaRowPadding)
                            }
                        }
                    }
                    return YUVBytes(yPlane, uPlane, vPlane, width, height, chromaPixelStride, yuvBytes)
                }
            }
            return null
        }
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
}