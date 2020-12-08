package com.huahuico.mynatiaveapplication.util

import android.graphics.Bitmap
import androidx.camera.core.ImageProxy

fun getBytesFromImage(
    planesY: ImageProxy.PlaneProxy,
    planesU: ImageProxy.PlaneProxy,
    planesV: ImageProxy.PlaneProxy
): ImageBytes {
//    val planes = image.planes
    val b0 = planesY.buffer
    val b1 = planesU.buffer
    val b2 = planesV.buffer
    val r0 = b0.remaining()
    val r1 = b1.remaining()
    val r2 = b2.remaining()
    val w0 = planesY.rowStride
    var h0 = r0 / w0
    if (r0 % w0 > 0) h0++
    val w1 = planesU.rowStride
    var h1 = r1 / w1
    if (r1 % w1 > 1) h1++
    val w2 = planesV.rowStride
    var h2 = r2 / w2
    if (r2 % w2 > 2) h2++
    val y = w0 * h0
    val u = w1 * h1
    val v = w2 * h2
    val bytes = ByteArray(y + u + v)
    b0[bytes, 0, r0]
    b1[bytes, y, r1] // u
    b2[bytes, y + u, r2] // v
    return ImageBytes(bytes, w0, h0)
}

/******************************* YUV420转换Bitmap算法  */ // 此方法虽然是官方的，但是耗时是下面方法的两倍
//    public static Bitmap nv21ToBitmap(byte[] data, int w, int h) {
//        final YuvImage image = new YuvImage(data, ImageFormat.NV21, w, h, null);
//        ByteArrayOutputStream os = new ByteArrayOutputStream(data.length);
//        if (image.compressToJpeg(new Rect(0, 0, w, h), 100, os)) {
//            byte[] tmp = os.toByteArray();
//            return BitmapFactory.decodeByteArray(tmp, 0, tmp.length);
//        }
//        return null;
//    }
fun nv12ToBitmap(data: ByteArray, w: Int, h: Int): Bitmap {
    return spToBitmap(data, w, h, 0, 1)
}

fun nv21ToBitmap(data: ByteArray, w: Int, h: Int): Bitmap {
    return spToBitmap(data, w, h, 1, 0)
}

private fun spToBitmap(data: ByteArray, w: Int, h: Int, uOff: Int, vOff: Int): Bitmap {
    val plane = w * h
    val colors = IntArray(plane)
    var yPos = 0
    var uvPos = plane
    for (j in 0 until h) {
        for (i in 0 until w) {
            // YUV byte to RGB int
            val y1: Int = data[yPos].toInt() and 0xff
            val u: Int = (data[uvPos + uOff].toInt() and 0xff) - 128
            val v: Int = (data[uvPos + vOff].toInt() and 0xff) - 128
            val y1192 = 1192 * y1
            var r = y1192 + 1634 * v
            var g = y1192 - 833 * v - 400 * u
            var b = y1192 + 2066 * u
            r = if (r < 0) 0 else if (r > 262143) 262143 else r
            g = if (g < 0) 0 else if (g > 262143) 262143 else g
            b = if (b < 0) 0 else if (b > 262143) 262143 else b
            colors[yPos] = r shl 6 and 0xff0000 or
                    (g shr 2 and 0xff00) or
                    (b shr 10 and 0xff)
            if (yPos++ and 1 == 1) uvPos += 2
        }
        if (j and 1 == 0) uvPos -= w
    }
    return Bitmap.createBitmap(colors, w, h, Bitmap.Config.RGB_565)
}

fun i420ToBitmap(data: ByteArray, w: Int, h: Int): Bitmap {
    return pToBitmap(data, w, h, true)
}

fun yv12ToBitmap(data: ByteArray, w: Int, h: Int): Bitmap {
    return pToBitmap(data, w, h, false)
}

private fun pToBitmap(data: ByteArray, w: Int, h: Int, uv: Boolean): Bitmap {
    val plane = w * h
    val colors = IntArray(plane)
    val off = plane shr 2
    var yPos = 0
    var uPos = plane + if (uv) 0 else off
    var vPos = plane + if (uv) off else 0
    for (j in 0 until h) {
        for (i in 0 until w) {
            // YUV byte to RGB int
            val y1: Int = data[yPos].toInt() and 0xff
            val u: Int = (data[uPos].toInt() and 0xff) - 128
            val v: Int = (data[vPos].toInt() and 0xff) - 128
            val y1192 = 1192 * y1
            var r = y1192 + 1634 * v
            var g = y1192 - 833 * v - 400 * u
            var b = y1192 + 2066 * u
            r = if (r < 0) 0 else if (r > 262143) 262143 else r
            g = if (g < 0) 0 else if (g > 262143) 262143 else g
            b = if (b < 0) 0 else if (b > 262143) 262143 else b
            colors[yPos] = r shl 6 and 0xff0000 or
                    (g shr 2 and 0xff00) or
                    (b shr 10 and 0xff)
            if (yPos++ and 1 == 1) {
                uPos++
                vPos++
            }
        }
        if (j and 1 == 0) {
            uPos -= w shr 1
            vPos -= w shr 1
        }
    }
    return Bitmap.createBitmap(colors, w, h, Bitmap.Config.RGB_565)
}

/******************************* YUV420旋转算法  */ // I420或YV12顺时针旋转
fun rotateP(src: ByteArray, dest: ByteArray, w: Int, h: Int, rotation: Int) {
    when (rotation) {
        0 -> System.arraycopy(src, 0, dest, 0, src.size)
        90 -> rotateP90(src, dest, w, h)
        180 -> rotateP180(src, dest, w, h)
        270 -> rotateP270(src, dest, w, h)
    }
}

// NV21或NV12顺时针旋转
fun rotateSP(src: ByteArray, dest: ByteArray, w: Int, h: Int, rotation: Int) {
    when (rotation) {
        0 -> System.arraycopy(src, 0, dest, 0, src.size)
        90 -> rotateSP90(src, dest, w, h)
        180 -> rotateSP180(src, dest, w, h)
        270 -> rotateSP270(src, dest, w, h)
    }
}

// NV21或NV12顺时针旋转90度
fun rotateSP90(src: ByteArray, dest: ByteArray, w: Int, h: Int) {
    var pos = 0
    var k = 0
    for (i in 0..w - 1) {
        for (j in h - 1 downTo 0) {
            dest[k++] = src[j * w + i]
        }
    }
    pos = w * h
    var i = 0
    while (i <= w - 2) {
        for (j in h / 2 - 1 downTo 0) {
            dest[k++] = src[pos + j * w + i]
            dest[k++] = src[pos + j * w + i + 1]
        }
        i += 2
    }
}

// NV21或NV12顺时针旋转270度
fun rotateSP270(src: ByteArray, dest: ByteArray, w: Int, h: Int) {
    var pos = 0
    var k = 0
    for (i in w - 1 downTo 0) {
        for (j in 0..h - 1) {
            dest[k++] = src[j * w + i]
        }
    }
    pos = w * h
    var i = w - 2
    while (i >= 0) {
        for (j in 0..h / 2 - 1) {
            dest[k++] = src[pos + j * w + i]
            dest[k++] = src[pos + j * w + i + 1]
        }
        i -= 2
    }
}

// NV21或NV12顺时针旋转180度
fun rotateSP180(src: ByteArray, dest: ByteArray, w: Int, h: Int) {
    var pos = 0
    var k = w * h - 1
    while (k >= 0) {
        dest[pos++] = src[k--]
    }
    k = src.size - 2
    while (pos < dest.size) {
        dest[pos++] = src[k]
        dest[pos++] = src[k + 1]
        k -= 2
    }
}

// I420或YV12顺时针旋转90度
fun rotateP90(src: ByteArray, dest: ByteArray, w: Int, h: Int) {
    var pos = 0
    //旋转Y
    var k = 0
    for (i in 0 until w) {
        for (j in h - 1 downTo 0) {
            dest[k++] = src[j * w + i]
        }
    }
    //旋转U
    pos = w * h
    for (i in 0 until w / 2) {
        for (j in h / 2 - 1 downTo 0) {
            dest[k++] = src[pos + j * w / 2 + i]
        }
    }

    //旋转V
    pos = w * h * 5 / 4
    for (i in 0 until w / 2) {
        for (j in h / 2 - 1 downTo 0) {
            dest[k++] = src[pos + j * w / 2 + i]
        }
    }
}

// I420或YV12顺时针旋转270度
fun rotateP270(src: ByteArray, dest: ByteArray, w: Int, h: Int) {
    var pos = 0
    //旋转Y
    var k = 0
    for (i in w - 1 downTo 0) {
        for (j in 0 until h) {
            dest[k++] = src[j * w + i]
        }
    }
    //旋转U
    pos = w * h
    for (i in w / 2 - 1 downTo 0) {
        for (j in 0 until h / 2) {
            dest[k++] = src[pos + j * w / 2 + i]
        }
    }

    //旋转V
    pos = w * h * 5 / 4
    for (i in w / 2 - 1 downTo 0) {
        for (j in 0 until h / 2) {
            dest[k++] = src[pos + j * w / 2 + i]
        }
    }
}

// I420或YV12顺时针旋转180度
fun rotateP180(src: ByteArray, dest: ByteArray, w: Int, h: Int) {
    var pos = 0
    var k = w * h - 1
    while (k >= 0) {
        dest[pos++] = src[k--]
    }
    k = w * h * 5 / 4
    while (k >= w * h) {
        dest[pos++] = src[k--]
    }
    k = src.size - 1
    while (pos < dest.size) {
        dest[pos++] = src[k--]
    }
}