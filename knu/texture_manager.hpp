//
//  texture_manager.hpp
//  SimpleTexture
//
//  Created by Brian Jones on 4/13/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#ifndef knu_texture_manager_hpp
#define knu_texture_manager_hpp

#include <unordered_map>
#include <memory>
#include <string>

namespace knu
{
    namespace graphics
    {
        template<typename Type>
        class Texture_Manager
        {
            std::unordered_map<std::string, std::unique_ptr<Type>> stringTextureMap;
            
            Texture_Manager<Type>(const Texture_Manager<Type> &other) = delete;
            Texture_Manager<Type> &operator=(const Texture_Manager<Type> &other) = delete;
            
        private:
            std::string strip_path_and_extension(std::string fileName)
            {
                unsigned long beginningOfFileName = 0;
                auto backslashPos = fileName.find_last_of("\\");
                if(backslashPos != fileName.npos)
                {
                    // Windows style file path
                    beginningOfFileName = backslashPos + 1; // +1 so that it doesn't point exactly at the \
                    
                }
                else{
                    // Try forward slash (Unix)
                    auto forwardSlashPos = fileName.find_last_of("/");
                    if(forwardSlashPos != fileName.npos)
                        beginningOfFileName = forwardSlashPos + 1;// +1 so that it doesn't point exactly at the /
    
                }
                
                auto periodPos = fileName.find_last_of(".");
                if(periodPos == fileName.npos)
                    periodPos = fileName.length();
                
                auto name = fileName.substr(beginningOfFileName, periodPos - beginningOfFileName);
                
                return name;
            }
        public:
            Texture_Manager<Type>() {}
            
            std::string add(std::string fileName)
            {
                std::unique_ptr<Type> texture(new Type(fileName));
                
                std::string name = strip_path_and_extension(fileName);
                
                stringTextureMap[name] = std::move(texture);
                
                return name;
            }
            
            Type& reference(std::string name)
            {
                return *stringTextureMap[name];
            }
            
            bool is_member(std::string name)
            {
                return stringTextureMap.find(name) != std::end(name);
            }
            
            void remove(std::string name)
            {
                auto pos = stringTextureMap.find(name);
                stringTextureMap.remove(pos);
            }
        };
    }
}
#endif
