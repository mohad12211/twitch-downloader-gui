#include "../libs/ui.h"
#define VERSION "1.0"

typedef struct {
  char *configDirectory;
  char *configFile;
} ConfigData;

uiWindow *mainwin;
char *binaryPath;

uiControl *VodDownloaderDrawUi(void);
uiControl *ClipDownloaderDrawUi(void);
uiControl *ChatDownloaderDrawUi(void);
uiControl *ChatRenderDrawUi(void);