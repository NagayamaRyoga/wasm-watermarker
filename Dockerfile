FROM alpine:3.8

COPY . /kyut
WORKDIR /kyut

RUN apk add --no-cache \
    cmake \
    gcc \
    g++ \
    libc6-compat \
    python \
    make \
    musl-dev
