#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <new>
#include <thread>

namespace {

constexpr auto log_tag = "PocketScene";

void log_error(const char* message) {
    __android_log_print(ANDROID_LOG_ERROR, log_tag, "%s (EGL error: 0x%x)", message, eglGetError());
}

class NativeRenderer final {
  public:
    NativeRenderer() : render_thread_(&NativeRenderer::render_loop, this) {}

    ~NativeRenderer() {
        ANativeWindow* requested_window = nullptr;
        {
            std::lock_guard lock{mutex_};
            stopping_ = true;
            requested_window = requested_window_;
            requested_window_ = nullptr;
            ++surface_generation_;
        }

        condition_.notify_one();
        render_thread_.join();

        if (requested_window != nullptr) {
            ANativeWindow_release(requested_window);
        }
    }

    NativeRenderer(const NativeRenderer&) = delete;
    NativeRenderer& operator=(const NativeRenderer&) = delete;

    // Takes ownership of the reference returned by ANativeWindow_fromSurface.
    void attach_surface(ANativeWindow* window) {
        ANativeWindow* previous_window = nullptr;
        {
            std::lock_guard lock{mutex_};
            previous_window = requested_window_;
            requested_window_ = window;
            ++surface_generation_;
        }

        condition_.notify_one();

        if (previous_window != nullptr) {
            ANativeWindow_release(previous_window);
        }
    }

    void detach_surface() { attach_surface(nullptr); }

  private:
    bool initialize_egl() {
        display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display_ == EGL_NO_DISPLAY || eglInitialize(display_, nullptr, nullptr) != EGL_TRUE) {
            log_error("Failed to initialize EGL display");
            return false;
        }

        if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) {
            log_error("Failed to bind the OpenGL ES API");
            return false;
        }

        constexpr EGLint config_attributes[] = {
            EGL_SURFACE_TYPE,
            EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE,
            8,
            EGL_GREEN_SIZE,
            8,
            EGL_BLUE_SIZE,
            8,
            EGL_ALPHA_SIZE,
            8,
            EGL_DEPTH_SIZE,
            24,
            EGL_NONE,
        };

        EGLint config_count = 0;
        if (eglChooseConfig(display_, config_attributes, &config_, 1, &config_count) != EGL_TRUE ||
            config_count == 0) {
            log_error("Failed to select an EGL config");
            return false;
        }

        constexpr EGLint context_attributes[] = {
            EGL_CONTEXT_CLIENT_VERSION,
            3,
            EGL_NONE,
        };

        context_ = eglCreateContext(display_, config_, EGL_NO_CONTEXT, context_attributes);
        if (context_ == EGL_NO_CONTEXT) {
            log_error("Failed to create an OpenGL ES 3 context");
            return false;
        }

        return true;
    }

    bool create_window_surface(ANativeWindow* window) {
        if (display_ == EGL_NO_DISPLAY && !initialize_egl()) {
            return false;
        }

        surface_ = eglCreateWindowSurface(display_, config_, window, nullptr);
        if (surface_ == EGL_NO_SURFACE) {
            log_error("Failed to create an EGL window surface");
            return false;
        }

        if (eglMakeCurrent(display_, surface_, surface_, context_) != EGL_TRUE) {
            log_error("Failed to make the EGL context current");
            destroy_window_surface();
            return false;
        }

        eglSwapInterval(display_, 1);
        return true;
    }

    void destroy_window_surface() {
        if (display_ == EGL_NO_DISPLAY || surface_ == EGL_NO_SURFACE) {
            return;
        }

        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(display_, surface_);
        surface_ = EGL_NO_SURFACE;
    }

    void shutdown_egl() {
        destroy_window_surface();

        if (display_ != EGL_NO_DISPLAY && context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
        }
        if (display_ != EGL_NO_DISPLAY) {
            eglTerminate(display_);
        }

        context_ = EGL_NO_CONTEXT;
        display_ = EGL_NO_DISPLAY;
        config_ = nullptr;
    }

    void draw_frame(ANativeWindow* window) {
        const auto width = ANativeWindow_getWidth(window);
        const auto height = ANativeWindow_getHeight(window);
        if (width <= 0 || height <= 0) {
            return;
        }

        glViewport(0, 0, width, height);
        glClearColor(0.055F, 0.16F, 0.24F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (eglSwapBuffers(display_, surface_) != EGL_TRUE) {
            log_error("Failed to swap EGL buffers");
        }
    }

    void render_loop() {
        using namespace std::chrono_literals;

        ANativeWindow* current_window = nullptr;
        std::uint64_t applied_generation = 0;

        while (true) {
            ANativeWindow* next_window = current_window;
            bool surface_changed = false;

            {
                std::unique_lock lock{mutex_};
                condition_.wait_for(lock, 16ms, [this, applied_generation] {
                    return stopping_ || surface_generation_ != applied_generation;
                });

                if (stopping_) {
                    break;
                }

                if (surface_generation_ != applied_generation) {
                    next_window = requested_window_;
                    if (next_window != nullptr) {
                        ANativeWindow_acquire(next_window);
                    }
                    applied_generation = surface_generation_;
                    surface_changed = true;
                }
            }

            if (surface_changed) {
                destroy_window_surface();
                if (current_window != nullptr) {
                    ANativeWindow_release(current_window);
                }

                current_window = next_window;
                if (current_window != nullptr && !create_window_surface(current_window)) {
                    ANativeWindow_release(current_window);
                    current_window = nullptr;
                }
            }

            if (current_window != nullptr && surface_ != EGL_NO_SURFACE) {
                draw_frame(current_window);
            }
        }

        destroy_window_surface();
        if (current_window != nullptr) {
            ANativeWindow_release(current_window);
        }
        shutdown_egl();
    }

    std::mutex mutex_;
    std::condition_variable condition_;
    std::thread render_thread_;
    ANativeWindow* requested_window_ = nullptr;
    std::uint64_t surface_generation_ = 0;
    bool stopping_ = false;

    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLConfig config_ = nullptr;
    EGLContext context_ = EGL_NO_CONTEXT;
    EGLSurface surface_ = EGL_NO_SURFACE;
};

NativeRenderer* from_handle(const jlong handle) {
    return reinterpret_cast<NativeRenderer*>(static_cast<std::uintptr_t>(handle));
}

jlong to_handle(NativeRenderer* renderer) {
    return static_cast<jlong>(reinterpret_cast<std::uintptr_t>(renderer));
}

} // namespace

extern "C" JNIEXPORT jlong JNICALL
Java_dev_pocketscene_app_rendering_NativeRenderer_nativeCreate(JNIEnv* env, jobject) {
    auto* renderer = new (std::nothrow) NativeRenderer{};
    if (renderer == nullptr) {
        const auto exception = env->FindClass("java/lang/OutOfMemoryError");
        env->ThrowNew(exception, "Unable to allocate native renderer");
        return 0;
    }
    return to_handle(renderer);
}

extern "C" JNIEXPORT void JNICALL
Java_dev_pocketscene_app_rendering_NativeRenderer_nativeAttachSurface(JNIEnv* env, jobject,
                                                                      const jlong handle,
                                                                      jobject surface) {
    auto* renderer = from_handle(handle);
    if (renderer == nullptr || surface == nullptr) {
        return;
    }

    auto* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr) {
        const auto exception = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(exception, "Surface does not provide an ANativeWindow");
        return;
    }

    renderer->attach_surface(window);
}

extern "C" JNIEXPORT void JNICALL
Java_dev_pocketscene_app_rendering_NativeRenderer_nativeDetachSurface(JNIEnv*, jobject,
                                                                      const jlong handle) {
    if (auto* renderer = from_handle(handle); renderer != nullptr) {
        renderer->detach_surface();
    }
}

extern "C" JNIEXPORT void JNICALL Java_dev_pocketscene_app_rendering_NativeRenderer_nativeDestroy(
    JNIEnv*, jobject, const jlong handle) {
    delete from_handle(handle);
}
