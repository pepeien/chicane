FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake ninja-build \
    git wget p7zip-full \
    mingw-w64 \
    glslang-tools \
    vulkan-tools \
    && rm -rf /var/lib/apt/lists/*

# ── Vulkan SDK ────────────
WORKDIR /opt/vulkan_setup
RUN wget -q https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe -O vulkan-sdk.exe && \
    7z x vulkan-sdk.exe -oextracted && \
    mkdir -p /opt/vulkan && \
    mv extracted/Include /opt/vulkan/Include && \
    mv extracted/Lib     /opt/vulkan/Lib && \
    rm -rf /opt/vulkan_setup

# ── Toolchain file ────────────────────────────────────────────────────────────
RUN cat > /opt/mingw-toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Vulkan — point directly at the SDK
set(Vulkan_INCLUDE_DIR /opt/vulkan/Include)
set(Vulkan_LIBRARY     /opt/vulkan/Lib/vulkan-1.lib)

EOF

WORKDIR /src