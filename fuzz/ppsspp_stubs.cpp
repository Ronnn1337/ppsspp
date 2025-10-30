#include <string>
#include <string_view>
#include <functional>

// Forward-declare simple enums/structs to avoid pulling big headers
enum class UIMessage : int;
struct GraphicsContext;
struct TouchInput;
struct KeyInput;
struct AxisInput;

// --- System_* UI/main-thread stubs (non-inline on many trees) ---
void System_PostUIMessage(UIMessage, std::string_view) {}
void System_RunOnMainThread(std::function<void()> fn) { if (fn) fn(); }

// --- System_* audio hooks (non-inline ones only) ---
void System_AudioClear() {}
void System_AudioPushSamples(const int*, int, float) {}
void System_AudioGetDebugStats(char*, unsigned long) {}

// --- Native* no-ops to avoid linking libnative/SDL ---
void NativeMix(short*, int, int, void*) {}
void Native_UpdateScreenScale(int, int, float) {}
void Native_NotifyWindowHidden(bool) {}
bool Native_IsWindowHidden() { return false; }
void NativeShutdown() {}
void NativeInitGraphics(GraphicsContext*) {}
void NativeShutdownGraphics() {}
void NativeFrame(GraphicsContext*) {}
void NativeMouseDelta(float, float) {}
void NativeTouch(const TouchInput&) {}
void NativeKey(const KeyInput&) {}
void NativeGetAppInfo(std::string*, std::string*, bool*, std::string*) {}
void NativeInit(int, const char**, const char*, const char*, const char*) {}
void NativeAxis(const AxisInput*, unsigned long) {}

// Secrets (RetroAchievements)
std::string NativeLoadSecret(std::string_view) { return {}; }
bool NativeSaveSecret(std::string_view, std::string_view) { return true; }
