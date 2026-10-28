#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${repo_root}/build-simulation-v2-tests"
mkdir -p "${build_dir}"
sources=()
for unit in RandomSource HillSkillCurve QualityModel Weather FlightModel LandingModel Scoring Manipulation Engine; do
    sources+=("${repo_root}/source/simulation-v2/${unit}.cpp")
done
"${CXX:-g++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread "${sources[@]}" \
    "${repo_root}/tests/simulation-v2-rain-analysis.cpp" -o "${build_dir}/rain-analysis"
exec "${build_dir}/rain-analysis" "${1:-5000}"
