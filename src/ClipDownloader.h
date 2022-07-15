#include "utils.h"
#include "../libs/stb_image.h"

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

typedef enum { DOWNLOADING, LOGGING, FINISH } Flags;

typedef struct {
	ClipOptions *clipOptions;
	Flags flag;
	int i;
	char *buf;
} uiData;

uiControl *ClipDownloaderDrawUi(void);