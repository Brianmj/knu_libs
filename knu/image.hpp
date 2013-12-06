//
//  image.hpp
//  sdl particle
//
//  Created by Brian Jones on 11/12/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef sdl_particle_image_hpp
#define sdl_particle_image_hpp

#include <vector>
#include <memory>

#ifdef __APPLE__
#include <SDL2_image/SDL_image.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif

#ifdef WIN32
#include <SDL_image.h>
#include <GL/glew.h>
#pragma comment(lib, "sdl2_image.lib")
#endif

namespace knu
{
    namespace image
    {
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
            static unsigned initialized = 0;
            if(!initialized)
            {
                if(!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
                    throw std::runtime_error("Unable to initialize SDL_Image");
                initialized = 1;
            }
            
            
            std::shared_ptr<SDL_Surface> surface(IMG_Load(fileName.c_str()), [](SDL_Surface *s)
                                                 {SDL_FreeSurface(s);});
            if(nullptr == surface.get())
                throw std::runtime_error("Unable to load file: " + fileName);
            
            ImageData i;
            SDL_LockSurface(surface.get());
            i.width = surface->w;
            i.height = surface->h;
            i.bitsPerPixel = surface->format->BitsPerPixel;
            i.bytesPerPixel = surface->format->BytesPerPixel;
            i.size = i.width * i.height * i.bytesPerPixel;
            
            if(i.bytesPerPixel == 4)
            {
                if(surface->format->Rmask == 0x000000ff)
                    i.format = GL_RGBA8;
                else
                    i.format = GL_BGRA;
                
            }
            else
                if(i.bytesPerPixel == 3)
                {
                    if(surface->format->Rmask ==  0x0000ff)
                        i.format = GL_RGB8;
                    else
                        i.format = GL_BGR;
                }
            else
            {
                // not a true color format
                SDL_UnlockSurface(surface.get());
                throw std::runtime_error("Unsupported bytes per pixel");
            }
                
            i.pixels.resize(i.size);
            memcpy(i.pixels.data(), surface->pixels, i.size);
            SDL_UnlockSurface(surface.get());
            
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
    } // image
} // knu


#endif
