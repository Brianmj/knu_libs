#ifndef KNU_CL_HPP
#define KNU_CL_HPP

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>

#define __CL_ENABLE_EXCEPTIONS
#include <knu/cl.hpp>
#ifdef WIN32
#pragma comment(lib, "opencl.lib")
#endif

namespace knu
{
	namespace compute
	{
		class OCL
        {
			void Create(cl_int device_type, cl_context_properties context_properties, cl_command_queue_properties queue_properties)
			{
                
				cl::Platform::get(&platform);
				platform[0].getDevices(device_type, &devices);
                try {
                    context = cl::Context(devices, nullptr);
                }catch (cl::Error &error)
                {
                    std::cerr << error.what() << ", " << error.err() << std::endl;
                    std::exit(error.err());
                }
				
				for (const auto &d : devices)
				{
                    queues.push_back(cl::CommandQueue (context, d, queue_properties));
				}
			}
        public:
            std::vector<cl::Platform> platform;
            cl::Context context;
            std::vector<cl::Device> devices;
            std::vector<cl::CommandQueue> queues;


		public:
			OCL(cl_int device_type, cl_context_properties context_properties = 0, cl_command_queue_properties queue_properties = 0) 
			{
				Create(device_type, context_properties, queue_properties);
			}

			std::string ReadSourceFile(std::string file_name)
			{
				std::ifstream file(file_name);
				if (!file)
					throw std::runtime_error("Unable to open file: " + file_name);

				std::string src((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
				return src;
			}

			cl::Program CreateProgramFromFile(std::string file_name, bool build = true)
			{
				std::string src = ReadSourceFile(file_name);
				
				cl::Program::Sources sources;
				sources.push_back({src.c_str(), src.length()});
				cl::Program program(context, sources);
				if (build)
				{
					try {
						program.build(devices);
					}
					catch (cl::Error &error)
					{
						std::string info_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
						std::cerr << info_log << std::endl;
						throw std::runtime_error(error.what() + std::string(", ") + std::to_string(error.err()) + " " + info_log);
					}
				}

				return program;
			}
		};
	}
}

#endif