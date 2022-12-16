#include "app.h"

uiWindow *mainwin;
char *configFile;
cJSON *configJson;

static void saveConfig(void) {
	FILE *fp = fopen(configFile, "w");
	char *str = cJSON_Print(configJson);
	fprintf(fp, "%s\n", str);

	fclose(fp);
	cJSON_Delete(configJson);

	free(str);
}

static void loadConfig(void) {
	char *buffer = NULL;
	size_t len;
	FILE *fp = fopen(configFile, "r");
	if (fp == NULL) {
		configJson = cJSON_CreateObject();
	} else {
		getdelim(&buffer, &len, '\0', fp);
		// TODO: handle erros
		configJson = cJSON_Parse(buffer);
		fclose(fp);
		free(buffer);
	}
}

static int onClosing(uiWindow *w, void *data) {
	ChatDownloaderResetUi();
	ClipDownloaderResetUi();
	VodDownloaderResetUi();
	ChatRenderResetUi();
	uiQuit();
	saveConfig();
	return 1;
}

static int onShouldQuit(void *data) {
	ChatDownloaderResetUi();
	ClipDownloaderResetUi();
	VodDownloaderResetUi();
	ChatRenderResetUi();
	uiControlDestroy(uiControl(data));
	saveConfig();
	return 1;
}

static void setupConfigFile(void) {
	//  TODO: make this better, e.g. use xdg and error handling
	char *HOME = getenv("HOME");
	char directory[strlen(HOME) + strlen("/.config/twitch-downloader-gui") + 1];
	sprintf(directory, "%s%s", HOME, "/.config/twitch-downloader-gui");
	mkdir(directory, 0700);
	char file[strlen(directory) + strlen("/twitch-downloader-gui.conf") + 1];
	sprintf(file, "%s%s", directory, "/twitch-downloader-gui.json");
	configFile = strdup(file);
}

static void initDefaults(void) {
	if (getJson(configJson, "tempFolder") == NULL) {
		setJson(configJson, "tempFolder", cJSON_CreateString("/tmp"));
	}

	if (getJson(configJson, "useCustomBinary") == NULL) {
		setJson(configJson, "useCustomBinary", cJSON_CreateNumber(0));
	}

	if (getJson(configJson, "binaryPath") == NULL) {
		setJson(configJson, "binaryPath", cJSON_CreateString(""));
	}
}

static void preferencesClicked(uiMenuItem *i, uiWindow *win, void *data) { ShowPreferences(); }

int main(int argc, char const *argv[]) {
	uiInitOptions options;
	const char *err;

	memset(&options, 0, sizeof(uiInitOptions));
	err = uiInit(&options);
	if (err != NULL) {
		fprintf(stderr, "error initializing libui: %s", err);
		uiFreeInitError(err);
		return 1;
	}

	setupConfigFile();
	loadConfig();
	initDefaults();

	uiMenu *menu;
	uiMenuItem *item;

	menu = uiNewMenu("Edit");
	item = uiMenuAppendPreferencesItem(menu);
	uiMenuItemOnClicked(item, preferencesClicked, NULL);

	item = uiMenuAppendQuitItem(menu);

	mainwin = uiNewWindow("Twitch Downloader", 1250, 700, 1);
	uiWindowSetMargined(mainwin, 1);
	uiWindowOnClosing(mainwin, onClosing, NULL);
	uiOnShouldQuit(onShouldQuit, mainwin);

	uiBox *mainVerticalBox = uiNewVerticalBox();
	uiBoxSetPadded(mainVerticalBox, 1);
	uiWindowSetChild(mainwin, uiControl(mainVerticalBox));

	// TODO: probably remove this
	uiLabel *update = uiNewLabel("New <a href=\"https://github.com/mohad12211/TwitchDownloader-gui/releases\">Update</a> is available.");
	uiBoxAppend(mainVerticalBox, uiControl(update), 0);
	uiControlHide(uiControl(update));

	uiTab *tabs = uiNewTab();
	uiTabAppend(tabs, "Vod Downloader", VodDownloaderDrawUi());
	uiTabSetMargined(tabs, 0, 1);
	uiTabAppend(tabs, "Clip Downloader", ClipDownloaderDrawUi());
	uiTabSetMargined(tabs, 1, 1);
	uiTabAppend(tabs, "Chat Downloader", ChatDownloaderDrawUi());
	uiTabSetMargined(tabs, 2, 1);
	uiTabAppend(tabs, "Chat Render", ChatRenderDrawUi());
	uiTabSetMargined(tabs, 3, 1);
	uiBoxAppend(mainVerticalBox, uiControl(tabs), 1);

	uiControlShow(uiControl(mainwin));
	uiMain();
	uiUninit();

	return 0;
}
