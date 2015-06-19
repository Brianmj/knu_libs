//
//  image4.hpp
//  Skybox
//
//  Created by Brian Jones on 4/21/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#ifndef knu_image4_hpp
#define knu_image4_hpp

#include <memory>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

#ifdef WIN32
#include <SDL_image.h>
#pragma comment(lib, "sdl2_image.lib")
#endif

namespace knu
{
    namespace graphics
    {
        static std::once_flag once;
        
        // just make sure that SDL_Image is initialzed once
        static void initialize_sdl_image()
        {
            if(!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
                throw std::runtime_error("Unable to initialize SDL Image");
        }
        
        class image
        {
            int width;
            int height;
            int bitsPerPixel;
            int bytesPerPixel;
            unsigned int format;
            unsigned int internalFormat;
            int imageSize;
            std::unique_ptr<unsigned char[]> imageData;
            
        public:
            void load_image(std::string name_)
            {
                std::call_once(once, initialize_sdl_image);
                
                std::shared_ptr<SDL_Surface> surface(IMG_Load(name_.c_str()), [](SDL_Surface* surf) {SDL_FreeSurface(surf);});
                
                if(!surface)
                    throw std::runtime_error("Unable to load image: " + name_);
                
                // lock surface
                SDL_LockSurface(surface.get());
                
                width = surface->w;
                height = surface->h;
                bitsPerPixel = surface->format->BitsPerPixel;
                bytesPerPixel = surface->format->BytesPerPixel;
                
                if (surface->locked)
                {
                    
                }
                
                
                if (bitsPerPixel == 32)
                {
                    if (surface->format->Rmask == 0x000000ff)
                    {
                        internalFormat = 0x8058; // GL_RGBA8;
                        format = 0x1908; //GL_RGBA;
                    }
                    else
                    {
                        internalFormat = 0x8058; //GL_RGBA8;
                        format = 0x80E1; //GL_BGRA;
                    }
                    
                }
                else
                    if (bitsPerPixel == 24)
                    {
                        if (surface->format->Rmask == 0x0000ff)
                        {
                            internalFormat = 0x8051; //GL_RGB8;
                            format = 0x1907; // GL_RGB;
                        }
                        else
                        {
                            internalFormat = 0x8051; //GL_RGB8;
                            format = 0x80E0; //GL_BGR;
                        }
                    }
                    else
                    {
                        // not a true color format
                        SDL_UnlockSurface(surface.get());
                        throw std::runtime_error("Unsupported bytes per pixel");
                    }
                
                
                imageSize = width * height * bytesPerPixel;
                std::unique_ptr<unsigned char[]> data(new unsigned char[imageSize]);
                memcpy(data.get(), surface->pixels, imageSize);
                
                imageData = std::move(data);
                
                // Release the surface
                SDL_UnlockSurface(surface.get());
            }
            
            int get_width() {return width;}
            int get_height() {return height;}
            unsigned int get_format() {return format;}
            unsigned int get_internal_format() {return internalFormat;}
            int get_size() {return imageSize;}
            unsigned char *get_data() { return imageData.get();}
        };
    }
}

#endif
