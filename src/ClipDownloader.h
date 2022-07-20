#include "utils.h"

#define commandTemplateLength 64

typedef struct {
	uiCombobox *qualities;
	uiEntry *linkEntry;
	uiLabel *nameLabel;
	uiLabel *titleLabel;
	uiLabel *durationLabel;
	uiLabel *createdLabel;
	uiMultilineEntry *logsEntry;
	uiProgressBar *pBar;
	uiLabel *status;
	uiButton *downloadBtn;
	uiButton *infoBtn;
	char *id;
	string *cmd;
	uiArea *imageArea;
	struct handler *handler;
} ClipOptions;

uiControl *ClipDownloaderDrawUi(void);
void ClipDownloaderResetUi(void);