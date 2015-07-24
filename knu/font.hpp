#ifndef knu_font_hpp
#define knu_font_hpp

#include <vector>
#include <unordered_map>
#include <regex>
#include <future>
#include <fstream>
#include <knu/texture.hpp>

namespace knu
{
	namespace graphics
	{
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
				normXLeft = x / (float)scaleW;
				normXWidth = (x + w) / (float)scaleW;
				normYBottom = ((y) / (float)scaleH);
				normYHeight = ((y + h) / (float)scaleH);
			}
		};

		class AngelCodeBitmapLoader
		{
			struct Kerning
			{
				int firstChar;
				int secondChar;
				int amount;

				Kerning(int char1, int char2, int amt) :
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

			void stage_1()
			{
				std::regex fontSizeRegex("size=(\\d+)");
				std::regex scaleWRegex("scaleW=(\\d+)");
				std::regex scaleHRegex("scaleH=(\\d+)");
				std::regex fileRegex("file=\"(\\w+\\.\\w+)\"");

				std::for_each(std::begin(lines), std::end(lines), [&](std::string line)
				{
					std::smatch scaleWMatch;
					if (std::regex_search(line, scaleWMatch, scaleWRegex))
					{
						scaleW = std::stoi(scaleWMatch[1]);
					}

					std::smatch scaleHMatch;
					if (std::regex_search(line, scaleHMatch, scaleHRegex))
					{
						scaleH = std::stoi(scaleHMatch[1]);
					}

					std::smatch fileMatch;
					if (std::regex_search(line, fileMatch, fileRegex))
					{
						this->imageFile = fileMatch[1];
					}

					std::smatch fontSizeMatch;
					if (std::regex_search(line, fontSizeMatch, fontSizeRegex))
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
				std::regex yOffsetRegex("yoffset=(-?\\d+)");
				std::smatch yOffsetMatch;
				std::regex xAdvanceRegex("xadvance=(\\d+)");
				std::smatch xAdvanceMatch;


				std::for_each(std::begin(lines), std::end(lines), [&](std::string line) {

					if (std::regex_search(line, charIDMatch, charIDRegex))
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
				});		// end of for_each
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
					if (std::regex_search(line, firstMatch, firstRegex))
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

				if (!file)
					throw std::runtime_error("Unable to open fnt file: " + fntFile);

				std::string line;
				while (!file.eof())
				{
					std::getline(file, line);
					lines.push_back(line);
				}
			}

		public:
			AngelCodeBitmapLoader() {}

			AngelCodeBitmapLoader(std::string fntFile)
			{
				load_font(fntFile);
			}

			int width_of_character(const char c) const
			{
				auto loc = charCharMap.find(c);
				if (loc == std::end(charCharMap))
					throw std::runtime_error(std::string("Unable to find character: ") + c);
				return loc->second.width;
			}

			int width_of_string(std::string s) const
			{
				if (s.empty())
					return 0;

				int length = 0;

				for (auto c : s)
					length += width_of_character(c);

				return length;
			}

			void load_font(std::string fntFile)
			{
				read_file_into_memory(fntFile);

				auto stage1Ftr = std::async(std::launch::async, std::bind(&AngelCodeBitmapLoader::stage_1, this));
				auto stage2Ftr = std::async(std::launch::async, std::bind(&AngelCodeBitmapLoader::stage_2, this));
				auto stage3Ftr = std::async(std::launch::async, std::bind(&AngelCodeBitmapLoader::stage_3, this));
	

				stage1Ftr.get();
				stage2Ftr.get();
				stage3Ftr.get();


				lines.clear();
				lines.shrink_to_fit();
			}

			std::vector<Character> characters_from_string(std::string text) const
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

			std::string get_image_file() const { return imageFile; }

			int font_size() const { return fontSize; }
		};

		class Font
		{
			AngelCodeBitmapLoader loader;

		public:
			knu::graphics::texture2d texture;

		public:
			Font() : loader() {}
			Font(std::string fontFile, std::string texturePath) : loader(fontFile) {
				if (!texture.load_texture(texturePath + loader.get_image_file(),
					GL_LINEAR, GL_LINEAR, GL_FALSE)) throw std::runtime_error("Unable to load texture: " + texturePath + loader.get_image_file());
			}

			~Font() {}

			void load_font(std::string fontFile, std::string texturePath)
			{
				loader.load_font(fontFile);
				if (!texture.load_texture(texturePath + loader.get_image_file(), GL_LINEAR, GL_LINEAR, GL_FALSE))
					throw std::runtime_error("Unable to load texture: " + texturePath + loader.get_image_file());
			}

			int char_width(char c) const
			{
				return loader.width_of_character(c);
			}

			int string_width(std::string str) const
			{
				return loader.width_of_string(str);
			}

			std::vector<Character> characters(std::string str) const
			{
				return loader.characters_from_string(str);
			}

			int font_size() const
			{
				return loader.font_size();
			}
		};
	}
}

#endif
