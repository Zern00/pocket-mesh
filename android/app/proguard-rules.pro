# Keep JNI entry points if shrinking is enabled later.
-keepclasseswithmembernames class * {
    native <methods>;
}
