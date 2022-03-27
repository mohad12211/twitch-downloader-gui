#include "utils.h"

// TODO: add error handling
string *performRequest(char *req) {
  string *chunk = malloc(sizeof(string));
  chunk->memory = malloc(1);
  chunk->used = 0;
  chunk->size = 0;
  CURL *hnd = curl_easy_init();
  struct curl_slist *slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type: application/json");
  slist1 = curl_slist_append(slist1, "Client-ID: kimne78kx3ncx6brgo4mv6wki5h1ko");

  curl_easy_setopt(hnd, CURLOPT_URL, "https://gql.twitch.tv/gql");
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, req);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)chunk);

  curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);
  curl_slist_free_all(slist1);
  return chunk;
}

string *requestImage(char *link) {
  string *chunk = malloc(sizeof(string));
  chunk->memory = malloc(1);
  chunk->used = 0;
  chunk->size = 0;
  CURL *hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, link);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)chunk);

  curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);

  return chunk;
}

string *getClipQualities(const char *id) {
  char *base = "[{\"operationName\":\"VideoAccessToken_Clip\",\"variables\":{\"slug\":\"%s\"},\"extensions\":{\"persistedQuery\":{"
               "\"version\":1,\"sha256Hash\":\"36b89d2507fce29e5ca551df756d27c1cfe079e2609642b4390aa4c35796eb11\"}}}]";
  char req[strlen(base) + strlen(id) - 1]; // '%s' counts as 2 more chars, we only need 1 more for null char
  sprintf(req, base, id);
  return performRequest(req);
}

string *getClipInfo(const char *id) {
  char *base =
      "{\"query\":\"query{clip(slug:\\\"%s\\\"){title,durationSeconds,createdAt,broadcaster{displayName},thumbnailURL(width:480,height:272),videoOffsetSeconds,video{id}}}\",\"variables\":{}}";
  char req[strlen(base) + strlen(id) - 1];
  sprintf(req, base, id);
  return performRequest(req);
}

string *getVodInfo(const char *id) {
  char *base = "{\"query\":\"query{video(id:\\\"%s\\\"){title,lengthSeconds,createdAt,owner{displayName},thumbnailURLs(height:180,width:320)}}\",\"variables\":{}}";
  char req[strlen(base) + strlen(id) - 1];
  sprintf(req, base, id);
  return performRequest(req);
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  string *mem = (string *)userp;

  mem->memory = realloc(mem->memory, mem->used + realsize + 1);
  memcpy(&(mem->memory[mem->used]), contents, realsize);
  mem->used += realsize;
  mem->memory[mem->used] = 0;

  return realsize;
}

char *getLocalTime(char *utcTime) {
  struct tm cal = {0};
  strptime(utcTime, "%Y-%m-%dT%H:%M:%SZ", &cal);
  cal.tm_isdst = -1;
  time_t t = timegm(&cal);
  struct tm localcal = *localtime(&t);
  char res[100];
  strftime(res, 100, "%d/%m/%Y %I:%M:%S %p", &localcal);
  return strdup(res);
}

// fgets but retuns after \n AND \r
char *mygets(char *buf, int n, FILE *f) {
  register int c;
  register char *mbuf;

  mbuf = buf;
  while (--n > 0 && (c = getc(f)) != EOF) {
    if (c == '\n' || c == '\r') {
      *mbuf++ = '\n';
      break;
    }
    *mbuf++ = c;
  }
  *mbuf = '\0';
  return (c == EOF && mbuf == buf) ? NULL : buf;
}

void concat(string *str, int count, ...) {
  va_list ap;
  int len = 0;
  va_start(ap, count);
  for (int i = 0; i < count; i++)
    len += strlen(va_arg(ap, char *));
  va_end(ap);

  if (str->used + len + 1 >= str->size) {
    str->memory = realloc(str->memory, (str->size + len + 100) * sizeof(char));
    str->size += len + 100;
  }
  va_start(ap, count);
  for (int i = 0; i < count; i++) {
    char *s = va_arg(ap, char *);
    strcpy((char *)&str->memory[str->used], s);
    str->used += strlen(s);
  }
  va_end(ap);
}