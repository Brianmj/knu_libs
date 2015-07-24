//
//  sprite.h
//  SimpleTexture
//
//  Created by Brian Jones on 4/13/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#ifndef knu_sprite_hpp
#define knu_sprite_hpp

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <knu/mathlibrary5.hpp>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#endif

namespace knu
{
    namespace graphics
    {
        struct sprite
        {
            int width;
            int height;
            GLuint vbo;
            GLuint vao;
            
            sprite(): width(100), height(100), vbo(0), vao(0) {}
            sprite(int width, int height): width(width), height(height), vbo(0), vao(0) {}
			~sprite()
			{
	//			glBindVertexArray(0);
	//			glDeleteVertexArrays(1, &vao);
			}
            
        private:
			
        };
        
        void remove_sprite(sprite *s);

		static std::shared_ptr<sprite> create_sprite(int width, int height)
		{
			int halfWidth = width / 2;
			int halfHeight = height / 2;

			// construct a sprite around the origin
			std::vector<knu::math::v3f> vertices = {
				{ static_cast<float>(-halfWidth), static_cast<float>(halfHeight), 0.0f },      // upper left
				{ static_cast<float>(-halfWidth), static_cast<float>(-halfHeight), 0.0f },     // lower left
				{ static_cast<float>(halfWidth), static_cast<float>(-halfWidth), 0.0f },       // lower right
				{ static_cast<float>(halfWidth), static_cast<float>(-halfWidth), 0.0f },       // lower right
				{ static_cast<float>(halfWidth), static_cast<float>(halfHeight), 0.0f },       // upper right
				{ static_cast<float>(-halfWidth), static_cast<float>(halfHeight), 0.0f }       // upper left
			};
			/* for now, use upsideTexCoords because of the way SDL_Image loads textures
			std::vector<knu::math::v2f> texCoords = {
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
			};*/

			std::vector<knu::math::v2f> upsideTexCoords = {
				{ 0.0f, 0.0f },
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },
				{ 1.0f, 1.0f },
				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f }
			};

			GLuint vao, vbo;
			auto bufferSize = (sizeof(knu::math::v3f) * vertices.size()) + (sizeof(knu::math::v2f) * upsideTexCoords.size());

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
			int vertexOffset = 0;
			long texCoordOffset = (sizeof(knu::math::v3f) * vertices.size());
			long verticesSize = static_cast<int>(sizeof(knu::math::v3f) * vertices.size());
			int texCoordSize = static_cast<int>(sizeof(knu::math::v2f) * upsideTexCoords.size());
			glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, verticesSize, vertices.data());
			glBufferSubData(GL_ARRAY_BUFFER, texCoordOffset, texCoordSize, upsideTexCoords.data());


			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(texCoordOffset));

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glBindVertexArray(0);

            auto s = std::shared_ptr<sprite>(new sprite(), remove_sprite);
            s->width = width;
            s->height = height;
			s->vbo = vbo;
			s->vao = vao;
            return s;

		}
        
		inline void remove_sprite(sprite *s)
		{
			glDeleteVertexArrays(1, &s->vao);
			glDeleteBuffers(1, &s->vbo);
            delete s;
		}
        
        class sprite_manager
        {
            using string_sprite_map_type = std::map<std::string, std::shared_ptr<sprite>>;
            std::map<std::string, std::shared_ptr<sprite>> string_sprite_map;
        public:
            void add(std::string name, int width, int height)
            {
                string_sprite_map[name] = create_sprite(width, height);
            }
            
            std::shared_ptr<sprite> reference(std::string name) {return string_sprite_map[name];}
        };
    }
}
#endif
