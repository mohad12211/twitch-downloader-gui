#include "VodDownloader.h"

static VodOptions *vodOptions;
static QualityList qualitiesList;

static void downloadBtnClicked(uiButton *b, void *data);
static void infoBtnClicked(uiButton *b, void *data);
static void cropToggle(uiCheckbox *c, void *data);
static char *getId(char *link);
static int setInfo(char *id);
static void setQualitites(char *id, uiCombobox *cBox);
static void setThumbnail(char *link);
static void *downloadTask(void *args);
static void runOnUiThread(void *args);
static void handlerMouseEvent(uiAreaHandler *ah, uiArea *area, uiAreaMouseEvent *e);
static void handlerMouseCrossed(uiAreaHandler *ah, uiArea *area, int left);
static void handlerDragBroken(uiAreaHandler *ah, uiArea *area);
static int handlerKeyEvent(uiAreaHandler *ah, uiArea *area, uiAreaKeyEvent *e);
static void handlerDraw(uiAreaHandler *ah, uiArea *area, uiAreaDrawParams *p);

uiControl *VodDownloaderDrawUi(void) {
	uiBox *mainVerticalBox = uiNewVerticalBox();
	uiBoxSetPadded(mainVerticalBox, 1);

	uiBox *linkBox = uiNewHorizontalBox();
	uiBoxSetPadded(linkBox, 1);
	uiForm *linkForm = uiNewForm();
	uiFormSetPadded(linkForm, 1);
	uiEntry *linkEntry = uiNewEntry();
	uiButton *infoBtn = uiNewButton("Get info");
	uiFormAppend(linkForm, "Vod Link: ", uiControl(linkEntry), 0);
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

	// TODO: figure out how to add quality option
	uiForm *qualityForm = uiNewForm();
	uiFormSetPadded(qualityForm, 1);
	uiCombobox *qualities = uiNewCombobox();
	uiFormAppend(qualityForm, "Quality: ", uiControl(qualities), 0);
	uiBoxAppend(optionsBox, uiControl(qualityForm), 0);

	// TODO: rename these boxes
	uiBox *startSpinsBox = uiNewHorizontalBox(); // box that continas the 3 spinsBoxes
	uiBoxSetPadded(startSpinsBox, 0);
	uiControlDisable(uiControl(startSpinsBox));
	uiBox *cropStartBox = uiNewHorizontalBox(); // box that contains the checkbox and startSpinsbox
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

	uiGrid *OAuthGrid = uiNewGrid();
	uiBoxAppend(optionsBox, uiControl(OAuthGrid), 0);
	uiForm *textForm = uiNewForm();
	uiFormSetPadded(textForm, 0);
	uiFormAppend(textForm, "OAuth (optional) : ",
							 uiControl(uiNewLabel("<a href=\"https://www.youtube.com/watch?v=1MBsUoFGuls\"title=\"Only "
																		"required for sub only VODs. All 3rd party OAuth "
																		"tokens will not work. Click to see YouTube video "
																		"on how to get OAuth token.\">(?)</a>  ")),
							 0);
	uiEntry *OAuth = uiNewEntry();
	uiGridAppend(OAuthGrid, uiControl(textForm), 0, 0, 1, 1, 0, uiAlignCenter, 0, uiAlignCenter);
	uiGridAppend(OAuthGrid, uiControl(OAuth), 1, 0, 1, 1, 1, uiAlignFill, 0, uiAlignCenter);

	uiForm *dwnThreadForm = uiNewForm();
	uiFormSetPadded(dwnThreadForm, 1);
	uiSpinbox *dwnThreadsSpin = uiNewSpinbox(1, 20);
	uiSpinboxSetValue(dwnThreadsSpin, 5);
	uiFormAppend(dwnThreadForm, "Download Threads:", uiControl(dwnThreadsSpin), 0);
	uiBoxAppend(optionsBox, uiControl(dwnThreadForm), 0);

	uiForm *threadBandwidthForm = uiNewForm();
	uiFormSetPadded(threadBandwidthForm, 1);
	uiSpinbox *threadBandwidthSpin = uiNewSpinbox(128, 40960); // 128KB/s - 40MB/s
	uiSpinboxSetValue(threadBandwidthSpin, 2048);
	uiFormAppend(threadBandwidthForm, "Bandwidth Per Thread (KB/s):", uiControl(threadBandwidthSpin), 0);
	uiBoxAppend(optionsBox, uiControl(threadBandwidthForm), 0);

	uiBoxAppend(middleHorizontalBox, uiControl(uiNewVerticalSeparator()), 0);

	uiForm *logForm = uiNewForm();
	uiFormSetPadded(logForm, 1);
	uiMultilineEntry *logsEntry = uiNewNonWrappingMultilineEntry();
	uiMultilineEntrySetReadOnly(logsEntry, 1);
	uiFormAppend(logForm, "Logs: ", uiControl(logsEntry), 1);
	uiBoxAppend(middleHorizontalBox, uiControl(logForm), 1);

	uiGrid *downloadGrid = uiNewGrid();
	uiGridSetPadded(downloadGrid, 1);
	uiButton *downloadBtn = uiNewButton("Download");
	uiControlDisable(uiControl(downloadBtn));
	uiGridAppend(downloadGrid, uiControl(downloadBtn), 0, 0, 1, 1, 1, uiAlignCenter, 1, uiAlignFill);
	uiBoxAppend(mainVerticalBox, uiControl(downloadGrid), 0);

	uiBox *pBarBox = uiNewHorizontalBox();
	uiBoxSetPadded(pBarBox, 1);
	uiProgressBar *pBar = uiNewProgressBar();
	uiLabel *status = uiNewLabel("Idle...");
	uiBoxAppend(pBarBox, uiControl(status), 0);
	uiBoxAppend(pBarBox, uiControl(pBar), 1);
	uiBoxAppend(mainVerticalBox, uiControl(pBarBox), 0);

	vodOptions = malloc(sizeof(VodOptions));
	*vodOptions = (VodOptions){
			linkEntry, nameLabel, titleLabel,			durationLabel, createdLabel,	 logsEntry,						pBar,				 status,		 downloadBtn, infoBtn,	NULL,
			NULL,			 0,					cropStartCheck, cropEndCheck,	 startSpinsBox,	 cropStartBox,				endSpinsBox, cropEndBox, startHour,		startMin, startSec,
			endHour,	 endMin,		endSec,					OAuth,				 dwnThreadsSpin, threadBandwidthSpin, qualities,	 imageArea,	 handler,			0,
	};

	uiButtonOnClicked(infoBtn, infoBtnClicked, NULL);
	uiButtonOnClicked(downloadBtn, downloadBtnClicked, NULL);

	return uiControl(mainVerticalBox);
}

static void infoBtnClicked(uiButton *b, void *data) {
	VodDownloaderResetUi();
	char *link = uiEntryText(vodOptions->linkEntry);
	char *id = getId(link);
	if (id == NULL) {
		uiMsgBoxError(mainwin, "Error", "Invalid Url");
		goto err;
	}
	int validID = setInfo(id);
	if (!validID) {
		uiMsgBoxError(mainwin, "Error", "Invalid Vod ID");
		free(id);
		goto err;
	}
	setQualitites(id, vodOptions->qualities);
	uiControlEnable(uiControl(vodOptions->cropStartBox));
	uiControlEnable(uiControl(vodOptions->cropEndBox));
	uiControlEnable(uiControl(vodOptions->downloadBtn));
	vodOptions->id = id;
err:
	uiFreeText(link);
}

static void downloadBtnClicked(uiButton *b, void *data) {
	char *title = uiLabelText(vodOptions->titleLabel);
	char defaultName[strlen(title) + strlen(".mp4") + 1];
	sprintf(defaultName, "%s%s", title, ".mp4");
	char *fileName = uiSaveFile(mainwin, NULL, defaultName, "mp4 File (*.mp4)|*.mp4");
	uiFreeText(title);
	if (fileName == NULL) {
		return;
	}

	int hour, sec, min;
	char *durationString = uiLabelText(vodOptions->durationLabel);
	sscanf(durationString, "%d:%d:%d", &hour, &min, &sec);
	int startSec = 0, endSec = hour * 3600 + min * 60 + sec;

	string *cmd = malloc(sizeof(string));
	*cmd = (string){malloc(sizeof(char) * 100), 0, 100};
	concat(cmd, 4, getBinaryPath(), " videodownload -u '", vodOptions->id, "'");
	concat(cmd, 3, " -q \"", qualitiesList.qualities[uiComboboxSelected(vodOptions->qualities)].memory, "\" ");
	concat(cmd, 3, " --temp-path \"", getJson(configJson, "tempFolder")->valuestring, "\" ");
	concat(cmd, 1, "--collision Overwrite ");

	if (uiCheckboxChecked(vodOptions->cropStartCheck)) {
		char seconds[12];
		int tempInt = uiSpinboxValue(vodOptions->startHour) * 3600 + uiSpinboxValue(vodOptions->startMin) * 60 + uiSpinboxValue(vodOptions->startSec);
		startSec = tempInt;
		sprintf(seconds, "%d", tempInt);
		concat(cmd, 2, " -b ", seconds);
	}

	if (uiCheckboxChecked(vodOptions->cropEndCheck)) {
		char seconds[12];
		int tempInt = uiSpinboxValue(vodOptions->endHour) * 3600 + uiSpinboxValue(vodOptions->endMin) * 60 + uiSpinboxValue(vodOptions->endSec);
		endSec = tempInt;
		sprintf(seconds, "%d", tempInt);
		concat(cmd, 2, " -e ", seconds);
	}

	char threadCount[12];
	sprintf(threadCount, "%d", uiSpinboxValue(vodOptions->dwnThreadsSpin));
	concat(cmd, 2, " -t ", threadCount);

	char threadBandwidth[12];
	sprintf(threadBandwidth, "%d", uiSpinboxValue(vodOptions->threadBandwidthSpin));
	concat(cmd, 2, " --bandwidth ", threadBandwidth);

	char *OAuth = uiEntryText(vodOptions->OAuth);
	if (strlen(OAuth))
		concat(cmd, 3, " --oauth \"", OAuth, "\" ");

	concat(cmd, 3, " -o \"", fileName, "\" ");
	vodOptions->duration = endSec - startSec;
	vodOptions->cmd = cmd;

	uiFreeText(OAuth);
	uiFreeText(durationString);
	uiFreeText(fileName);
	if ((endSec - startSec) <= 0) {
		uiMsgBoxError(mainwin, "Error", "Invalid Crop Range\nMust be Hours/Mintes/Seconds");
		return;
	}
	pthread_t thread;
	pthread_create(&thread, NULL, downloadTask, data);
	pthread_detach(thread);
}

static void *downloadTask(void *args) {
	char buf[200];
	FILE *fp;
	pid_t pid;

	if ((fp = mypopen((char *)vodOptions->cmd->memory, &pid)) == NULL) {
		printf("Error opening pipe!\n");
		return NULL;
	}

	vodOptions->downloadpid = pid;

	uiData *data = malloc(sizeof(uiData));
	*data = (uiData){.flag = PREPARE};
	uiQueueMain(runOnUiThread, data);

	while (mygets(buf, 200, fp) != NULL) {
		uiData *logData = malloc(sizeof(uiData));
		if (strstr(buf, "Fetching")) {
			*logData = (uiData){.flag = PREPARE};
		} else if (strstr(buf, "Downloading")) {
			int offset = strlen("[STATUS] - Downloading ");
			int percentage = atoi(buf + offset);
			*logData = (uiData){.i = percentage, .flag = DOWNLOADING};
		} else if (strstr(buf, "Verifying")) {
			int offset = strlen("[STATUS] - Verifying Parts ");
			int percentage = atoi(buf + offset);
			*logData = (uiData){.i = percentage, .flag = VERIFYING};
		} else if (strstr(buf, "Combining")) {
			int offset = strlen("[STATUS] - Combining Parts ");
			int percentage = atoi(buf + offset);
			*logData = (uiData){.i = percentage, .flag = COMBINING};
		} else if (strstr(buf, "Finalizing")) {
			*logData = (uiData){.flag = FINALIZING};
		} else {
			*logData = (uiData){.flag = LOGGING, .buf = strdup(buf)};
		}
		uiQueueMain(runOnUiThread, logData);
	}

	data = malloc(sizeof(uiData));
	*data = (uiData){.flag = FINISH, .i = mypclose(fp, pid)};
	uiQueueMain(runOnUiThread, data);

	free(vodOptions->cmd->memory);
	free(vodOptions->cmd);
	vodOptions->cmd = NULL;
	vodOptions->downloadpid = 0;

	return NULL;
}

//  TODO: support receiving ID as input
static char *getId(char *link) {
	if (!strstr(link, "twitch.tv/videos/")) {
		return NULL;
	}

	CURLU *h = curl_url();
	CURLUcode uc;
	char *path = NULL;
	char *id = NULL;

	uc = curl_url_set(h, CURLUPART_URL, link, 0);
	uc = curl_url_get(h, CURLUPART_PATH, &path, 0);
	if (!uc) {
		id = strdup(strrchr(path, '/') + 1);
		curl_free(path);
	}
	curl_url_cleanup(h);
	return id;
}

static void setQualitites(char *id, uiCombobox *cBox) {
	string *accessTokenRes = getVodAcessToken(id);
	cJSON *root = cJSON_Parse((char *)accessTokenRes->memory);
	const char *value = cJSONUtils_GetPointer(root, "/data/videoPlaybackAccessToken/value")->valuestring;
	const char *signature = cJSONUtils_GetPointer(root, "/data/videoPlaybackAccessToken/signature")->valuestring;
	if (value && signature) {
		qualitiesList = getVodQualities(id, value, signature);
		// sort qualities
		for (size_t i = 0; i < qualitiesList.count - 1; i++) {
			for (size_t j = i + 1; j < qualitiesList.count; j++) {
				int qi = atoi((char *)qualitiesList.qualities[i].memory);
				int qj = atoi((char *)qualitiesList.qualities[j].memory);
				if (qi < qj) {
					string temp = qualitiesList.qualities[i];
					qualitiesList.qualities[i] = qualitiesList.qualities[j];
					qualitiesList.qualities[j] = temp;
				}
			}
		}

		if (qualitiesList.count > 0) {
			for (size_t i = 0; i < qualitiesList.count; i++) {
				uiComboboxAppend(cBox, (char *)qualitiesList.qualities[i].memory);
			}
			uiComboboxSetSelected(cBox, 0);
		} else {
			uiMsgBoxError(mainwin, "Error", "No Qualities Found");
		}
	}
	free(accessTokenRes->memory);
	free(accessTokenRes);
	cJSON_Delete(root);
}

static int setInfo(char *id) {
	int validID = 1;
	string *infoRes = getVodInfo(id);
	cJSON *root = cJSON_Parse((char *)infoRes->memory);
	if (cJSON_IsNull(cJSONUtils_GetPointer(root, "/data/video"))) {
		validID = 0;
		goto err;
	}
	char duration[11];
	uiLabelSetText(vodOptions->nameLabel, cJSONUtils_GetPointer(root, "/data/video/owner/displayName")->valuestring);
	uiLabelSetText(vodOptions->titleLabel, cJSONUtils_GetPointer(root, "/data/video/title")->valuestring);
	int seconds = cJSONUtils_GetPointer(root, "/data/video/lengthSeconds")->valueint;
	sprintf(duration, "%02u:%02u:%02u", seconds / 3600, (seconds % 3600) / 60, seconds % 60);
	uiLabelSetText(vodOptions->durationLabel, duration);
	char *createdLocalTime = getLocalTime(cJSONUtils_GetPointer(root, "/data/video/createdAt")->valuestring);
	uiLabelSetText(vodOptions->createdLabel, createdLocalTime);
	cJSON *thumbnail = cJSONUtils_GetPointer(root, "/data/video/thumbnailURLs/0");
	if (thumbnail)
		setThumbnail(thumbnail->valuestring);

	free(createdLocalTime);
err:
	free(infoRes->memory);
	free(infoRes);
	cJSON_Delete(root);
	return validID;
}

// https://www.cairographics.org/manual/cairo-Image-Surfaces.html#cairo-format-t
// stb gets the image in RGBA, but cairo needs it in ARGB, AND in native-endian, so little-endian, so it will be BGRA
// so I have to swap the B and the R, the image is 320*180 so should be fast.
static void setThumbnail(char *link) {
	int x, y, n;
	string *response = requestImage(link);
	vodOptions->handler->binaryData = stbi_load_from_memory(response->memory, response->used, &x, &y, &n, 4);
	vodOptions->handler->width = x;
	vodOptions->handler->height = y;
	for (int i = 0; i < x * y; i++) {
		unsigned char red = vodOptions->handler->binaryData[i * 4];
		vodOptions->handler->binaryData[i * 4] = vodOptions->handler->binaryData[i * 4 + 2];
		vodOptions->handler->binaryData[i * 4 + 2] = red;
	}
	vodOptions->handler->img = uiNewImageBuffer(NULL, x, y, 1);
	uiImageBufferUpdate(vodOptions->handler->img, (void *)vodOptions->handler->binaryData);
	uiAreaQueueRedrawAll(vodOptions->imageArea);
	free(response->memory);
	free(response);
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
	case PREPARE:
		uiControlDisable(uiControl(vodOptions->downloadBtn));
		uiControlDisable(uiControl(vodOptions->infoBtn));
		uiLabelSetText(vodOptions->status, "Fetching Video Info [1/5]");
		uiProgressBarSetValue(vodOptions->pBar, -1);
		break;
	case DOWNLOADING:
		uiProgressBarSetValue(vodOptions->pBar, MIN(data->i, 100));
		uiLabelSetText(vodOptions->status, "Downloading...[2/5]");
		break;
	case VERIFYING:
		uiProgressBarSetValue(vodOptions->pBar, MIN(data->i, 100));
		uiLabelSetText(vodOptions->status, "Verifying Parts...[3/5]");
		break;
	case COMBINING:
		uiProgressBarSetValue(vodOptions->pBar, MIN(data->i, 100));
		uiLabelSetText(vodOptions->status, "Combining Parts...[4/5]");
		break;
	case FINALIZING:
		uiProgressBarSetValue(vodOptions->pBar, -1);
		uiLabelSetText(vodOptions->status, "Finalizing Video...[5/5]");
		break;
	case LOGGING:
		uiMultilineEntryAppend(vodOptions->logsEntry, data->buf);
		if (strstr(data->buf, "command not found") && strstr(data->buf, "TwitchDownloaderCLI")) {
			uiMultilineEntryAppend(vodOptions->logsEntry, "Please specify the TwitchDownloaderCLI path from the options");
		}
		free(data->buf);
		break;
	case FINISH:
		if (WIFEXITED(data->i) && WEXITSTATUS(data->i) == 143)
			break;
		if (data->i) {
			uiLabelSetText(vodOptions->status, "Error...");
			uiProgressBarSetValue(vodOptions->pBar, 0);
		} else {
			uiLabelSetText(vodOptions->status, "Done!");
			uiProgressBarSetValue(vodOptions->pBar, 100);
		}
		uiControlEnable(uiControl(vodOptions->downloadBtn));
		uiControlEnable(uiControl(vodOptions->infoBtn));
		break;
	default:
		break;
	}
	free(data);
}

void VodDownloaderResetUi(void) {
	if (vodOptions->downloadpid)
		killpg(vodOptions->downloadpid, SIGTERM);
	free(vodOptions->id);
	vodOptions->id = NULL;
	stbi_image_free(vodOptions->handler->binaryData);
	vodOptions->handler->binaryData = NULL;
	if (vodOptions->handler->img) {
		uiFreeImageBuffer(vodOptions->handler->img);
		vodOptions->handler->img = NULL;
	}
	QualityList_destroy(&qualitiesList);
	uiLabelSetText(vodOptions->nameLabel, "");
	uiLabelSetText(vodOptions->titleLabel, "");
	uiLabelSetText(vodOptions->durationLabel, "");
	uiLabelSetText(vodOptions->createdLabel, "");
	uiLabelSetText(vodOptions->status, "Idle...");
	uiComboboxClear(vodOptions->qualities);
	uiProgressBarSetValue(vodOptions->pBar, 0);
	uiCheckboxSetChecked(vodOptions->cropStartCheck, 0);
	uiCheckboxSetChecked(vodOptions->cropEndCheck, 0);
	uiControlDisable(uiControl(vodOptions->startSpinsBox));
	uiControlDisable(uiControl(vodOptions->endSpinsBox));
	uiControlDisable(uiControl(vodOptions->cropStartBox));
	uiControlDisable(uiControl(vodOptions->cropEndBox));
	uiControlDisable(uiControl(vodOptions->downloadBtn));
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
