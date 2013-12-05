//
//  gl_utility.h
//  OpenGL Instancing
//
//  Created by Brian Jones on 9/11/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef OpenGL_Instancing_gl_utility_h
#define OpenGL_Instancing_gl_utility_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#endif

#include <knu/mathlibrary5.hpp>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <iostream>

namespace knu
{
    namespace gl
    {
        struct Shader
        {
            std::string computeSource;
            std::string computeShaderPath;
            std::string fragmentSource;
            std::string fragmentShaderPath;
            std::string vertexSource;
            std::string vertexShaderPath;
            std::string geometrySource;
            std::string geometryShaderPath;
            std::vector<std::string> uniforms;
            std::vector<std::string> attributes;
        };
        
        class Program
        {
            GLuint object;
            GLuint fragmentShader;
            GLuint vertexShader;
            GLuint geometryShader;
            GLuint computeShader;
            
            std::string fragmentString;
            std::string vertexString;
            std::string geometryString;
            std::string computeString;
            
            std::unordered_map<std::string, GLint> uniforms;
            std::unordered_map<std::string, GLint> attributes;
            
            
        private:
            
            std::string read_file(std::string fileName)
            {
                std::ifstream file(fileName, std::ios::in);
                if(!file)
                    throw std::runtime_error("Unable to open file: " + fileName);
                
                std::string source, line;
                
                while(!file.eof())
                {
                    std::getline(file, line);
                    source += (line + "\n");
                }
                
                return source;
            }
            
            void compile_shader(GLuint shader)
            {
                glCompileShader(shader);
                GLint success;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                
                if(!success)
                {
                    std::vector<GLchar> errorMsg(256);
                    GLsizei length;
                    glGetShaderInfoLog(shader, (GLsizei)errorMsg.size(), &length, &errorMsg[0]);
                    throw std::runtime_error(std::string(std::begin(errorMsg), std::end(errorMsg)));
                }
            }
            
            void build_program()
            {
                object = glCreateProgram();
                
                if(computeShader)
                {
                    glAttachShader(object, computeShader);
                }
                else
                {
                    if(fragmentShader)
                        glAttachShader(object, fragmentShader);
                    if(vertexShader)
                        glAttachShader(object, vertexShader);
                    if(geometryShader)
                        glAttachShader(object, geometryShader);
                }
                
                link_program();
            }
            
            void link_program()
            {
                glLinkProgram(object);
                
                GLint success;
                glGetProgramiv(object, GL_LINK_STATUS, &success);
                
                if(!success)
                {
                    std::vector<GLchar> errorMsg(256);
                    GLsizei length;
                    glGetProgramInfoLog(object, (GLsizei)errorMsg.size(), &length, &errorMsg[0]);
                    throw std::runtime_error(std::string(std::begin(errorMsg), std::end(errorMsg)));
                }
            }
            
            void build_fragment(Shader properties)
            {
                if(properties.fragmentShaderPath.empty() && properties.fragmentSource.empty() && fragmentString.empty())
                    throw std::runtime_error("No fragment shader");
                
                if(!properties.fragmentShaderPath.empty())
                    fragmentString += read_file(properties.fragmentShaderPath);
                
                if(!properties.fragmentSource.empty())
                    fragmentString += properties.fragmentSource;
                
                fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                const char* source = fragmentString.c_str();
                GLint length = (GLint)fragmentString.size();
                glShaderSource(fragmentShader, 1, &source, &length);
                
                try
                {
                    compile_shader(fragmentShader);
                }catch(std::runtime_error &e)
                {
                    std::cout << e.what() << std::endl;
                    throw std::runtime_error(std::string("Fragment Shader: ") + e.what());
                }
            }
            
            void build_vertex(Shader properties)
            {
                if(properties.vertexShaderPath.empty() && properties.vertexSource.empty() && vertexString.empty())
                    throw std::runtime_error("No vertex shader");
                
                if(!properties.vertexShaderPath.empty())
                    vertexString += read_file(properties.vertexShaderPath);
                
                if(!properties.vertexSource.empty())
                    vertexString += properties.vertexSource;
                
                vertexShader = glCreateShader(GL_VERTEX_SHADER);
                const char* source = vertexString.c_str();
                GLint length = (GLint)vertexString.size();
                glShaderSource(vertexShader, 1, &source, &length);
                
                try
                {
                    compile_shader(vertexShader);
                }catch(std::runtime_error &e)
                {
                    std::cout << e.what() << std::endl;
                    throw std::runtime_error(std::string("Vertex Shader: ") + e.what());
                }
            }
            
            void build_geometry(Shader properties)
            {
                
            }
            
            void build_compute(Shader properties)
            {
                
            }
            
            void resolve_uniforms(Shader properties)
            {
                std::for_each(std::begin(properties.uniforms), std::end(properties.uniforms), [&](std::string u)
                              {
                                  uniforms[u] = glGetUniformLocation(object, u.c_str());
                              });
            }
            
            void resolve_attributes(Shader properties)
            {
                std::for_each(std::begin(properties.attributes), std::end(properties.attributes), [&](std::string a)
                              {
                                  attributes[a] = glGetAttribLocation(object, a.c_str());
                              });
            }
            
        public:
            Program():object(0),
            fragmentShader(0),
            vertexShader(0),
            geometryShader(0),
            computeShader(0){}
            
            void add_fragment_file(std::string fileName)
            {
                fragmentString += read_file(fileName);
            }
            
            void add_fragment_source(std::string fragmentSource)
            {
                fragmentString += fragmentSource;
            }
            
            void add_vertex_file(std::string fileName)
            {
                vertexString += read_file(fileName);
            }
            
            void add_vertex_source(std::string vertexSource)
            {
                vertexString += vertexSource;
            }
            
            void fragment_shader_from_file(std::string fileName)
            {
                fragmentString += read_file(fileName);
            }
            
            void fragment_shader_from_string(std::string fragmentSource)
            {
                fragmentString += fragmentSource;
            }
            
            void vertex_shader_from_file(std::string fileName)
            {
                vertexString += read_file(fileName);
            }
            
            void vertex_shader_from_string(std::string vertexSource)
            {
                vertexString += vertexSource;
            }
            
            void compute_shader_from_file(std::string fileName)
            {
                computeString += read_file(fileName);
            }
            
            void compute_shader_from_source(std::string computeSource)
            {
                computeString += computeSource;
            }
            void make()
            {
                // this function assumes strings have been loaded using fragment_shader_from_string etc functions
                Shader properties;
                make(properties);
            }
            
            
            void make(Shader properties)
            {
                // If either compute source or path is set, process that only as a compute program can only be in a program by itself
                if(!properties.computeShaderPath.empty() || !properties.computeSource.empty())
                {
                    build_compute(properties);
                }
                else
                {
                    build_fragment(properties);
                    build_vertex(properties);
                    build_geometry(properties);
                }
                
                build_program();
                resolve_uniforms(properties);
                resolve_attributes(properties);
                unbind_program();
            }
            
            void bind_program()
            {
                glUseProgram(object);
            }
            
            void unbind_program()
            {
                glUseProgram(0);
            }
            
            GLint get_uniform(std::string u)
            {
                auto i = uniforms.find(u);
                if(i != end(uniforms))
                    return i->second;
                return -1;
            }
            
            GLuint get_id()const
            {
                return object;
            }
            
            
        };
        
        class UniformBufferObject
        {
            
        public:
            UniformBufferObject():
            po(0),
            bkIndex(0),
            bindingIndex(0),
            ub(0)
            {
                
            }
            
            ~UniformBufferObject()
            {
                if(!ub)
                {
                    glBindBuffer(GL_UNIFORM_BUFFER, 0);
                    glDeleteBuffers(1, &ub);
                }
            }
            
            // resolves the indices, and creates a backing buffer object for the uniform block
            void resolve(GLuint programObject, GLuint bindingPoint, std::string blockName,  std::vector<std::string> names)
            {
                resolve_indices(programObject, blockName, names);
                store_index_info();
                setup_buffer();
                bind_to_index(bindingPoint);
            }
            
            void resolve(const Program &po, GLuint bindingpoint, std::string blockName, std::vector<std::string> names)
            {
                resolve(po.get_id(), bindingpoint, blockName, names);
            }
            
            // Rebind this ubo to another program object
            void bind(GLuint programObject, GLuint bindingPoint)
            {
                auto names = get_uniform_names();
                resolve_indices(programObject, bkName, names);
                bind_to_index(bindingPoint);
            }
            
            void bind(Program const &program, GLuint bindingPoint)
            {
                bind(program.get_id(), bindingPoint);
            }
            
            void rebind(GLuint program)
            {
                po = program;
                bind_to_index(bindingIndex);
            }
            void rebind(Program const &program)
            {
                po = program.get_id();
                bind_to_index(bindingIndex);
            }
            
            void *map_buffer()
            {
                glBindBuffer(GL_UNIFORM_BUFFER, ub);
                void *ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
                return ptr;
            }
            
            void unmap_buffer()
            {
                glUnmapBuffer(GL_UNIFORM_BUFFER);
            }
            
            // retrieves the offset of the uniform name in the uniform block
            GLint offset(std::string uniformName)
            {
                UniformInfo ui = get_uniform_info(uniformName);
                return ui.uniformOffset;
            }
            
            template<typename T> void set(std::string uniformName, T data)
            {
                UniformInfo ui = get_uniform_info(uniformName);
                do_update(ui, data);
            }
            
        private:
            struct UniformInfo
            {
                GLint uniformType;
                GLint arrayStride;
                GLint matrixStride;
                GLint uniformOffset;
            };
            
        private:
            void resolve_indices(GLuint programObject, std::string blockName, std::vector<std::string> names)
            {
                // this function resolve the indices the "names" in the uniform block
                po = programObject;
                bkName = blockName;
                bkIndex = glGetUniformBlockIndex(po, bkName.c_str());
                
                if(GL_INVALID_INDEX == bkIndex) throw std::runtime_error("Unable to resolve block name: " + blockName);
                
                std::vector<const GLchar *> cnames(names.size());
                std::vector<GLuint> indices(names.size());
                
                for(std::size_t i = 0; i < names.size(); ++i)
                    cnames[i] = const_cast<char*>(names[i].c_str());
                
                glGetUniformIndices(po, (GLsizei)cnames.size(), &cnames [0], &indices[0]);
                
                // validate all the indices
                for(std::size_t i = 0; i < indices.size(); ++ i)
                {
                    if(GL_INVALID_INDEX == indices[i])
                        throw std::runtime_error("UBO Invalid name: " + names[i]);
                }
                
                for(std::size_t i = 0; i < indices.size(); ++ i)
                {
                    auto p = std::make_pair(indices[i], UniformInfo());
                    nameIndexMap[names[i]] = p;
                }
            }
            
            void bind_to_index(GLuint bindingPoint)
            {
                // bind the ubo and blockIndex to a binding point
                bindingIndex = bindingPoint;
                glUniformBlockBinding(po, bkIndex, bindingIndex);
                glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, ub);
            }
            
            
            void store_index_info()
            {
                // this function retrieves information about each uniform
                for(auto &p : nameIndexMap)
                {
                    std::pair<GLuint, UniformInfo> indexUIMap = p.second;
                    glGetActiveUniformsiv(po, 1, &indexUIMap.first, GL_UNIFORM_OFFSET, &indexUIMap.second.uniformOffset);
                    glGetActiveUniformsiv(po, 1, &indexUIMap.first, GL_UNIFORM_ARRAY_STRIDE, &indexUIMap.second.arrayStride);
                    glGetActiveUniformsiv(po, 1, &indexUIMap.first, GL_UNIFORM_MATRIX_STRIDE, &indexUIMap.second.matrixStride);
                    glGetActiveUniformsiv(po, 1, &indexUIMap.first, GL_UNIFORM_TYPE, &indexUIMap.second.uniformType);
                    p.second = indexUIMap;
                }
            }
            
            std::vector<std::string> get_uniform_names()
            {
                std::vector<std::string> names_;
                
                for(const auto &p : nameIndexMap)
                {
                    names_.push_back(p.first);
                }
                
                return names_;
            }
            
            void setup_buffer()
            {
                GLint blockSize = 0;
                glGetActiveUniformBlockiv(po, bkIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
                
                glGenBuffers(1, &ub);
                glBindBuffer(GL_UNIFORM_BUFFER, ub);
                glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
            
            UniformInfo get_uniform_info(std::string uniformName)
            {
                auto i = nameIndexMap.find(uniformName);
                if(i == std::end(nameIndexMap))
                    throw std::runtime_error("Unable to find uniform name: " + uniformName + " in registered list");
                
                return i->second.second;
            }
            
            void do_update(UniformInfo ui, knu::math::v4f const v)
            {
                int offset = ui.uniformOffset;
                
                glBindBuffer(GL_UNIFORM_BUFFER, ub);
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v4f), &v.x);
            }
            
            void do_update(UniformInfo ui, knu::math::m4f const m)
            {
                int offset = ui.uniformOffset;
                int stride = ui.matrixStride;
                knu::math::v4f row = m.get_row_0();
                
                glBindBuffer(GL_UNIFORM_BUFFER, ub);
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v4f), &row.x);
                
                offset += stride;
                row = m.get_row_1();
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v4f), &row.x);
                
                offset += stride;
                row = m.get_row_2();
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v4f), &row.x);
                
                offset += stride;
                row = m.get_row_3();
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v4f), &row.x);
            }
            
            void do_update(UniformInfo ui, knu::math::m3f const m)
            {
                int offset = ui.uniformOffset;
                int stride = ui.matrixStride;
                knu::math::v3f row = m.get_row_0();
                
                glBindBuffer(GL_UNIFORM_BUFFER, ub);
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v3f), &row.x);
                
                offset += stride;
                row = m.get_row_1();
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v3f), &row.x);
                
                offset += stride;
                row = m.get_row_2();
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(knu::math::v3f), &row.x);
            }
            
        private:
            GLuint                                                              po;                 // program object
            GLuint                                                              bkIndex;
            GLuint                                                              bindingIndex;
            GLuint                                                              ub;                 // uniform buffer
            std::string                                                         bkName;             // nameof the block
            std::unordered_map<std::string, std::pair<GLuint, UniformInfo>>     nameIndexMap;
        };
        
    } // namespace gl
} // namespace knu


#endif
