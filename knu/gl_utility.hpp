#ifndef knu_gl_utility2_hpp
#define knu_gl_utility2_hpp

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif

#ifdef WIN32
#include <Windows.h>
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
#include <stdexcept>

namespace knu
{
    namespace graphics
    {
        class program
        {
            GLuint p_obj;       // the program object
            GLuint v_shader;
            GLuint f_shader;
            GLuint c_shader;    // compute
            GLuint g_shader;    // geometry
            
            std::string v_string_src;
            std::string f_string_src;
            std::string c_string_src;
            std::string g_string_src;
            
            std::unordered_map<std::string, GLuint> uniforms;
            
        private:
            std::string read_file(std::string file_name)
            {
                std::ifstream file(file_name);
                
                if(!file)
                    throw std::runtime_error("Unable to open file: " + file_name);
                
                std::string src(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
                return src;
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
#ifdef WIN32
                    OutputDebugStringA(std::string(std::begin(errorMsg), std::end(errorMsg)).c_str());
#endif
                    throw std::runtime_error(std::string(std::begin(errorMsg), std::end(errorMsg)));
                }
            }
            
            void link_program()
            {
                glLinkProgram(p_obj);
                
                GLint success;
                glGetProgramiv(p_obj, GL_LINK_STATUS, &success);
                
                if(!success)
                {
                    std::vector<GLchar> errorMsg(256);
                    GLsizei length;
                    glGetProgramInfoLog(p_obj, (GLsizei)errorMsg.size(), &length, &errorMsg[0]);
#ifdef WIN32
                    OutputDebugStringA(std::string(std::begin(errorMsg), std::end(errorMsg)).c_str());
#endif
                    throw std::runtime_error(std::string(std::begin(errorMsg), std::end(errorMsg)));
                }
            }
            
            void build_fragment()
            {
                if(f_string_src.empty())
                    throw std::runtime_error("No fragment shader");
                
                f_shader = glCreateShader(GL_FRAGMENT_SHADER);
                const char* source = f_string_src.c_str();
                GLint length = (GLint)f_string_src.size();
                glShaderSource(f_shader, 1, &source, &length);
                
                try
                {
                    compile_shader(f_shader);
                }catch(std::runtime_error &e)
                {
                    std::cout << e.what() << std::endl;
                    throw std::runtime_error(std::string("Fragment Shader: ") + e.what());
                }
                
                glAttachShader(p_obj, f_shader);
                glDeleteShader(f_shader);
            }
            
            void build_vertex()
            {
                if(v_string_src.empty())
                    throw std::runtime_error("No vertex shader");
                
                v_shader = glCreateShader(GL_VERTEX_SHADER);
                const char* source = v_string_src.c_str();
                GLint length = (GLint)v_string_src.size();
                glShaderSource(v_shader, 1, &source, &length);
                
                try
                {
                    compile_shader(v_shader);
                }catch(std::runtime_error &e)
                {
                    std::cout << e.what() << std::endl;
                    throw std::runtime_error(std::string("Vertex Shader: ") + e.what());
                }
                
                glAttachShader(p_obj, v_shader);
                glDeleteShader(v_shader);
            }
            
            void build_program()
            {
                p_obj = glCreateProgram();
                
                if(!c_string_src.empty())
                {
                    //glAttachShader(object, computeShader);
                }
                else
                {
                    if(!f_string_src.empty())
                        build_fragment();
                    if(!v_string_src.empty())
                        build_vertex();
                }
                
                link_program();
            }
            
            void retrieve_active_uniforms()
            {
                glUseProgram(p_obj);
                GLint uniformCount;
                glGetProgramiv(p_obj, GL_ACTIVE_UNIFORMS, &uniformCount);
                
                const int MAX_CHARS = 256;
                
                GLsizei length;
                GLint size;
                GLenum type;
                
                for (int g = 0; g < uniformCount; ++g)
                {
                    std::vector<GLchar> un(MAX_CHARS); // uniform name
                    glGetActiveUniform(p_obj, g, MAX_CHARS, &length, &size, &type, &un[0]);
                    std::string str(un.begin(), un.begin() + length);
                    GLint loc = glGetUniformLocation(p_obj, str.c_str());
                    
                    // -1 is returned for uniforms in uniform blocks
                    if(loc != -1)
                        uniforms[str] = loc;
                    un.clear();
                }
            }
            
            void resolve_uniforms()
            {
                retrieve_active_uniforms();
            }
            
        public:
            
            inline GLuint obj() const
            {
                return p_obj;
            }
            
            void add_fragment_file(std::string file_name)
            {
                f_string_src = read_file(file_name);
            }
            
            void add_vertex_file(std::string file_name)
            {
                v_string_src = read_file(file_name);
            }
            
            void build()
            {
                build_program();
                resolve_uniforms();
            }
            
            void bind()
            {
                glUseProgram(p_obj);
            }
            
            GLuint uniform(std::string name)
            {
                auto i = uniforms.find(name);
                
                if(i == uniforms.end())
                    throw std::runtime_error("Unable to find uniform name: " + name);
                
                return i->second;
            }
        };
        
        template<typename t>
        class buffer
        {
            GLuint id;
            GLenum target;
            GLenum usage;
            
        public:
            buffer():id(0), target(GL_ARRAY_BUFFER), usage(GL_STATIC_DRAW)
            {

            }

            buffer(unsigned int count):id(0), target(GL_ARRAY_BUFFER), usage(GL_STATIC_DRAW)
            {
                glGenBuffers(1, &id);
                glBindBuffer(GL_ARRAY_BUFFER, id);
                glBufferData(GL_ARRAY_BUFFER, sizeof(t) * count, nullptr, usage);
            }
            
            buffer(GLenum target, GLenum usage):id(0), target(target), usage(usage)
            {
                
            }
            
            buffer(GLenum target, unsigned int count, GLenum usage):id(0), target(target), usage(usage)
            {
                glGenBuffers(1, &id);
                
                glBindBuffer(target, id);
                glBufferData(target, sizeof(t) * count, nullptr, usage);
            }
            
            buffer(GLenum target, unsigned int count, t* data, GLenum usage):id(0), target(target), usage(usage)
            {
                glGenBuffers(1, &id);
                
                glBindBuffer(target, id);
                glBufferData(target, sizeof(t) * count, data, usage);
            }
            
            ~buffer()
            {
                glDeleteBuffers(1, &id);
            }
            
            // No copy constructor or assignment
            buffer<t>(const buffer<t> &) = delete;
            buffer<t> &operator=(const buffer<t> &) = delete;
            
            inline void bind()
            {
                glBindBuffer(target, id);
            }
            
            inline GLuint obj()
            {
                return id;
            }
            
            void allocate(unsigned int count)
            {
                if(!id)
                {
                    glGenBuffers(1, &id);
                }
                
                bind();
                glBufferData(target, sizeof(t) * count, nullptr, usage);
            }
            
            void allocate(std::vector<t> &v)
            {
                if(!id)
                {
                    glGenBuffers(1, &id);
                }
                
                bind();
                glBufferData(target, sizeof(t) * v.size(), v.data(), usage);
            }
            
            void insert(GLintptr offset, GLsizeiptr byte_size ,t* array)
            {
                bind();
                glBufferSubData(target, offset, byte_size, array);
            }
            
            void insert(std::vector<t> &v)
            {
                bind();
                glBufferSubData(target, 0, v.size() * sizeof(t), v.data());
            }
            
            void set_target(GLenum target, GLenum usage)
            {
                this->target = target;
                this->usage = usage;
            }
            
            inline t* map(GLenum flags) {return glMapBuffer(target, flags);}
            inline void unmap() {glUnmapBuffer(target);}
        };
        
        enum class vao
        {
            vertex, vertex_texture, vertex_texture_normal, vertex_normal, vertex_color
        };
        
        class vertex_array_object
        {
        private:
            std::vector<GLuint> ids;
            
        public:
            vertex_array_object():ids() {}
            
            ~vertex_array_object()
            {
                if(!ids.empty())
                    glDeleteVertexArrays((GLsizei)ids.size(), &ids[0]);
            }
            
            // No copy constructor or assignment
            vertex_array_object(const vertex_array_object &) = delete;
            vertex_array_object &operator=(const vertex_array_object &) = delete;
            
            template<typename ... offsets>
            void set(vao type, GLsizei stride, std::tuple<offsets...> at)
            {
                if(ids.empty())
                {
                    ids.push_back(0);
                    glGenVertexArrays(1, &ids[0]);
                }
                
                switch(type)
                {
                    case vao::vertex:
                    {
                        
                    }break;
                        
                    case vao::vertex_texture:
                    {
                        
                        bind();
                        
                        GLsizei vertex_offset, texture_offset;
                        
                        std::tie(vertex_offset, texture_offset) = at;
                        
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(vertex_offset));
                        glEnableVertexAttribArray(0);
                        
                        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(texture_offset));
                        glEnableVertexAttribArray(1);
                        unbind();
                    }break;
                        
                    case vao::vertex_texture_normal:
                    {
                        
                    }break;
                        
                    case vao::vertex_normal:
                    {
                        
                    }break;
                        
                    case vao::vertex_color:
                    {
                        bind();
                        
                        GLsizei vertex_offset, color_offset;
                        std::tie(vertex_offset, color_offset) = at;
                        
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(vertex_offset));
                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(color_offset));
                        glEnableVertexAttribArray(4);
                        unbind();
                    }
                }
            }
            
            void bind()
            {
                glBindVertexArray(ids[0]);
            }
            
            void unbind()
            {
                glBindVertexArray(0);
            }
        };
        
        
    }
}

#endif