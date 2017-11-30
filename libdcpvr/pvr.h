#pragma once

#include <cstdint>
#include <string>
#include <fstream>

// TODO: exceptions instead
enum PVR_ERROR
{
	PVR_OK,
	PVR_FILE_NOT_PVR,
	PVR_FILE_OPEN_FAIL,
	PVR_EARLY_EOF,
	PVR_INPUT_NULL,
	PVR_OUTPUT_NULL,
	PVR_FORMAT_NOT_SUPPORTED
};

// TODO: exceptions instead
enum PVR_DECODE
{
	PVR_DECODE_24BIT,
	PVR_DECODE_32BIT,
};

// PVR Pixel Formats
enum PVR_PIXEL_FORMAT
{
	PVR_PIXEL_FORMAT_ARGB1555 = 0x00,
	PVR_PIXEL_FORMAT_RGB565   = 0x01,
	PVR_PIXEL_FORMAT_ARGB4444 = 0x02,
	PVR_PIXEL_FORMAT_YUV422   = 0x03,
	PVR_PIXEL_FORMAT_BUMP     = 0x04,
	PVR_PIXEL_FORMAT_UNKNOWN  = 0xFF,
};


// Pvr Data Formats
enum PVR_DATA_FORMAT
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
	PVR_DATA_FORMAT_SMALLVQ_MM         = 0x11
};

struct IPVRTexture
{
	uint32_t gbix;
	uint32_t data_size;
	uint8_t  pixel_format;
	uint8_t  data_format;
	uint16_t width;
	uint16_t height;
};

class PVRReader : public IPVRTexture, public FileReader<PVR_ERROR>
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
	PVR_ERROR get_header();
	void check() override;
};
