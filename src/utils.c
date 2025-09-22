#include "utils.h"

// TODO: add error handling
string *performGQLRequest(char *req) {
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

string *performUsherRequest(char *url) {
	string *chunk = malloc(sizeof(string));
	chunk->memory = malloc(1);
	chunk->used = 0;
	chunk->size = 0;
	CURL *hnd = curl_easy_init();

	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)chunk);

	curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);

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

string *getVodAcessToken(const char *id) {
	char *base = "{\"operationName\":\"PlaybackAccessToken_Template\",\"query\":\"query PlaybackAccessToken_Template($login: String!, $isLive: Boolean!, $vodID: "
							 "ID!, $isVod: Boolean!, $playerType: String!) {  streamPlaybackAccessToken(channelName: $login, params: {platform: \\\"web\\\", playerBackend: "
							 "\\\"mediaplayer\\\", playerType: $playerType}) @include(if: $isLive) {    value    signature    __typename  }  videoPlaybackAccessToken(id: "
							 "$vodID, params: {platform: \\\"web\\\", playerBackend: \\\"mediaplayer\\\", playerType: $playerType}) @include(if: $isVod) {    value    "
							 "signature    __typename  }}\",\"variables\":{\"isLive\":false,\"login\":\"\",\"isVod\":true,\"vodID\":\"%s\",\"playerType\":\"embed\"}}";
	char req[strlen(base) + strlen(id) - 1]; // '%s' counts as 2 more chars, we only need 1 more for null char
	sprintf(req, base, id);
	return performGQLRequest(req);
}

QualityList getVodQualities(const char *id, const char *token, const char *sig) {
	CURL *hnd = curl_easy_init();
	char *escaped_sig = curl_easy_escape(hnd, sig, 0);
	char *escaped_token = curl_easy_escape(hnd, token, 0);
	char *base = "https://usher.ttvnw.net/vod/"
							 "%s.m3u8?sig=%s&token=%s&allow_source=true&allow_audio_only=true&include_unavailable=true&platform=web&player_backend=mediaplayer&playlist_"
							 "include_framerate=true&supported_codecs=av1,h265,h264";
	size_t url_len = strlen(base) + strlen(id) + strlen(escaped_sig) + strlen(escaped_token) - 5; // 3 '%s' count as 6 chars, we only need 1 more for null char
	char url[url_len];
	sprintf(url, base, id, escaped_sig, escaped_token);

	string *chunk = malloc(sizeof(string));
	chunk->memory = malloc(1);
	chunk->used = 0;
	chunk->size = 0;
	printf("Usher URL: %s\n", url);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)chunk);

	curl_easy_perform(hnd);
	curl_free(escaped_sig);
	curl_free(escaped_token);
	curl_easy_cleanup(hnd);

	// parse m3u8
	const char *MEDIA_TAG = "#EXT-X-STREAM-INF";
	const size_t MEDIA_TAG_LEN = strlen(MEDIA_TAG);
	const char *NAME_ATTR_TAG = "VIDEO=\"";
	const size_t NAME_ATTR_TAG_LEN = strlen(NAME_ATTR_TAG);

	QualityList list;
	QualityList_init(&list, 10);

	const char *p = (char *)chunk->memory;
	const char *line_end;

	while ((line_end = strchr(p, '\n')) != NULL) {
		size_t line_len = line_end - p;

		if (line_len >= MEDIA_TAG_LEN && strncmp(p, MEDIA_TAG, MEDIA_TAG_LEN) == 0) {
			const char *name_attr = strstr(p, NAME_ATTR_TAG);
			// Ensure name_attr is found and is within the current line
			if (name_attr != NULL && name_attr < p + line_len) {
				const char *quality_start = name_attr + NAME_ATTR_TAG_LEN;
				const char *quality_end = strchr(quality_start, '"');

				// Ensure quality_end is found and is within the current line
				if (quality_end != NULL && quality_end < p + line_len) {
					size_t quality_len = quality_end - quality_start;
					QualityList_add(&list, quality_start, quality_len);
				}
			}
		}
		p = line_end + 1;
	}

	free(chunk->memory);
	free(chunk);
	return list;
}

string *getClipQualities(const char *id) {
	char *base = "[{\"operationName\":\"VideoAccessToken_Clip\",\"variables\":{\"slug\":\"%s\"},\"extensions\":{\"persistedQuery\":{"
							 "\"version\":1,\"sha256Hash\":\"36b89d2507fce29e5ca551df756d27c1cfe079e2609642b4390aa4c35796eb11\"}}}]";
	char req[strlen(base) + strlen(id) - 1]; // '%s' counts as 2 more chars, we only need 1 more for null char
	sprintf(req, base, id);
	return performGQLRequest(req);
}

string *getClipInfo(const char *id) {
	char *base = "{\"query\":\"query{clip(slug:\\\"%s\\\"){title,durationSeconds,createdAt,broadcaster{displayName},thumbnailURL(width:480,height:272),"
							 "videoOffsetSeconds,video{id}}"
							 "}\",\"variables\":{}}";
	char req[strlen(base) + strlen(id) - 1];
	sprintf(req, base, id);
	return performGQLRequest(req);
}

string *getVodInfo(const char *id) {
	char *base =
			"{\"query\":\"query{video(id:\\\"%s\\\"){title,lengthSeconds,createdAt,owner{displayName},thumbnailURLs(height:180,width:320)}}\",\"variables\":{}}";
	char req[strlen(base) + strlen(id) - 1];
	sprintf(req, base, id);
	return performGQLRequest(req);
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

// fgets but retuns after \n OR \r
char *mygets(char *buf, int n, FILE *f) {
	register int c = 0;
	register char *mbuf = buf;

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

cJSON *getJson(cJSON *obj, char *name) { return cJSON_GetObjectItem(obj, name); }

void setJson(cJSON *obj, char *name, cJSON *item) {
	if (getJson(obj, name) == NULL) {
		cJSON_AddItemToObject(obj, name, item);
	} else {
		cJSON_ReplaceItemInObject(obj, name, item);
	}
}

char *getBinaryPath(void) {
	cJSON *binaryJson = getJson(configJson, "binaryPath");
	cJSON *useCustomBinary = getJson(configJson, "useCustomBinary");
	if (binaryJson == NULL || useCustomBinary->valueint == 0 || strlen(binaryJson->valuestring) == 0) {
		return DEFAULT_PATH;
	} else {
		return binaryJson->valuestring;
	}
}

// popen that gives the pid of the process
// returns a FILE that reads both stdout and stderr
FILE *mypopen(const char *cmd, pid_t *pid_ptr) {
	char *argv[] = {"sh", "-c", (char *)cmd, NULL};
	FILE *f;
	int pdes[2];
	pid_t pid;

	if (pipe(pdes) < 0)
		return NULL;

	switch (pid = fork()) {
	case -1:
		close(pdes[0]);
		close(pdes[1]);
		return NULL;
	case 0:
		if (setsid() == -1)
			return NULL;
		close(pdes[0]);
		dup2(pdes[1], STDOUT_FILENO);
		dup2(pdes[1], STDERR_FILENO);
		close(pdes[1]);
		execvp("sh", argv);
	}
	f = fdopen(pdes[0], "r");
	*pid_ptr = pid;
	close(pdes[1]);
	return f;
}

int mypclose(FILE *f, pid_t pid) {
	int pstat;
	int returned_pid;
	returned_pid = waitpid(pid, &pstat, 0);
	fclose(f);
	return returned_pid == -1 ? -1 : pstat;
}

void QualityList_init(QualityList *list, size_t initial_capacity) {
	list->qualities = malloc(initial_capacity * sizeof(string));
	list->count = 0;
	list->capacity = initial_capacity;
}

void QualityList_add(QualityList *list, const char *quality_start, size_t quality_len) {
	if (list->count >= list->capacity) {
		size_t new_capacity = list->capacity * 2;
		string *new_qualities = realloc(list->qualities, new_capacity * sizeof(string));
		list->qualities = new_qualities;
		list->capacity = new_capacity;
	}

	string *current_quality_string = &list->qualities[list->count];

	current_quality_string->memory = malloc(quality_len + 1);

	memcpy(current_quality_string->memory, quality_start, quality_len);

	current_quality_string->memory[quality_len] = '\0';

	current_quality_string->used = quality_len;
	current_quality_string->size = quality_len + 1; // null terminator

	list->count++;
}

void QualityList_destroy(QualityList *list) {
	for (size_t i = 0; i < list->count; i++) {
		if (list->qualities[i].memory != NULL) {
			free(list->qualities[i].memory);
		}
	}

	if (list->qualities != NULL) {
		free(list->qualities);
	}

	list->qualities = NULL;
	list->count = 0;
	list->capacity = 0;
}
