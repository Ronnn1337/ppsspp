#include "Common/Log.h"
#include "Common/LogReporting.h"

namespace {
bool g_logEnabled = false;
}

bool *g_bLogEnabledSetting = &g_logEnabled;
LogChannel g_log[(size_t)Log::NUMBER_OF_LOGS];
u8 g_debugCounters[8] = {};

void GenericLog(Log, LogLevel, const char *, int, const char *, ...) {}

bool HandleAssert(const char *, const char *, int, const char *, const char *, ...) {
    return false;
}

bool HitAnyAsserts() {
    return false;
}

void ResetHitAnyAsserts() {}

void SetExtraAssertInfo(const char *) {}

void SetDebugValue(DebugCounter counter, int value) {
    if (counter >= DebugCounter::APP_BOOT && counter <= DebugCounter::CPUCORE_SWITCHES)
        g_debugCounters[(int)counter] = (u8)value;
}

void IncrementDebugCounter(DebugCounter counter) {
    SetDebugValue(counter, g_debugCounters[(int)counter] + 1);
}

void SetAssertCancelCallback(AssertNoCallbackFunc, void *) {}

void SetCleanExitOnAssert() {}

void BreakIntoPSPDebugger(const char *) {}

void SetAssertDialogParent(void *) {}

namespace Reporting {
void ReportMessage(const char *, ...) {}

bool ShouldLogNTimes(const char *, int) {
    return true;
}
}
