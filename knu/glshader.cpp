//
//  glshader.cpp
//  MacOGL
//
//  Created by Brian Jones on 3/4/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#include <knu/GLShader.h>
#include <algorithm>

#include <fstream>
#include <string>
#include <vector>

namespace knu
{
	struct BinaryMode { typedef unsigned char TYPE; };

	struct StreamMode { typedef std::string TYPE; };

	template<typename Mode>
	class ShaderFileReader
	{
		std::ifstream file;

	private:
		std::vector<BinaryMode::TYPE> doRead(BinaryMode)
		{
			long length = fileLength();
			std::vector<BinaryMode::TYPE> v(length);
			file.read(v.data(), length);
			return v;
		}

		std::vector<StreamMode::TYPE> doRead(StreamMode)
		{
			std::vector<StreamMode::TYPE> v;
			std::string line;
			while ((!file.eof()) && (std::getline(file, line)))
			{
				v.push_back(line);
			}

			return v;
		}
	public:
		ShaderFileReader() {}

		ShaderFileReader(std::string fileName) { open(fileName); }

		~ShaderFileReader() { close(); }

		static std::string aggregate(std::vector<std::string> vStrings)
		{
			std::string ret;
			for (auto s : vStrings)
				ret += (s + "\n");

			return ret;
		}

		void open(std::string fileName)
		{
			std::ios::openmode mode = std::ios::in;
			if (std::is_same<Mode, BinaryMode>::value)
				mode |= std::ios::binary;

			file.open(fileName, mode);
		}

		bool isOpen() const { return file.is_open(); }

		void close() { file.close(); }

		long fileLength()
		{
			long currentPosition = file.tellg();
			file.seekg(0, std::ios::end);
			long length = file.tellg();
			file.seekg(currentPosition);
			return length;
		}

		std::vector<typename Mode::TYPE> readFile(){ return doRead(Mode()); }
	};

	typedef ShaderFileReader<StreamMode> ShaderStreamReader;
	typedef ShaderFileReader<BinaryMode> ShaderBinaryReader;


    GLint GLProgram::attributeIndexCounter = 0;

	GLProgram buildProgram(PP programProperties)
	{
		knu::ShaderStreamReader vFile(programProperties.vertexPath);
		knu::ShaderStreamReader fFile(programProperties.fragmentPath);

		if (vFile.isOpen() && fFile.isOpen()){

		}
		else
		{
			if (!vFile.isOpen() && !fFile.isOpen())
				throw std::runtime_error("Unable to open vertex shader and fragment shader");
			if (!vFile.isOpen())
				throw std::runtime_error("Unable to open vertex shader");
			if (!fFile.isOpen())
				throw std::runtime_error("Unable to open fragment shader");
		}

		knu::GLShader vShader(GL_VERTEX_SHADER, knu::ShaderStreamReader::aggregate(vFile.readFile()));
		knu::GLShader fShader(GL_FRAGMENT_SHADER, knu::ShaderStreamReader::aggregate(fFile.readFile()));

		bool success;
		std::string errorString = GLCompiler::compileShader(vShader, &success);
		if (!success)
			throw std::runtime_error("Vertex shader : " + errorString);

		errorString = GLCompiler::compileShader(fShader, &success);
		if (!success)
			throw std::runtime_error("Fragment shader : " + errorString);

		//program.bindProgram();
		GLProgram program;

		if (!program.program_id())
			program.create_program();


		program.attach_shaders(vShader, fShader);

		//try {
		//	program.registerAttributes(programProperties.attributeNames);
		//}
		//catch (std::exception &ex)
		//{
		//	errorString = ex.what();
		//	throw std::runtime_error(errorString);
		//}

		errorString = GLLinker::linkProgram(program, &success);
		if (!success)
			throw std::runtime_error("Linking Program : " + errorString);

		try {
			program.bind_attributes(programProperties.attributeNames);
		}
		catch (std::exception &ex)
		{
			errorString = ex.what();
			throw std::runtime_error( errorString);
		}

		try {
			program.register_uniforms(programProperties.uniformNames);
		}
		catch (std::exception &ex)
		{
			errorString = ex.what();
			throw std::runtime_error(errorString);
		}

		errorString = GLValidator::validateProgram(program, &success);

		// Something should be done about validation later
		return program;


	}

#ifdef WIN32
	GLProgram buildCompute(PP programProperties)
	{
		knu::ShaderStreamReader cFile(programProperties.computePath);

		if (cFile.isOpen()){

		}
		else
		{
			if (!cFile.isOpen())
				throw std::runtime_error("Unable to open vertex shader");

		}

		knu::GLShader cShader(GL_COMPUTE_SHADER, knu::ShaderStreamReader::aggregate(cFile.readFile()));

		bool success;
		std::string errorString = GLCompiler::compileShader(cShader, &success);
		if (!success)
			throw std::runtime_error("Vertex shader : " + errorString);

		GLProgram program;

		if (!program.program_id())
			program.create_program();

		program.attach_shader(cShader);

		errorString = GLLinker::linkProgram(program, &success);
		if (!success)
			throw std::runtime_error("Linking Program : " + errorString);

		try {
			program.bind_attributes(programProperties.attributeNames);
		}
		catch (std::exception &ex)
		{
			errorString = ex.what();
			throw std::runtime_error(errorString);
		}

		try {
			program.register_uniforms(programProperties.uniformNames);
		}
		catch (std::exception &ex)
		{
			errorString = ex.what();
			throw std::runtime_error(errorString);
		}

		errorString = GLValidator::validateProgram(program, &success);

		// Something should be done about validation later
		return program;
	}
    
#endif

    std::string buildProgram(std::string vertexPath, std::string fragmentPath,
                             std::vector<std::string> attributeNames, std::vector<std::string> uniformNames, GLProgram &program)
    {
        knu::ShaderStreamReader vFile(vertexPath);
        knu::ShaderStreamReader fFile(fragmentPath);
        
        if(vFile.isOpen() && fFile.isOpen()){
            
        }else
        {
            if(!vFile.isOpen() && !fFile.isOpen())
                return std::string("Unable to open vertex shader and fragment shader");
            if(!vFile.isOpen())
                return std::string("Unable to open vertex shader");
            if(!fFile.isOpen())
                return std::string("Unable to open fragment shader");
        }
        
        knu::GLShader vShader(GL_VERTEX_SHADER, knu::ShaderStreamReader::aggregate(vFile.readFile()));
        knu::GLShader fShader(GL_FRAGMENT_SHADER, knu::ShaderStreamReader::aggregate(fFile.readFile()));
        
        bool success;
        std::string errorString = GLCompiler::compileShader(vShader, &success);
        if(!success)
            return std::string("Vertex shader : ") + errorString;
        
        errorString = GLCompiler::compileShader(fShader, &success);
        if(!success)
            return std::string("Fragment shader : ") + errorString;
        
        //program.bindProgram();
        if(!program.program_id())
            program.create_program();
        
        
        program.attach_shaders(vShader, fShader);
        
        try {
            program.register_attributes(attributeNames);
        }catch(std::exception &ex)
        {
            errorString = ex.what();
            return errorString;
        }
        
        errorString = GLLinker::linkProgram(program, &success);
        if(!success)
            return std::string("Linking Program : ") + errorString;
        
        try {
            program.bind_attributes(attributeNames);
        }catch(std::exception &ex)
        {
            errorString = ex.what();
            return errorString;
        }
        
        try {
            program.register_uniforms(uniformNames);
        }catch(std::exception &ex)
        {
            errorString = ex.what();
            return errorString;
        }
    
        errorString = GLValidator::validateProgram(program, &success);
        
        // Something should be done about validation later
        return std::string("");
    }

    std::string GLCompiler::compileShader(const knu::GLShader &shader, bool *success)
    {
        if(success != nullptr)
            *success = true;
        
        glCompileShader(shader.shaderId());
        GLint s = 0;
        glGetShaderiv(shader.shaderId(), GL_COMPILE_STATUS, &s);
        
        if(s == GL_FALSE)
        {
            std::vector<GLchar> error(256);
            glGetShaderInfoLog(shader.shaderId(), (GLsizei)error.size(), nullptr, error.data());
            
            if(success != nullptr)
            {
                *success = false;
            }
            
            return std::string(std::begin(error), std::end(error));
        }
        
        return std::string("");
    }

    std::string GLLinker::linkProgram(const knu::GLProgram &program, bool *success)
    {
        if(success != nullptr)
            *success = true;
        
        glLinkProgram(program.program_id());
        GLint s = 0;
        glGetProgramiv(program.program_id(), GL_LINK_STATUS, &s);
        
        if(s == GL_FALSE)
        {
            std::vector<GLchar> error(256);
            glGetProgramInfoLog(program.program_id(), (GLsizei)error.size(), nullptr, error.data());
            
            if(success != nullptr)
                *success = false;
            
            return std::string(std::begin(error), std::end(error));
        }
        
        return std::string("");
    }

    std::string GLValidator::validateProgram(const knu::GLProgram &program, bool *success)
    {
        if(success != nullptr)
            *success = true;
        
        glValidateProgram(program.program_id());
        GLint s = 0;
        glGetProgramiv(program.program_id(), GL_VALIDATE_STATUS, &s);
        
        if(s == GL_FALSE)
        {
            std::vector<GLchar> error(256);
            glGetProgramInfoLog(program.program_id(), (GLsizei)error.size(), nullptr, error.data());
            
            if(success != nullptr)
                *success = false;
            
            return std::string(std::begin(error), std::end(error));
        }
        
        return std::string("");
    }

    void GLProgram::register_attributes(std::vector<std::string> attributes)
    {
        std::for_each(std::begin(attributes), std::end(attributes), [&](std::string attribute)
                      {
                          glBindAttribLocation(program_id(), ++attributeIndexCounter, attribute.c_str());
                      });
        
    }

    void GLProgram::bind_attributes(std::vector<std::string> attributes)
    {
        
        std::for_each(std::begin(attributes), std::end(attributes), [&](std::string attribute)
                      {
                          GLint location = glGetAttribLocation(program_id(), attribute.c_str());
                          if(location < 0)
                              throw std::runtime_error("Invalid attribute name: " + attribute);
                          attributeMap[attribute] = location;
                      });
    }

    void GLProgram::register_uniforms(std::vector<std::string> uniforms)
    {
        for (auto uniform : uniforms)
        {
            int location = glGetUniformLocation(program_id(), uniform.c_str());
            if(location < 0)
                throw std::runtime_error("Invalid uniform name: " + uniform);
            uniformMap[uniform] = location;
        }
    }
}