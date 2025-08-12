#ifndef COMPUTE_SHADER_HPP
#define COMPUTE_SHADER_HPP

#include <CL/opencl.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#define __CL_ENABLE_EXCEPTIONS

enum class Permissions
{
	Read	= CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
	Write	= CL_MEM_WRITE_ONLY,
	All		= CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR
};

class ComputeShader {
    private:	
        static cl::Program		program;
	    static cl::Context		context;
	    static cl::Device		device;
	    static cl::CommandQueue	queue;
    public:
        // get the list of the devices of the computer
        static std::vector<cl::Device> getListDevices();

        // get the current device
        static cl::Device getDevice(){return device;}

        // change the current device
        static void setDevice(cl::Device device);

        // initialize the OpenCL context and command queue
        static void init(std::string path);
        static void init(std::string path, cl::Device device);

        // launch a kernel
        static void launch(const std::string& function, const std::vector<cl::Buffer*>& buffers, const cl::NDRange& global, const cl::NDRange& local);
        static void launch(const std::string& function, const std::vector<cl::Buffer*>& buffers, const cl::NDRange& global);
        
        // get the opencl context
        static cl::Context getContext(){return context;}

        // get the command queue
        static cl::CommandQueue getQueue() { return queue; }

        // create a buffer with a vector or a pointer
        template <typename T>
	    static cl::Buffer Buffer(std::vector<T>& data, Permissions permissions = Permissions::All){
            cl_mem_flags flags = static_cast<cl_mem_flags>(permissions);
            void* ptr = (flags & CL_MEM_COPY_HOST_PTR) ? data.data() : nullptr;
            return cl::Buffer(context, flags, data.size() * sizeof(T), ptr);
        }
        template <typename T>
        static cl::Buffer Buffer(T* data, Permissions permissions = Permissions::All){
            cl_mem_flags flags = static_cast<cl_mem_flags>(permissions);
            void* ptr = (flags & CL_MEM_COPY_HOST_PTR) ? data : nullptr;
            return cl::Buffer(context, flags, sizeof(T), ptr);
        }
};
#endif
