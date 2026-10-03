#include "foxhollow_mod_api.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/* The original screen runs 0x3C frames before card access starts. The FoxHollow
   feature/fast-memory-card-loading branch runs it once; this matches that. */
#define LOADING_MSG_FRAMES 1

typedef struct GameObject GameObject;
typedef struct Texture Texture;

/* Leading slots of the game's ScreenTransitionInterface (main/screen_transition.h). */
typedef struct ScreenTransitionInterface {
  void* unused00;
  void (*init)(int transitionId, int value, int flags);
} ScreenTransitionInterface;

typedef struct Symbol {
  const char* name;
  void** address;
} Symbol;

/* Everything cardShowLoadingMsg touches, with the prototypes from the game headers. */
static struct {
  void (*gameTextSetWindow)(uint8_t* textBox);
  void (*padUpdate)(void);
  void (*mmFreeTick)(int arg);
  void (*waitNextFrame)(void);
  int (*getButtonObjects)(GameObject*** objectsOut);
  void (*drawRect)(float sx, float sy, int x, int y);
  void (*objRenderModelAndHitVolumes)(GameObject* obj, int p2, int p3, int p4, int p5, float scale);
  void (*curUiDllDraw)(int a, int b, int c, int d);
  void (*hudDrawColored)(Texture* texture, int x, int y, uint32_t* color, int scale, int flags);
  Texture* (*getReflectionTexture1)(void);
  void (*gameTextSetColor)(int r, int g, int b, int a);
  void (*gameTextShowAt)(int a, int b, int c);
  void (*gameTextRun)(void);
  int (*GXFlush_)(uint8_t visible, int unused);
  ScreenTransitionInterface*** screenTransitionInterface;
  int* saveCardBackdropColor;
} game;

static const Symbol kSymbols[] = {
    {"gameTextSetWindow", (void**)&game.gameTextSetWindow},
    {"padUpdate", (void**)&game.padUpdate},
    {"mmFreeTick", (void**)&game.mmFreeTick},
    {"waitNextFrame", (void**)&game.waitNextFrame},
    {"getButtonObjects", (void**)&game.getButtonObjects},
    {"drawRect", (void**)&game.drawRect},
    {"objRenderModelAndHitVolumes", (void**)&game.objRenderModelAndHitVolumes},
    {"curUiDllDraw", (void**)&game.curUiDllDraw},
    {"hudDrawColored", (void**)&game.hudDrawColored},
    {"getReflectionTexture1", (void**)&game.getReflectionTexture1},
    {"gameTextSetColor", (void**)&game.gameTextSetColor},
    {"gameTextShowAt", (void**)&game.gameTextShowAt},
    {"gameTextRun", (void**)&game.gameTextRun},
    {"GXFlush_", (void**)&game.GXFlush_},
    {"gScreenTransitionInterface", (void**)&game.screenTransitionInterface},
    {"gSaveCardBackdropColor", (void**)&game.saveCardBackdropColor},
};

#define COUNT_OF(array) ((int)(sizeof(array) / sizeof((array)[0])))

static const FhModHost* H;
static FhMod* M;
static void* sHookTarget;

static void modLog(FhLogLevel level, const char* format, ...) {
  char message[256];
  int prefix;
  va_list args;

  if (!H || !H->log || !M) return;
  prefix = snprintf(message, sizeof(message), "[Fast Memory Card Loading] ");
  va_start(args, format);
  vsnprintf(message + prefix, sizeof(message) - (size_t)prefix, format, args);
  va_end(args);
  H->log(M, level, message);
}

/* cardShowLoadingMsg from game/src/track/intersect_memcard.c, statement for
   statement, with only the frame count changed. Callers (_saveGame,
   maybeTryLoadSave, loadSaveGame, cardCreateSaveFile) still show the screen and
   then perform the card access themselves; nothing here touches card state. */
static void hookCardShowLoadingMsg(uint8_t kind) {
  GameObject** buttons;
  uint32_t saved;
  int frame;
  int j;
  int count;
  float rectAlpha;
  void (*draw)(int, int, int);
  uint8_t mode = kind;

  game.gameTextSetWindow(NULL);
  for (frame = 0; frame < LOADING_MSG_FRAMES; frame++) {
    game.padUpdate();
    game.mmFreeTick(0);
    game.waitNextFrame();
    count = game.getButtonObjects(&buttons) & 0xFF;
    if ((uint32_t)count != 0) {
      draw = (**game.screenTransitionInterface)->init;
      draw(0, 0, 0);
      rectAlpha = 0.0f;
      game.drawRect(rectAlpha, rectAlpha, 0x280, 0x1E0);
      for (j = 0; j < count; j++) {
        game.objRenderModelAndHitVolumes(buttons[j], 0, 0, 0, 0, 1.0f);
      }
      game.curUiDllDraw(0, 0, 0, 0);
    } else {
      saved = (uint32_t)*game.saveCardBackdropColor;
      game.hudDrawColored(game.getReflectionTexture1(), 0, 0, &saved, 0x200, 0);
    }
    game.gameTextSetColor(0xFF, 0xFF, 0xFF, 0xFF);
    if (mode == 1) {
      game.gameTextShowAt(0x323, 0, 0xC8);
    } else if (mode == 2) {
      game.gameTextShowAt(0x573, 0, 0xC8);
    } else {
      game.gameTextShowAt(0x56C, 0, 0xC8);
    }
    game.gameTextRun();
    game.GXFlush_(1, 0);
  }
}

static void reset_state(void) {
  int i;

  for (i = 0; i < COUNT_OF(kSymbols); i++) {
    *kSymbols[i].address = NULL;
  }
  sHookTarget = NULL;
  H = NULL;
  M = NULL;
}

/* Resolves everything before patching anything, so a failure leaves the game untouched. */
static int install(FhMod* mod, const FhModHost* host) {
  void* target;
  int i;

  for (i = 0; i < COUNT_OF(kSymbols); i++) {
    *kSymbols[i].address = host->symbolAddress(mod, kSymbols[i].name);
    if (*kSymbols[i].address == NULL) {
      modLog(FH_LOG_ERROR, "disabled: could not resolve %s", kSymbols[i].name);
      return 0;
    }
  }
  target = host->symbolAddress(mod, "cardShowLoadingMsg");
  if (target == NULL) {
    modLog(FH_LOG_ERROR, "disabled: could not resolve cardShowLoadingMsg");
    return 0;
  }
  if (host->hookInstall(mod, target, (void*)hookCardShowLoadingMsg, NULL) != FH_MOD_OK) {
    modLog(FH_LOG_ERROR, "disabled: could not hook cardShowLoadingMsg");
    return 0;
  }
  sHookTarget = target;
  return 1;
}

FH_MOD_EXPORT int fh_mod_initialize(FhMod* mod, const FhModHost* host) {
  if (!host || host->abiVersion != FH_MOD_ABI_VERSION || host->structSize < sizeof(FhModHost)) return FH_MOD_ERROR;
  if (!host->log || !host->symbolAddress || !host->hookInstall || !host->hookRemove) return FH_MOD_ERROR;
  H = host;
  M = mod;
  if (!install(mod, host)) {
    reset_state();
    return FH_MOD_ERROR;
  }
  modLog(FH_LOG_INFO, "v1.0.0 loaded");
  return FH_MOD_OK;
}

FH_MOD_EXPORT void fh_mod_shutdown(FhMod* mod) {
  (void)mod;
  if (H && M && sHookTarget) H->hookRemove(M, sHookTarget);
  reset_state();
}
