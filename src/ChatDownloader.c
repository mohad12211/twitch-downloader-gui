#include "ChatDownloader.h"

const char *const timeFormatsArr[] = {"Utc", "Relative", "None"};
static ChatDwnOptions *chatOptions;

static void downloadFormatSelect(uiRadioButtons *r, void *data);
static void infoBtnCallBack(uiButton *b, void *data);
static void downloadBtnClicked(uiButton *b, void *data);
static void *downloadTask(void *args);
static int setClipInfo(char *id);
static int setVodInfo(char *i);
static int getId(char *link, char **id);
static void cropToggle(uiCheckbox *c, void *data);
static void runOnUiThread(void *args);
static void handlerMouseEvent(uiAreaHandler *ah, uiArea *area, uiAreaMouseEvent *e);
static void handlerMouseCrossed(uiAreaHandler *ah, uiArea *area, int left);
static void handlerDragBroken(uiAreaHandler *ah, uiArea *area);
static int handlerKeyEvent(uiAreaHandler *ah, uiArea *area, uiAreaKeyEvent *e);
static void handlerDraw(uiAreaHandler *ah, uiArea *area, uiAreaDrawParams *p);
static void setThumbnail(char *link);

uiControl *ChatDownloaderDrawUi(void) {
	uiBox *mainVerticalBox = uiNewVerticalBox();
	uiBoxSetPadded(mainVerticalBox, 1);

	uiBox *linkBox = uiNewHorizontalBox();
	uiBoxSetPadded(linkBox, 1);
	uiForm *linkForm = uiNewForm();
	uiFormSetPadded(linkForm, 1);
	uiEntry *linkEntry = uiNewEntry();
	uiButton *infoBtn = uiNewButton("Get info");
	uiFormAppend(linkForm, "Clip/Vod Link: ", uiControl(linkEntry), 0);
	uiBoxAppend(linkBox, uiControl(linkForm), 1);
	uiBoxAppend(linkBox, uiControl(infoBtn), 0);
	uiBoxAppend(mainVerticalBox, uiControl(linkBox), 0);

	uiBox *middleHorizontalBox = uiNewHorizontalBox();
	uiBoxSetPadded(middleHorizontalBox, 1);
	uiBoxAppend(mainVerticalBox, uiControl(middleHorizontalBox), 1);

	uiBox *leftVerticalBox = uiNewVerticalBox();
	uiBoxAppend(middleHorizontalBox, uiControl(leftVerticalBox), 1);

	uiBox *infoBox = uiNewVerticalBox();
	uiGrid *infoGrid = uiNewGrid();
	uiBoxAppend(infoBox, uiControl(infoGrid), 1);
	uiBoxAppend(leftVerticalBox, uiControl(infoBox), 1);

	uiForm *nameForm = uiNewForm();
	uiFormSetPadded(nameForm, 1);
	uiLabel *nameLabel = uiNewLabel("");
	uiFormAppend(nameForm, "Streamer: ", uiControl(nameLabel), 0);

	uiForm *titleForm = uiNewForm();
	uiFormSetPadded(titleForm, 1);
	uiLabel *titleLabel = uiNewLabel("");
	uiFormAppend(titleForm, "Title: ", uiControl(titleLabel), 0);

	uiForm *durationForm = uiNewForm();
	uiFormSetPadded(durationForm, 1);
	uiLabel *durationLabel = uiNewLabel("");
	uiFormAppend(durationForm, "Duration: ", uiControl(durationLabel), 0);

	uiForm *createdForm = uiNewForm();
	uiFormSetPadded(createdForm, 1);
	uiLabel *createdLabel = uiNewLabel("");
	uiFormAppend(createdForm, "Created at: ", uiControl(createdLabel), 0);

	uiGridAppend(infoGrid, uiControl(nameForm), 0, 0, 1, 1, 1, uiAlignStart, 1, uiAlignCenter);
	uiGridAppend(infoGrid, uiControl(titleForm), 0, 1, 1, 1, 1, uiAlignStart, 1, uiAlignCenter);
	uiGridAppend(infoGrid, uiControl(durationForm), 0, 2, 1, 1, 1, uiAlignStart, 1, uiAlignCenter);
	uiGridAppend(infoGrid, uiControl(createdForm), 0, 3, 1, 1, 1, uiAlignStart, 1, uiAlignCenter);

	struct handler *handler = malloc(sizeof(struct handler));
	*handler = (struct handler){.binaryData = NULL, .img = NULL};
	handler->ah.DragBroken = handlerDragBroken;
	handler->ah.Draw = handlerDraw;
	handler->ah.KeyEvent = handlerKeyEvent;
	handler->ah.MouseCrossed = handlerMouseCrossed;
	handler->ah.MouseEvent = handlerMouseEvent;
	uiBox *thumbnailBox = uiNewVerticalBox();
	uiArea *imageArea = uiNewArea((uiAreaHandler *)handler);
	uiBoxAppend(thumbnailBox, uiControl(imageArea), 1);
	uiBoxAppend(leftVerticalBox, uiControl(thumbnailBox), 1);

	uiBoxAppend(middleHorizontalBox, uiControl(uiNewVerticalSeparator()), 0);

	uiBox *optionsBox = uiNewVerticalBox();
	uiBoxSetPadded(optionsBox, 1);
	uiBoxAppend(middleHorizontalBox, uiControl(optionsBox), 0);

	uiForm *formatForm = uiNewForm();
	uiBoxAppend(optionsBox, uiControl(formatForm), 0);
	uiRadioButtons *downloadFormats = uiNewRadioButtons(GTK_ORIENTATION_HORIZONTAL);
	uiRadioButtonsAppend(downloadFormats, "Advanced JSON");
	uiRadioButtonsAppend(downloadFormats, "Simple Text");
	uiRadioButtonsSetSelected(downloadFormats, 0);
	uiFormAppend(formatForm, "Download Formats: ", uiControl(downloadFormats), 0);
	uiRadioButtons *timeFormats = uiNewRadioButtons(GTK_ORIENTATION_HORIZONTAL);
	uiControlHide(uiControl(timeFormats));
	uiRadioButtonsAppend(timeFormats, "UTC");
	uiRadioButtonsAppend(timeFormats, "Relative");
	uiRadioButtonsAppend(timeFormats, "None");
	uiRadioButtonsSetSelected(timeFormats, 0);
	uiFormAppend(formatForm, "Timestamp Format: ", uiControl(timeFormats), 0);
	uiRadioButtonsOnSelected(downloadFormats, downloadFormatSelect, timeFormats);

	// TODO: rename these boxes
	uiBox *startSpinsBox = uiNewHorizontalBox(); // box that continas the 3 spinBoxes
	uiBoxSetPadded(startSpinsBox, 0);
	uiControlDisable(uiControl(startSpinsBox));
	uiBox *cropStartBox = uiNewHorizontalBox(); // box that continas the checkbox and startSpinBox
	uiControlDisable(uiControl(cropStartBox));
	uiCheckbox *cropStartCheck = uiNewCheckbox("Crop Start :");
	uiCheckboxOnToggled(cropStartCheck, cropToggle, startSpinsBox);
	uiSpinbox *startHour = uiNewSpinbox(0, 100);
	uiSpinbox *startMin = uiNewSpinbox(0, 59);
	uiSpinbox *startSec = uiNewSpinbox(0, 59);
	uiBoxAppend(startSpinsBox, uiControl(startHour), 0);
	uiBoxAppend(startSpinsBox, uiControl(startMin), 0);
	uiBoxAppend(startSpinsBox, uiControl(startSec), 0);
	uiBoxAppend(cropStartBox, uiControl(cropStartCheck), 0);
	uiBoxAppend(cropStartBox, uiControl(startSpinsBox), 0);
	uiBoxAppend(optionsBox, uiControl(cropStartBox), 0);

	uiBox *endSpinsBox = uiNewHorizontalBox();
	uiBoxSetPadded(endSpinsBox, 0);
	uiControlDisable(uiControl(endSpinsBox));
	uiBox *cropEndBox = uiNewHorizontalBox();
	uiControlDisable(uiControl(cropEndBox));
	uiCheckbox *cropEndCheck = uiNewCheckbox("Crop End :  ");
	uiCheckboxOnToggled(cropEndCheck, cropToggle, endSpinsBox);
	uiSpinbox *endHour = uiNewSpinbox(0, 100);
	uiSpinbox *endMin = uiNewSpinbox(0, 59);
	uiSpinbox *endSec = uiNewSpinbox(0, 59);
	uiBoxAppend(endSpinsBox, uiControl(endHour), 0);
	uiBoxAppend(endSpinsBox, uiControl(endMin), 0);
	uiBoxAppend(endSpinsBox, uiControl(endSec), 0);
	uiBoxAppend(cropEndBox, uiControl(cropEndCheck), 0);
	uiBoxAppend(cropEndBox, uiControl(endSpinsBox), 0);
	uiBoxAppend(optionsBox, uiControl(cropEndBox), 0);

	uiGrid *embedBox = uiNewGrid();
	uiBoxAppend(optionsBox, uiControl(embedBox), 0);
	uiCheckbox *embedEmotes = uiNewCheckbox("Embed Emotes");
	uiGridAppend(embedBox, uiControl(embedEmotes), 0, 0, 1, 1, 0, uiAlignCenter, 0, uiAlignCenter);
	uiGridAppend(embedBox,
							 uiControl(uiNewLabel("<a href=\"\"title=\"Embeds emotes into the JSON file so in "
																		"the future when an emote is removed from Twitch or a 3rd "
																		"party, it will still render correctly. Useful for archival "
																		"purposes, file size will be larger\">(?)</a>")),
							 1, 0, 1, 1, 0, uiAlignCenter, 0, uiAlignCenter);

	uiBoxAppend(middleHorizontalBox, uiControl(uiNewVerticalSeparator()), 0);

	uiForm *logForm = uiNewForm();
	uiFormSetPadded(logForm, 1);
	uiMultilineEntry *logsEntry = uiNewNonWrappingMultilineEntry();
	uiMultilineEntrySetReadOnly(logsEntry, 1);
	uiFormAppend(logForm, "Logs: ", uiControl(logsEntry), 1);
	uiBoxAppend(middleHorizontalBox, uiControl(logForm), 1);

	uiButton *downloadBtn = uiNewButton("Download");
	uiControlDisable(uiControl(downloadBtn));
	uiGrid *downloadGrid = uiNewGrid();
	uiGridSetPadded(downloadGrid, 1);
	uiGridAppend(downloadGrid, uiControl(downloadBtn), 0, 0, 1, 1, 1, uiAlignCenter, 1, uiAlignFill);
	uiBoxAppend(mainVerticalBox, uiControl(downloadGrid), 0);

	uiBox *pBarBox = uiNewHorizontalBox();
	uiBoxSetPadded(pBarBox, 1);
	uiProgressBar *pBar = uiNewProgressBar();
	uiLabel *status = uiNewLabel("Idle...");
	uiBoxAppend(pBarBox, uiControl(status), 0);
	uiBoxAppend(pBarBox, uiControl(pBar), 1);
	uiBoxAppend(mainVerticalBox, uiControl(pBarBox), 0);

	chatOptions = malloc(sizeof(ChatDwnOptions));
	*chatOptions = (ChatDwnOptions){
			linkEntry, nameLabel, titleLabel,			 durationLabel, createdLabel,		logsEntry,		pBar,					 status,			 downloadBtn, infoBtn,
			NULL,			 NULL,			downloadFormats, timeFormats,		cropStartCheck, cropEndCheck, startSpinsBox, cropStartBox, endSpinsBox, cropEndBox,
			startHour, startMin,	startSec,				 endHour,				endMin,					endSec,				embedEmotes,	 imageArea,		 handler,			0,
	};

	uiButtonOnClicked(infoBtn, infoBtnCallBack, NULL);
	uiButtonOnClicked(downloadBtn, downloadBtnClicked, NULL);

	return uiControl(mainVerticalBox);
}

static void infoBtnCallBack(uiButton *b, void *data) {
	ChatDownloaderResetUi();
	char *link = uiEntryText(chatOptions->linkEntry);
	char *id = NULL;
	int idType = getId(link, &id);
	int validClipID;
	int validVodID;
	switch (idType) {
	case 0:
		uiMsgBoxError(mainwin, "Error", "Invalid Url");
		break;
	case 1:
		validClipID = setClipInfo(id);
		if (validClipID) {
			chatOptions->id = id;
			uiControlEnable(uiControl(chatOptions->downloadBtn));
		} else {
			uiMsgBoxError(mainwin, "Error", "Invalid Clip ID, or deleted/expired Vod");
			free(id);
		}
		break;
	case 2:
		validVodID = setVodInfo(id);
		if (validVodID) {
			uiControlEnable(uiControl(chatOptions->cropStartBox));
			uiControlEnable(uiControl(chatOptions->cropEndBox));
			uiControlEnable(uiControl(chatOptions->downloadBtn));
			chatOptions->id = id;
		} else {
			uiMsgBoxError(mainwin, "Error", "Invalid Vod ID");
			free(id);
		}
		break;
	default:
		break;
	}
	uiFreeText(link);
}

static void downloadBtnClicked(uiButton *b, void *data) {
	int format = uiRadioButtonsSelected(chatOptions->downloadFormats);
	// TODO: change this to what I *will* be using for ChatRender containersBox
	char *defaultName = format == 0 ? "chat.json" : "chat.txt";
	char *filter = format == 0 ? "JSON File (*.json)|*.json" : "Text File (*.txt)|*.txt";
	char *fileName = uiSaveFile(mainwin, NULL, defaultName, filter);
	if (fileName == NULL) {
		return;
	}

	string *cmd = malloc(sizeof(string));
	*cmd = (string){malloc(sizeof(char) * 100), 0, 100};
	concat(cmd, 4, getBinaryPath(), " chatdownload -u '", chatOptions->id, "'");

	if (uiCheckboxChecked(chatOptions->cropStartCheck)) {
		char seconds[12];
		sprintf(seconds, "%d", uiSpinboxValue(chatOptions->startHour) * 3600 + uiSpinboxValue(chatOptions->startMin) * 60 + uiSpinboxValue(chatOptions->startSec));
		concat(cmd, 2, " -b ", seconds);
	}

	if (uiCheckboxChecked(chatOptions->cropEndCheck)) {
		char seconds[12];
		sprintf(seconds, "%d", uiSpinboxValue(chatOptions->endHour) * 3600 + uiSpinboxValue(chatOptions->endMin) * 60 + uiSpinboxValue(chatOptions->endSec));
		concat(cmd, 2, " -e ", seconds);
	}

	if (uiRadioButtonsSelected(chatOptions->downloadFormats) == 1) {
		concat(cmd, 2, " --timestamp-format ", timeFormatsArr[uiRadioButtonsSelected(chatOptions->timeFormats)]);
	}

	if (uiCheckboxChecked(chatOptions->embedEmotes)) {
		concat(cmd, 1, " -E ");
	}

	concat(cmd, 3, " --temp-path \"", getJson(configJson, "tempFolder")->valuestring, "\" ");

	concat(cmd, 3, " -o \"", fileName, "\" ");
	chatOptions->cmd = cmd;

	uiFreeText(fileName);

	pthread_t thread;
	pthread_create(&thread, NULL, downloadTask, data);
	pthread_detach(thread);
}

static void *downloadTask(void *args) {
	char buf[200];
	FILE *fp;
	pid_t pid;

	if ((fp = mypopen((char *)chatOptions->cmd->memory, &pid)) == NULL) {
		printf("Error opening pipe!\n");
		return NULL;
	}

	chatOptions->downloadpid = pid;

	uiData *data = malloc(sizeof(uiData));
	*data = (uiData){.flag = DOWNLOADING};
	uiQueueMain(runOnUiThread, data);

	while (mygets(buf, 200, fp) != NULL) {
		uiData *logData = malloc(sizeof(uiData));
		if (strstr(buf, "%")) {
			int offset = strlen("[STATUS] - Downloading ");
			int percentage = atoi(buf + offset);
			*logData = (uiData){.flag = PROGRESS, .i = percentage};
		} else if (strstr(buf, "+")) {
			*logData = (uiData){.flag = PROGRESS, .i = -1};
		} else {
			*logData = (uiData){.flag = LOGGING, .buf = strdup(buf)};
		}
		uiQueueMain(runOnUiThread, logData);
	}

	data = malloc(sizeof(uiData));
	*data = (uiData){.flag = FINISH, .i = mypclose(fp, pid)};
	uiQueueMain(runOnUiThread, data);

	free(chatOptions->cmd->memory);
	free(chatOptions->cmd);
	chatOptions->cmd = NULL;
	chatOptions->downloadpid = 0;

	return NULL;
}

static int setVodInfo(char *id) {
	int validID = 1;
	string *infoRes = getVodInfo(id);
	cJSON *root = cJSON_Parse((char *)infoRes->memory);
	if (cJSON_IsNull(cJSONUtils_GetPointer(root, "/data/video"))) {
		validID = 0;
		goto err;
	}
	char duration[11];
	uiLabelSetText(chatOptions->nameLabel, cJSONUtils_GetPointer(root, "/data/video/owner/displayName")->valuestring);
	uiLabelSetText(chatOptions->titleLabel, cJSONUtils_GetPointer(root, "/data/video/title")->valuestring);
	int seconds = cJSONUtils_GetPointer(root, "/data/video/lengthSeconds")->valueint;
	sprintf(duration, "%02u:%02u:%02u", seconds / 3600, (seconds % 3600) / 60, seconds % 60);
	uiLabelSetText(chatOptions->durationLabel, duration);
	char *createdLocalTime = getLocalTime(cJSONUtils_GetPointer(root, "/data/video/createdAt")->valuestring);
	uiLabelSetText(chatOptions->createdLabel, createdLocalTime);
	free(createdLocalTime);
	cJSON *thumbnail = cJSONUtils_GetPointer(root, "/data/video/thumbnailURLs/0");
	if (thumbnail)
		setThumbnail(thumbnail->valuestring);

err:
	free(infoRes->memory);
	free(infoRes);
	cJSON_Delete(root);
	return validID;
}

static int setClipInfo(char *id) {
	int validID = 1;
	string *infoRes = getClipInfo(id);
	cJSON *root = cJSON_Parse((char *)infoRes->memory);
	if (cJSON_IsNull(cJSONUtils_GetPointer(root, "/data/clip"))) {
		validID = 0;
		goto err;
	} else if (cJSON_IsNull(cJSONUtils_GetPointer(root, "/data/clip/video")) || cJSON_IsNull(cJSONUtils_GetPointer(root, "/data/clip/videoOffsetSeconds"))) {
		validID = 0;
		goto err;
	}
	char duration[11];
	uiLabelSetText(chatOptions->nameLabel, cJSONUtils_GetPointer(root, "/data/clip/broadcaster/displayName")->valuestring);
	uiLabelSetText(chatOptions->titleLabel, cJSONUtils_GetPointer(root, "/data/clip/title")->valuestring);
	sprintf(duration, "%d %s", cJSONUtils_GetPointer(root, "/data/clip/durationSeconds")->valueint, "Seconds");
	uiLabelSetText(chatOptions->durationLabel, duration);
	char *createdLocalTime = getLocalTime(cJSONUtils_GetPointer(root, "/data/clip/createdAt")->valuestring);
	uiLabelSetText(chatOptions->createdLabel, createdLocalTime);
	free(createdLocalTime);
	cJSON *thumbnail = cJSONUtils_GetPointer(root, "/data/clip/thumbnailURL");
	if (thumbnail)
		setThumbnail(thumbnail->valuestring);

err:
	free(infoRes->memory);
	free(infoRes);
	cJSON_Delete(root);
	return validID;
}

// https://www.cairographics.org/manual/cairo-Image-Surfaces.html#cairo-format-t
// stb gets the image in RGBA, but cairo needs it in ARGB, AND in native-endian,
// so little-endian, so it will be BGRA so I have to swap the B and the R, the
// image is 480x272 so should be fast.
static void setThumbnail(char *link) {
	int x, y, n;
	string *response = requestImage(link);
	chatOptions->handler->binaryData = stbi_load_from_memory(response->memory, response->used, &x, &y, &n, 4);
	chatOptions->handler->width = x;
	chatOptions->handler->height = y;
	for (int i = 0; i < x * y; i++) {
		unsigned char red = chatOptions->handler->binaryData[i * 4];
		chatOptions->handler->binaryData[i * 4] = chatOptions->handler->binaryData[i * 4 + 2];
		chatOptions->handler->binaryData[i * 4 + 2] = red;
	}
	chatOptions->handler->img = uiNewImageBuffer(NULL, x, y, 1);
	uiImageBufferUpdate(chatOptions->handler->img, (void *)chatOptions->handler->binaryData);
	uiAreaQueueRedrawAll(chatOptions->imageArea);
	free(response->memory);
	free(response);
}

// TODO: support receiving ID as input
static int getId(char *link, char **id) {
	int resCode = 0;
	regex_t clipRegex;
	regcomp(&clipRegex, "twitch[.]tv/[^[:space:]]+/clip/", REG_EXTENDED);
	if (!regexec(&clipRegex, link, 0, NULL, 0) || strstr(link, "clips.twitch.tv/")) {
		resCode = 1;
	} else if (strstr(link, "twitch.tv/videos/")) {
		resCode = 2;
	} else {
		regfree(&clipRegex);
		return 0;
	}

	CURLU *h;
	CURLUcode uc;
	char *path;

	h = curl_url();
	uc = curl_url_set(h, CURLUPART_URL, link, 0);
	uc = curl_url_get(h, CURLUPART_PATH, &path, 0);
	if (!uc) {
		*id = strdup(strrchr(path, '/') + 1);
		curl_free(path);
	}
	curl_url_cleanup(h);
	regfree(&clipRegex);
	return resCode;
}

static void downloadFormatSelect(uiRadioButtons *r, void *data) {
	if (uiRadioButtonsSelected(r) == 0)
		uiControlHide(uiControl((uiRadioButtons *)data));
	else
		uiControlShow(uiControl((uiRadioButtons *)data));
}

static void cropToggle(uiCheckbox *c, void *data) {
	uiBox *box = (uiBox *)data;
	if (uiCheckboxChecked(c)) {
		uiControlEnable(uiControl(box));
	} else {
		uiControlDisable(uiControl(box));
	}
}

static void runOnUiThread(void *args) {
	uiData *data = (uiData *)args;
	switch (data->flag) {
	case DOWNLOADING:
		uiControlDisable(uiControl(chatOptions->downloadBtn));
		uiControlDisable(uiControl(chatOptions->infoBtn));
		uiLabelSetText(chatOptions->status, "Downloading...");
		uiProgressBarSetValue(chatOptions->pBar, 0);
		break;
	case PROGRESS:
		uiProgressBarSetValue(chatOptions->pBar, MIN(data->i, 100));
		if (data->i == -1)
			uiLabelSetText(chatOptions->status, "Embedding Emotes...");
		break;
	case LOGGING:
		uiMultilineEntryAppend(chatOptions->logsEntry, data->buf);
		if (strstr(data->buf, "command not found") && strstr(data->buf, "TwitchDownloaderCLI")) {
			uiMultilineEntryAppend(chatOptions->logsEntry, "Please specify the TwitchDownloaderCLI path from the options");
		}
		free(data->buf);
		break;
	case FINISH:
		if (WIFEXITED(data->i) && WEXITSTATUS(data->i) == 143)
			break;
		if (data->i) {
			uiLabelSetText(chatOptions->status, "Error...");
			uiProgressBarSetValue(chatOptions->pBar, 0);
		} else {
			uiLabelSetText(chatOptions->status, "Done!");
			uiProgressBarSetValue(chatOptions->pBar, 100);
		}
		uiControlEnable(uiControl(chatOptions->downloadBtn));
		uiControlEnable(uiControl(chatOptions->infoBtn));
		break;

	default:
		fprintf(stderr, "unknown flag %d", data->flag);
		break;
	}
	free(data);
}

void ChatDownloaderResetUi(void) {
	if (chatOptions->downloadpid)
		killpg(chatOptions->downloadpid, SIGTERM);
	free(chatOptions->id);
	chatOptions->id = NULL;
	stbi_image_free(chatOptions->handler->binaryData);
	chatOptions->handler->binaryData = NULL;
	if (chatOptions->handler->img) {
		uiFreeImageBuffer(chatOptions->handler->img);
		chatOptions->handler->img = NULL;
	}
	uiLabelSetText(chatOptions->nameLabel, "");
	uiLabelSetText(chatOptions->titleLabel, "");
	uiLabelSetText(chatOptions->durationLabel, "");
	uiLabelSetText(chatOptions->createdLabel, "");
	uiLabelSetText(chatOptions->status, "Idle...");
	uiProgressBarSetValue(chatOptions->pBar, 0);
	uiCheckboxSetChecked(chatOptions->cropStartCheck, 0);
	uiCheckboxSetChecked(chatOptions->cropEndCheck, 0);
	uiControlDisable(uiControl(chatOptions->startSpinsBox));
	uiControlDisable(uiControl(chatOptions->endSpinsBox));
	uiControlDisable(uiControl(chatOptions->cropStartBox));
	uiControlDisable(uiControl(chatOptions->cropEndBox));
	uiControlDisable(uiControl(chatOptions->downloadBtn));
}

static void handlerMouseEvent(uiAreaHandler *ah, uiArea *area, uiAreaMouseEvent *e) {}
static void handlerMouseCrossed(uiAreaHandler *ah, uiArea *area, int left) {}
static void handlerDragBroken(uiAreaHandler *ah, uiArea *area) {}
static int handlerKeyEvent(uiAreaHandler *ah, uiArea *area, uiAreaKeyEvent *e) { return 0; }
static void handlerDraw(uiAreaHandler *ah, uiArea *area, uiAreaDrawParams *p) {
	struct handler *handler = (struct handler *)ah;
	if (!(handler->img))
		return;
	uiRect src = {0, 0, handler->width, handler->height};
	uiRect dest = {0, 0, 320, 180};
	uiImageBufferDraw(p->Context, handler->img, &src, &dest, 1);
}
