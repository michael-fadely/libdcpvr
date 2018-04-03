#include "stdafx.h"

#include <algorithm>
#include <cstdint>
#include <fstream>

#include "pvr.h"
#include "streamext.h"

static constexpr uint32_t PVR_FOURCC  = 'TRVP'; // PVRT
static constexpr uint32_t GBIX_FOURCC = 'XIBG'; // GBIX

PVRUnsupportedPixelFormat::PVRUnsupportedPixelFormat(uint8_t format)
	: format_specifier(format)
{
}

PVRUnsupportedDataFormat::PVRUnsupportedDataFormat(uint8_t format)
	: format_specifier(format)
{
}

PVRReader::PVRReader(const std::string& path)
	: FileReader(path)
{
	PVRReader::check();
}

PVRReader::PVRReader(PVRReader&& other) noexcept
	: IPVRTexture(other), FileReader(std::move(other))
{
	gbix_pos       = other.gbix_pos;
	pvrt_pos       = other.pvrt_pos;
	data_pos       = other.data_pos;
	has_alpha      = other.has_alpha;
	is_twiddled    = other.is_twiddled;
	has_mipmaps    = other.has_mipmaps;
	is_vq          = other.is_vq;
	is_rect        = other.is_rect;
	code_book_size = other.code_book_size;
	palette_depth  = other.palette_depth;
}

PVRReader::PVRReader(std::ifstream& stream, size_t size, bool owner)
	: FileReader(stream, size, owner)
{
	PVRReader::check();
}

PVRReader::~PVRReader()
{
	FileReader::close();
}

void PVRReader::build(std::ofstream& file)
{
	if (gbix_pos != pos_t(-1))
	{
		write_t(file, GBIX_FOURCC);
		write_t(file, static_cast<uint32_t>(file.tellp()));
		write_t(file, gbix);
	}

	write_t(file, PVR_FOURCC);

	write_t(file, data_size);
	write_t(file, pixel_format);
	write_t(file, data_format);
	write_t(file, static_cast<uint16_t>(0));
	write_t(file, width);
	write_t(file, height);

	char buffer[4096] {};

	stream.seekg(data_pos);

	const auto end = data_pos + pos_t(data_size);

	pos_t pos;
	while (!stream.eof() && (pos = stream.tellg()) < end)
	{
		const auto remainder = std::min(static_cast<size_t>(end - pos),
			static_cast<size_t>(sizeof(buffer)));

		stream.read(buffer, remainder);
		file.write(buffer, remainder);
	}

	stream.clear();
}

void PVRReader::write(std::ofstream& file)
{
	char buffer[4096] {};

	stream.seekg(start_);

	const auto end = start_ + size_;

	pos_t pos;
	while (!stream.eof() && (pos = stream.tellg()) < end)
	{
		const auto remainder = std::min(static_cast<size_t>(end - pos),
			static_cast<size_t>(sizeof(buffer)));

		stream.read(buffer, remainder);
		file.write(buffer, remainder);
	}

	stream.clear();
}

void PVRReader::read_gbix()
{
	gbix_pos = stream.tellg() - pos_t(sizeof(uint32_t));

	uint32_t _pvrt_pos = 0;
	read_t(stream, _pvrt_pos);
	pvrt_pos = 8 + pos_t(_pvrt_pos);
	read_t(stream, gbix);
}

void PVRReader::get_header()
{
	if (!stream.is_open())
	{
		throw PVRFileOpenFail();
	}

	const auto pos = stream.tellg();

	uint32_t fourcc = 0;
	read_t(stream, fourcc);

	if (fourcc == PVR_FOURCC)
	{
		gbix_pos = -1;
		pvrt_pos = 0;
	}
	else if (fourcc == GBIX_FOURCC)
	{
		read_gbix();
	}
	else
	{
		read_t(stream, fourcc);

		if (fourcc == GBIX_FOURCC)
		{
			read_gbix();
		}
		else if (fourcc == PVR_FOURCC)
		{
			gbix_pos = -1;
			pvrt_pos = stream.tellg() - pos_t(sizeof(uint32_t));
		}
		else
		{
			throw PVRFileNotPVR();
		}
	}

	if (gbix_pos != pos_t(-1))
	{
		stream.seekg(pvrt_pos);
		read_t(stream, fourcc);

		if (fourcc != PVR_FOURCC)
		{
			throw PVRFileNotPVR();
		}
	}

	read_t(stream, data_size);
	read_t(stream, pixel_format);
	read_t(stream, data_format);
	stream.seekg(sizeof(uint16_t), std::ios::cur);
	read_t(stream, width);
	read_t(stream, height);

	switch (pixel_format)
	{
		case PVR_PIXEL_FORMAT_ARGB1555:
			has_alpha = true;
			break;

		case PVR_PIXEL_FORMAT_RGB565:
			break;

		case PVR_PIXEL_FORMAT_ARGB4444:
			has_alpha = true;
			break;

		case PVR_PIXEL_FORMAT_YUV422:
			break;

		default:
			throw PVRUnsupportedPixelFormat(pixel_format);
	}

	switch (static_cast<PVR_DATA_FORMAT>(data_format))
	{
		case PVR_DATA_FORMAT_TWIDDLED:
			is_twiddled = true;
			break;

		case PVR_DATA_FORMAT_TWIDDLED_MM:
			is_twiddled = true;
			has_mipmaps = true;
			break;

		case PVR_DATA_FORMAT_VQ:
			is_vq = true;
			break;

		case PVR_DATA_FORMAT_VQ_MM:
			is_vq       = true;
			has_mipmaps = true;
			break;

		case PVR_DATA_FORMAT_PALETTIZE4:
			is_twiddled   = true;
			palette_depth = 4;
			break;

		case PVR_DATA_FORMAT_PALETTIZE4_MM:
			is_twiddled   = true;
			palette_depth = 4;
			has_mipmaps   = true;
			break;

		case PVR_DATA_FORMAT_PALETTIZE8:
			is_twiddled   = true;
			palette_depth = 8;
			break;

		case PVR_DATA_FORMAT_PALETTIZE8_MM:
			is_twiddled   = true;
			palette_depth = 8;
			has_mipmaps   = true;
			break;

		case PVR_DATA_FORMAT_RECTANGLE:
			break;

		case PVR_DATA_FORMAT_RECTANGLE_MM:
			has_mipmaps = true;
			break;

		case PVR_DATA_FORMAT_STRIDE:
			break;

		case PVR_DATA_FORMAT_STRIDE_MM:
			has_mipmaps = true;
			break;

		case PVR_DATA_FORMAT_TWIDDLED_RECTANGLE:
			is_twiddled = true;
			break;

		case PVR_DATA_FORMAT_SMALLVQ:
			is_vq = true;

			if (width <= 16)
			{
				code_book_size = 16;
			}
			else if (width == 32)
			{
				code_book_size = 32;
			}
			else if (width == 64)
			{
				code_book_size = 128;
			}
			else
			{
				code_book_size = 256;
			}
			break;

		case PVR_DATA_FORMAT_SMALLVQ_MM:
			is_vq       = true;
			has_mipmaps = true;

			if (width <= 16)
			{
				code_book_size = 16;
			}
			else if (width == 32)
			{
				code_book_size = 64;
			}
			else
			{
				code_book_size = 256;
			}
			break;

		default:
			throw PVRUnsupportedDataFormat(data_format);
	}

	data_pos = stream.tellg();
	stream.seekg(pos);
}

void PVRReader::check()
{
	get_header();
	is_open_ = true;
}
