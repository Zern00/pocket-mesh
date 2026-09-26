#!/usr/bin/env bash

set -euo pipefail

branch_name="${1:-}"
if [[ -z "${branch_name}" ]]; then
    branch_name="$(git branch --show-current)"
fi

branch_pattern='^(feat|fix|refactor|perf|test|docs|build|ci|chore)/[a-z0-9]+(-[a-z0-9]+)*$'

if [[ "${branch_name}" =~ ${branch_pattern} ]]; then
    printf 'Valid branch name: %s\n' "${branch_name}"
    exit 0
fi

cat >&2 <<EOF
Invalid branch name: ${branch_name:-<detached HEAD>}

Expected: <type>/<kebab-case-description>
Types: feat, fix, refactor, perf, test, docs, build, ci, chore
Examples: feat/gltf-loader, fix/surface-lifecycle, ci/android-cache
EOF
exit 1
