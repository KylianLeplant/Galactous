#include "ComputeShader.hpp"
cl::Program			ComputeShader::program;
cl::Context			ComputeShader::context;
cl::Device			ComputeShader::device;
cl::CommandQueue	ComputeShader::queue;
std::vector<cl::Device> ComputeShader::getListDevices() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        throw std::runtime_error("No OpenCL platforms found");
    }
    std::vector<cl::Device> devices;
    std::vector<cl::Device> bufferDevices;
    for (auto& platform : platforms) {
        platform.getDevices(CL_DEVICE_TYPE_ALL, &bufferDevices);
        devices.insert(devices.end(), bufferDevices.begin(), bufferDevices.end());
    }
    return devices;
}




void ComputeShader::setDevice(cl::Device device) {
    ComputeShader::device = device;
}

void ComputeShader::init(std::string path) {
    device = getListDevices()[0];
    if (device.getInfo<CL_DEVICE_TYPE>() != CL_DEVICE_TYPE_GPU) {
        throw std::runtime_error("Device is not a GPU");
    }
    // Read OpenCL kernel file as a string.
	std::ifstream kernel_file(path);
	std::string src(std::istreambuf_iterator<char>(kernel_file), (std::istreambuf_iterator<char>()));

	// Compile kernel program which will run on the device.
	cl::Program::Sources sources{ {src.c_str(), src.length() + 1} };
	context = cl::Context(device);
	program = cl::Program(context, sources);

	auto err = program.build();
	if(err != CL_BUILD_SUCCESS)
	{
		std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		std::cerr << "Build log:\n" << buildlog << std::endl;
		throw std::runtime_error("Error building program");
	}
}

void ComputeShader::init(std::string path, cl::Device device) {
    ComputeShader::device = device;
    if (device.getInfo<CL_DEVICE_TYPE>() != CL_DEVICE_TYPE_GPU) {
        throw std::runtime_error("Device is not a GPU");
    }
    // Read OpenCL kernel file as a string.
	std::ifstream kernel_file(path);
	std::string src(std::istreambuf_iterator<char>(kernel_file), (std::istreambuf_iterator<char>()));

	// Compile kernel program which will run on the device.
	cl::Program::Sources sources{ {src.c_str(), src.length() + 1} };
	context = cl::Context(device);
	program = cl::Program(context, sources);

	auto err = program.build();
	if(err != CL_BUILD_SUCCESS){
		throw std::runtime_error("Error building program");
	}
}

void ComputeShader::launch(const std::string& function, const std::vector<cl::Buffer*>& buffers, const cl::NDRange& global, const cl::NDRange& local)
{
	cl::Kernel kernel(program, function.data());

	for (int i = 0; i < buffers.size(); i++)
		kernel.setArg(i, *(buffers[i]));

	queue = cl::CommandQueue(context, device);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
}

void ComputeShader::launch(const std::string& function, const std::vector<cl::Buffer*>& buffers, const cl::NDRange& global)
{
	cl::Kernel kernel(program, function.data());

	for (int i = 0; i < buffers.size(); i++)
		kernel.setArg(i, *(buffers[i]));

	queue = cl::CommandQueue(context, device);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global);
}
