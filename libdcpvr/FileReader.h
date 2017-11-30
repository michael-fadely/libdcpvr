#pragma once

#include <fstream>

template <typename error_t>
class FileReader
{
protected:
	using pos_t = std::ifstream::pos_type;

	std::ifstream stream;
	error_t error_; // TODO: exceptions instead
	pos_t start_;
	pos_t size_;

	bool is_open_ = false;

public:
	explicit FileReader(const std::string& path);
	FileReader(FileReader&& other) noexcept;
	FileReader(std::ifstream& stream, size_t size, bool owner = false);

protected:
	~FileReader();

public:
	const bool is_owner;

	bool is_open() const
	{
		return is_open_;
	}

	// TODO: exceptions instead
	error_t error() const
	{
		return error_;
	}

	size_t size() const
	{
		return static_cast<size_t>(size_);
	}

	virtual void close();

private:
	virtual void check() = 0;
};

template <typename error_t>
FileReader<error_t>::FileReader(const std::string& path)
	: stream(path, std::ios::ate | std::ios::binary), size_(stream.tellg()), is_owner(true)
{
	stream.seekg(0);
}

template <typename error_t>
FileReader<error_t>::FileReader(FileReader&& other) noexcept
	: is_owner(other.is_owner)
{
	stream = move(other.stream);
	error_ = other.error_;
	start_ = other.start_;
	size_ = other.size_;
	is_open_ = other.is_open_;
}

template <typename error_t>
FileReader<error_t>::FileReader(std::ifstream& stream, size_t size, bool owner)
	: stream(move(stream)), start_(stream.tellg()), size_(size), is_owner(owner)
{
}

template <typename error_t>
FileReader<error_t>::~FileReader()
{
	FileReader<error_t>::close();
}

template <typename error_t>
void FileReader<error_t>::close()
{
	is_open_ = false;
	size_ = 0;
	start_ = 0;
	if (is_owner)
	{
		stream.close();
	}
}
