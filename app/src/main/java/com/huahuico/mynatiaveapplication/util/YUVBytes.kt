package com.huahuico.mynatiaveapplication.util

import java.nio.ByteBuffer

data class YUVBytes(var channelY : ByteBuffer, var channelUV1 : ByteBuffer, var channelUV2 : ByteBuffer, var width : Int, var height : Int, var chromaPixelStride : Int = 2, var channelYUV : ByteArray? = null)