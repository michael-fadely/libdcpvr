#pragma once

#include <fstream>

template <typename error_t>
class FileReader
{
protected:
	using pos_t = std::ifstream::pos_type;

	std::ifstream stream;
	error_t _error; // TODO: exceptions instead
	pos_t _start;
	pos_t _size;

	bool _is_open = false;

public:
	explicit FileReader(const std::string& path);
	explicit FileReader(std::ifstream& stream, size_t size, bool owner = false);

protected:
	~FileReader();

public:
	const bool is_owner;

	bool is_open() const
	{
		return _is_open;
	}

	error_t error() const
	{
		return _error;
	}

	size_t size() const
	{
		return static_cast<size_t>(_size);
	}

	virtual void close();

private:
	virtual void check() = 0;
};

template <typename error_t>
FileReader<error_t>::FileReader(const std::string& path)
	: stream(path, std::ios::ate | std::ios::binary), _size(stream.tellg()), is_owner(true)
{
	stream.seekg(0);
}

template <typename error_t>
FileReader<error_t>::FileReader(std::ifstream& stream, size_t size, bool owner)
	: stream(move(stream)), _start(stream.tellg()), _size(size), is_owner(owner)
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
	_is_open = false;
	_size = 0;
	_start = 0;
	if (is_owner)
	{
		stream.close();
	}
}
