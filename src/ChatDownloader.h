#include "utils.h"

typedef struct {
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
	uiRadioButtons *downloadFormats;
	uiRadioButtons *timeFormats;
	uiCheckbox *cropStartCheck;
	uiCheckbox *cropEndCheck;
	uiBox *startSpinsBox;
	uiBox *cropStartBox;
	uiBox *endSpinsBox;
	uiBox *cropEndBox;
	uiSpinbox *startHour;
	uiSpinbox *startMin;
	uiSpinbox *startSec;
	uiSpinbox *endHour;
	uiSpinbox *endMin;
	uiSpinbox *endSec;
	uiCheckbox *embedEmotes;
	uiArea *imageArea;
	struct handler *handler;
} ChatDwnOptions;

uiControl *ChatDownloaderDrawUi(void);
void ChatDownloaderResetUi(void);
