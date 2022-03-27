#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"

extern uiWindow *mainwin;

struct handler {
  uiAreaHandler ah;
  uiImageBuffer *img;
  unsigned char *binaryData;
  int width;
  int height;
};

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

typedef enum { PREPARE, DOWNLOADING, COMBINING, FINALIZING, PROGRESS, LOGGING, FINISH } Flags;

typedef struct {
  VodOptions *vodOptions;
  int i;
  char *buf;
  Flags flag;
} uiData;