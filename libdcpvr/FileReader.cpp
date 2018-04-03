#include "stdafx.h"
#include "FileReader.h"

FileReader::FileReader(const std::string& path)
	: stream(path, std::ios::ate | std::ios::binary), size_(stream.tellg()), is_owner(true)
{
	stream.seekg(0);
}

FileReader::FileReader(FileReader&& other) noexcept
	: is_owner(other.is_owner)
{
	stream = move(other.stream);
	start_ = other.start_;
	size_ = other.size_;
	is_open_ = other.is_open_;
}

FileReader::FileReader(std::ifstream& stream, size_t size, bool owner)
	: stream(move(stream)), start_(stream.tellg()), size_(size), is_owner(owner)
{
}

FileReader::~FileReader()
{
	FileReader::close();
}

bool FileReader::is_open() const
{
	return is_open_;
}

size_t FileReader::size() const
{
	return static_cast<size_t>(size_);
}

void FileReader::close()
{
	is_open_ = false;
	size_ = 0;
	start_ = 0;
	if (is_owner)
	{
		stream.close();
	}
}
