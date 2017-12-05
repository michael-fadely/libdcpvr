#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "pvr.h"
#include "FileReader.h"

// TODO: exceptions instead
enum PVM_ERROR
{
	PVM_OK,
	PVM_FILE_NOT_PVM,
	PVM_FILE_OPEN_FAIL,
	PVM_EARLY_EOF,
	PVM_INPUT_NULL,
	PVM_OUTPUT_NULL
};

// TODO: the other 8 bits
enum PVM_ATTR : uint16_t
{
	PVM_ATTR_GBIX       = 1u << 0u,
	PVM_ATTR_DIMENSIONS = 1u << 1u,
	PVM_ATTR_FORMAT     = 1u << 2u,
	PVM_ATTR_NAME       = 1u << 3u
};

struct PVMHeader
{
	uint32_t data_offset;
	PVM_ATTR attributes;
	uint16_t entry_count;
};

struct PVMEntry
{
	uint16_t index;
	char name[28];

	union
	{
		struct
		{
			PVR_PIXEL_FORMAT pixel_format;
			PVR_DATA_FORMAT data_format;
		};

		uint16_t format;
	};

	uint16_t dimensions;
	uint32_t gbix;
};

struct IPVMArchive
{
	PVMHeader header;
	PVMEntry* entries;
};

class PVMReader : public IPVMArchive, public FileReader<PVM_ERROR>
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
	PVM_ERROR get_header();
};
