#ifdef FUZZING_BUILD
#include "Core/Config.h"
#include "Core/MemMap.h"
#include "Core/HLE/HLE.h"
#include "Core/HLE/sceKernel.h"
#include "Core/CoreTiming.h"

extern Config g_Config;

extern "C" void Memory_EnableFastmem(bool)        __attribute__((weak));
extern "C" void Memory_SetAllowFastmem(bool)      __attribute__((weak));
extern "C" void MIPS_SetFastMemory(bool)          __attribute__((weak));
extern "C" bool Memory_IsValidAddress(u32)        __attribute__((weak));
extern "C" void Memory_Write_U8(u8, u32)          __attribute__((weak));

extern "C" void FUZZ_SystemInitHeadless() {
    // Model & RAM size (match System.cpp behavior)
    if (g_Config.iPSPModel == PSP_MODEL_FAT) {
        Memory::g_MemorySize = Memory::RAM_NORMAL_SIZE;   // 32 MB
    } else {
        Memory::g_MemorySize = Memory::RAM_DOUBLE_SIZE;   // 64 MB
    }
    Memory::g_PSPModel = g_Config.iPSPModel;

    // Turn every fastmem switch OFF before Memory::Init().
    g_Config.bFastMemory = false;
    if (Memory_EnableFastmem)   Memory_EnableFastmem(false);
    if (Memory_SetAllowFastmem) Memory_SetAllowFastmem(false);
    if (MIPS_SetFastMemory)     MIPS_SetFastMemory(false);

    // Map PSP address space
    if (!Memory::Init()) {
        return;  // mapping failed; skip this iteration
    }

    // Optional: sanity check that RAM base (0x08800000) is mapped.
    if (Memory_IsValidAddress && !Memory_IsValidAddress(0x08800000)) {
        return;  // mapping didn't take; avoid ASan crash
    }
    if (Memory_Write_U8) Memory_Write_U8(0xAA, 0x08800000);

    CoreTiming::Init();
    HLEInit();

    // Only now is it safe to touch RAM en masse
    __KernelInit();
}
#endif
