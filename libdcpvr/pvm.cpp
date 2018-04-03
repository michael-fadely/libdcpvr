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

static constexpr uint32_t MAX_GBIX   = 0xFFFFFFEF;
static constexpr uint32_t PVM_FOURCC = 'HMVP';

PVMReader::PVMReader(const std::string& path)
	: IPVMArchive(),
	  FileReader(path)
{
	header  = {};
	entries = nullptr;
	PVMReader::check();
}

PVMReader::PVMReader(PVMReader&& other) noexcept
	: IPVMArchive(other),
	  FileReader(std::move(other))
{
	entries_ = move(other.entries_);
	entries  = entries_.data();
}

PVMReader::PVMReader(std::ifstream& stream, size_t size, bool owner)
	: IPVMArchive(),
	  FileReader(stream, size, owner)
{
	header  = {};
	entries = nullptr;
	PVMReader::check();
}

PVMReader::~PVMReader()
{
	PVMReader::close();
}

void PVMReader::get_header()
{
	if (!stream.is_open())
	{
		throw PVMFileOpenFail();
	}

	uint32_t fourcc = 0;
	read_t(stream, fourcc);

	if (fourcc != PVM_FOURCC)
	{
		throw FileNotPVM();
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
		throw PVMEarlyEOF();
	}

	entries = entries_.data();
}

void PVMReader::check()
{
	get_header();
	is_open_ = true;
}

void PVMReader::close()
{
	FileReader::close();
	entries_.clear();
}

const std::vector<PVMEntry>& PVMReader::get_entries() const
{
	return entries_;
}
