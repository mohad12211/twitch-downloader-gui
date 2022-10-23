#include "Preferences.h"

static PreferencesOptions *prefOptions;

static int preferencesOnClose(uiWindow *w, void *data);
static void tempFileBrowseClicked(uiButton *b, void *data);
static void binaryPathClicked(uiButton *b, void *data);
static void useCustomBinaryToggled(uiCheckbox *c, void *data);

void ShowPreferences(void) {
	uiBox *mainVerticalBox = uiNewVerticalBox();
	uiBoxSetPadded(mainVerticalBox, 1);

	uiBox *tempFolderBox = uiNewHorizontalBox();
	uiBoxSetPadded(tempFolderBox, 1);
	uiForm *tempFolderForm = uiNewForm();
	uiFormSetPadded(tempFolderForm, 1);
	uiEntry *tempFolderEntry = uiNewEntry();
	uiEntrySetText(tempFolderEntry, getJson(configJson, "tempFolder")->valuestring);
	uiButton *browseTempFolderBtn = uiNewButton("Browse");
	uiFormAppend(tempFolderForm, "Temp Folder:", uiControl(tempFolderEntry), 0);
	uiBoxAppend(tempFolderBox, uiControl(tempFolderForm), 1);
	uiBoxAppend(tempFolderBox, uiControl(browseTempFolderBtn), 0);
	uiBoxAppend(mainVerticalBox, uiControl(tempFolderBox), 0);

	uiBox *clearCacheBox = uiNewHorizontalBox();
	uiBoxSetPadded(clearCacheBox, 1);
	uiForm *clearCacheForm = uiNewForm();
	uiFormSetPadded(clearCacheForm, 1);
	uiButton *clearCacheBtn = uiNewButton("Clear Cache");
	uiFormAppend(clearCacheForm, "Clear Cache Folder:", uiControl(clearCacheBtn), 0);
	uiBoxAppend(clearCacheBox, uiControl(clearCacheForm), 0);
	uiBoxAppend(mainVerticalBox, uiControl(clearCacheBox), 0);

	uiBox *useCustomBinaryBox = uiNewHorizontalBox();
	uiBoxSetPadded(useCustomBinaryBox, 1);
	uiForm *useCustomBinaryForm = uiNewForm();
	uiFormSetPadded(useCustomBinaryForm, 1);
	uiCheckbox *useCustomBinaryCheck = uiNewCheckbox("");
	uiCheckboxSetChecked(useCustomBinaryCheck, getJson(configJson, "useCustomBinary")->valueint);
	uiFormAppend(useCustomBinaryForm, "Use Custom TwitchDownloaderCLI Binary:", uiControl(useCustomBinaryCheck), 0);
	uiBoxAppend(useCustomBinaryBox, uiControl(useCustomBinaryForm), 0);
	uiBoxAppend(mainVerticalBox, uiControl(useCustomBinaryBox), 0);

	uiBox *customBinaryBox = uiNewHorizontalBox();
	if (getJson(configJson, "useCustomBinary")->valueint == 0) {
		uiControlDisable(uiControl(customBinaryBox));
	}
	uiBoxSetPadded(customBinaryBox, 1);
	uiForm *customBinaryForm = uiNewForm();
	uiFormSetPadded(customBinaryForm, 1);
	uiEntry *customBinaryEntry = uiNewEntry();
	uiEntrySetText(customBinaryEntry, getJson(configJson, "binaryPath")->valuestring);
	uiButton *browseBinaryBtn = uiNewButton("Browse");
	uiFormAppend(customBinaryForm, "Binary Path:", uiControl(customBinaryEntry), 0);
	uiBoxAppend(customBinaryBox, uiControl(customBinaryForm), 1);
	uiBoxAppend(customBinaryBox, uiControl(browseBinaryBtn), 0);
	uiBoxAppend(mainVerticalBox, uiControl(customBinaryBox), 0);

	prefOptions = malloc(sizeof(PreferencesOptions));
	*prefOptions = (PreferencesOptions){tempFolderEntry, customBinaryEntry, useCustomBinaryCheck};

	uiButtonOnClicked(browseTempFolderBtn, tempFileBrowseClicked, tempFolderEntry);
	uiButtonOnClicked(browseBinaryBtn, binaryPathClicked, customBinaryEntry);
	uiCheckboxOnToggled(useCustomBinaryCheck, useCustomBinaryToggled, customBinaryBox);

	uiWindow *preferencesWindow = uiNewWindow("Preferences", 500, 300, 0);
	uiWindowSetMargined(preferencesWindow, 1);
	uiWindowSetChild(preferencesWindow, uiControl(mainVerticalBox));
	uiWindowOnClosing(preferencesWindow, preferencesOnClose, NULL);
	uiControlShow(uiControl(preferencesWindow));
}

static int preferencesOnClose(uiWindow *w, void *data) {
	char *selectedCustomBinary = uiEntryText(prefOptions->customBinaryEntry);
	if (strlen(selectedCustomBinary)) {
		setJson(configJson, "binaryPath", cJSON_CreateString(selectedCustomBinary));
	}
	uiFreeText(selectedCustomBinary);

	char *selectedTempFolder = uiEntryText(prefOptions->tempFolderEntry);
	if (strlen(selectedTempFolder)) {
		setJson(configJson, "tempFolder", cJSON_CreateString(selectedTempFolder));
	} else {
		setJson(configJson, "tempFolder", cJSON_CreateString("/tmp"));
	}
	uiFreeText(selectedTempFolder);

	setJson(configJson, "useCustomBinary", cJSON_CreateNumber(uiCheckboxChecked(prefOptions->useCustomBinaryCheck)));

	free(prefOptions);
	return 1;
}

static void tempFileBrowseClicked(uiButton *b, void *data) {
	char *selectedTempFolder = uiOpenFolder(mainwin, NULL);
	if (selectedTempFolder == NULL) {
		return;
	}
	uiEntrySetText(uiEntry(data), selectedTempFolder);
	uiFreeText(selectedTempFolder);
}

static void binaryPathClicked(uiButton *b, void *data) {
	char *selectedBinaryPath = uiOpenFile(mainwin, NULL, "");
	if (selectedBinaryPath == NULL) {
		return;
	}
	uiEntrySetText(uiEntry(data), selectedBinaryPath);
	uiFreeText(selectedBinaryPath);
}

static void useCustomBinaryToggled(uiCheckbox *c, void *data) {
	if (uiCheckboxChecked(c)) {
		uiControlEnable(uiControl(uiBox(data)));
	} else {
		uiControlDisable(uiControl(uiBox(data)));
	}
}
