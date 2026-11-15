#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
exec "${repo_root}/scripts/with-qt-runtime.sh" "${repo_root}/build-quick-ui/sj-sim" "$@"
