#ifndef string_draw_hpp
#define string_draw_hpp

#include <string>
#include <knu/mathlibrary5.hpp>
#include <knu/font.hpp>

#ifdef WIN32
#include <GL/glew.h>
#endif

namespace knu
{
	namespace graphics
	{

		template<typename T>
		inline std::string make_into_string(T val) { return std::to_string(val); }

		template<>
		inline std::string make_into_string<std::string>(std::string s) { return s; }

		template<>
		inline std::string make_into_string<const char*>(const char* s) { return std::string(s); }

		template<typename T>
		inline std::string make_args_to_string(T val) { return std::to_string(val); }

		template<>
		inline std::string make_args_to_string<const char*>(const char *n) { return std::string(n); }

		template<>
		inline std::string make_args_to_string<std::string>(std::string s) { return s; }

		template<typename T1, typename ... Types>
		inline std::string make_args_to_string(T1 val1, Types ...vals)
		{
			std::string s;
			s += make_into_string(val1) + make_args_to_string(vals...);
			return s;
		}

		class String_Draw
		{
			enum { ARRAY_BUFFER, TEXTURE_COORD_BUFFER, VERTICES_PER_QUAD = 6, MAX_CHARACTERS = 512 };

			GLuint buffers[2];			// buffer[0] == ARRAY_BUFFER, buffer[1] == TEXTURE_COORD_BUFFER
			GLuint vao;

		private:
			void initialize()
			{
				setup_buffers();
				setup_vao();
			}

			void uninitialize()
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDeleteBuffers(2, buffers);

				glBindVertexArray(0);
				glDeleteVertexArrays(1, &vao);
			}

			void setup_buffers()
			{
				GLsizei tBufSize = MAX_CHARACTERS * VERTICES_PER_QUAD * sizeof(knu::math::v2f);
				GLsizei vBufSize = MAX_CHARACTERS * VERTICES_PER_QUAD * sizeof(knu::math::v3f);

				glGenBuffers(2, buffers);
				glBindBuffer(GL_ARRAY_BUFFER, buffers[ARRAY_BUFFER]);      // buffer for vertices
				glBufferData(GL_ARRAY_BUFFER, vBufSize, nullptr, GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXTURE_COORD_BUFFER]);      // buffers for texture coordinates
				glBufferData(GL_ARRAY_BUFFER, tBufSize, nullptr, GL_DYNAMIC_DRAW);
			}

			void setup_vao()
			{
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				glBindBuffer(GL_ARRAY_BUFFER, buffers[ARRAY_BUFFER]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

				glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXTURE_COORD_BUFFER]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);

				glBindVertexArray(0);
			}

			void ready_texture_buffer(std::vector<Character> characters)
			{
				glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXTURE_COORD_BUFFER]);
				math::v2f *ptr = static_cast<math::v2f *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, VERTICES_PER_QUAD * sizeof(math::v2f) * characters.size(), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT));

				for (auto const &c : characters)
				{
					math::v2f lowerLeft = { c.normXLeft, c.normYBottom };
					math::v2f lowerRight = { c.normXWidth, c.normYBottom };
					math::v2f upperLeft = { c.normXLeft, c.normYHeight };
					math::v2f upperRight = { c.normXWidth, c.normYHeight };

					*ptr++ = upperLeft;
					*ptr++ = lowerLeft;
					*ptr++ = lowerRight;

					*ptr++ = lowerRight;
					*ptr++ = upperRight;
					*ptr++ = upperLeft;
				}

				glUnmapBuffer(GL_ARRAY_BUFFER);
				ptr = nullptr;
			}

			void ready_vertex_buffer(std::vector<Character> const &characters)
			{
				glBindBuffer(GL_ARRAY_BUFFER, buffers[ARRAY_BUFFER]);
				math::v3f *ptr = static_cast<math::v3f *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, VERTICES_PER_QUAD * sizeof(math::v3f) * characters.size(), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT));

				math::v3f origin = { 0.0f, 0.0f, 0.0f };

				for (auto const &c : characters)
				{
					math::v3f lowerLeft = origin + math::v3f((float)c.xOffset, ((float)-c.yOffset), 0.0f);
					math::v3f lowerRight = origin + math::v3f((float)(c.xOffset + c.width), (float)(-c.yOffset), 0.0);
					math::v3f upperRight = origin + math::v3f((float)(c.xOffset + c.width), (float)((-c.height) + (-c.yOffset)), 0.0);
					math::v3f upperLeft = origin + math::v3f((float)c.xOffset, (float)((-c.height) + (-c.yOffset)), 0.0f);

					*ptr++ = upperLeft;                // first triangle
					*ptr++ = lowerLeft;
					*ptr++ = lowerRight;

					*ptr++ = lowerRight;               // second triangle
					*ptr++ = upperRight;
					*ptr++ = upperLeft;

					origin.x += c.xAdvance;
				}

				glUnmapBuffer(GL_ARRAY_BUFFER);
				ptr = nullptr;
			}

			void string_to_buffer(knu::graphics::Font &font, std::string str)
			{
				auto characters = font.characters(str);
				ready_texture_buffer(characters);
				ready_vertex_buffer(characters);
			}

			void draw_text(Font &font, int characterCount)
			{
				int verticesCount = characterCount * VERTICES_PER_QUAD;

				glActiveTexture(GL_TEXTURE0);
				font.texture.bind();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDepthMask(false);
				glBindVertexArray(vao);
				glDrawArrays(GL_TRIANGLES, 0, verticesCount);
				glBindVertexArray(0);
				glDepthMask(true);
				glDisable(GL_BLEND);
			}

		public:
			String_Draw() { initialize(); }
			~String_Draw() { uninitialize(); }

			template<typename ... Args>
			void draw_string(knu::graphics::Font &font, Args ...args)
			{
				std::string str = make_args_to_string(args...);
				// make sure str isn't > MAX_CHARACTERS
				if (str.length() > MAX_CHARACTERS)
					str.assign(std::begin(str), std::begin(str) + MAX_CHARACTERS);

				string_to_buffer(font, str);
				draw_text(font, static_cast<int>(str.length()));
			}
		};
	}
}
#endif
