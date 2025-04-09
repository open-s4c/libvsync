FROM ubuntu:22.04

# Install useful package
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
    	ca-certificates \
        cmake \
    	wget \
        file \
        gnupg \
        less \
        ncdu \
        python3-git \
        sudo \
        vim \
        lld \
        ninja-build \
	python3-pip \
    	g++-arm-linux-gnueabi \
        g++-aarch64-linux-gnu \
    	exuberant-ctags \
        gcc-aarch64-linux-gnu \
        libc-dev-arm64-cross \
        ssh-client \
    	gcc-arm-linux-gnueabi \
    	libc6-dev-armel-cross \
        linux-libc-dev-armel-cross \
    	libglib2.0-dev \
        libfdt-dev \
        libpixman-1-dev \
        zlib1g-dev \
        gcc-riscv64-linux-gnu \
        g++-riscv64-linux-gnu \
    && rm -rf /var/lib/apt/lists/*

RUN yes | pip install cmakelang

################################################################################
# compile qemu 4.2
################################################################################
RUN cd /tmp \
    && git clone --depth 1 -b stable-6.1 https://github.com/qemu/qemu.git && \
	cd qemu && mkdir build && cd build && \
	../configure --target-list="aarch64-linux-user arm-linux-user armeb-linux-user x86_64-linux-user riscv64-linux-user" && \
	make && make install \
    && rm -rf /tmp/qemu
################################################################################
# copy and extract armeb-linux-gnueabi
################################################################################
RUN wget --no-verbose --show-progress --progress=dot:mega \
      -O /tmp/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi.tar.xz \
      http://releases.linaro.org/components/toolchain/binaries/latest-7/armeb-linux-gnueabi/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi.tar.xz \
    && tar -xf /tmp/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi.tar.xz -C /tmp \
    && cp -r /tmp/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi/armeb-linux-gnueabi /usr/ \
    && rm -f /tmp/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi.tar.xz

# this used by vsync user - since PATH could not be updated.
ENV ARMEB_PATH=/tmp/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi/bin
# this will work for super user.
RUN echo "PATH=$PATH:/tmp/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi/bin" >> /root/.bashrc

