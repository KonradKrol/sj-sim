#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${repo_root}/build-simulation-v2-tests"
mkdir -p "${build_dir}"
"${CXX:-g++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread \
    "${repo_root}/source/simulation-v2/RandomSource.cpp" \
    "${repo_root}/tests/simulation-v2-random.cpp" \
    -o "${build_dir}/simulation-v2-random"
"${build_dir}/simulation-v2-random"
"${CXX:-g++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread \
    "${repo_root}/source/simulation-v2/RandomSource.cpp" \
    "${repo_root}/source/simulation-v2/HillSkillCurve.cpp" \
    "${repo_root}/source/simulation-v2/QualityModel.cpp" \
    "${repo_root}/tests/simulation-v2-quality.cpp" \
    -o "${build_dir}/simulation-v2-quality"
"${build_dir}/simulation-v2-quality"
"${CXX:-g++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread \
    "${repo_root}/source/simulation-v2/RandomSource.cpp" \
    "${repo_root}/source/simulation-v2/Weather.cpp" \
    "${repo_root}/source/simulation-v2/WeatherPresets.cpp" \
    "${repo_root}/tests/simulation-v2-weather.cpp" \
    -o "${build_dir}/simulation-v2-weather"
"${build_dir}/simulation-v2-weather"
"${CXX:-g++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread \
    "${repo_root}/source/simulation-v2/RandomSource.cpp" \
    "${repo_root}/source/simulation-v2/HillSkillCurve.cpp" \
    "${repo_root}/source/simulation-v2/QualityModel.cpp" \
    "${repo_root}/source/simulation-v2/Weather.cpp" \
    "${repo_root}/source/simulation-v2/FlightModel.cpp" \
    "${repo_root}/source/simulation-v2/LandingModel.cpp" \
    "${repo_root}/source/simulation-v2/Scoring.cpp" \
    "${repo_root}/source/simulation-v2/Manipulation.cpp" \
    "${repo_root}/source/simulation-v2/Engine.cpp" \
    "${repo_root}/tests/simulation-v2-engine.cpp" \
    -o "${build_dir}/simulation-v2-engine"
"${build_dir}/simulation-v2-engine"
