FROM ubuntu:20.04

RUN DEBIAN_FRONTEND=noninteractive \
	apt-get update && \
	apt-get -y install tzdata

RUN apt-get -y install \
	git make gdb python3-dev \
	libbsd-dev libc6-dev gcc-multilib linux-libc-dev \
	pkg-config libfuse2 libfuse-dev

WORKDIR /fisopfs
