#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "Core/ELF/ElfReader.h"
#include "Core/MemMap.h"
#include "Core/HLE/ErrorCodes.h"

namespace {

constexpr size_t kBackingMemorySize = 0x10000000;  // Enough for RAM/VRAM windows we touch.

uint8_t *EnsureMemoryArena() {
    static std::unique_ptr<uint8_t[]> arena;
    if (!arena) {
        arena.reset(new uint8_t[kBackingMemorySize]);
        std::fill_n(arena.get(), kBackingMemorySize, 0);
    }
    Memory::base = arena.get();
    Memory::g_MemorySize = Memory::RAM_NORMAL_SIZE;
    return arena.get();
}

bool HeaderLooksElf(const uint8_t *data, size_t size) {
    if (size < sizeof(Elf32_Ehdr))
        return false;

    const Elf32_Ehdr *hdr = reinterpret_cast<const Elf32_Ehdr *>(data);
    if (hdr->e_ident[0] != ELFMAG0 || hdr->e_ident[1] != ELFMAG1 ||
        hdr->e_ident[2] != ELFMAG2 || hdr->e_ident[3] != ELFMAG3)
        return false;

    if (hdr->e_ident[EI_CLASS] != ELFCLASS32)
        return false;

    if (hdr->e_ident[EI_DATA] != ELFDATA2LSB)
        return false;

    return true;
}

}  // namespace

static bool ReadFile(const char *path, std::vector<uint8_t> &out) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.good())
        return false;
    file.unsetf(std::ios::skipws);
    out.insert(out.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
    return true;
}

static void ExerciseElf(const uint8_t *data, size_t size) {
    if (size < sizeof(Elf32_Ehdr) || size > (1 << 26))
        return;

    if (!HeaderLooksElf(data, size))
        return;

    EnsureMemoryArena();

    std::vector<uint8_t> buffer(data, data + size);
    ElfReader reader(buffer.data(), buffer.size());

    int loadResult = reader.LoadInto(0, /*fromTop=*/false);
    if (loadResult != SCE_KERNEL_ERROR_OK)
        return;

    reader.LoadSymbols();

    const int sections = reader.GetNumSections();
    for (int i = 0; i < sections; ++i) {
        reader.GetSectionName(i);
        reader.GetSectionDataOffset(i);
        reader.GetSectionDataPtr(i);
        reader.GetSectionSize(i);
        reader.GetSectionAddr(i);
    }

    const int segments = reader.GetNumSegments();
    for (int i = 0; i < segments; ++i) {
        reader.GetSegmentPtr(i);
        reader.GetSegmentDataSize(i);
        reader.GetSegmentMemSize(i);
        reader.GetSegmentVaddr(i);
        reader.GetSegmentOffset(i);
    }

    reader.GetEntryPoint();
    reader.GetFlags();
    reader.GetType();
    reader.GetMachine();
    reader.GetTotalTextSize();
    reader.GetTotalDataSize();
}

int main(int argc, char **argv) {
    std::vector<uint8_t> input;
    if (argc > 1) {
        if (!ReadFile(argv[1], input))
            return 0;
    } else {
        std::vector<uint8_t> buf(4096);
        while (true) {
            size_t bytes = fread(buf.data(), 1, buf.size(), stdin);
            if (bytes == 0)
                break;
            input.insert(input.end(), buf.begin(), buf.begin() + bytes);
        }
    }

    ExerciseElf(input.data(), input.size());
    return 0;
}
