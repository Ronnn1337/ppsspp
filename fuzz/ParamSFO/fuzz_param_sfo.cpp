#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "Core/ELF/ParamSFO.h"

static bool ReadFileToVector(const char *path, std::vector<uint8_t> &out) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.good())
        return false;
    file.unsetf(std::ios::skipws);
    out.insert(out.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
    return true;
}

static void ExerciseParamSFO(const uint8_t *data, size_t size) {
    if (size == 0 || size > (1 << 20))
        return;

    ParamSFOData sfo;
    if (!sfo.ReadSFO(data, size))
        return;

    const auto keys = sfo.GetKeys();
    for (const auto &key : keys) {
        sfo.GetValueString(key);
        unsigned int blobSize = 0;
        sfo.GetValueData(key, &blobSize);
        sfo.GetValueInt(key);
    }

    if (sfo.HasKey("DISC_ID")) {
        std::string discId = sfo.GetValueString("DISC_ID");
        DetectGameRegionFromID(discId);
    }

    bool canWrite = true;
    for (const auto &entry : sfo.Values()) {
        const auto &value = entry.second;
        switch (value.type) {
        case ParamSFOData::VT_INT:
            if (value.max_size < 4)
                canWrite = false;
            break;
        case ParamSFOData::VT_UTF8:
            if (value.max_size < static_cast<int>(value.s_value.size()) + 1)
                canWrite = false;
            break;
        case ParamSFOData::VT_UTF8_SPE:
            if (value.max_size < static_cast<int>(value.u_size))
                canWrite = false;
            break;
        }
        if (!canWrite)
            break;
    }

    if (canWrite && sfo.IsValid()) {
        u8 *buffer = nullptr;
        size_t bufferSize = 0;
        sfo.WriteSFO(&buffer, &bufferSize);
        delete[] buffer;
    }
}

int main(int argc, char **argv) {
    std::vector<uint8_t> input;

    if (argc > 1) {
        if (!ReadFileToVector(argv[1], input))
            return 0;
    } else {
        std::vector<uint8_t> buffer(4096);
        while (true) {
            size_t bytes = fread(buffer.data(), 1, buffer.size(), stdin);
            if (bytes == 0)
                break;
            input.insert(input.end(), buffer.begin(), buffer.begin() + bytes);
        }
    }

    ExerciseParamSFO(input.data(), input.size());
    return 0;
}
