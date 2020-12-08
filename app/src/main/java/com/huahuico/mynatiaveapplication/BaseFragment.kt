package com.huahuico.mynatiaveapplication

import android.content.ContentValues
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.provider.MediaStore
import android.view.View
import com.huahuico.mynatiaveapplication.rxpermissions.RxPermissionsFragment
import com.huahuico.mynatiaveapplication.util.Rx3Common
import io.reactivex.rxjava3.core.Observable
import io.reactivex.rxjava3.core.ObservableEmitter
import io.reactivex.rxjava3.core.ObservableOnSubscribe
import kotlinx.coroutines.*

abstract class BaseFragment : RxPermissionsFragment(), CoroutineScope by MainScope() {

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        initialize()
    }
    private fun initialize() {
        launch {
            val deferred = async(Dispatchers.IO) {
                loadAssetsBitmap(getAssetsImageNames())
            }
            val srcImages = deferred.await()
            renderingSrcImages(srcImages)
            val doSomething = async(Dispatchers.IO) {
                doSomething(srcImages)
            }
            renderingDstImages(doSomething.await())
        }
    }

    private fun loadAssetsBitmap(names: Array<String>) : Array<Bitmap> {
        return Array(names.size) {
            BitmapFactory.decodeStream(resources.assets.open(names[it]))
        }
    }

    abstract fun doSomething(bitmaps: Array<Bitmap>) : Array<Bitmap>
    abstract fun renderingDstImages(bitmaps: Array<Bitmap>)
    abstract fun renderingSrcImages(bitmaps: Array<Bitmap>)

    abstract fun getAssetsImageNames() : Array<String>

    open fun saveBitmapToAlbum(requireContext: Context?, image: Bitmap) {
        Observable.create(ObservableOnSubscribe { emitter: ObservableEmitter<Void?>? ->
            addPictureToAlbum(
                requireContext(),
                image,
                System.currentTimeMillis().toString()
            )
        } as ObservableOnSubscribe<Void?>).compose(Rx3Common.Obs_io_main())
            .to(Rx3Common.autoDispose(viewLifecycleOwner)).subscribe()
    }

    /**
     * 保存图片到picture 目录，Android Q适配，最简单的做法就是保存到公共目录，不用SAF存储
     *
     * @param context
     * @param bitmap
     * @param fileName
     */
    open fun addPictureToAlbum(context: Context, bitmap: Bitmap, fileName: String?): Boolean {
        val contentValues = ContentValues()
        contentValues.put(MediaStore.Images.Media.DISPLAY_NAME, fileName)
        contentValues.put(MediaStore.Images.Media.DESCRIPTION, fileName)
        contentValues.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg")
        val uri =
            context.contentResolver.insert(
                MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                contentValues
            )
                ?: return false
        try {
            context.contentResolver.openOutputStream(uri).use { outputStream ->
                bitmap.compress(Bitmap.CompressFormat.JPEG, 100, outputStream)
                return true
            }
        } catch (e: Exception) {
            e.printStackTrace()
            return false
        }
    }
}