#!/usr/bin/env bash
set -euo pipefail
task_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
task_build="${task_root}/build-form-engine"
mkdir -p "${task_build}"
${CXX:-g++} -std=c++17 -O2 -fPIC -Wall -Wextra -pthread \
    "${task_root}/tests/form-engine.cpp" \
    "${task_root}/source/form-engine/FormEngine.cpp" \
    "${task_root}/source/form-engine/FormSimulation.cpp" \
    "${task_root}/source/simulation-v2/RandomSource.cpp" \
    $(pkg-config --cflags --libs Qt5Core) -o "${task_build}/form-engine-tests"
"${task_build}/form-engine-tests"
