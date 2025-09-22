#ifndef UTILS_H
#define UTILS_H
#define _GNU_SOURCE
#include <ctype.h>
#include <curl/curl.h>
#include <locale.h>
#include <math.h>
#include <pthread.h>
#include <regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#include "../libs/base64.h"
#include "../libs/cJSON.h"
#include "../libs/cJSON_Utils.h"
#include "../libs/stb_image.h"
#include "../libs/ui.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define DEFAULT_PATH "TwitchDownloaderCLI"

typedef enum { PREPARE, DOWNLOADING, VERIFYING, COMBINING, FINALIZING, PROGRESS, LOGGING, FINISH, STATUS } Flags;

enum { GTK_ORIENTATION_HORIZONTAL, GTK_ORIENTATION_VERTICAL };

struct handler {
	uiAreaHandler ah;
	uiImageBuffer *img;
	unsigned char *binaryData;
	int width;
	int height;
};

typedef struct {
	unsigned char *memory;
	size_t used;
	size_t size;
} string;

typedef struct {
	string *qualities;
	size_t count;
	size_t capacity;
} QualityList;

void QualityList_init(QualityList *list, size_t initial_capacity);
void QualityList_add(QualityList *list, const char *quality_start, size_t quality_len);
void QualityList_destroy(QualityList *list);

typedef struct {
	int i;
	char *buf;
	Flags flag;
} uiData;

extern uiWindow *mainwin;
extern char *configFile;
extern cJSON *configJson;

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
FILE *mypopen(const char *cmd, pid_t *pid);
int mypclose(FILE *f, pid_t pid);
string *requestImage(char *link);
string *performGQLRequest(char *req);
string *performUsherRequest(char *req);
string *getClipQualities(const char *id);
string *getClipInfo(const char *id);
string *getVodInfo(const char *id);
string *getVodAcessToken(const char *id);
QualityList getVodQualities(const char *id, const char *token, const char *sig);
char *mygets(char *buf, int n, FILE *f);
char *getLocalTime(char *utcTime);
void concat(string *str, int count, ...);
cJSON *getJson(cJSON *obj, char *name);
void setJson(cJSON *obj, char *name, cJSON *item);
char *getBinaryPath(void);

#endif
