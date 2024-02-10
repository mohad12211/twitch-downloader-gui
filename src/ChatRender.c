#include "ChatRender.h"

static ChatRenderOptions *renderOptions;
static const container containers[4];

static void resetArgs(uiButton *b, void *args);
static void containerChanged(uiCombobox *c, void *args);
static void codecChanged(uiCombobox *c, void *args);
static void browseBtnClicked(uiButton *b, void *args);
static void renderBtnClicked(uiButton *b, void *args);
static void *renderTask(void *args);
static void runOnUiThread(void *args);

uiControl *ChatRenderDrawUi(void) {
	uiBox *mainVerticalBox = uiNewVerticalBox();
	uiBoxSetPadded(mainVerticalBox, 1);

	// TODO: changes names
	uiBox *linkBox = uiNewHorizontalBox();
	uiBoxSetPadded(linkBox, 1);
	uiForm *linkForm = uiNewForm();
	uiFormSetPadded(linkForm, 1);
	uiEntry *filePath = uiNewEntry();
	uiButton *browseBtn = uiNewButton("Browse");
	uiFormAppend(linkForm, "JSON File: ", uiControl(filePath), 0);
	uiBoxAppend(linkBox, uiControl(linkForm), 1);
	uiBoxAppend(linkBox, uiControl(browseBtn), 0);
	uiBoxAppend(mainVerticalBox, uiControl(linkBox), 0);

	uiBox *middleHorizontalBox = uiNewHorizontalBox();
	uiBoxSetPadded(middleHorizontalBox, 1);
	uiBoxAppend(mainVerticalBox, uiControl(middleHorizontalBox), 1);

	uiForm *advancedOptions = uiNewForm();
	uiFormSetPadded(advancedOptions, 1);
	uiEntry *inputArgs = uiNewEntry();
	uiEntry *outputArgs = uiNewEntry();
	uiButton *reset = uiNewButton("Reset to default");
	uiFormAppend(advancedOptions, "Input Arguments:", uiControl(inputArgs), 0);
	uiFormAppend(advancedOptions, "Output Arguments:", uiControl(outputArgs), 0);
	uiFormAppend(advancedOptions, "", uiControl(reset), 0);

	uiBox *optionsVerticalBox = uiNewVerticalBox();
	uiBoxSetPadded(optionsVerticalBox, 1);

	uiBox *optionsBox = uiNewHorizontalBox();
	uiBoxSetPadded(optionsBox, 1);
	uiBoxAppend(optionsVerticalBox, uiControl(optionsBox), 0);

	uiTab *tab = uiNewTab();
	uiTabAppend(tab, "Options", uiControl(optionsVerticalBox));
	uiTabAppend(tab, "Advanced", uiControl(uiControl(advancedOptions)));
	uiTabSetMargined(tab, 0, 1);
	uiTabSetMargined(tab, 1, 1);
	uiBoxAppend(middleHorizontalBox, uiControl(tab), 0);

	uiForm *firstOptionsForm = uiNewForm();
	uiFormSetPadded(firstOptionsForm, 1);
	uiBoxAppend(optionsBox, uiControl(firstOptionsForm), 0);

	uiFontButton *fontOptions = uiNewFontButton();
	uiFormAppend(firstOptionsForm, "Font:", uiControl(fontOptions), 0);

	uiColorButton *fontColor = uiNewColorButton();
	uiColorButtonSetColor(fontColor, 1, 1, 1, 1);
	uiFormAppend(firstOptionsForm, "Font Color:", uiControl(fontColor), 0);

	uiColorButton *backgroundColor = uiNewColorButton();
	uiColorButtonSetColor(backgroundColor, 17.0 / 255.0, 17.0 / 255.0, 17.0 / 255.0, 1);
	uiFormAppend(firstOptionsForm, "Background Color: ", uiControl(backgroundColor), 0);

	uiEntry *frameRate = uiNewEntry();
	uiEntrySetText(frameRate, "60");
	uiFormAppend(firstOptionsForm, "Framerate:", uiControl(frameRate), 0);

	uiEntry *width = uiNewEntry();
	uiEntrySetText(width, "350");
	uiFormAppend(firstOptionsForm, "Width:", uiControl(width), 0);

	uiEntry *height = uiNewEntry();
	uiEntrySetText(height, "600");
	uiFormAppend(firstOptionsForm, "Height:", uiControl(height), 0);

	uiEntry *updateTime = uiNewEntry();
	uiEntrySetText(updateTime, "0.2");
	uiFormAppend(firstOptionsForm, "Update Time:", uiControl(updateTime), 0);

	uiBoxAppend(optionsBox, uiControl(uiNewVerticalSeparator()), 0);

	uiForm *secondOptionsForm = uiNewForm();
	uiFormSetPadded(secondOptionsForm, 1);
	uiBoxAppend(optionsBox, uiControl(secondOptionsForm), 0);

	uiCheckbox *outlineCheck = uiNewCheckbox("");
	uiFormAppend(secondOptionsForm, "Outline:", uiControl(outlineCheck), 0);

	uiCheckbox *timestampCheck = uiNewCheckbox("");
	uiFormAppend(secondOptionsForm, "Timestamp:", uiControl(timestampCheck), 0);

	uiCheckbox *FFZEmotesCheck = uiNewCheckbox("");
	uiCheckboxSetChecked(FFZEmotesCheck, 1);
	uiFormAppend(secondOptionsForm, "FFZ Emotes:", uiControl(FFZEmotesCheck), 0);

	uiCheckbox *BTTVEmotesCheck = uiNewCheckbox("");
	uiCheckboxSetChecked(BTTVEmotesCheck, 1);
	uiFormAppend(secondOptionsForm, "BTTV Emotes:", uiControl(BTTVEmotesCheck), 0);

	uiCheckbox *sevenTVEmotesCheck = uiNewCheckbox("");
	uiCheckboxSetChecked(sevenTVEmotesCheck, 1);
	uiFormAppend(secondOptionsForm, "7TV Emotes:", uiControl(sevenTVEmotesCheck), 0);

	uiCheckbox *subMsgCheck = uiNewCheckbox("");
	uiCheckboxSetChecked(subMsgCheck, 1);
	uiFormAppend(secondOptionsForm, "Sub Messages:", uiControl(subMsgCheck), 0);

	uiCheckbox *chatBadgesCheck = uiNewCheckbox("");
	uiCheckboxSetChecked(chatBadgesCheck, 1);
	uiFormAppend(secondOptionsForm, "Chat Badges:", uiControl(chatBadgesCheck), 0);

	uiCheckbox *generateMaskCheck = uiNewCheckbox("");
	uiFormAppend(secondOptionsForm, "Generate Mask:", uiControl(generateMaskCheck), 0);

	uiFormAppend(secondOptionsForm, "Mask Tutorial:",
							 uiControl(uiNewLabel("<a href=\"https://www.youtube.com/watch?v=D-_TD1-w3fY\"title=\""
																		"Chat Transparency Tutorial with Mask, Click to watch\">Video</a>")),
							 0);

	uiBoxAppend(optionsBox, uiControl(uiNewVerticalSeparator()), 0);

	uiForm *thirdOptionsForm = uiNewForm();
	uiFormSetPadded(thirdOptionsForm, 1);
	uiBoxAppend(optionsBox, uiControl(thirdOptionsForm), 0);

	uiEntry *outlineSize = uiNewEntry();
	uiEntrySetText(outlineSize, "4");
	uiFormAppend(thirdOptionsForm, "Outline Size:", uiControl(outlineSize), 0);

	uiCombobox *msgFontStyle = uiNewCombobox();
	uiComboboxAppend(msgFontStyle, "Normal");
	uiComboboxAppend(msgFontStyle, "Bold");
	uiComboboxAppend(msgFontStyle, "Italic");
	uiComboboxSetSelected(msgFontStyle, 0);
	uiFormAppend(thirdOptionsForm, "Message Fontstyle:", uiControl(msgFontStyle), 0);

	uiCombobox *userFontStyle = uiNewCombobox();
	uiComboboxAppend(userFontStyle, "Normal");
	uiComboboxAppend(userFontStyle, "Bold");
	uiComboboxAppend(userFontStyle, "Italic");
	uiComboboxSetSelected(userFontStyle, 0);
	uiFormAppend(thirdOptionsForm, "Username FontStyle:", uiControl(userFontStyle), 0);

	uiCombobox *containersBox = uiNewCombobox();
	uiComboboxAppend(containersBox, "MP4");
	uiComboboxAppend(containersBox, "MOV");
	uiComboboxAppend(containersBox, "WEBM");
	uiComboboxAppend(containersBox, "MKV");
	uiComboboxSetSelected(containersBox, 0);
	uiFormAppend(thirdOptionsForm, "Format:", uiControl(containersBox), 0);

	uiCombobox *codecs = uiNewCombobox();
	uiComboboxAppend(codecs, "H264");
	uiComboboxAppend(codecs, "H265");
	uiComboboxAppend(codecs, "H264 NVIDIA");
	uiComboboxSetSelected(codecs, 0);
	uiEntrySetText(inputArgs, containers[0].supportedCodecs[0].inputArgs);
	uiEntrySetText(outputArgs, containers[0].supportedCodecs[0].outputArgs);
	uiFormAppend(thirdOptionsForm, "Codec:", uiControl(codecs), 0);

	uiGrid *ignoreLIstGrid = uiNewGrid();
	uiBoxAppend(optionsVerticalBox, uiControl(ignoreLIstGrid), 0);
	uiForm *textForm = uiNewForm();
	uiFormSetPadded(textForm, 0);
	uiFormAppend(textForm, "Ignore users list: ",
							 uiControl(uiNewLabel("<a href=\"\"title=\"List of usernames - comma separated, spaces around commas ignored\">(?)</a>  ")), 0);
	uiEntry *ignoreListEntry = uiNewEntry();
	uiGridAppend(ignoreLIstGrid, uiControl(textForm), 0, 0, 1, 1, 0, uiAlignCenter, 0, uiAlignCenter);
	uiGridAppend(ignoreLIstGrid, uiControl(ignoreListEntry), 1, 0, 1, 1, 1, uiAlignFill, 0, uiAlignCenter);

	uiForm *logForm = uiNewForm();
	uiFormSetPadded(logForm, 1);
	uiMultilineEntry *logsEntry = uiNewNonWrappingMultilineEntry();
	uiMultilineEntrySetReadOnly(logsEntry, 1);
	uiFormAppend(logForm, "Logs: ", uiControl(logsEntry), 1);
	uiBoxAppend(middleHorizontalBox, uiControl(logForm), 1);

	uiButton *renderBtn = uiNewButton("Render");
	uiGrid *downloadGrid = uiNewGrid();
	uiGridSetPadded(downloadGrid, 1);
	uiGridAppend(downloadGrid, uiControl(renderBtn), 0, 0, 1, 1, 1, uiAlignCenter, 1, uiAlignFill);
	uiBoxAppend(mainVerticalBox, uiControl(downloadGrid), 0);

	uiBox *pBarBox = uiNewHorizontalBox();
	uiBoxSetPadded(pBarBox, 1);
	uiProgressBar *pBar = uiNewProgressBar();
	uiLabel *status = uiNewLabel("Idle...");
	uiBoxAppend(pBarBox, uiControl(status), 0);
	uiBoxAppend(pBarBox, uiControl(pBar), 1);
	uiBoxAppend(mainVerticalBox, uiControl(pBarBox), 0);

	renderOptions = malloc(sizeof(ChatRenderOptions));
	*renderOptions = (ChatRenderOptions){
			fontOptions,
			fontColor,
			backgroundColor,
			filePath,
			frameRate,
			width,
			height,
			updateTime,
			ignoreListEntry,
			outlineSize,
			inputArgs,
			outputArgs,
			outlineCheck,
			timestampCheck,
			FFZEmotesCheck,
			BTTVEmotesCheck,
			sevenTVEmotesCheck,
			subMsgCheck,
			chatBadgesCheck,
			generateMaskCheck,
			msgFontStyle,
			userFontStyle,
			containersBox,
			codecs,
			logsEntry,
			pBar,
			status,
			renderBtn,
			browseBtn,
			NULL,
			0,
	};

	uiComboboxOnSelected(containersBox, containerChanged, NULL);
	uiComboboxOnSelected(codecs, codecChanged, NULL);
	uiButtonOnClicked(reset, resetArgs, NULL);
	uiButtonOnClicked(browseBtn, browseBtnClicked, NULL);
	uiButtonOnClicked(renderBtn, renderBtnClicked, NULL);

	return uiControl(mainVerticalBox);
}

static void browseBtnClicked(uiButton *b, void *args) {
	char *chatFile = uiOpenFile(mainwin, NULL, "JSON File (*.json)|*.json");
	uiEntrySetText(renderOptions->filePath, chatFile ? chatFile : "");
	uiFreeText(chatFile);
}

static void renderBtnClicked(uiButton *b, void *args) {
	int selectedContainer = uiComboboxSelected(renderOptions->containersBox);
	char *chatFile = uiEntryText(renderOptions->filePath);
	char *name = strdup(strrchr(chatFile, '/') + 1);
	// "abcdefg.json"
	//          ^
	//       strlen-4
	int offset = strlen(name) - 4;
	size_t i = 0;
	for (; i < strlen(containers[selectedContainer].name); i++) {
		name[i + offset] = tolower(containers[selectedContainer].name[i]);
	}
	name[i + offset] = '\0';
	char *videoFile = uiSaveFile(mainwin, NULL, name, containers[selectedContainer].filter);
	if (videoFile == NULL) {
		free(name);
		uiFreeText(chatFile);
		return;
	}

	string *cmd = malloc(sizeof(string));
	*cmd = (string){malloc(sizeof(char) * 100), 0, 100};

	concat(cmd, 4, getBinaryPath(), " chatrender -i \"", chatFile, "\" ");
	concat(cmd, 3, " --temp-path \"", getJson(configJson, "tempFolder")->valuestring, "\" ");

	uiFontDescriptor font;
	uiFontButtonFont(renderOptions->fontOptions, &font);
	char fontSize[9];
	// use default locale so that the decimal symbol is dot '.' not a comma ','
	setlocale(LC_NUMERIC, "C");
	snprintf(fontSize, 9, "%f", font.Size);
	concat(cmd, 4, " -f '", font.Family, "' --font-size ", fontSize);

	char fontColorHex[10];
	double red, green, blue, alpha;
	uiColorButtonColor(renderOptions->fontColor, &red, &green, &blue, &alpha);
	sprintf(fontColorHex, "'#%02X%02X%02X'", (int)(red * 255), (int)(green * 255), (int)(blue * 255));
	concat(cmd, 2, " --message-color ", fontColorHex);

	if (uiCheckboxChecked(renderOptions->generateMaskCheck)) {
		concat(cmd, 1, " --generate-mask --background-color '#00000000' ");
	} else {
		uiColorButtonColor(renderOptions->backgroundColor, &red, &green, &blue, &alpha);
		sprintf(fontColorHex, "'#%02X%02X%02X'", (int)(red * 255), (int)(green * 255), (int)(blue * 255));
		concat(cmd, 2, " --background-color ", fontColorHex);
	}

	char *fr, *width, *height, *updateTime, *outlineSize, *ignorelist;
	fr = uiEntryText(renderOptions->frameRate);
	width = uiEntryText(renderOptions->width);
	height = uiEntryText(renderOptions->height);
	updateTime = uiEntryText(renderOptions->updateTime);
	outlineSize = uiEntryText(renderOptions->outlineSize);
	ignorelist = uiEntryText(renderOptions->ignoreListEntry);
	concat(cmd, 2, " --framerate ", fr);
	concat(cmd, 2, " -w ", width);
	concat(cmd, 2, " -h ", height);
	concat(cmd, 2, " --update-rate ", updateTime);
	concat(cmd, 3, " --ignore-users \"", ignorelist, "\" ");

	if (uiCheckboxChecked(renderOptions->outlineCheck)) {
		concat(cmd, 1, " --outline ");
		concat(cmd, 2, " --outline-size ", outlineSize);
	}
	if (uiCheckboxChecked(renderOptions->timestampCheck))
		concat(cmd, 1, " --timestamp ");
	if (!uiCheckboxChecked(renderOptions->FFZEmotesCheck))
		concat(cmd, 1, " --ffz=false ");
	if (!uiCheckboxChecked(renderOptions->BTTVEmotesCheck))
		concat(cmd, 1, " --bttv=false ");
	if (!uiCheckboxChecked(renderOptions->sevenTVEmotesCheck))
		concat(cmd, 1, " --stv=false ");
	if (!uiCheckboxChecked(renderOptions->subMsgCheck))
		concat(cmd, 1, " --sub-messages=false ");
	if (!uiCheckboxChecked(renderOptions->chatBadgesCheck))
		concat(cmd, 1, " --badges=false ");

	char *inputArgs = uiEntryText(renderOptions->inputArgs);
	char *outputArgs = uiEntryText(renderOptions->outputArgs);

	concat(cmd, 5, " --input-args='", inputArgs, "' --output-args='", outputArgs, "' ");

	concat(cmd, 3, " -o \"", videoFile, "\" ");
	renderOptions->cmd = cmd;

	free(fr);
	free(name);
	free(width);
	free(height);
	free(updateTime);
	free(outlineSize);
	free(inputArgs);
	free(outputArgs);
	free(ignorelist);
	uiFreeText(videoFile);
	uiFreeText(chatFile);

	pthread_t thread;
	pthread_create(&thread, NULL, renderTask, args);
	pthread_detach(thread);
}

static void *renderTask(void *args) {
	char buf[200];
	FILE *fp;
	pid_t pid;

	if ((fp = mypopen((char *)renderOptions->cmd->memory, &pid)) == NULL) {
		printf("Error opening pipe!\n");
		return NULL;
	}

	renderOptions->renderpid = pid;

	uiData *data = malloc(sizeof(uiData));
	*data = (uiData){.flag = PREPARE};
	uiQueueMain(runOnUiThread, data);

	while (mygets(buf, 200, fp) != NULL) {
		uiData *logData = malloc(sizeof(uiData));
		if (strstr(buf, "STATUS")) {
			int statusOffset = strlen("[STATUS] - ");
			buf[strlen(buf) - 1] = '\0';
			logData->buf = strdup(buf + statusOffset);
			logData->i = -1;
			if (strstr(buf, "%")) {
				int offset = strlen("[STATUS] - Rendering Video ");
				int percentage = atoi(buf + offset);
				logData->i = percentage;
			}
			logData->flag = STATUS;
		} else {
			logData->buf = strdup(buf);
			logData->flag = LOGGING;
		}
		uiQueueMain(runOnUiThread, logData);
	}

	data = malloc(sizeof(uiData));
	*data = (uiData){.flag = FINISH, .i = mypclose(fp, pid)};
	uiQueueMain(runOnUiThread, data);

	free(renderOptions->cmd->memory);
	free(renderOptions->cmd);
	renderOptions->cmd = NULL;
	renderOptions->renderpid = 0;

	return NULL;
}

static void codecChanged(uiCombobox *c, void *args) {
	if (uiComboboxSelected(c) == -1)
		return;
	int selectedContainer = uiComboboxSelected(renderOptions->containersBox);
	int seleectedCodec = uiComboboxSelected(renderOptions->codecs);
	uiEntrySetText(renderOptions->inputArgs, containers[selectedContainer].supportedCodecs[seleectedCodec].inputArgs);
	uiEntrySetText(renderOptions->outputArgs, containers[selectedContainer].supportedCodecs[seleectedCodec].outputArgs);
}

static void containerChanged(uiCombobox *c, void *args) {
	uiCombobox *codecs = renderOptions->codecs;
	int selectedContainer = uiComboboxSelected(c);
	uiComboboxClear(codecs);
	for (int i = 0; i < 4 && containers[selectedContainer].supportedCodecs[i].name; i++)
		uiComboboxAppend(codecs, containers[selectedContainer].supportedCodecs[i].name);
	uiComboboxSetSelected(codecs, 0);
	uiEntrySetText(renderOptions->inputArgs, containers[selectedContainer].supportedCodecs[0].inputArgs);
	uiEntrySetText(renderOptions->outputArgs, containers[selectedContainer].supportedCodecs[0].outputArgs);
}

static void resetArgs(uiButton *b, void *args) {
	int selectedContainer = uiComboboxSelected(renderOptions->containersBox);
	int seleectedCodec = uiComboboxSelected(renderOptions->codecs);
	uiEntrySetText(renderOptions->inputArgs, containers[selectedContainer].supportedCodecs[seleectedCodec].inputArgs);
	uiEntrySetText(renderOptions->outputArgs, containers[selectedContainer].supportedCodecs[seleectedCodec].outputArgs);
}

static void runOnUiThread(void *args) {
	uiData *data = (uiData *)args;
	switch (data->flag) {
	case PREPARE:
		uiControlDisable(uiControl(renderOptions->renderBtn));
		uiControlDisable(uiControl(renderOptions->browseBtn));
		uiLabelSetText(renderOptions->status, "Preparing...");
		uiProgressBarSetValue(renderOptions->pBar, -1);
		break;
	case STATUS:
		uiLabelSetText(renderOptions->status, data->buf);
		uiProgressBarSetValue(renderOptions->pBar, data->i);
		free(data->buf);
		break;
	case LOGGING:
		uiMultilineEntryAppend(renderOptions->logsEntry, data->buf);
		if (strstr(data->buf, "command not found") && strstr(data->buf, "TwitchDownloaderCLI")) {
			uiMultilineEntryAppend(renderOptions->logsEntry, "Please specify the TwitchDownloaderCLI path from the options");
		}
		free(data->buf);
		break;
	case FINISH:
		if (WIFEXITED(data->i) && WEXITSTATUS(data->i) == 143)
			break;
		if (data->i) {
			uiLabelSetText(renderOptions->status, "Error...");
			uiProgressBarSetValue(renderOptions->pBar, 0);
		} else {
			uiLabelSetText(renderOptions->status, "Done!");
			uiProgressBarSetValue(renderOptions->pBar, 100);
		}
		uiControlEnable(uiControl(renderOptions->renderBtn));
		uiControlEnable(uiControl(renderOptions->browseBtn));
		break;

	default:
		break;
	}
	free(data);
}

void ChatRenderResetUi(void) {
	if (renderOptions->renderpid)
		killpg(renderOptions->renderpid, SIGTERM);
}

static const container containers[4] = {{"MP4",
																				 "mp4 File (*.mp4)|*.mp4",
																				 {{"H264",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libx264 -preset veryfast -crf 18 -pix_fmt yuv420p \"{save_path}\""},
																					{"H265",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libx265 -preset veryfast -crf 18 -pix_fmt yuv420p \"{save_path}\""},
																					{"H264 NVIDIA",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v h264_nvenc -preset fast -cq 20 -pix_fmt yuv420p \"{save_path}\""}}},
																				{"MOV",
																				 "mov file (*.mov)|*.mov",
																				 {{"H264",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libx264 -preset veryfast -crf 18 -pix_fmt yuv420p \"{save_path}\""},
																					{"H265",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libx265 -preset veryfast -crf 18 -pix_fmt yuv420p \"{save_path}\""},
																					{"ProRes",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v prores_ks -qscale:v 62 -pix_fmt argb \"{save_path}\""},
																					{"RLE",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v qtrle -pix_fmt argb \"{save_path}\""}}},
																				{"WEBM",
																				 "webm File (*.webm)|*.webm",
																				 {{"VP8",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libvpx -crf 18 -b:v 2M -pix_fmt yuva420p -auto-alt-ref 0 \"{save_path}\""},
																					{"VP9",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libvpx-vp9 -crf 18 -b:v 2M -pix_fmt yuva420p \"{save_path}\""}}},
																				{"MKV",
																				 "mkv File (*.mkv)|*.mkv",
																				 {{"H264",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libx264 -preset veryfast -crf 18 -pix_fmt yuv420p \"{save_path}\""},
																					{"H265",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libx265 -preset veryfast -crf 18 -pix_fmt yuv420p \"{save_path}\""},
																					{"VP8",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libvpx -crf 18 -b:v 2M -pix_fmt yuva420p -auto-alt-ref 0 \"{save_path}\""},
																					{"VP9",
																					 "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize {max_int} -pix_fmt bgra -video_size "
																					 "{width}x{height} -i -",
																					 "-c:v libvpx-vp9 -crf 18 -b:v 2M -pix_fmt yuva420p \"{save_path}\""}}}};
