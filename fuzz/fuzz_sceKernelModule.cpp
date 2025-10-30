#include <cstdint>
#include <cstddef>
#include <atomic>
#include <cstring>
#include <cstdlib>

#include "Core/Config.h"                  // g_Config
#include "Core/HLE/sceKernelModule.h"     // PSPModule

extern Config g_Config;

// From the Core bridge above
extern "C" void FUZZ_SystemInitHeadless();

// Your LoadELF bridge compiled into Core/HLE/sceKernelModule.cpp with -DFUZZING_BUILD
extern "C" PSPModule *FUZZ_Bridge_LoadELFFromPtr(const unsigned char *ptr, size_t elfSize);

static std::atomic<bool> g_once{false};

static inline void init_once() {
    bool exp = false;
    if (!g_once.compare_exchange_strong(exp, true))
        return;

    // Pick a 64-MB model like System.cpp would when ELF/non-FAT.
    // PSP_MODEL_FAT gives 32 MB; anything else maps to 64 MB in our bridge.
    g_Config.iPSPModel = 1;              // non-FAT (e.g., 2000/3000)
    g_Config.bFastMemory = false;
    g_Config.bIgnoreBadMemAccess = true;

    FUZZ_SystemInitHeadless();           // sets RAM size, maps memory, HLEInit, __KernelInit
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (!data || size == 0) return 0;
    init_once();

    void *buf = std::malloc(size + 1);
    if (!buf) return 0;
    std::memcpy(buf, data, size);
    static_cast<unsigned char*>(buf)[size] = 0;

    (void)FUZZ_Bridge_LoadELFFromPtr(static_cast<unsigned char*>(buf), size);
    std::free(buf);
    return 0;
}
