buildscript {
    dependencies {
        // AGP 9 uses built-in Kotlin. This selects the compiler version used by
        // the Compose compiler plugin without applying kotlin-android.
        classpath("org.jetbrains.kotlin:kotlin-gradle-plugin:2.4.10")
    }
}

plugins {
    id("com.android.application") version "9.4.1" apply false
    id("org.jetbrains.kotlin.plugin.compose") version "2.4.10" apply false
}
