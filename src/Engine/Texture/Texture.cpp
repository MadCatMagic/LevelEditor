#include "Engine/Texture/Texture.h"

Texture::Format Texture::FormatType(Format format)
{
	switch (format) {
		case Format::Depth:
			return Format::Depth;
		case Format::Depth16:
			return Format::Depth;
		case Format::Depth24:
			return Format::Depth;
		case Format::None:
			return Format::None;
		case Format::Red:
			return Format::Red;
		case Format::RGB:
			return Format::RGB;
		case Format::RGB8:
			return Format::RGB;
		case Format::RGBFloat:
			return Format::RGB;
		case Format::RGBA:
			return Format::RGBA;
		case Format::RGBA8:
			return Format::RGBA;
		case Format::RGBAFloat:
			return Format::RGBA;
		default:
			return Format::None;
	}
}
