#pragma once

#include <fstream>

template <typename T>
auto& read_t(std::istream& f, T& value)
{
	return f.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
auto& write_t(std::ostream& f, const T& value)
{
	return f.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename T>
auto& read_t(std::fstream& f, T& value)
{
	return f.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
auto& write_t(std::fstream& f, const T& value)
{
	return f.write(reinterpret_cast<const char*>(&value), sizeof(T));
}
