#pragma once

#include <fstream>

template <typename T>
auto& read_t(std::ifstream& f, T& value)
{
	return f.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
auto& write_t(std::ofstream& f, const T& value)
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
