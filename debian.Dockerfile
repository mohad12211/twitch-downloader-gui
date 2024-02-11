FROM ubuntu:20.04


RUN apt-get -y update && apt-get install -y
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install tzdata
RUN apt-get -y install libgtk-3-dev
RUN apt-get -y install gcc
RUN apt-get -y install build-essential devscripts debhelper
RUN apt-get -y install libcurl4-openssl-dev

WORKDIR /project/

ARG VERSION

ADD https://github.com/mohad12211/twitch-downloader-gui/archive/refs/tags/v${VERSION}.tar.gz /project/

RUN mv v${VERSION}.tar.gz twitch-downloader-gui_${VERSION}.orig.tar.gz

RUN tar xf twitch-downloader-gui_${VERSION}.orig.tar.gz

WORKDIR /project/twitch-downloader-gui-${VERSION}/

RUN cp -rf packaging/debian .

WORKDIR /project/twitch-downloader-gui-${VERSION}/debian/

RUN debuild -us -uc

WORKDIR /project

RUN mv twitch-downloader-gui_${VERSION}-1_amd64.deb twitch-downloader-gui.deb

CMD ["cp", "twitch-downloader-gui.deb", "/artifact"]
