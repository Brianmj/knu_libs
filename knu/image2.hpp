#ifndef knu_image2
#define knu_image2

#include <memory>
#include <vector>
#include <string>

//#define STBI_HEADER_FILE_ONLY
#include <knu/stb_image.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#endif

namespace knu
{
	namespace image
	{
		struct RGB
		{
			unsigned char r, g, b;
		};

		struct RGBA
		{
			unsigned char r, g, b, a;
		};

		struct ImageData
		{
			int width;
			int height;
			int bitsPerPixel;
			int bytesPerPixel;
			int format;
			std::size_t size;
			std::vector<unsigned char> pixels;
		};

		inline ImageData load_image(std::string fileName)
		{
			int w, h, comp;
			stbi_uc *temp = stbi_load(fileName.c_str(), &w, &h, &comp, 0);

			if (nullptr == temp)
				throw std::runtime_error("Unable to load file: " + fileName);

			std::shared_ptr<stbi_uc> data(temp, [](stbi_uc *d) {stbi_image_free(d); });
			temp = nullptr;

			ImageData i;
			i.width = w;
			i.height = h;
			i.bitsPerPixel = comp * 8;
			i.bytesPerPixel = comp;
			i.size = i.width * i.height * i.bytesPerPixel;

			// the image needs to be flipped vertically because
			if (i.bytesPerPixel == 4)
			{
				i.format = GL_RGBA8;
				std::vector<RGBA> pixelData((RGBA*)data.get(), ((RGBA*)data.get() + (w * h)));
				std::vector<RGBA> flippedData(w * h);
				std::vector<RGBA>::iterator iter = flippedData.begin();

				for (int yPos = (h - 1) * w; yPos >= 0; yPos -= w)
				{
					for (int xPos = 0; xPos < w; ++xPos, ++iter)
					{
						*iter = pixelData[yPos + xPos];
					}
				}

				i.pixels.resize(i.size);
				memcpy(i.pixels.data(), flippedData.data(), i.size);
			}
			else
			if (i.bytesPerPixel == 3)
			{
				i.format = GL_RGB8;
				std::vector<RGB> pixelData((RGB*)data.get(), ((RGB*)data.get() + (w * h)));
				std::vector<RGB> flippedData(w * h);
				std::vector<RGB>::iterator iter = flippedData.begin();

				for (int yPos = (h - 1) * w; yPos >= 0; yPos -= w)
				{
					for (int xPos = 0; xPos < w; ++xPos, ++iter)
					{
						*iter = pixelData[yPos + xPos];
						
					}
				}

				i.pixels.resize(i.size);
				memcpy(i.pixels.data(), flippedData.data(), i.size);
			}
			else
			{
				// not a true color format
				throw std::runtime_error("Unsupported bytes per pixel");
			}
			return i;
		}

		inline GLuint make_texture(std::string fileName)
		{
			ImageData i = load_image(fileName);

			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);

			glTexStorage2D(GL_TEXTURE_2D, 1, (i.bytesPerPixel == 4) ? GL_RGBA8 : GL_RGB8, i.width, i.height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, i.width, i.height, i.format, GL_UNSIGNED_BYTE, i.pixels.data());

			return tex;
		}

		inline GLuint make_texture(std::string fileName, GLint minFilter, GLint magFilter)
		{
			GLuint tex = make_texture(fileName);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			return tex;
		}
	}
}

#endif