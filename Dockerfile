FROM python:3.10-alpine

RUN apk update && apk add --no-cache make gcc build-base valgrind

RUN adduser -D fisop
USER fisop
ENV PATH="/home/fisop/.local/bin:${PATH}"

WORKDIR /fork

COPY . .
