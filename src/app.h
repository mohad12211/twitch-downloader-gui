#include "utils.h"
#define VERSION "1.0"

uiWindow *mainwin;
char *configFile;
cJSON *configJson;

uiControl *VodDownloaderDrawUi(void);
uiControl *ClipDownloaderDrawUi(void);
uiControl *ChatDownloaderDrawUi(void);
uiControl *ChatRenderDrawUi(void);
void ShowPreferences(void);
static void saveConfig(void);
static void loadConfig(void);
static void setupConfigFile(void);
static void initDefaults(void);