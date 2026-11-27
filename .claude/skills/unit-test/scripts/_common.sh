#!/usr/bin/env bash
# coverage.sh / mcdc.sh 공통 함수 (직접 실행하지 않고 source 한다)
#
# 프로덕션 소스는 C(.c)이면 clang(C 모드), 그 외는 clang++ 로 컴파일한다.
# 테스트 소스(Google Test)는 clang++ 로 컴파일하고 clang++ 로 링크한다.
# C 헤더를 C++ 테스트에서 쓰려면 헤더에 extern "C" 가드가 있어야 한다.
#
# 환경변수:
#   CC / CXX        컴파일러 (기본 clang / clang++)
#   C_FLAGS         C 소스 추가 옵션 (기본 -std=c11)
#   CXX_FLAGS       C++ 소스 추가 옵션 (기본 -std=c++17)
#   EXTRA_FLAGS     공통 추가 옵션 (예: "-Iinclude -Isrc")
#   LIBS            링크 라이브러리 (기본 gtest/gmock/pthread; gtest_main 이 main 제공)
#   IGNORE_REGEX    리포트에서 제외할 파일 경로 정규식
#   LLVM_PROFDATA, LLVM_COV

CC="${CC:-clang}"
CXX="${CXX:-clang++}"
C_FLAGS="${C_FLAGS:--std=c11}"
CXX_FLAGS="${CXX_FLAGS:--std=c++17}"
EXTRA_FLAGS="${EXTRA_FLAGS:-}"
LIBS="${LIBS:--lgtest -lgtest_main -lgmock -lpthread}"
IGNORE_REGEX="${IGNORE_REGEX:-(/tests?/|/usr/|gtest|gmock)}"
PROFDATA="${LLVM_PROFDATA:-llvm-profdata}"
COV="${LLVM_COV:-llvm-cov}"

# build_and_measure <프로덕션 소스...> -- <테스트 소스...> ; 인자: OUT, 추가 커버리지 플래그(COV_FLAGS)
# 결과: $OUT/test_bin, $OUT/test.profdata, $OUT/test-results.xml, $OUT/summary.json, $OUT/lcov.info
# 반환: 테스트 실행 종료 코드(0=성공). 빌드/병합 실패는 2.
build_and_measure() {
    local out="$1" src="$2" tst="$3" cov_flags="$4" f obj objs=() rc=0
    mkdir -p "$out/obj"

    for f in $src; do
        obj="$out/obj/$(echo "$f" | tr '/.' '__').o"
        case "$f" in
            *.c) # shellcheck disable=SC2086
                 "$CC" -g -O0 $cov_flags $C_FLAGS $EXTRA_FLAGS -c "$f" -o "$obj" || return 2 ;;
            *)   # shellcheck disable=SC2086
                 "$CXX" -g -O0 $cov_flags $CXX_FLAGS $EXTRA_FLAGS -c "$f" -o "$obj" || return 2 ;;
        esac
        objs+=("$obj")
    done
    for f in $tst; do
        obj="$out/obj/$(echo "$f" | tr '/.' '__').o"
        # shellcheck disable=SC2086
        "$CXX" -g -O0 $cov_flags $CXX_FLAGS $EXTRA_FLAGS -c "$f" -o "$obj" || return 2
        objs+=("$obj")
    done
    # shellcheck disable=SC2086
    "$CXX" $cov_flags "${objs[@]}" $LIBS -o "$out/test_bin" || return 2

    LLVM_PROFILE_FILE="$out/test.profraw" "$out/test_bin" \
        --gtest_output="xml:$out/test-results.xml" || rc=$?

    "$PROFDATA" merge -sparse "$out/test.profraw" -o "$out/test.profdata" || return 2
    return "$rc"
}

# export_summaries <OUT>: JSON 요약(게이트 판정용)과 lcov(리포트 플러그인용) 생성
export_summaries() {
    local out="$1"
    "$COV" export "$out/test_bin" -instr-profile="$out/test.profdata" \
        -ignore-filename-regex="$IGNORE_REGEX" -summary-only > "$out/summary.json"
    "$COV" export "$out/test_bin" -instr-profile="$out/test.profdata" \
        -ignore-filename-regex="$IGNORE_REGEX" -format=lcov > "$out/lcov.info"
}
