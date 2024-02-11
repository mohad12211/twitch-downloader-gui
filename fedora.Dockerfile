FROM fedora:38


RUN dnf -y group install "C Development Tools and Libraries"
RUN dnf -y install libcurl-devel gtk3-devel
RUN dnf -y install gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools
WORKDIR /project/

ARG VERSION

ADD https://github.com/mohad12211/twitch-downloader-gui/archive/refs/tags/v${VERSION}.tar.gz /project/

RUN mv v${VERSION}.tar.gz twitch-downloader-gui-${VERSION}.tar.gz

RUN tar xf twitch-downloader-gui-${VERSION}.tar.gz

WORKDIR /project/twitch-downloader-gui-${VERSION}/packaging/rpm/

RUN mkdir SOURCES/

RUN cp /project/twitch-downloader-gui-${VERSION}.tar.gz SOURCES/

RUN rpmbuild --define "_topdir `pwd`" -D 'debug_package %{nil}' -bb SPECS/twitch-downloader-gui.spec

RUN mv RPMS/x86_64/twitch-downloader-gui-${VERSION}-1.fc38.x86_64.rpm RPMS/x86_64/twitch-downloader-gui.rpm

CMD ["cp", "RPMS/x86_64/twitch-downloader-gui.rpm", "/artifact"]
