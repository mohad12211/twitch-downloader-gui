#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "app.h"

static int onClosing(uiWindow *w, void *data) {
  uiQuit();
  return 1;
}

static int onShouldQuit(void *data) {
  uiWindow *mMainwin = uiWindow(data);

  uiControlDestroy(uiControl(mMainwin));
  return 1;
}

static void setBinaryPath(uiMenuItem *i, uiWindow *win, void *data) {
  char *specifiedPath = uiOpenFile(mainwin, NULL, "");
  if (!specifiedPath)
    return;
  if (access(specifiedPath, X_OK) == -1) {
    uiMsgBoxError(mainwin, "Binary is not Executable!", "This program not executable, to make it so, run this at the location of the program\nchmod +x TwitchDownloaderCLI");
  }
  free(binaryPath);
  binaryPath = specifiedPath;
  ConfigData *configData = (ConfigData *)data;
  mkdir(configData->configDirectory, 0700);
  //  TODO: make this better, e.g. use xdg and error handling
  FILE *fp = fopen(configData->configFile, "w");
  fprintf(fp, "%s\n", binaryPath);
  fclose(fp);
}

static char *getBinaryPath(char *configFile) {
  char buf[500]; // TODO: make this safer
  char *path = NULL;
  FILE *fp = fopen(configFile, "r");
  if (fp == NULL || fgets(buf, 500, fp) == NULL) {
    path = strdup("TwitchDownloaderCLI");
  } else {
    buf[strlen(buf) - 1] = '\0';
    path = strdup(buf);
  }

  if (fp)
    fclose(fp);
  return path;
}

static void clearBinaryPath(uiMenuItem *i, uiWindow *win, void *configFile) {
  FILE *fp = fopen((char *)configFile, "w");
  if (fp)
    fclose(fp);
  free(binaryPath);
  binaryPath = strdup("TwitchDownloaderCLI");
}

int main(int argc, char const *argv[]) {
  uiInitOptions options;
  const char *err;
  uiTab *tab;
  uiMenu *menu;
  uiMenuItem *item;

  memset(&options, 0, sizeof(uiInitOptions));
  err = uiInit(&options);
  if (err != NULL) {
    fprintf(stderr, "error initializing libui: %s", err);
    uiFreeInitError(err);
    return 1;
  }

  char *HOME = getenv("HOME");
  char configDirectory[strlen(HOME) + strlen("/.config/TwitchDownloader-gui") + 1];
  sprintf(configDirectory, "%s%s", HOME, "/.config/TwitchDownloader-gui");
  char configFile[strlen(configDirectory) + strlen("/TwitchDownloader-gui.conf") + 1];
  sprintf(configFile, "%s%s", configDirectory, "/TwitchDownloader-gui.conf");
  binaryPath = getBinaryPath(configFile);
  ConfigData configData = {configDirectory, configFile};

  menu = uiNewMenu("Options");

  item = uiMenuAppendItem(menu, "Set TwitchDownloaderCLI binary location");
  uiMenuItemOnClicked(item, setBinaryPath, &configData);

  item = uiMenuAppendItem(menu, "Use TwitchDownloaderCLI binary from the PATH env");
  uiMenuItemOnClicked(item, clearBinaryPath, configFile);

  item = uiMenuAppendQuitItem(menu);

  mainwin = uiNewWindow("TwitchDownloader-gui", 1250, 700, 1);
  uiWindowSetMargined(mainwin, 1);
  uiWindowOnClosing(mainwin, onClosing, NULL);
  uiOnShouldQuit(onShouldQuit, mainwin);

  uiBox *vbox = uiNewVerticalBox();
  uiBoxSetPadded(vbox, 1);
  uiWindowSetChild(mainwin, uiControl(vbox));

  uiLabel *update = uiNewLabel("New <a href=\"https://github.com/mohad12211/TwitchDownloader-gui/releases\">Update</a> is available.");
  uiBoxAppend(vbox, uiControl(update), 0);
  uiControlHide(uiControl(update));

  tab = uiNewTab();
  uiBoxAppend(vbox, uiControl(tab), 1);

  uiTabAppend(tab, "Vod Downloader", VodDownloaderDrawUi());
  uiTabSetMargined(tab, 0, 1);

  uiTabAppend(tab, "Clip Downloader", ClipDownloaderDrawUi());
  uiTabSetMargined(tab, 1, 1);

  uiTabAppend(tab, "Chat Downloader", ChatDownloaderDrawUi());
  uiTabSetMargined(tab, 2, 1);

  uiTabAppend(tab, "Chat Render", ChatRenderDrawUi());
  uiTabSetMargined(tab, 3, 1);

  uiControlShow(uiControl(mainwin));
  uiMain();

  return 0;
}
