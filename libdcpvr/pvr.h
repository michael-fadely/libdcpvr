#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include "FileReader.h"

enum PVR_DECODE
{
	PVR_DECODE_RGB,
	PVR_DECODE_RGBA,
};

// PVR Pixel Formats
enum PVR_PIXEL_FORMAT : uint8_t
{
	PVR_PIXEL_FORMAT_ARGB1555 = 0x00,
	PVR_PIXEL_FORMAT_RGB565   = 0x01,
	PVR_PIXEL_FORMAT_ARGB4444 = 0x02,
	PVR_PIXEL_FORMAT_YUV422   = 0x03,
	PVR_PIXEL_FORMAT_BUMP     = 0x04,
	PVR_PIXEL_FORMAT_UNKNOWN  = 0xFF,
};


// Pvr Data Formats
enum PVR_DATA_FORMAT : uint8_t
{
	PVR_DATA_FORMAT_TWIDDLED           = 0x01,
	PVR_DATA_FORMAT_TWIDDLED_MM        = 0x02,
	PVR_DATA_FORMAT_VQ                 = 0x03,
	PVR_DATA_FORMAT_VQ_MM              = 0x04,
	PVR_DATA_FORMAT_PALETTIZE4         = 0x05,
	PVR_DATA_FORMAT_PALETTIZE4_MM      = 0x06,
	PVR_DATA_FORMAT_PALETTIZE8         = 0x07,
	PVR_DATA_FORMAT_PALETTIZE8_MM      = 0x08,
	PVR_DATA_FORMAT_RECTANGLE          = 0x09,
	PVR_DATA_FORMAT_RECTANGLE_MM       = 0x0A,
	PVR_DATA_FORMAT_STRIDE             = 0x0B,
	PVR_DATA_FORMAT_STRIDE_MM          = 0x0C,
	PVR_DATA_FORMAT_TWIDDLED_RECTANGLE = 0x0D,
	PVR_DATA_FORMAT_BMP                = 0x0E,
	PVR_DATA_FORMAT_BMP_MM             = 0x0F,
	PVR_DATA_FORMAT_SMALLVQ            = 0x10,
	PVR_DATA_FORMAT_SMALLVQ_MM         = 0x11,
	PVR_DATA_FORMAT_UNKNOWN            = 0xFF
};

struct IPVRTexture
{
	uint32_t         gbix         = 0;
	uint32_t         data_size    = 0;
	PVR_PIXEL_FORMAT pixel_format = PVR_PIXEL_FORMAT_UNKNOWN;
	PVR_DATA_FORMAT  data_format  = PVR_DATA_FORMAT_UNKNOWN;
	uint16_t         width        = 0;
	uint16_t         height       = 0;
};

class PVRFileOpenFail : public std::exception
{
};

class PVRFileNotPVR : public std::exception
{
};

class PVRUnsupportedPixelFormat : public std::exception
{
public:
	const uint8_t format_specifier;
	explicit PVRUnsupportedPixelFormat(uint8_t format);
};

class PVRUnsupportedDataFormat : public std::exception
{
public:
	const uint8_t format_specifier;
	explicit PVRUnsupportedDataFormat(uint8_t format);
};

class PVRReader : public IPVRTexture, public FileReader
{
	pos_t gbix_pos;
	pos_t pvrt_pos;
	pos_t data_pos;

	bool has_alpha = false;
	bool is_twiddled = false;
	bool has_mipmaps = false;
	bool is_vq = false;
	// TODO: use?
	bool is_rect = false;
	int code_book_size = 0;
	int palette_depth = 0;

public:
	explicit PVRReader(const std::string& path);
	PVRReader(PVRReader&& other) noexcept;
	PVRReader(std::ifstream& stream, size_t size, bool owner = false);
	~PVRReader();

	void build(std::ofstream& file);
	void write(std::ofstream& file);

private:
	void read_gbix();
	void get_header();
	void check() override;
};
