//
//  cl.cpp
//  mycl
//
//  Created by Brian Jones on 5/3/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#include "knu_cl.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace knu
{
    namespace compute
    {
        CL::CL(cl_int deviceType):platform(nullptr), devices(), context(nullptr),
        queues()
        {
            cl_int err;
            err = clGetPlatformIDs(1, &platform, nullptr);
            
            if(err)
            {
                
            }
            
            cl_uint ret = 0;
            err = clGetDeviceIDs(platform, deviceType, 0, nullptr, &ret);
            
            if(err)
            {
                
            }else
            {
                devices.resize(ret);
                clGetDeviceIDs(platform, deviceType, (cl_int)devices.size(), &devices[0], nullptr);
            }
            
            context = clCreateContext(0, (cl_int)devices.size(), &devices[0], nullptr, nullptr, &err);
            
            if(err || !context)
            {
                
            }
            
            cl_command_queue_properties props = 0;
            queues.resize(devices.size());
            
            for(size_t i = 0; i < devices.size(); ++i)
            {
                queues[i] = clCreateCommandQueue(context, devices[i], props, &err);
                
                if(err || !queues[i])
                {
                    
                }
            }
        }
        
        CL::~CL()
        {
            release();
        }
        
        void CL::release()
        {
            if(queues.size())
            {
                for(auto q : queues)
                    clReleaseCommandQueue(q);
            }
            
            if(context)
            {
                clReleaseContext(context);
                context = nullptr;
            }
            
            if(devices.size())
            {
                for(auto d : devices)
                    clReleaseDevice(d);
            }
            
            if(platform)
            {
                
            }
        }
        
        cl_context CL::get_context()
        {
            return context;
        }
        
        cl_device_id CL::get_device(int idx)
        {
            return devices[idx];
        }
        
        cl_command_queue CL::get_queue(int idx)
        {
            return queues[idx];
        }
        
        std::string CL::load_source(std::string fileName) const
        {
            std::ifstream file(fileName, std::ios::binary | std::ios::ate);
            
            if(!file)
                throw std::runtime_error("unable to open file: " + fileName);
            
            auto fileSize = file.tellg();
            file.seekg(std::ios::beg);
            std::vector<char> data(fileSize);
            file.read(data.data(), data.size());
            std::string src(std::begin(data), std::end(data));
            return src;
        }
        
        void CL::create_program(cl_program *prog, const char **sources, cl_int count)
        {
            cl_int err = 0;
            *prog = clCreateProgramWithSource(context, count, sources, nullptr, &err);
        }
        
        void CL::build_program(cl_program prog, int deviceIdx)
        {
            cl_int res = clBuildProgram(prog, 1, &devices[deviceIdx], nullptr, nullptr, nullptr);
            
            if(res)
            {
                size_t length;
                clGetProgramBuildInfo(prog, devices[deviceIdx], CL_PROGRAM_BUILD_LOG, 0, nullptr, &length);
                std::vector<char> temp(length);
                clGetProgramBuildInfo(prog, devices[deviceIdx], CL_PROGRAM_BUILD_LOG, length, &temp[0], nullptr);
                std::string err(std::begin(temp), std::end(temp));
                std::cerr << err << std::endl;
                throw std::runtime_error(err);
            }
        }
        
        void CL::make_program(cl_program *prog, int deviceIdx, const char **sources, cl_int count)
        {
            create_program(prog, sources, count);
            build_program(*prog, deviceIdx);
        }
        
        std::string CL::print_platform() const
        {
            std::stringstream ss;
            
            std::vector<char> buffer;
            
            size_t bufferSize;
            clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetPlatformInfo(platform, CL_PLATFORM_NAME, bufferSize, buffer.data(), nullptr);
            std::string platformName = std::string(std::begin(buffer), std::end(buffer));
            
            buffer.clear();
            clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, bufferSize, buffer.data(), nullptr);
            std::string platformVendor = std::string(std::begin(buffer), std::end(buffer));
            
            buffer.clear();
            clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetPlatformInfo(platform, CL_PLATFORM_VERSION, bufferSize, buffer.data(), nullptr);
            std::string platformVersion = std::string(std::begin(buffer), std::end(buffer));
            
            buffer.clear();
            clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, bufferSize, buffer.data(), nullptr);
            std::string platformProfile = std::string(std::begin(buffer), std::end(buffer));
            
            buffer.clear();
            clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, bufferSize, buffer.data(), nullptr);
            std::string platformExtension = std::string(std::begin(buffer), std::end(buffer));
            
            ss << platformName << "\n" << platformVendor << "\n" << platformVersion << "\n" << platformProfile << "\n"
            << platformExtension;
            
            std::cout << ss.str() << "\n";
            
            return ss.str();
        }
        
        std::string CL::print_device(int deviceIdx) const
        {
            std::stringstream ss;
            std::vector<char> buffer;
            size_t bufferSize;
            
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_NAME, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_NAME, bufferSize, buffer.data(), nullptr);
            std::string deviceName = std::string(std::begin(buffer), std::end(buffer));
            
            buffer.clear();
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_VENDOR, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_VENDOR, bufferSize, buffer.data(), nullptr);
            std::string deviceVendor = std::string(std::begin(buffer), std::end(buffer));
            
            buffer.clear();
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_EXTENSIONS, 0, nullptr, &bufferSize);
            buffer.resize(bufferSize);
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_EXTENSIONS, bufferSize, buffer.data(), nullptr);
            std::string deviceExtensions = std::string(std::begin(buffer), std::end(buffer));
            
            cl_ulong memSize;
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &memSize, &bufferSize);
            
            
            cl_uint addressBits;
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &addressBits, &bufferSize);
            
            ss << deviceName << "\n" << deviceVendor << "\n" << deviceExtensions << "\n" << std::string("Memory Size ") << memSize << "\n" << std::string("Address bits: ") <<  addressBits << "\n";
            
            std::cout << ss.str() << std::endl;
            return ss.str();
        }
    }
}