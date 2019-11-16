#include <memory>

namespace Lynxdom
{
	namespace ImageFileReader
	{
		struct RGBPixel
		{
			unsigned char R;
			unsigned char G;
			unsigned char B;
		};

		auto ReadBMP(const char* cFileName, int& width, int& height)->std::unique_ptr<RGBPixel[]>;
		auto ReadTGA(const char* cFileName, int& width, int& height)->std::unique_ptr<RGBPixel[]>;
	}
}
