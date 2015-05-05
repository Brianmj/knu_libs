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
#include <knu/image4.hpp>

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
        class Texture2D
        {
        private:
            GLuint          id;
            
        public:
            Texture2D(): id(0) {glGenTextures(1, &id);}
            Texture2D(std::string name): id(0) {glGenTextures(1, &id); if(!load_texture(name, GL_LINEAR, GL_LINEAR, GL_FALSE)) throw (std::runtime_error("Unable to load texture: " + name)); }
            Texture2D(std::string name, GLuint minFilter, GLuint magFilter): id(0)
            {glGenTextures(1, &id); if(!load_texture(name, minFilter, magFilter, GL_FALSE)) throw (std::runtime_error("Unable to load texture: " + name)); }
            
			~Texture2D() {
				glBindTexture(GL_TEXTURE_2D, 0); glDeleteTextures(1, &id); 
			}
            
            bool load_texture(std::string name_, GLuint minFilter, GLuint magFilter, bool mipmapping)
            {
                knu::graphics::Image img;
                img.load_image(name_);
                
                glBindTexture(GL_TEXTURE_2D, id);
				// For some reason this does not work in window 10 beta drivers!!!! use glTexImage2D
                //glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
                //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data.get());
                glTexImage2D(GL_TEXTURE_2D, 0, img.get_internal_format(), img.get_width(), img.get_height(), 0, img.get_format(), GL_UNSIGNED_BYTE, img.get_data());

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                
                if(mipmapping)
                    glGenerateMipmap(GL_TEXTURE_2D);
                
                return true;
            }

			inline void bind() { glBindTexture(GL_TEXTURE_2D, id); }
        };
    }
}
#endif
