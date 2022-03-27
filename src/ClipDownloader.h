#include "utils.h"
#include "../libs/stb_image.h"

#define commandTemplateLength 64

extern uiWindow *mainwin;

struct handler {
  uiAreaHandler ah;
  uiImageBuffer *img;
  unsigned char *binaryData;
  int width;
  int height;
};

char qualityArray[5][8];

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
  char *cmd;
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