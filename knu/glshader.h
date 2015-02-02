//
//  GLShader.h
//  MacOGL
//
//  Created by Brian Jones on 3/4/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef MacOGL_GLShader_h
#define MacOGL_GLShader_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#endif

#include <vector>
#include <map>
#include <string>

namespace knu
{
    class GLShader;
    class GLCompiler;
    class GLLinker;
    class GLProgram;
    
	struct PP	// Program properties
	{
		std::string vertexPath;
		std::string fragmentPath;
		std::string computePath;
		std::vector<std::string> attributeNames;
		std::vector<std::string> uniformNames;
	};


    // Helper functions
    std::string buildProgram(std::string vertexPath, std::string fragmentPath,
                             std::vector<std::string> attributeNames,
                             std::vector<std::string> uniformNames,
                             GLProgram &program);

	// this version of buildProgram implies that attribute ids are generated in the shader
	GLProgram buildProgram(PP programProperties);

	// this build only compute shaders (for MS windows for now)
	GLProgram buildCompute(PP programProperties);

    
    
    class GLShader
    {
        GLint shaderObject;
        std::string source;
        
    private:
        GLShader &operator=(GLShader &);
        
		GLShader(GLShader &);
    public:
              // Delegating constructor
		GLShader(GLint shaderType)
        {
			shaderObject = glCreateShader(shaderType);
			source = "";
        }
        
        GLShader(GLenum shaderType, std::string sourceCode)
        {
            shaderObject = glCreateShader(shaderType);
            addSource(sourceCode);
        }
        
        ~GLShader()
        {
            glDeleteShader(shaderId());
        }
        
        void addSource(std::string sourceCode)
        {
            source += ("\n" + sourceCode);
            const char *cString = source.c_str();
            GLint length = (GLint)source.length();
            glShaderSource(shaderObject, 1, &cString, &length);
        }
        
        void resetSource()
        {
            source.clear();
        }
        
        GLint shaderId()const
        {
            return shaderObject;
        }
    };
    
    class GLProgram
    {
        GLuint programObject;
        static GLint attributeIndexCounter;
        std::map<std::string, GLint> attributeMap;
        std::map<std::string, GLint> uniformMap;
        
    public:
        GLProgram(const GLShader &s1, const GLShader &s2)
        :programObject(0)
        {
			create_program();
            attach_shaders(s1, s2);
        }
        
        GLProgram():
        programObject(0)
        {
            
        }
        
        GLProgram(const GLProgram &p):
        programObject(p.programObject),
        attributeMap(p.attributeMap),
        uniformMap(p.uniformMap) { }
        
        GLProgram &operator=(const GLProgram &p)
        {
            programObject = p.programObject;
            attributeMap = p.attributeMap;
            uniformMap = p.uniformMap;
            return *this;
        }
        
        void create_program()
        {
            programObject = glCreateProgram();
        }

		void attach_shader(const GLShader &s)
		{
			glAttachShader(programObject, s.shaderId());
		}
        
        void attach_shaders(const GLShader &s1, const GLShader &s2)
        {
            glAttachShader(programObject, s1.shaderId());
            glAttachShader(programObject, s2.shaderId());
        }
        
        void detatch_shader(const GLShader &s)
        {
            glDetachShader(programObject, s.shaderId());
        }
        
        GLint attribute_id(std::string attributeName)
        {
            auto i = attributeMap.find(attributeName);
            
            if(i == std::end(attributeMap))
                throw std::runtime_error("Unable to find attribute : " + attributeName);
            
            return i->second;
        }
        
        GLint uniform_id(std::string uniformName)
        {
            auto i = uniformMap.find(uniformName);
            
            if(i == std::end(uniformMap))
                throw std::runtime_error("Unable to find uniform : " + uniformName);
            
            return i->second;
        }
        
        // Pre link
        void register_attributes(std::vector<std::string> attributes);
        
        // Post link
        void bind_attributes(std::vector<std::string> attributes);
        
        void register_uniforms(std::vector<std::string> uniforms);
        
        inline void bind_program() { glUseProgram(programObject); }
        
        inline void unbind_program() { glUseProgram(0); }
        
        inline GLint program_id() const { return programObject; }
    };

    class GLCompiler
    {
    public:
        static std::string compileShader(const GLShader &shader, bool *success);
    };
    
    class GLLinker
    {
    public:
        static std::string linkProgram(const GLProgram &program, bool *success);
    };
    
    class GLValidator
    {
    public:
        static std::string validateProgram(const GLProgram &program, bool *succdss);
    };

}



#endif
