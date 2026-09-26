package dev.pocketscene.app.rendering

import android.content.Context
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView

private class RendererSurfaceView(
    context: Context,
    private val renderer: NativeRenderer,
) : SurfaceView(context), SurfaceHolder.Callback {

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        renderer.attachSurface(holder.surface)
    }

    override fun surfaceChanged(
        holder: SurfaceHolder,
        format: Int,
        width: Int,
        height: Int,
    ) = Unit

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        renderer.detachSurface()
    }
}

@Composable
internal fun NativeRendererView(modifier: Modifier = Modifier) {
    val renderer = remember { NativeRenderer() }

    DisposableEffect(renderer) {
        onDispose(renderer::close)
    }

    AndroidView(
        factory = { context -> RendererSurfaceView(context, renderer) },
        modifier = modifier,
    )
}
