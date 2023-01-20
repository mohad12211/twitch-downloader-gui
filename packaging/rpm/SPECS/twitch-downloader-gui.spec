Name:           twitch-downloader-gui
Version:        2.0.0
Release:        1%{?dist}
Summary:        GUI Linux App to download clips/vods/chats and render chat for Twitch

License:        GPLv3
URL:            https://github.com/mohad12211/TwitchDownloader-gui
Source0:        %{url}/archive/v%{version}/%{name}-%{version}.tar.gz

Requires:       libcurl (ffmpeg or ffmpeg-free)

BuildArch:      x86_64

%description
Download Clips, Vods, Chat in multiple formats from Twitch.tv, and Render the Chat. A GUI for linux.

%prep
%setup -q

%build
make %{?_smp_mflags}

%install
%make_install

%files
%{_bindir}/%{name}
%{_datadir}/applications/twitch-downloader-gui.desktop
%{_datadir}/icons/hicolor/*/apps/twitch-downloader-gui.png
%license LICENSE



%changelog
* Mon May 16 03:58:10 +03 2022 mohad12211 <mohammedkaabi64@gmail.com>
- First Release
