#pragma once

#include <fstream>

// TODO: not this

class FileReader
{
protected:
	using pos_t = std::ifstream::pos_type;

	std::ifstream stream;
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
	bool is_open() const;
	size_t size() const;
	virtual void close();

private:
	virtual void check() = 0;
};
