#include "utils.h"

typedef struct ChatRenderOptions {
	uiFontButton *fontOptions;
	uiColorButton *fontColor;
	uiColorButton *backgroundColor;
	uiEntry *filePath;
	uiEntry *frameRate;
	uiEntry *width;
	uiEntry *height;
	uiEntry *updateTime;
	uiEntry *ignoreListEntry;
	uiEntry *outlineSize;
	uiEntry *inputArgs;
	uiEntry *outputArgs;
	uiCheckbox *outlineCheck;
	uiCheckbox *timestampCheck;
	uiCheckbox *FFZEmotesCheck;
	uiCheckbox *BTTVEmotesCheck;
	uiCheckbox *sevenTVEmotesCheck;
	uiCheckbox *subMsgCheck;
	uiCheckbox *chatBadgesCheck;
	uiCheckbox *generateMaskCheck;
	uiCombobox *msgFontStyle;
	uiCombobox *userFontStyle;
	uiCombobox *containersBox;
	uiCombobox *codecs;
	uiMultilineEntry *logsEntry;
	uiProgressBar *pBar;
	uiLabel *status;
	uiButton *renderBtn;
	uiButton *browseBtn;
	string *cmd;
	pid_t renderpid;
} ChatRenderOptions;

typedef struct {
	const char *const name;
	const char *const filter;
	const struct codec {
		const char *const name;
		const char *const inputArgs;
		const char *const outputArgs;
	} supportedCodecs[4];
} container;

uiControl *ChatRenderDrawUi(void);
void ChatRenderResetUi(void);
