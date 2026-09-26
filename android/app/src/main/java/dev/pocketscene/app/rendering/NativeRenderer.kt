package dev.pocketscene.app.rendering

import android.view.Surface
import java.io.Closeable

internal class NativeRenderer : Closeable {
    private var nativeHandle: Long = nativeCreate()

    fun attachSurface(surface: Surface) {
        val handle = nativeHandle
        check(handle != 0L) { "Renderer is already closed" }
        nativeAttachSurface(handle, surface)
    }

    fun detachSurface() {
        val handle = nativeHandle
        if (handle != 0L) {
            nativeDetachSurface(handle)
        }
    }

    override fun close() {
        val handle = nativeHandle
        if (handle != 0L) {
            nativeHandle = 0L
            nativeDestroy(handle)
        }
    }

    private companion object {
        init {
            System.loadLibrary("pocketscene-jni")
        }

        @JvmStatic
        private external fun nativeCreate(): Long

        @JvmStatic
        private external fun nativeAttachSurface(handle: Long, surface: Surface)

        @JvmStatic
        private external fun nativeDetachSurface(handle: Long)

        @JvmStatic
        private external fun nativeDestroy(handle: Long)
    }
}
