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
	float duration;
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
	uiEntry *OAuth;
	uiSpinbox *dwnThreadsSpin;
	uiArea *imageArea;
	struct handler *handler;
} VodOptions;

uiControl *VodDownloaderDrawUi(void);
void VodDownloaderResetUi(void);
