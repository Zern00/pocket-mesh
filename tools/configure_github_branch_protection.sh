#!/usr/bin/env bash

set -euo pipefail

repository="${GITHUB_REPOSITORY:-Zern00/pocket-mesh}"
github_token="${GH_TOKEN:-${GITHUB_TOKEN:-}}"

if [[ -z "${github_token}" ]]; then
    cat >&2 <<'EOF'
GH_TOKEN or GITHUB_TOKEN is required.
The token must be allowed to edit repository branch protection rules.
EOF
    exit 1
fi

curl --fail-with-body --silent --show-error --location \
    --request PUT \
    --header "Accept: application/vnd.github+json" \
    --header "Authorization: Bearer ${github_token}" \
    --header "X-GitHub-Api-Version: 2026-03-10" \
    "https://api.github.com/repos/${repository}/branches/main/protection" \
    --data @- \
    --output /dev/null <<'JSON'
{
  "required_status_checks": {
    "strict": true,
    "contexts": [
      "Branch name",
      "Android debug build and tests",
      "C++ build and tests",
      "API contracts"
    ]
  },
  "enforce_admins": true,
  "required_pull_request_reviews": {
    "dismiss_stale_reviews": true,
    "require_code_owner_reviews": false,
    "required_approving_review_count": 1,
    "require_last_push_approval": true
  },
  "restrictions": null,
  "required_linear_history": true,
  "allow_force_pushes": false,
  "allow_deletions": false,
  "block_creations": false,
  "required_conversation_resolution": true,
  "lock_branch": false,
  "allow_fork_syncing": false
}
JSON

printf 'Protected %s:main; changes now require an approved pull request.\n' "${repository}"
