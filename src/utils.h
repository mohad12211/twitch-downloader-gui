#define _GNU_SOURCE
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <regex.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>

#include "../libs/ui.h"
#include "../libs/cJSON.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define json(x, y) cJSON_GetObjectItem(x, y)

typedef struct {
  unsigned char *memory;
  size_t used;
  size_t size;
} string;

enum { GTK_ORIENTATION_HORIZONTAL, GTK_ORIENTATION_VERTICAL };
extern char *binaryPath;
extern uiWindow *mainwin;

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
string *requestImage(char *link);
string *performRequest(char *req);
string *getClipQualities(const char *id);
string *getClipInfo(const char *id);
string *getVodInfo(const char *id);
char *mygets(char *buf, int n, FILE *f);
char *getLocalTime(char *utcTime);
void concat(string *str, int count, ...);