# Development workflow

The project uses a lightweight trunk-based workflow. `main` must remain
buildable; work happens in short-lived branches and is integrated through pull
requests.

## Branches

Direct development in `main` is forbidden. Start every change from an updated
`main` and create a short-lived branch whose name matches
`<type>/<kebab-case-description>`:

```text
feat/short-name
fix/short-name
refactor/short-name
perf/short-name
test/short-name
chore/short-name
docs/short-name
build/short-name
ci/short-name
```

Uppercase letters, underscores, a missing prefix, and a branch named `main`
are rejected for pull requests. Check a name locally with:

```bash
tools/check_branch_name.sh feat/gltf-loader
```

Keep branches focused on one vertical change. Avoid long-running branches per
team member or subsystem because renderer, backend, and AI need frequent
integration.

## Contract-first changes

For a REST or WebSocket change:

1. Edit the relevant file under `schemas/`.
2. Add or update examples and semantics in `schemas/README.md`.
3. Run `python tools/validate_contracts.py`.
4. Implement the server handler and domain operation.
5. Implement or regenerate the client model.
6. Add an integration test covering the request or event.

Before the first Android integration, incompatible changes may be made directly.
After integration begins, breaking changes require either a migration window or
a new API/protocol version.

## Local checks

Run before opening a pull request:

```bash
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
./gradlew :android:app:assembleDebug :android:app:testDebugUnitTest :android:app:lintDebug
.venv/bin/python tools/validate_contracts.py
git diff --check
```

Run `clang-format` on changed C++ files. Do not format unrelated files in the
same pull request.

## Pull requests

A pull request should explain the outcome, contract impact, verification, and
known risks. Prefer changes that can be reviewed independently and demonstrate
one working path through the system.

Required protected-branch checks:

- `Branch name`;
- `Android debug build and tests`;
- `C++ build and tests`;
- `API contracts`;

`main` must also be protected on GitHub. A merge requires all four checks, an
up-to-date branch, one approving review from someone other than the last
pusher, and all review conversations resolved. Direct pushes, force pushes,
branch deletion, and administrator bypass are disabled. An administrator can
apply the exact policy after placing a suitable token in `GH_TOKEN`:

```bash
tools/configure_github_branch_protection.sh
```

This server-side rule is required: CI alone runs after a push and therefore
cannot prevent a direct update of `main`.

Use squash merge for small feature branches. The squash commit should describe
the result rather than the sequence of intermediate fixes.

## Definition of done

A change is done when:

- code builds without warnings introduced by the change;
- unit or integration tests cover important behavior;
- public contracts and migrations match the implementation;
- errors are observable and do not expose secrets;
- documentation describes any new setup step;
- CI passes;
- temporary compatibility code has an explicit follow-up issue.

## Initial delivery order

```text
1. Local GLB load and render
2. Hierarchy and debug modes
3. Triangle picking
4. Backend project/asset metadata
5. Persistent annotations
6. Realtime review session
7. Semantic indexing and search
8. Additional rendering backends
```

Vulkan, Metal, advanced PBR, and image-to-3D must not block steps 1-7.
