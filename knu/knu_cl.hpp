//
//  cl.h
//  mycl
//
//  Created by Brian Jones on 5/3/15.
//  Copyright (c) 2015 Brian Jones. All rights reserved.
//

#ifndef knucl__cl__
#define knucl__cl__

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#endif
#ifdef WIN32
#include <CL/cl.h>
#pragma comment(lib, "opencl.lib")
#endif

#include <string>
#include <vector>

namespace knu
{
    namespace compute
    {
        class CL
        {
            cl_platform_id platform;
            std::vector<cl_device_id> devices;
            cl_context context;
            std::vector<cl_command_queue> queues;
            
        private:
            void release();
            
        public:
            CL(cl_int deviceType);
            ~CL();
            
            cl_context get_context();
            cl_device_id get_device(int idx);
            cl_command_queue get_queue(int idx);
            std::string load_source(std::string fileName) const;
            void create_program(cl_program *prog, const char **sources, cl_int count);
            void build_program(cl_program prog, int deviceIdx);
            void make_program(cl_program *prog, int deviceIdx, const char **sources, cl_int count);
            std::string print_platform() const;
            std::string print_device(int deviceIdx) const;
        };
    }
}

#endif /* defined(__mycl__cl__) */
