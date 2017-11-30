#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "FileReader.h"

// TODO: exceptions instead
enum PVMError
{
	PVM_OK,
	PVM_FILE_NOT_PVM,
	PVM_FILE_OPEN_FAIL,
	PVM_EARLY_EOF,
	PVM_INPUT_NULL,
	PVM_OUTPUT_NULL
};

enum PVMAttributes : uint16_t
{
	PVM_ATTR_GBIX       = 1u << 0u,
	PVM_ATTR_DIMENSIONS = 1u << 1u,
	PVM_ATTR_FORMAT     = 1u << 2u,
	PVM_ATTR_NAME       = 1u << 3u
};

struct PVMHeader
{
	uint32_t data_offset;
	uint16_t attributes;
	uint16_t entry_count;
};

struct PVMEntry
{
	uint16_t index;
	char     name[28];
	uint16_t format;
	uint16_t dimensions;
	uint32_t gbix;
};

struct IPVMArchive
{
	PVMHeader header;
	PVMEntry* entries;
};

class PVMReader : public IPVMArchive, public FileReader<PVMError>
{
	std::vector<PVMEntry> entries_;

public:
	explicit PVMReader(const std::string& path);
	PVMReader(PVMReader&& other) noexcept;
	PVMReader(std::ifstream& stream, size_t size, bool owner = false);
	~PVMReader();

	void close() override;
	const std::vector<PVMEntry>& get_entries() const;

private:
	void check() override;
	PVMError get_header();
};
