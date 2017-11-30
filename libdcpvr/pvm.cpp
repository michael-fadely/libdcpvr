#include "stdafx.h"

#include <cstdint>
#include <fstream>
#include <vector>

#include "pvm.h"
#include "streamext.h"

/*
 * 00 .. 03 PVMH
 * 04 .. 07 data offset
 * 08 .. 09 attributes
 * 0A .. 0B entry count
 */

// ReSharper disable once CppDeclaratorNeverUsed
static constexpr uint32_t MAX_GBIX   = 0xFFFFFFEF;
static constexpr uint32_t PVM_FOURCC = 'HMVP';

PVMReader::PVMReader(const std::string& path)
	: IPVMArchive(),
	  FileReader<PVMError>(path)
{
	header  = {};
	entries = nullptr;
	PVMReader::check();
}

PVMReader::PVMReader(PVMReader&& other) noexcept
	: IPVMArchive(other),
	  FileReader<PVMError>(std::move(other))
{
	entries_ = move(other.entries_);
	entries  = entries_.data();
}

PVMReader::PVMReader(std::ifstream& stream, size_t size, bool owner)
	: IPVMArchive(),
	  FileReader<PVMError>(stream, size, owner)
{
	header  = {};
	entries = nullptr;
	PVMReader::check();
}

PVMReader::~PVMReader()
{
	PVMReader::close();
}

PVMError PVMReader::get_header()
{
	if (!stream.is_open())
	{
		return PVM_FILE_OPEN_FAIL;
	}

	uint32_t fourcc = 0;
	read_t(stream, fourcc);

	if (fourcc != PVM_FOURCC)
	{
		return PVM_FILE_NOT_PVM;
	}

	read_t(stream, header.data_offset);
	read_t(stream, header.attributes);
	read_t(stream, header.entry_count);
	header.data_offset += 8;

	for (int i = 0; i < header.entry_count; i++)
	{
		PVMEntry entry {};

		read_t(stream, entry.index);

		if (header.attributes & PVM_ATTR_NAME)
		{
			stream.read(entry.name, sizeof(entry.name));
		}

		if (header.attributes & PVM_ATTR_FORMAT)
		{
			read_t(stream, entry.format);
		}

		if (header.attributes & PVM_ATTR_DIMENSIONS)
		{
			read_t(stream, entry.dimensions);
		}

		if (header.attributes & PVM_ATTR_GBIX)
		{
			read_t(stream, entry.gbix);
		}

		if (stream.eof())
		{
			break;
		}

		entries_.push_back(entry);
	}

	stream.seekg(0);

	if (entries_.size() != header.entry_count)
	{
		return PVM_EARLY_EOF;
	}

	entries = entries_.data();
	return PVM_OK;
}

void PVMReader::check()
{
	const auto result = get_header();

	if (result != PVM_OK)
	{
		error_ = result;
		close();
	}
	else
	{
		is_open_ = true;
	}
}

void PVMReader::close()
{
	FileReader<PVMError>::close();
	entries_.clear();
}

const std::vector<PVMEntry>& PVMReader::get_entries() const
{
	return entries_;
}
