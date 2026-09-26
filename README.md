# PocketScene

PocketScene is a mobile inspector and collaborative review tool for glTF/GLB
assets. The repository currently contains the project skeleton: an Android
application, a C++20 rendering core, a backend placeholder, API contracts, and
local development infrastructure.

The repository name is still `pocket-mesh`; `PocketScene` is the working
product and CMake project name.

## Target architecture

```text
Android (Kotlin/Compose)                  Future iOS client
          |                                      |
          +---------- RenderingServer -----------+
                             |
                  C++ scene and resource core
                     /                   \
        OpenGL ES compatibility       RenderingDevice
                                         /     \
                                    Vulkan     Metal

Android client -- HTTPS/WebSocket --> C++ backend
                                           |
                              PostgreSQL + pgvector
```

The graphics module follows a deliberately small, Godot-inspired boundary:
the application owns product state, while the C++ `RenderingServer` owns
render resources represented by opaque RIDs. Graphics API objects must never
cross that boundary.

## Repository layout

```text
android/                 Android application and JNI/EGL bridge
renderer/                C++ rendering core and backend-independent API
server/                  C++ backend and SQL migrations
ai/                      Python AI service (added with semantic search)
schemas/openapi/         Public REST contract
schemas/websocket/       Realtime protocol contract
infra/                   Local development services
docs/                    Architecture and development workflow
```

Directories that have no implementation yet are intentionally not populated
with placeholder frameworks.

## Prerequisites

- CMake 3.25+
- a C++20 compiler
- Ninja
- Android Studio 2026.1.4+ with JDK 17+
- Android SDK Platform 37.0, Build Tools 36.0.0, and NDK 30.0.16248370
- Python 3.10+ for contract validation
- Docker with Compose for local PostgreSQL

## Build and test

```bash
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The backend executable is currently a compile-time skeleton, not an HTTP
server:

```bash
./build/dev/server/pocketscene-server --version
```

## Run the Android application

Open the repository root in Android Studio, wait for Gradle Sync, select the
`app` run configuration, and run it on an API 24+ device or emulator.
The first screen is an executable smoke test: Kotlin/Compose owns the Android
lifecycle and UI, while a C++ render thread creates an EGL/OpenGL ES 3 context
and paints the `SurfaceView`.

For command-line builds, create the machine-local `local.properties` file if
Android Studio has not already done so:

```properties
sdk.dir=/absolute/path/to/Android/Sdk
```

Then build and install the debug APK:

```bash
./gradlew :android:app:assembleDebug
./gradlew :android:app:installDebug
```

The APK is written to
`android/app/build/outputs/apk/debug/app-debug.apk`. `local.properties` is
intentionally ignored because SDK paths differ between machines. The Gradle
build pins all other tool versions and builds both `arm64-v8a` and `x86_64`.

## Validate contracts

Create a disposable virtual environment and install the pinned validation
tools:

```bash
python3 -m venv .venv
.venv/bin/pip install -r tools/requirements.txt
.venv/bin/python tools/validate_contracts.py
```

REST is described by OpenAPI 3.1. WebSocket messages use JSON Schema. These
contracts are provisional: breaking changes are allowed before the first
mobile integration, but must be made in `schemas/` before implementation.

## Local database

```bash
cp .env.example .env
docker compose -f infra/compose.yaml up -d postgres
docker compose -f infra/compose.yaml down
```

PostgreSQL is exposed on `localhost:5432` by default. The Compose environment
is for development only; its credentials must not be reused in deployment.
Binary assets use a local filesystem directory during the first vertical
slice. An S3-compatible service will be selected when upload/storage code is
implemented.

## Development workflow

1. Update `main`, then create a short-lived branch matching
   `<type>/<kebab-case-description>`; never develop directly in `main`.
2. Change OpenAPI or WebSocket schemas first when crossing a component boundary.
3. Implement the smallest vertical slice and add tests.
4. Run the same commands as CI locally.
5. Open a pull request to `main`; direct pushes, force pushes, and deletion of
   `main` are forbidden.

See [docs/development.md](docs/development.md) for branch rules, contract
changes, and the definition of done.

## Current scope

The first end-to-end milestone is:

```text
download/open GLB
        -> create render resources
        -> display hierarchy and model
        -> enable wireframe/normals
        -> pick a triangle
```

Authentication, collaboration, semantic search, Vulkan, Metal, and image-to-3D
are introduced only after that local inspector flow works.

## License

MIT. See [LICENSE](LICENSE).
