#!/bin/sh
set -xe

docker build --tag projeto/linux --file ./Dockerfile .
