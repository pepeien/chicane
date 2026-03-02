FROM ubuntu:24.04

# ──────────── Args ────────────
ARG VULKAN_SOURCE
ARG VULKAN_VERSION

# ──────────── Envs ────────────
ENV DEBIAN_FRONTEND=noninteractive

ENV VULKAN_SOURCE=${VULKAN_SOURCE}
ENV VULKAN_SDK=/opt/vulkan/${VULKAN_VERSION}/x86_64
ENV PATH="$VULKAN_SDK/bin:${PATH}"
ENV LD_LIBRARY_PATH="$VULKAN_SDK/lib:${LD_LIBRARY_PATH}"

# ──────────── Dependencies ────────────
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    wget \
    xz-utils \
    glslang-tools \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    gnome-desktop-testing \
    libasound2-dev \
    libpulse-dev \
    libaudio-dev \
    libfribidi-dev \
    libjack-dev \
    libsndio-dev \
    libxext-dev \
    libxfixes-dev \
    libxss-dev \
    libxtst-dev \
    libxkbcommon-dev \
    libdrm-dev \
    libgbm-dev \
    libgles2-mesa-dev \
    libegl1-mesa-dev \
    libdbus-1-dev \
    libibus-1.0-dev \
    libudev-dev \
    libthai-dev \
    libpipewire-0.3-dev \
    libwayland-dev \
    libdecor-0-dev \
    liburing-dev \
    libglu1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

# ──────────── Vulkan SDK ────────────
WORKDIR /opt/vulkan

RUN wget -q ${VULKAN_SOURCE} -O vulkan-sdk.tar.xz && \
    tar -xf vulkan-sdk.tar.xz && \
    rm vulkan-sdk.tar.xz

WORKDIR /src