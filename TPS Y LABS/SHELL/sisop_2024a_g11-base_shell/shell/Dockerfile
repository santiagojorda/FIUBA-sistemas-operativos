FROM ubuntu:22.04

RUN apt-get update && apt-get install -y make gcc build-essential python3 python3-pip time

RUN adduser --disabled-login --disabled-password fisop
USER fisop
ENV PATH="/home/fisop/.local/bin:${PATH}"

RUN pip install termcolor pyyaml

WORKDIR /shell

COPY . .

RUN make clean && make -B -e SHELL_TEST=true
