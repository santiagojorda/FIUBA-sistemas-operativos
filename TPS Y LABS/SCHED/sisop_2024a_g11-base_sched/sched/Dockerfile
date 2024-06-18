FROM ubuntu:16.04

RUN apt-get update && apt-get -y install \
	git make gdb python3-dev \
	libbsd-dev libc6-dev gcc-multilib linux-libc-dev \
	seabios qemu-system-x86

WORKDIR /sched
