//
//  textren.h
//  knulodepng
//
//  Created by Brian Jones on 7/21/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef knu_textren_h
#define knu_textren_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#endif

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <regex>
#include <future>
#include <unordered_map>
#include <knu/mathlibrary5.hpp>
#include <knu/tga.h>
#include <knu/shaderlocations.h>

namespace knu
{
    struct QuadTexture
    {
        knu::Math::Vector2f topLeft;
        knu::Math::Vector2f topRight;
        knu::Math::Vector2f bottomLeft;
        knu::Math::Vector2f bottomRight;
    };
    
    struct Glyph
    {
        QuadTexture qt;
        int translateNextChar;
    };

	struct Character
	{
		char charId;
		int xPos;
		int yPos;
		int width;
		int height;
		int xOffset;
		int yOffset;
		int xAdvance;
		float normXLeft;
		float normYTop;
		float normXWidth;
		float normYHeight;

		Character(char cId, int x, int y, int w, int h, int xOff, int yOff, int xAdv, int scaleW, int scaleH) :
			charId(cId),
			xPos(x),
			yPos(y),
			width(w),
			height(h),
			xOffset(xOff),
			yOffset(yOff),
			xAdvance(xAdv)
		{
			normXLeft = x / (float) scaleW;
			normXWidth = (x + w) / (float) scaleW;
			normYTop =  (y / (float) scaleH);
			normYHeight =   ((y + h) / (float) scaleH);
		}
	};

    class BitmapFont
    {

        struct Kerning
        {
            int firstChar;
            int secondChar;
            int amount;
            
            Kerning(int char1, int char2, int amt):
            firstChar(char1),
            secondChar(char2),
            amount(amt) {}
            
            bool operator==(const Kerning &k)
            {
                return (firstChar == k.firstChar && secondChar == k.secondChar);
            }
        };
        
        int scaleW;
        int scaleH;
        std::string imageFile;
        std::vector<std::string> lines;
        std::unordered_map<char, Character> charCharMap;
        std::vector<Kerning> kerningList;
        
        QuadTexture make_quad_texture(float x1, float x2, float y1, float y2)
        {
            QuadTexture qt;
            qt.topLeft = knu::Math::Vector2f(x1, y1);
            qt.topRight = knu::Math::Vector2f(x2, y1);
            qt.bottomLeft = knu::Math::Vector2f(x1, y2);
            qt.bottomRight = knu::Math::Vector2f(x2, y2);
            
            return qt;
        }
        
        // read the fnt face info
        void stage_1()
        {
            std::regex scaleWRegex("scaleW=(\\d+)");
            std::regex scaleHRegex("scaleH=(\\d+)");
            std::regex fileRegex("file=\"(\\w+\\.\\w+)\"");
            
            std::for_each(std::begin(lines), std::end(lines), [&](std::string line)
                          {
                              std::smatch scaleWMatch;
                              if(std::regex_search(line, scaleWMatch, scaleWRegex))
                              {
                                  scaleW = std::stoi ( scaleWMatch[1]);
                              }
                              
                              std::smatch scaleHMatch;
                              if(std::regex_search(line, scaleHMatch, scaleHRegex))
                              {
                                  scaleH = std::stoi ( scaleHMatch[1]);
                              }
                              
                              std::smatch fileMatch;
                              if(std::regex_search(line, fileMatch, fileRegex))
                              {
                                  this->imageFile = fileMatch[1];
                              }
                          });
        }
        
        // read the character info
        void stage_2()
        {
            std::regex charIDRegex("char id=(\\d+)");
            std::smatch charIDMatch;
            std::regex xPosRegex("x=(\\d+)");
            std::smatch xPosMatch;
            std::regex yPosRegex("y=(\\d+)");
            std::smatch yPosMatch;
            std::regex widthRegex("width=(\\d+)");
            std::smatch widthMatch;
            std::regex heightRegex("height=(\\d+)");
            std::smatch heightMatch;
            std::regex xOffsetRegex("xoffset=(-?\\d+)");
            std::smatch xOffsetMatch;
            std::regex yOffsetRegex("yoffset=(\\d+)");
            std::smatch yOffsetMatch;
            std::regex xAdvanceRegex("xadvance=(\\d+)");
            std::smatch xAdvanceMatch;
                        
            
            std::for_each(std::begin(lines), std::end(lines), [&](std::string line){
                
                if(std::regex_search(line, charIDMatch, charIDRegex))
                {
                    char charID = std::stoi(charIDMatch[1]);
                    
                    std::regex_search(line, xPosMatch, xPosRegex);
                    int xPos = std::stoi(xPosMatch[1]);
                    
                    std::regex_search(line, yPosMatch, yPosRegex);
                    int yPos = std::stoi(yPosMatch[1]);
                    
                    std::regex_search(line, widthMatch, widthRegex);
                    int charWidth = std::stoi(widthMatch[1]);
                    
                    std::regex_search(line, heightMatch, heightRegex);
                    int charHeight = std::stoi(heightMatch[1]);
                    
                    std::regex_search(line, xOffsetMatch, xOffsetRegex);
                    int xOffset = std::stoi(xOffsetMatch[1]);
                    
                    std::regex_search(line, yOffsetMatch, yOffsetRegex);
                    int yOffset = std::stoi(yOffsetMatch[1]);
                    
                    std::regex_search(line, xAdvanceMatch, xAdvanceRegex);
                    int xAdvance = std::stoi(xAdvanceMatch[1]);
                    
                    Character newCharacter(charID, xPos, yPos, charWidth, charHeight, xOffset, yOffset, xAdvance, this->scaleW, this->scaleH);
                    
                    charCharMap.insert(std::make_pair(charID, newCharacter));
                }
            });
        }
        
        // parse the kerning info
        void stage_3()
        {
            std::regex firstRegex("kerning first=(\\d+)");
            std::smatch firstMatch;
            std::regex secondRegex("second=(\\d+)");
            std::smatch secondMatch;
            std::regex amountRegex("amount=(-?\\d+)");
            std::smatch amountMatch;
            
            std::for_each(std::begin(lines), std::end(lines), [&](std::string line)
                          {
                              if(std::regex_search(line, firstMatch, firstRegex))
                              {
                                  int first = std::stoi(firstMatch[1]);
                                  
                                  std::regex_search(line, secondMatch, secondRegex);
                                  int second = std::stoi(secondMatch[1]);
                                  
                                  std::regex_search(line, amountMatch, amountRegex);
                                  int amount = std::stoi(amountMatch[1]);
                                  
                                  kerningList.push_back(Kerning(first, second, amount));
                              }
                          });
        }
        
        void read_file_into_memory(std::string fntFile)
        {
            std::ifstream file(fntFile);
            
            if(!file)
                throw std::runtime_error("Unable to open fnt file: " + fntFile);
            
            std::string line;
            while(!file.eof())
            {
                std::getline(file, line);
                lines.push_back(line);
            }
            
            
        }
    public:
        BitmapFont() {}
        
        BitmapFont(std::string fntFile)
        {
            load_font(fntFile);
        }
        
        void load_font(std::string fntFile)
        {
            read_file_into_memory(fntFile);

            auto stage1Ftr = std::async(std::launch::async, std::bind(&BitmapFont::stage_1, this));
            auto stage2Ftr = std::async(std::launch::async, std::bind(&BitmapFont::stage_2, this));
            auto stage3Ftr = std::async(std::launch::async, std::bind(&BitmapFont::stage_3, this));
            
            stage1Ftr.get();
            stage2Ftr.get();
            stage3Ftr.get();
            
            lines.clear();
            lines.shrink_to_fit();
        }

		std::vector<Character> characters_from_string(std::string text)
		{
			std::vector<Character> characters;

			if (!text.empty())
				for (auto c : text)
				{
					auto loc = charCharMap.find(c);
					if (loc == charCharMap.end())
						throw std::runtime_error("Unable to find character = " + c);
					characters.push_back(loc->second);
				}

				return characters;
		}
        
        std::vector<Glyph> glyphs_from_string(std::string text)
        {
            std::vector<Glyph> glyphs;
            glyphs.reserve(text.length());
            
            for(auto b = std::begin(text); b != std::end(text); ++b)
            {
                auto i = charCharMap.find(*b);
                
                if(i != std::end(charCharMap))
                {
                    Character c = i->second;
                    QuadTexture qt = make_quad_texture(c.normXLeft, c.normXWidth, c.normYTop, c.normYHeight);
                    int xAdvance = c.xAdvance;
                    
                    glyphs.push_back(Glyph());
                    Glyph &g = glyphs.back();
                    g.qt = qt;
                    g.translateNextChar = xAdvance;
                }
            }
            
            return glyphs;
        }
        
        std::string get_image_file() const {return imageFile;}
        
        
    };
    
	class TextRenderer
	{
		enum { VBO = 0, TBO = 1, VERTICES_PER_QUAD = 6, MAX_CHARS = 256 };
		enum Attribs {pt, tc};
		std::array<GLuint, 2> buffers;
		GLuint tex, vao;
		std::vector<knu::Math::Vector3f> vertices;
		std::vector<knu::Math::Vector2f> texcoords;
		BitmapFont bf;
		knu::Math::Vector3f cursor, renderCursor;

		void setup_buffers()
		{
			glGenBuffers(2, buffers.data());
			int vertexBufferSize = MAX_CHARS * VERTICES_PER_QUAD * sizeof(knu::Math::Vector3f);
			int texcoordBufferSize = MAX_CHARS * VERTICES_PER_QUAD * sizeof(knu::Math::Vector2f);

			glBindBuffer(GL_ARRAY_BUFFER, buffers[VBO]);
			glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, buffers[TBO]);
			glBufferData(GL_ARRAY_BUFFER, texcoordBufferSize, nullptr, GL_DYNAMIC_DRAW);
		}

		void setup_vao()
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			
			glBindBuffer(GL_ARRAY_BUFFER, buffers[VBO]);
			//glVertexAttribFormat(pt, 3, GL_FLOAT, GL_FALSE, 0);
			//glVertexAttribBinding(pt, 0);
			glVertexAttribPointer(pt, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(pt);

			//glVertexAttribFormat(tc, 2, GL_FLOAT, GL_FALSE, 0);
			//glVertexAttribBinding(tc, 1);
			glBindBuffer(GL_ARRAY_BUFFER, buffers[TBO]);
			glVertexAttribPointer(tc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(tc);

			glBindVertexArray(0);
		}

		void setup_texture(std::string fileName)
		{
			// Only supports tga files for now
			TGA tga;
			tga.open_file(fileName);
			
			GLuint format;
			GLuint internalFormat;
			if (tga.bytesPerPixel == 4)
			{
				format = GL_RGBA;
				internalFormat = GL_RGBA8;
			}
			else if (tga.bytesPerPixel == 3)
			{
				format = GL_RGB;
				internalFormat = GL_RGB8;
			}
			else
				throw std::runtime_error("Unsupported tga format. Only 32 bits or 24 bits supported");
			
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tga.imageWidth, tga.imageHeight, 0, format, GL_UNSIGNED_BYTE, tga.imageData.data());


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		}

		void make_square(std::vector<knu::Math::Vector3f> &v, std::vector<knu::Math::Vector2f> &t, const Character &charId)
		{
			float left = renderCursor.x + charId.xOffset;
			float right = left + charId.width;
			float top = renderCursor.y - charId.yOffset;
			float bottom = renderCursor.y - charId.height - charId.yOffset ;

			auto vecBottomLeft = knu::Math::Vector3f(left, bottom, 0);
			auto vecBottomRight = knu::Math::Vector3f(right, bottom, 0);
			auto vecTopLeft = knu::Math::Vector3f(left, top, 0);
			auto vecTopRight = knu::Math::Vector3f(right, top, 0);

			auto texBottomLeft = knu::Math::Vector2f(charId.normXLeft, charId.normYHeight);
			auto texBottomRight = knu::Math::Vector2f(charId.normXWidth, charId.normYHeight);
			auto texUpperLeft = knu::Math::Vector2f(charId.normXLeft, charId.normYTop);
			auto texUpperRight = knu::Math::Vector2f(charId.normXWidth, charId.normYTop);

			v.push_back(vecBottomLeft);
			v.push_back(vecTopRight);
			v.push_back(vecTopLeft);
			v.push_back(vecTopRight);
			v.push_back(vecBottomLeft);
			v.push_back(vecBottomRight);

			t.push_back(texBottomLeft);
			t.push_back(texUpperRight);
			t.push_back(texUpperLeft);
			t.push_back(texUpperRight);
			t.push_back(texBottomLeft);
			t.push_back(texBottomRight);

			renderCursor.x += charId.xAdvance;
		}

		void grab_vertiuces(std::string textToRender, std::vector<knu::Math::Vector3f> &v, std::vector<knu::Math::Vector2f> &t)
		{
			auto vc = bf.characters_from_string(textToRender);

			for (auto c : vc)
			{
				make_square(v, t, c);
			}
		}

		void copy_vertices_to_buffer(std::vector<knu::Math::Vector3f> &v, std::vector<knu::Math::Vector2f> &t)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffers[VBO]);
			knu::Math::Vector3f *p1= (knu::Math::Vector3f*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(p1, v.data(), v.size() * sizeof(knu::Math::Vector3f));
			glUnmapBuffer(GL_ARRAY_BUFFER);

			glBindBuffer(GL_ARRAY_BUFFER, buffers[TBO]);
			knu::Math::Vector2f *p2 = (knu::Math::Vector2f*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(p2, t.data(), t.size() * sizeof(knu::Math::Vector2f));
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}

		void prepare_resources(std::string textToRender)
		{
			renderCursor = cursor;
			grab_vertiuces(textToRender, vertices, texcoords);
			copy_vertices_to_buffer(vertices, texcoords);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(texUniform, 0);
		}

		void clear_resources()
		{
			renderCursor = cursor;
			vertices.clear();
			texcoords.clear();
		}

		void draw_text(int verticesCount)
		{
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, verticesCount);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}

	public:
		TextRenderer()
		{
			vertices.reserve(MAX_CHARS * VERTICES_PER_QUAD);
			texcoords.reserve(MAX_CHARS * VERTICES_PER_QUAD);
		}

		~TextRenderer()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDeleteBuffers(2, buffers.data());
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &tex);
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &vao);
		}
        
		bool load_font(std::string fontFile, std::string pathOfTexture)
		{
			try {
				bf.load_font(fontFile);
				setup_texture(pathOfTexture + "/" + bf.get_image_file());
			}
			catch (std::exception &ex)
			{
				ex;
				return false;
			}

			setup_buffers();
			setup_vao();

			return true;
		}

		void set_cursor(knu::Math::Vector3f c)
		{
			cursor = c;
		}

		void render_string(std::string textToRender)
		{
			if (textToRender.length() > MAX_CHARS)
			{
				textToRender.assign(textToRender.begin(), textToRender.begin() + MAX_CHARS);
			}

			prepare_resources(textToRender);
			draw_text(textToRender.length() * VERTICES_PER_QUAD);
			clear_resources();
		}
    };
}

#endif
