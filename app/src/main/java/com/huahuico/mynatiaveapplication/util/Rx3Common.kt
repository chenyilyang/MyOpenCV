package com.huahuico.mynatiaveapplication.util

import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleOwner
import autodispose2.AutoDispose
import autodispose2.AutoDisposeConverter
import autodispose2.androidx.lifecycle.AndroidLifecycleScopeProvider
import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers
import io.reactivex.rxjava3.core.FlowableTransformer
import io.reactivex.rxjava3.core.ObservableTransformer
import io.reactivex.rxjava3.schedulers.Schedulers

class Rx3Common {
    companion object {
        fun <T> Flo_io_main(): FlowableTransformer<T, T>? {
            return FlowableTransformer<T, T> { upstream ->
                upstream.subscribeOn(Schedulers.io()).observeOn(AndroidSchedulers.mainThread())
            }
        }

        fun <T> Obs_io_main(): ObservableTransformer<T, T>? {
            return ObservableTransformer<T, T> { upstream ->
                upstream.subscribeOn(Schedulers.io()).observeOn(AndroidSchedulers.mainThread())
            }
        }

        fun <T> autoDispose(lifecycleOwner: LifecycleOwner?): AutoDisposeConverter<T> {
            return AutoDispose.autoDisposable(
                AndroidLifecycleScopeProvider.from(
                    lifecycleOwner,
                    Lifecycle.Event.ON_DESTROY
                )
            )
        }
    }
}