package com.huahuico.mynatiaveapplication.rxpermissions;

import android.annotation.TargetApi;
import android.app.Activity;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.VisibleForTesting;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.fragment.app.FragmentManager;

import java.util.ArrayList;
import java.util.List;

import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.core.ObservableSource;
import io.reactivex.rxjava3.core.ObservableTransformer;
import io.reactivex.rxjava3.functions.Function;
import io.reactivex.rxjava3.subjects.PublishSubject;

public class RxPermissions {
    /**
     * 延时实例化的接口声明
     * @param <T>
     */
    @FunctionalInterface
    public interface Lazy<T> {
        T get();
    }
    private final static String TAG = RxPermissions.class.getSimpleName();
    private final static Object TRIGGER = new Object();
    @VisibleForTesting
    Lazy<RxPermissionsFragment> mRxPermissionFragment;

    public RxPermissions(@NonNull final FragmentActivity activity) {
        mRxPermissionFragment = getLazySingleton(activity.getSupportFragmentManager());
    }
    public RxPermissions(@NonNull final Fragment fragment) {
        mRxPermissionFragment = getLazySingleton(fragment.getChildFragmentManager());
    }

    private Lazy<RxPermissionsFragment> getLazySingleton(@NonNull final FragmentManager fragmentManager) {
        return new Lazy<RxPermissionsFragment>() {
            private RxPermissionsFragment rxPermissionsFragment;

            @Override
            public RxPermissionsFragment get() {
                if (rxPermissionsFragment == null) {
                    rxPermissionsFragment = getRxPermissionsFragment(fragmentManager);
                }
                return rxPermissionsFragment;
            }
        };
    }
    private RxPermissionsFragment getRxPermissionsFragment(@NonNull final FragmentManager fragmentManager) {
        RxPermissionsFragment rxPermissionsFragment = findRxPermissionsFragment(fragmentManager);
        boolean isNewInstance = rxPermissionsFragment == null;
        if (isNewInstance) {
            rxPermissionsFragment = new RxPermissionsFragment();
            fragmentManager.beginTransaction().add(rxPermissionsFragment, TAG).commitNow();
        }
        return rxPermissionsFragment;
    }
    private RxPermissionsFragment findRxPermissionsFragment(@NonNull final FragmentManager fragmentManager) {
        return (RxPermissionsFragment) fragmentManager.findFragmentByTag(TAG);
    }
    public Observable<Boolean> request(final String ... permissions) {
        return Observable.just(TRIGGER).compose(ensure(permissions));
    }
    public Observable<Permission> requestEach(final String ... permissions) {
        return Observable.just(TRIGGER).compose(ensureEach(permissions));
    }
    public Observable<Permission> requestEachCombined(final String ... permissions) {
        return Observable.just(TRIGGER).compose(ensureEachCombined(permissions));
    }

    /**
     * 对多个权限申请只进行一次确认，有一个不通过都会返回false
     * @param permissions
     * @param <T>
     * @return boolean类型的被观察者
     */
    public <T> ObservableTransformer<T, Boolean> ensure(final String ... permissions) {
        return new ObservableTransformer<T, Boolean>() {
            @Override
            public ObservableSource<Boolean> apply(Observable<T> upstream) {
                return request(upstream, permissions)
                        .buffer(permissions.length)
                        .flatMap(new Function<List<Permission>, ObservableSource<Boolean>>() {
                            @Override
                            public ObservableSource<Boolean> apply(List<Permission> permissions) throws Throwable {
                                if (permissions.isEmpty()) {
                                    return Observable.empty();
                                }
                                for (Permission permission : permissions) {
                                    if (!permission.granted) {
                                        return Observable.just(false);
                                    }
                                }
                                return Observable.just(true);
                            }
                        });
            }
        };
    }

    /**
     * 对每一个权限申请的结果进行确认
     * @param permissions
     * @param <T>
     * @return
     */
    public <T> ObservableTransformer<T, Permission> ensureEach(final String ... permissions) {
        return new ObservableTransformer<T, Permission>() {
            @Override
            public ObservableSource<Permission> apply(Observable<T> upstream) {
                return request(upstream, permissions);
            }
        };
    }

    public <T> ObservableTransformer<T, Permission> ensureEachCombined(final String ... permissions) {
        return new ObservableTransformer<T, Permission>() {
            @Override
            public ObservableSource<Permission> apply(Observable<T> upstream) {
                return request(upstream, permissions).buffer(permissions.length)
                        .flatMap(new Function<List<Permission>, ObservableSource<Permission>>() {
                            @Override
                            public ObservableSource<Permission> apply(List<Permission> permissions) throws Throwable {
                                if (permissions.isEmpty()) {
                                    return Observable.empty();
                                }
                                return Observable.just(new Permission(permissions));
                            }
                        });
            }
        };
    }

    /**
     * 对一到多个权限进行申请
     * @param trigger
     * @param permissions
     * @return
     */
    public Observable<Permission> request(final Observable<?> trigger, final String ... permissions) {
        if (permissions == null || permissions.length == 0) {
            throw new IllegalStateException("RxPermissions.request/requestEach requires at least one input permission");
        }
        return oneOf(trigger, pending(permissions))
                .flatMap(new Function<Object, Observable<Permission>>() {
                    @Override
                    public Observable<Permission> apply(Object o) throws Throwable {
                        return requestImplementation(permissions);
                    }
                });
    }
    private Observable<?> oneOf(Observable<?> trigger, Observable<?> pending) {
        if (trigger == null) {
            return Observable.just(TRIGGER);
        }
        return Observable.merge(trigger, pending);
    }

    private Observable<?> pending(final String ... permissions) {
        for (String permission : permissions) {
            if (!mRxPermissionFragment.get().containsByPermission(permission)) {
                return Observable.empty();
            }
        }
        return Observable.just(TRIGGER);
    }

    /**
     * 实际向系统申请权限的实现
     * @param permissions
     * @return
     */
    private Observable<Permission> requestImplementation(final String ... permissions) {
        List<Observable<Permission>> permissionList = new ArrayList<>(permissions.length);
        List<String> unrequestedPermissions = new ArrayList<>();
        for (String permission : permissions) {
            Log.d(TAG, "Requesting permission " + permission);
            if (isGranted(permission)) {
                permissionList.add(Observable.just(new Permission(permission, true, false)));
                continue;
            }
            if (isRevoked(permission)) {
                permissionList.add(Observable.just(new Permission(permission, false, false)));
                continue;
            }
            PublishSubject<Permission> subject = mRxPermissionFragment.get().getSubjectByPermission(permission);
            if (subject == null) {
                unrequestedPermissions.add(permission);
                subject = PublishSubject.create();
                mRxPermissionFragment.get().setSubjectForPermission(permission, subject);
            }
            permissionList.add(subject);
        }
        if (!unrequestedPermissions.isEmpty()) {
            String [] unrequestPermissionsArray = unrequestedPermissions.toArray(new String[unrequestedPermissions.size()]);
            requestPermissionsFromFragment(unrequestPermissionsArray);
        }
        return Observable.concat(Observable.fromIterable(permissionList));
    }
    public Observable<Boolean> shouldShowRequestPermissionRationale(final Activity activity, final String ... permissions) {
        if (!isMarshmallow()) {
            return Observable.just(false);
        }
        return Observable.just(shouldShowRequestPermissionRationaleImplementation(activity, permissions));
    }
    @TargetApi(Build.VERSION_CODES.M)
    private boolean shouldShowRequestPermissionRationaleImplementation(final Activity activity, final String ... permissions) {
        for (String permission : permissions) {
            if (!isGranted(permission) && !activity.shouldShowRequestPermissionRationale(permission)) {
                return false;
            }
        }
        return true;
    }
    void requestPermissionsFromFragment(String [] permissions) {
        Log.d(TAG, "reqeustPermissionsFromFragment" + TextUtils.join(", ", permissions));
        mRxPermissionFragment.get().requestPermissions(permissions);
    }
    @TargetApi(Build.VERSION_CODES.M)
    public boolean isGranted(String permission) {
        return isMarshmallow() && mRxPermissionFragment.get().isGranted(permission);
    }

    boolean isMarshmallow() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.M;
    }
    public boolean isRevoked(String permission) {
        return isMarshmallow() && mRxPermissionFragment.get().isRevoked(permission);
    }

    void onRequestPermissionsResult(String [] permissions, int [] grantResults) {
        mRxPermissionFragment.get().onRequestPermissionsResult(permissions, grantResults, new boolean[permissions.length]);
    }
}
