#pragma once

#include "Core/CoreParameter.h"

inline CoreParameter &PSP_CoreParameter() {
    static CoreParameter param;
    return param;
}
