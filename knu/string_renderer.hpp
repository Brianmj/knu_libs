//
//  string_renderer.hpp
//  String Renderer
//
//  Created by Brian Jones on 11/28/13.
//
//

#ifndef KNU_STRINGRENDERER
#define KNU_STRINGRENDERER

#include <knu/mathlibrary5.hpp>
#include <regex>
#include <future>
#include <stdexcept>
#include <string>

namespace knu
{
    namespace gl
    {
        struct TextureCoordinates
        {
            knu::math::v2f                         topLeft;
            knu::math::v2f                         topRight;
            knu::math::v2f                         bottomLeft;
            knu::math::v2f                         bottomRight;
        };
        
        struct Character
        {
            char                                    charId;
            int                                     xPos;
            int                                     yPos;
            int                                     width;
            int                                     height;
            int                                     xOffset;
            int                                     yOffset;
            int                                     xAdvance;
            float                                   normXLeft;
            float                                   normYBottom;
            float                                   normXWidth;
            float                                   normYHeight;
            
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
                normYBottom =  ((y) / (float) scaleH);
                normYHeight =   ((y + h) / (float) scaleH);
            }
        };

        
        class AngelCodeLoader
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
            int fontSize;
            std::string imageFile;
            std::vector<std::string> lines;
            std::unordered_map<char, Character> charCharMap;
            std::vector<Kerning> kerningList;
            
            TextureCoordinates make_quad_texture(float x1, float x2, float y1, float y2)
            {
                TextureCoordinates qt;
                qt.topLeft = knu::math::Vector2f(x1, y1);
                qt.topRight = knu::math::Vector2f(x2, y1);
                qt.bottomLeft = knu::math::Vector2f(x1, y2);
                qt.bottomRight = knu::math::Vector2f(x2, y2);
                
                return qt;
            }
            
            // read the fnt face info
            void stage_1()
            {
                std::regex fontSizeRegex("size=(\\d+)");
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
                                  
                                  std::smatch fontSizeMatch;
                                  if(std::regex_search(line, fontSizeMatch, fontSizeRegex))
                                  {
                                      this->fontSize = std::stoi(fontSizeMatch[1]);
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
            AngelCodeLoader() {}
            
            AngelCodeLoader(std::string fntFile)
            {
                load_font(fntFile);
            }
            
            int width_of_character(const char c)
            {
                auto loc = charCharMap.find(c);
                if(loc == std::end(charCharMap))
                    throw std::runtime_error(std::string("Unable to find character: ") + c);
                return loc->second.width;
            }
            
            int width_of_string(std::string s)
            {
                if(s.empty())
                    return 0;
                
                int length = 0;
                
                for(auto c : s)
                    length += width_of_character(c);
                
                return length;
            }
            
            void load_font(std::string fntFile)
            {
                read_file_into_memory(fntFile);
                
                auto stage1Ftr = std::async(std::launch::async, std::bind(&AngelCodeLoader::stage_1, this));
                auto stage2Ftr = std::async(std::launch::async, std::bind(&AngelCodeLoader::stage_2, this));
                auto stage3Ftr = std::async(std::launch::async, std::bind(&AngelCodeLoader::stage_3, this));
                
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
                            throw std::runtime_error(std::string("Unable to find character = ") + c);
                        characters.push_back(loc->second);
                    }
                
				return characters;
            }
            
            std::string get_image_file() const {return imageFile;}
            
            int font_size() const { return fontSize; }
        };
        
#ifndef DO_NOT_LOAD_STRING_RENDERER_PORTION
#define DO_NOT_LOAD_STRING_RENDERER_PORTION
        
#include <knu/image.hpp>
#include <knu/gl_utility.hpp>
        
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
        inline std::string make_args_to_string<std::string>(std::string s){ return s; }
        
        template<typename T1, typename ... Types>
        inline std::string make_args_to_string(T1 val1, Types ...vals)
        {
            std::string s;
            s += make_into_string(val1) + " " + make_args_to_string(vals...);
            return s;
        }
        
        class StringRenderer
        {
        public:
            StringRenderer() { }
            StringRenderer(GLuint program, std::string fontName, std::string texturePath)
            {
                create(program, fontName, texturePath);
            }
            
            StringRenderer(gl::Program const &program, std::string fontName, std::string texturePath)
            {
                create(program, fontName, texturePath);
            }
            
            ~StringRenderer()
            {
                
            }
            
            void create(GLuint program, std::string fontName, std::string texturePath)
            {
                this->program = program;
                loader.load_font(fontName);
                setup_texture(texturePath);
                setup_buffers();
                setup_vao();
            }
            
            void create(gl::Program const &program, std::string fontName, std::string texturePath)
            {
                create(program.get_id(), fontName, texturePath);
            }
            
            void origin(math::v3f origin)
            {
                cursor = origin;
            }
            
            void matrices(math::m4f const &newProjection, math::m4f const &newModelView)
            {
                projection = newProjection;
                modelView = newModelView;
                transformBuffer.set("projection", projection);
                transformBuffer.set("model_view", modelView);
            }
            
            template<typename ... Args>
            void draw_string(Args ...args)
            {
                std::string str = make_args_to_string(args...);
                // make sure str isn't > MAX_CHARACTERS
                if(str.length() > MAX_CHARACTERS)
                    str.assign(std::begin(str), std::begin(str) + MAX_CHARACTERS);
                
                string_to_buffer(str);
                draw_text((int)str.length());
            }
            
            int font_size() const { return loader.font_size(); }
            
        private:
            void setup_texture(std::string texturePath)
            {
                std::string fileName = texturePath.empty() ? (loader.get_image_file()) : (texturePath + "/" + loader.get_image_file());
                texture = knu::image::make_texture(fileName, GL_LINEAR, GL_LINEAR);
                texUniform = glGetUniformLocation(program, "tex");
            }
            
            void setup_buffers()
            {
                GLsizei tBufSize = MAX_CHARACTERS * VERTICES_PER_QUAD * sizeof(math::v2f);
                GLsizei vBufSize = MAX_CHARACTERS * VERTICES_PER_QUAD * sizeof(math::v3f);
                
                glGenBuffers(2, buffers);
                glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);      // buffer for vertices
                glBufferData(GL_ARRAY_BUFFER, vBufSize, nullptr, GL_DYNAMIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);      // buffers for texture coordinates
                glBufferData(GL_ARRAY_BUFFER, tBufSize, nullptr, GL_DYNAMIC_DRAW);
                
                transformBuffer.resolve(program, 0, "Transform", {"projection", "model_view"});
                projection = math::utility::make_ortho<float>(0, 1024, 768, 0, 0.1, 100.0);
                modelView.identity();
                transformBuffer.set("projection", projection);
                transformBuffer.set("model_view", modelView);
            }
            
            void setup_vao()
            {
                glGenVertexArrays(1, &vao);
                glBindVertexArray(vao);
                
                glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                
                glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                
                glBindVertexArray(0);
            }
            
            void ready_tex_buffer(std::vector<Character> characters)
            {
                glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
                math::v2f *ptr = reinterpret_cast<math::v2f *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, VERTICES_PER_QUAD * sizeof(math::v2f) * characters.size(), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT));
                
                for(auto const &c : characters)
                {
                    math::v2f lowerLeft     =   {c.normXLeft, c.normYBottom};
                    math::v2f lowerRight    =  {c.normXWidth, c.normYBottom};
                    math::v2f upperLeft     =   {c.normXLeft, c.normYHeight};
                    math::v2f upperRight    =   {c.normXWidth, c.normYHeight};
                    
                    *ptr++                  = upperLeft;
                    *ptr++                  = lowerLeft;
                    *ptr++                  = lowerRight;
                    
                    *ptr++                  = lowerRight;
                    *ptr++                  = upperRight;
                    *ptr++                  = upperLeft;
                }
                
                glUnmapBuffer(GL_ARRAY_BUFFER);
                ptr = nullptr;
            }
            
            void ready_vertex_buffer(std::vector<Character> const &characters)
            {
                glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
                math::v3f *ptr = reinterpret_cast<math::v3f *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, VERTICES_PER_QUAD * sizeof(math::v3f) * characters.size(), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT));
                
                math::v3f origin = cursor;
                
                for(auto const &c : characters)
                {
                    math::v3f lowerLeft = origin + math::v3f(c.xOffset,                  (-c.yOffset), 0.0f);
                    math::v3f lowerRight = origin + math::v3f(c.xOffset + c.width,       (-c.yOffset), 0.0);
                    math::v3f upperRight = origin + math::v3f(c.xOffset + c.width,     (-c.height) + (-c.yOffset), 0.0);
                    math::v3f upperLeft = origin + math::v3f(c.xOffset,                 (-c.height) +(-c.yOffset), 0.0f);
                    
                    *ptr++                  = upperLeft;                // first triangle
                    *ptr++                  = lowerLeft;
                    *ptr++                  = lowerRight;
                    
                    *ptr++                  = lowerRight;               // second triangle
                    *ptr++                  = upperRight;
                    *ptr++                  = upperLeft;
                    
                    origin.x += c.xAdvance;
                }
                
                glUnmapBuffer(GL_ARRAY_BUFFER);
                ptr = nullptr;
            }
            
            void string_to_buffer(std::string str)
            {
                std::vector<Character> characters = loader.characters_from_string(str);
                ready_tex_buffer(characters);
                ready_vertex_buffer(characters);
                transformBuffer.rebind(program);         // keep transform uniform at binding 0
            }
            
            void draw_text(int characterCount)
            {
                int verticesCount = characterCount * VERTICES_PER_QUAD;
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
                
                glUseProgram(program);
                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDepthMask(false);
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, verticesCount);
                glBindVertexArray(0);
                glDepthMask(true);
                glDisable(GL_BLEND);
                glUseProgram(0);
            }
            
            
            
        private:
            enum {VERTICES_PER_QUAD = 6, MAX_CHARACTERS = 256};
            
            AngelCodeLoader         loader;
            GLuint                  vao;
            GLuint                  program;
            GLuint                  buffers[2];
            GLuint                  texture;
            
            GLint                   texUniform;
            
            math::v3f               cursor;
            
            math::m4f               projection;
            math::m4f               modelView;
            gl::UniformBufferObject transformBuffer;
        };
        
#endif              // DO_NOT_LOAD_STRING_RENDERER_PORTION
        
        
    }       // namespace gl
}       // namespace knu

#endif  // STRINGRENDERER
