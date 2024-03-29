#include "utils.h"

#define commandTemplateLength 64

typedef struct {
	uiCombobox *qualities;
	uiSpinbox *dwnBandwidthSpin;
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
	pid_t downloadpid;
	struct handler *handler;
} ClipOptions;

uiControl *ClipDownloaderDrawUi(void);
void ClipDownloaderResetUi(void);
