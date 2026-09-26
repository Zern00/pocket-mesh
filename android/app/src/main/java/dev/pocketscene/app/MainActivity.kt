package dev.pocketscene.app

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import dev.pocketscene.app.rendering.NativeRendererView

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            MaterialTheme {
                PocketSceneApp()
            }
        }
    }
}

@Composable
private fun PocketSceneApp() {
    Surface(
        modifier = Modifier.fillMaxSize(),
        color = Color(0xFF10131A),
    ) {
        Box(modifier = Modifier.fillMaxSize()) {
            NativeRendererView(modifier = Modifier.fillMaxSize())
            Text(
                text = "PocketScene · OpenGL ES smoke test",
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .statusBarsPadding()
                    .padding(16.dp),
                color = Color.White,
                style = MaterialTheme.typography.titleMedium,
            )
        }
    }
}
