# TwitchDownloader-gui
This is a gui wrapper for the cli version of the great project [TwitchDownloader](https://github.com/lay295/TwitchDownloader), Linux only.
I used the library [libui-ng](https://github.com/libui-ng/libui-ng) with a few extra [patches](https://github.com/mohad12211/libui-ng). 
The library is still in mid-alpha, but it's a promising project, simple, cross-platform (macos can be done, I just don't care much about it) and very intuitive.
## Preview

## Installation
Aur:
## Building

## Does it include all the features? any bugs?
it includes the basic functionalities, There's no task queue.
### Bugs...
- you can't disable options that are true by default in chatrender (like 3rd party emotes or badges) [#297](https://github.com/lay295/TwitchDownloader/issues/297) 
- Emojis don't render [#298](https://github.com/lay295/TwitchDownloader/issues/298) (there's a PR to fix it)
- You can only download the heighest quality for the clip [#289](https://github.com/lay295/TwitchDownloader/issues/289) (will be solved on the next release)
- You can only download the heighest quality for the vod (I will solve it soon).
