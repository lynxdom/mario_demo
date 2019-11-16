#include "ImageFileReader.h"

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <inttypes.h>

namespace Lynxdom
{
	namespace ImageFileReader
	{
		struct PalletEntry
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
		};

		struct BMPHeader
		{
			char 		id[2];
			__int32		iSize;
			char		reserved1[2];
			char		reserved2[2];
			__int32		offset;
		};

		struct BMPCoreHeader
		{
			__int32		size;
			__int32		width;
			__int32		height;
			__int16		colorPlane;
			__int16		bitsPerPixel;
			__int32		compression;
			__int32		imageSize;
			__int32		horzRes;
			__int32		verRes;
			__int32		colorPallete;
			__int32		import;
		};

		struct TGAHeader
		{

		};

		auto ReadBMP(const char* cFileName, int& width, int& height)->std::unique_ptr<RGBPixel[]>
		{
			std::fstream inputFile(cFileName, std::ios_base::in | std::ios_base::binary);

			if(!inputFile.is_open())
			{
				return nullptr;
			}

			BMPHeader bmpHeader;
			inputFile.read(reinterpret_cast<char*>(&bmpHeader.id), 			2);
			inputFile.read(reinterpret_cast<char*>(&bmpHeader.iSize), 		4);
			inputFile.read(reinterpret_cast<char*>(&bmpHeader.reserved1), 	2);
			inputFile.read(reinterpret_cast<char*>(&bmpHeader.reserved2), 	2);
			inputFile.read(reinterpret_cast<char*>(&bmpHeader.offset), 		4);

			if(bmpHeader.id[0] != 'B' || bmpHeader.id[1] != 'M')
			{
				return nullptr;
			}

			BMPCoreHeader bmpCoreHeader;
			inputFile.seekg(14);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.size), 			4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.width), 			4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.height), 			4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.colorPlane), 		2);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.bitsPerPixel), 	2);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.compression), 	4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.imageSize), 		4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.horzRes), 		4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.verRes), 			4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.colorPallete), 	4);
			inputFile.read(reinterpret_cast<char*>(&bmpCoreHeader.import),			4);

			width = bmpCoreHeader.width;
			height = bmpCoreHeader.height;

			__int32 iImageSize = bmpHeader.iSize - bmpHeader.offset;

			auto pImageBuffer = std::unique_ptr<RGBPixel[]>(new RGBPixel[iImageSize]);
			auto pColorPallet = std::unique_ptr<PalletEntry[]>(new PalletEntry[256]);

			size_t current = inputFile.tellg();

			if(bmpCoreHeader.bitsPerPixel == 8)
			{
				for(int i = 0; i < 256; ++i)
				{
					char cEmpty = 0;

					inputFile.read(reinterpret_cast<char*>(&pColorPallet[i].b),			1);
					inputFile.read(reinterpret_cast<char*>(&pColorPallet[i].g),			1);
					inputFile.read(reinterpret_cast<char*>(&pColorPallet[i].r),			1);
					inputFile.read(reinterpret_cast<char*>(&cEmpty),					1);
				}
			}

			current = inputFile.tellg();
			inputFile.seekg(bmpHeader.offset);

			// Map the pixel values to the palate.
			for(int i = 0; i < iImageSize; ++i)
			{
				if(bmpCoreHeader.bitsPerPixel == 8)
				{
					unsigned char cColorIndex = 0;

					inputFile.read(reinterpret_cast<char*>(&cColorIndex),				1);

					pImageBuffer[i].R = pColorPallet[cColorIndex].r;
					pImageBuffer[i].G = pColorPallet[cColorIndex].g;
					pImageBuffer[i].B = pColorPallet[cColorIndex].b;
				}
				else
				if(bmpCoreHeader.bitsPerPixel == 24)
				{
					inputFile.read(reinterpret_cast<char*>(&pImageBuffer[i].R),			1);
					inputFile.read(reinterpret_cast<char*>(&pImageBuffer[i].G),			1);
					inputFile.read(reinterpret_cast<char*>(&pImageBuffer[i].B),			1);
				}
			}

			inputFile.close();

			return move(pImageBuffer);
		}

		auto ReadTGA(const char* cFileName, int& width, int& height)->std::unique_ptr<RGBPixel[]>
		{
			return nullptr;
		}
	}
}
