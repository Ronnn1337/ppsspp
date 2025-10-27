#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "Core/Loaders.h"
#include "Core/FileSystems/BlockDevices.h"
#include "Common/File/Path.h"

class VectorFileLoader : public FileLoader {
public:
	explicit VectorFileLoader(std::vector<uint8_t> data) : data_(std::move(data)) {}

	bool Exists() override { return true; }
	bool IsDirectory() override { return false; }
	s64 FileSize() override { return static_cast<s64>(data_.size()); }
	Path GetPath() const override { return Path(path_); }

		size_t ReadAt(s64 absolutePos, size_t bytes, size_t count, void *dst, FileLoader::Flags) override {
			uint8_t *out = static_cast<uint8_t *>(dst);
			size_t total = 0;
			for (size_t i = 0; i < count; ++i) {
				size_t read = ReadAt(absolutePos + bytes * i, bytes, out + bytes * i);
				if (read != bytes)
					break;
				total += read;
			}
			return total;
	}

	size_t ReadAt(s64 absolutePos, size_t bytes, void *dst, FileLoader::Flags = FileLoader::Flags::NONE) override {
		if (absolutePos < 0 || static_cast<size_t>(absolutePos) >= data_.size())
			return 0;
		size_t pos = static_cast<size_t>(absolutePos);
		size_t available = data_.size() - pos;
		size_t toCopy = bytes < available ? bytes : available;
		std::memcpy(dst, data_.data() + pos, toCopy);
		return toCopy;
	}

private:
	std::vector<uint8_t> data_;
	std::string path_ = "fuzz.cso";
};

static bool ReadInput(const char *path, std::vector<uint8_t> &out) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.good())
		return false;
	file.unsetf(std::ios::skipws);
	out.insert(out.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
	return true;
}

static void ExerciseCISO(const uint8_t *data, size_t size) {
	if (size == 0 || size > (1 << 26))
		return;

	std::vector<uint8_t> buffer(data, data + size);
	VectorFileLoader loader(std::move(buffer));

	std::unique_ptr<CISOFileBlockDevice> device(new CISOFileBlockDevice(&loader));
	if (!device->IsOK())
		return;

	u8 block[2048]{};
	device->ReadBlock(0, block, false);
	device->ReadBlocks(0, 2, block);
}

int main(int argc, char **argv) {
	std::vector<uint8_t> input;
	if (argc > 1) {
		if (!ReadInput(argv[1], input))
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

	ExerciseCISO(input.data(), input.size());
	return 0;
}
