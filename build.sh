#!/bin/sh


if [[ $# -eq 0 ]] ; then
    echo 'Supply the version'
    exit 1
fi

set -xe


docker buildx build -f debian.Dockerfile -t twitchdownloadergui-debian:1.0.0 --build-arg VERSION="$1" .
docker run --rm --net=host -e DISPLAY=$DISPLAY -v "$HOME/.Xauthority:/root/.Xauthority:rw" -v "$(pwd)/build/artifact:/artifact" twitchdownloadergui-debian:1.0.0

docker buildx build -f fedora.Dockerfile -t twitchdownloadergui-fedora:1.0.0 --build-arg VERSION="$1" .
docker run --rm --net=host -e DISPLAY=$DISPLAY -v "$HOME/.Xauthority:/root/.Xauthority:rw" -v "$(pwd)/build/artifact:/artifact" twitchdownloadergui-fedora:1.0.0

