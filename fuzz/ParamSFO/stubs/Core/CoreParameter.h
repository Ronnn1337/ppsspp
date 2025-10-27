#pragma once

#include "Common/File/Path.h"

enum class GPUCore : int {
    GPUCORE_GLES = 0,
};

enum class FPSLimit {
    NORMAL = 0,
};

struct CoreParameter {
    Path fileToStart;
};
