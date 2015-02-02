//
//  tgareader.h
//  Texture
//
//  Created by Brian Jones on 6/14/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef Texture_tgareader_h
#define Texture_tgareader_h

#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

namespace knu
{
    
    class TGA
    {
    private:
        enum class TgaImageType
        {NO_IMAGE_DATA, COLORMAPPED_IMAGE_DATA, TRUECOLOR_IMAGE_DATA, MONOCHROME_IMAGE_DATA, COLORMAPPED_RLE = 9, TRUECOLOR_RLE = 10, MONOCHROME_RLE = 11 };
        
        struct TgaHeader
        {
            unsigned char idLength;
            unsigned char colorMap;
            unsigned char imageType;
            short colorMapOrigin;
            short colorMapLength;
            unsigned char colorMapDepth;
            short xOrigin;
            short yOrigin;
            short imageWidth;
            short imageHeight;
            unsigned char pixelDepth;
            unsigned char imageDescriptor;
        };
        
    private:
        TgaHeader header;
        std::string imageID;
        
        void readFile(std::string fileName)
        {
            std::ifstream file(fileName, std::ios::binary | std::ios::in);
            
            if(!file)
                throw std::runtime_error("Unable to open " + fileName);
            
            readHeader(file);
            if(header.idLength)
                readImageID(file);
            verify();
            if(header.imageType == (int)TgaImageType::TRUECOLOR_IMAGE_DATA)
                readData(file);

        }
        
        void readHeader(std::ifstream &file)
        {
            file.read((char*)&header.idLength, 1);
            file.read((char*)&header.colorMap, 1);
            file.read((char*)&header.imageType, 1);
            file.read((char*)&header.colorMapOrigin, 2);
            file.read((char*)&header.colorMapLength, 2);
            file.read((char*)&header.colorMapDepth, 1);
            file.read((char*)&header.xOrigin, 2);
            file.read((char*)&header.yOrigin, 2);
            file.read((char*)&header.imageWidth, 2);
            file.read((char*)&header.imageHeight, 2);
            file.read((char*)&header.pixelDepth, 1);
            file.read((char*)&header.imageDescriptor, 1);
        }
        
        void readImageID(std::ifstream &file)
        {
            std::vector<unsigned char> i;
            i.resize(header.idLength);
            file.read((char*)i.data(), header.idLength);
            imageID.assign(std::begin(i), std::end(i));
        }
        
        void readData(std::ifstream &file)
        {
            file.read((char*)imageData.data(), imageData.size());
        }
        
        void verify()
        {
            // For now, only truecolor raw data is read
            if(header.imageType != (int)TgaImageType::TRUECOLOR_IMAGE_DATA)
                throw std::runtime_error("Unsupported tga image data type. Only raw true color data is supported");
            
            if((header.pixelDepth % 8) != 0)
                throw std::runtime_error("Unsupported pixel depth. Must be a value divisible by 8");
            
            bytesPerPixel = header.pixelDepth / 8;
            imageWidth = header.imageWidth;
            imageHeight = header.imageHeight;
            
            imageData.resize(imageWidth * imageHeight * bytesPerPixel);
        }
        
    public:
        int imageWidth;
        int imageHeight;
        int bytesPerPixel;
        std::vector<char> imageData;
        
    public:
        TGA() {};
        
        TGA(std::string fileName)
        {
            open_file(fileName);
        }
        
        void open_file(std::string fileName)
        {
            readFile(fileName);
        }
    };
}

#endif
