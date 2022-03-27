#include "utils.h"

uiControl *ChatRender(void);

extern uiWindow *mainwin;

typedef struct {
  char *name;
  char *defaultName;
  char *filter;
  struct codec {
    char *name;
    char *inputArgs;
    char *outputArgs;
  } supportedCodecs[4];
} container;

container containers[4] = {{"MP4",
                            "chat.mp4",
                            "mp4 File (*.mp4)|*.mp4",
                            {{"H264",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libx264 -preset veryfast -crf 18 -pix_fmt yuv420p "
                              "\"{save_path}\""},
                             {"H265",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libx265 -preset veryfast -crf 18 -pix_fmt yuv420p "
                              "\"{save_path}\""}}},
                           {"MOV",
                            "chat.mov",
                            "mov file (*.mov)|*.mov",
                            {{"H264",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libx264 -preset veryfast -crf 18 -pix_fmt yuv420p "
                              "\"{save_path}\""},
                             {"H265",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libx265 -preset veryfast -crf 18 -pix_fmt yuv420p "
                              "\"{save_path}\""},
                             {"ProRes",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v prores_ks -qscale:v 62 -pix_fmt argb \"{save_path}\""},
                             {"RLE",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v qtrle -pix_fmt argb \"{save_path}\""}}},
                           {"WEBM",
                            "chat.webm",
                            "webm File (*.webm)|*.webm",
                            {{"VP8",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libvpx -crf 18 -b:v 2M -pix_fmt yuva420p -auto-alt-ref 0 "
                              "\"{save_path}\""},
                             {"VP9",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libvpx-vp9 -crf 18 -b:v 2M -pix_fmt yuva420p \"{save_path}\""}}},
                           {"MKV",
                            "chat.mkv",
                            "mkv File (*.mkv)|*.mkv",
                            {{"H264",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libx264 -preset veryfast -crf 18 -pix_fmt yuv420p "
                              "\"{save_path}\""},
                             {"H265",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libx265 -preset veryfast -crf 18 -pix_fmt yuv420p "
                              "\"{save_path}\""},
                             {"VP8",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libvpx -crf 18 -b:v 2M -pix_fmt yuva420p -auto-alt-ref 0 "
                              "\"{save_path}\""},
                             {"VP9",
                              "-framerate {fps} -f rawvideo -analyzeduration {max_int} -probesize "
                              "{max_int} -pix_fmt bgra -video_size {width}x{height} -i -",
                              "-c:v libvpx-vp9 -crf 18 -b:v 2M -pix_fmt yuva420p \"{save_path}\""}}}};

typedef struct ChatRenderOptions {
  uiFontButton *fontOptions;
  uiColorButton *fontColor;
  uiColorButton *backgroundColor;
  uiEntry *filePath;
  uiEntry *frameRate;
  uiEntry *width;
  uiEntry *height;
  uiEntry *updateTime;
  uiEntry *leftPadding;
  uiEntry *outlineSize;
  uiEntry *inputArgs;
  uiEntry *outputArgs;
  uiCheckbox *outlineCheck;
  uiCheckbox *timestampCheck;
  uiCheckbox *FFZEmotesCheck;
  uiCheckbox *BTTVEmotesCheck;
  uiCheckbox *sevenTVEmotesCheck;
  uiCheckbox *subMsgCheck;
  uiCheckbox *chatBadgesCheck;
  uiCheckbox *generateMaskCheck;
  uiCombobox *msgFontStyle;
  uiCombobox *userFontStyle;
  uiCombobox *containersBox;
  uiCombobox *codecs;
  uiMultilineEntry *logsEntry;
  uiProgressBar *pBar;
  uiLabel *status;
  uiButton *renderBtn;
  uiButton *browseBtn;
  string *cmd;
  char *fileName;
} ChatRenderOptions;

typedef enum { STATUS, LOGGING, FINISH } Flags;

typedef struct {
  ChatRenderOptions *renderOptions;
  Flags flag;
  int i;
  char *buf;
} uiData;