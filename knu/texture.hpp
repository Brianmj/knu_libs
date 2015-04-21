//
//  texture.hpp
//  Skybox
//
//  Created by Brian Jones on 4/2/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#ifndef knu_texture_hpp
#define knu_texture_hpp

#include <string>
#include <mutex>
#include <memory>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
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
        
        class Texture2D
        {
        private:
            std::string     name;
            int             width;
            int             height;
            int             bitsPerPixel;
			int				bytesPerPixel;
            GLuint          format;
			GLuint			internalFormat;
            GLuint          id;
            
        public:
            Texture2D(): name(), width(0), height(0), bitsPerPixel(0), id(0) {}
            Texture2D(std::string name): name(name), width(0), height(0), bitsPerPixel(0), id(0) {if(!load_texture(this->name, GL_LINEAR, GL_LINEAR, GL_FALSE)) throw (std::runtime_error("Unable to load texture: " + name)); }
            Texture2D(std::string name, GLuint minFilter, GLuint magFilter): name(name), width(0), height(0), bitsPerPixel(0), id(0)
            {if(!load_texture(this->name, minFilter, magFilter, GL_FALSE)) throw (std::runtime_error("Unable to load texture: " + name)); }
            
			~Texture2D() {
				glBindTexture(GL_TEXTURE_2D, 0); glDeleteTextures(1, &id); 
			}
            
            bool load_texture(std::string name_, GLuint minFilter, GLuint magFilter, bool mipmapping)
            {
                std::call_once(once, initialize_sdl_image);
                
                std::shared_ptr<SDL_Surface> surface(IMG_Load(name_.c_str()), [](SDL_Surface* surf) {SDL_FreeSurface(surf);});
                
                if(!surface)
                    return false;

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
						internalFormat = GL_RGBA8;
						format = GL_RGBA;
					}
					else
					{
						internalFormat = GL_RGBA8;
						format = GL_BGRA;
					}
                    
                }
                else
					if (bitsPerPixel == 24)
					{
						if (surface->format->Rmask == 0x0000ff)
						{
							internalFormat = GL_RGB8;
							format = GL_RGB;
						}
						else
						{
							internalFormat = GL_RGBA8;
							format = GL_BGR;
						}
                    }
                    else
                    {
                        // not a true color format
                        SDL_UnlockSurface(surface.get());
                        throw std::runtime_error("Unsupported bytes per pixel");
                    }


				int size = width * height * bytesPerPixel;
                std::unique_ptr<unsigned char[]> data(new unsigned char[size]);
				memcpy(data.get(), surface->pixels, size);

                glGenTextures(1, &id);
                glBindTexture(GL_TEXTURE_2D, id);
				// For some reason this does not work in window 10 beta drivers!!!! use glTexImage2D
                //glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
                //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data.get());
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data.get());

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                
                if(mipmapping)
                    glGenerateMipmap(GL_TEXTURE_2D);

				// Release the surface
				SDL_UnlockSurface(surface.get());
                
                return true;
            }

			inline void bind() { glBindTexture(GL_TEXTURE_2D, id); }
        };
    }
}
#endif
