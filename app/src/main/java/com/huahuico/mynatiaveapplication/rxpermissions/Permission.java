package com.huahuico.mynatiaveapplication.rxpermissions;

import java.util.List;

import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.functions.BiConsumer;
import io.reactivex.rxjava3.functions.Function;
import io.reactivex.rxjava3.functions.Predicate;

public class Permission {
    public final String name;
    public final boolean granted;
    public final boolean shouldShowRequestPermissionRationale;

    public Permission(String name, boolean granted) {
        this(name, granted, false);
    }

    public Permission(String name, boolean granted, boolean shouldShowRequestPermissionRationale) {
        this.name = name;
        this.granted = granted;
        this.shouldShowRequestPermissionRationale = shouldShowRequestPermissionRationale;
    }

    public Permission(List<Permission> permissions) {
        name = combineName(permissions);
        granted = combineGranted(permissions);
        shouldShowRequestPermissionRationale = combineShouldShowRequestPermissionRationale(permissions);
    }

    @Override
    @SuppressWarnings("SimplifiableIfStatement")
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        final Permission that = (Permission) o;

        if (granted != that.granted) return false;
        if (shouldShowRequestPermissionRationale != that.shouldShowRequestPermissionRationale)
            return false;
        return name.equals(that.name);
    }
    @Override
    public int hashCode() {
        int result = name.hashCode();
        result = 31 * result + (granted ? 1 : 0);
        result = 31 * result + (shouldShowRequestPermissionRationale ? 1 : 0);
        return result;
    }
    private String combineName(List<Permission> permissions) {
        return Observable.fromIterable(permissions).map(new Function<Permission, String>() {
            @Override
            public String apply(Permission permission) throws Throwable {
                return permission.name;
            }
        }).collectInto(new StringBuffer(), new BiConsumer<StringBuffer, String>() {
            @Override
            public void accept(StringBuffer stringBuffer, String s) throws Throwable {
                if (stringBuffer.length() == 0) {
                    stringBuffer.append(s);
                } else {
                    stringBuffer.append(",").append(s);
                }
            }
        }).blockingGet().toString();
    }
    private Boolean combineGranted(List<Permission> permissions) {
        return Observable.fromIterable(permissions).all(new Predicate<Permission>() {
            @Override
            public boolean test(Permission permission) throws Throwable {
                return permission.granted;
            }
        }).blockingGet();
    }
    private Boolean combineShouldShowRequestPermissionRationale(List<Permission> permissions) {
        return Observable.fromIterable(permissions).any(new Predicate<Permission>() {
            @Override
            public boolean test(Permission permission) throws Throwable {
                return permission.shouldShowRequestPermissionRationale;
            }
        }).blockingGet();
    }
}
