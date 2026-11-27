# ClaudeCLangBuild: 개발 빌드와 CI 빌드가 공통으로 사용하는 이미지.
# Docker 이미지 이름은 소문자만 허용되므로 태그는 claudeclangbuild 이다.
#   docker build -t claudeclangbuild:latest -f docker/build/Dockerfile docker/build
FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
# Clang/LLVM 주 버전 (18 이상, MC/DC 는 18+). apt.llvm.org 의 해당 버전을 사용한다.
ARG LLVM_VERSION=20
ARG PICT_VERSION=v3.7.4

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates curl gnupg git make g++ \
        cmake ninja-build cppcheck cloc doxygen graphviz \
        python3 python3-venv python3-pip \
        libgtest-dev libgmock-dev \
    && rm -rf /var/lib/apt/lists/*

# LLVM (clang, clang++, clang-tidy, clang-format, llvm-cov, llvm-profdata, 프로파일 런타임)
RUN curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] http://apt.llvm.org/noble/ llvm-toolchain-noble-${LLVM_VERSION} main" \
        > /etc/apt/sources.list.d/llvm.list \
    && apt-get update && apt-get install -y --no-install-recommends \
        clang-${LLVM_VERSION} clang-tidy-${LLVM_VERSION} clang-format-${LLVM_VERSION} \
        llvm-${LLVM_VERSION} libclang-rt-${LLVM_VERSION}-dev \
    && rm -rf /var/lib/apt/lists/* \
    && for t in clang clang++ clang-tidy clang-format llvm-cov llvm-profdata; do \
         ln -sf "$(command -v ${t}-${LLVM_VERSION})" /usr/local/bin/${t}; done

# PICT (Microsoft) 소스 빌드
RUN git clone --depth 1 --branch ${PICT_VERSION} https://github.com/microsoft/pict.git /tmp/pict \
    && make -C /tmp/pict -j"$(nproc)" \
    && install -m 0755 /tmp/pict/pict /usr/local/bin/pict \
    && rm -rf /tmp/pict

# Python 도구: Lizard(순환복잡도/라인수), lcov→Cobertura 변환, 시스템 테스트 엑셀 생성(openpyxl)
RUN python3 -m venv /opt/venv \
    && /opt/venv/bin/pip install --no-cache-dir lizard lcov_cobertura openpyxl
ENV PATH="/opt/venv/bin:${PATH}"

# Jenkins(uid 1000)와 같은 uid 로 실행해 워크스페이스 파일 소유권 충돌을 피한다.
RUN userdel -r ubuntu 2>/dev/null || true \
    && useradd -m -u 1000 -s /bin/bash builder
USER builder
WORKDIR /work

# 도구 버전을 빌드 로그에 남긴다.
RUN clang --version | head -n1 && cmake --version | head -n1 && ninja --version \
    && cppcheck --version && lizard --version && cloc --version \
    && doxygen --version && pict 2>&1 | head -n1 || true
