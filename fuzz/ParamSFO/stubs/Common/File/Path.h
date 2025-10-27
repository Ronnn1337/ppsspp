#pragma once

#include <string>
#include <string_view>

enum class PathType {
    UNDEFINED = 0,
    NATIVE = 1,
};

class Path {
public:
    Path() = default;
    explicit Path(std::string_view str) : path_(str) {}

    bool empty() const { return path_.empty(); }
    size_t size() const { return path_.size(); }

    const std::string &ToString() const { return path_; }
    const char *c_str() const { return path_.c_str(); }

    std::string GetFilename() const {
        if (path_.empty())
            return std::string();
        size_t pos = path_.find_last_of("/\\");
        if (pos == std::string::npos || pos + 1 >= path_.size())
            return path_;
        return path_.substr(pos + 1);
    }

	std::string GetFileExtension() const {
		std::string filename = GetFilename();
		size_t pos = filename.find_last_of('.');
		if (pos == std::string::npos)
			return std::string();
		return filename.substr(pos);
	}

	std::string ToVisualString(const char * = nullptr) const {
		return path_;
	}

private:
	std::string path_;
};
