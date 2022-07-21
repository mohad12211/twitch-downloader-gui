# TwitchDownloader-gui

This is a gui wrapper for the cli version of the great project [TwitchDownloader](https://github.com/lay295/TwitchDownloader), Linux only.
I used the library [libui-ng](https://github.com/libui-ng/libui-ng) with a few extra [patches](https://github.com/mohad12211/libui-ng).
The library is still in mid-alpha, but it's a promising project, simple, cross-platform (macos can be done, I just don't care much about it) and very intuitive.

## Preview

![preview](https://user-images.githubusercontent.com/51754973/167058990-a574493a-fd44-4f12-be59-185798fb93bf.gif)

## Installation

Packages available for debian, fedora And Aur.

Pre-compiled packages will ship with `TwitchDownloadCLI` binary. You can specify another binary from the settings.
If you compile from source/install for another distro. You need to spicify a path for `TwitchDownloaderCLI` in the settings or place it in `/usr/share/twitch-downloader-gui/TwitchDownloaderCLI`

## Building

### Build Dependencies

- make
- libcurl

static library for libui is in the repo, but you can use your own compiled version and just replace it, instructions [here](https://github.com/mohad12211/libui-ng/blob/customs/README.md#quick-building-instructions)

```
$ git clone https://github.com/mohad12211/TwitchDownloader-gui.git
$ cd TwitchDownloader-gui
$ make
```

binary is inside `build/bin`

## Does it include all the features? any bugs?

it includes the basic functionalities, There's no task queue.

### Bugs...

- You can only download the heighest quality for the vod (I will solve it soon).

Please report any bugs.
