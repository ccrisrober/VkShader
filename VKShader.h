// Simplify SPIR-V style interaction with C++ 
// Copyright (C) <2015-2016> - Cristian Rodríguez Bernal (maldicion069) 

// This program is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by 
// the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
//  
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details. 
//  
// You should have received a copy of the GNU General Public License 
// along with this program.  If not, see <http://www.gnu.org/licenses/>. 

#ifndef _VKSHADER_H_ 
#define _VKSHADER_H_ 

#include "vk_cpp.hpp"

#include <string> 
#include <vector> 
#include <vulkan\vulkan.h>

struct dummy {};

class VKShader {
public:
	VKShader(void);
	~VKShader(void);

	//void destroy();
	void load(const std::string& vsFile, const std::string& fgFile, VkDevice& device);
	void load(const std::string& fileName, VkShaderStageFlagBits type, VkDevice& device);
	void loadAsGLSL(const std::string& vsFile, const std::string& fgFile, VkDevice& device);
	void loadAsGLSL(const std::string& fileName, VkShaderStageFlagBits type, VkDevice& device);

	//TODO: FINISH!
	uint8_t* send_uniform(VkDevice& device, VkDeviceMemory& memory, dummy* ubo);
	void unmap(VkDevice& device, VkDeviceMemory& memory);
public:
	VkPipeline program;
	std::vector<VkPipelineShaderStageCreateInfo> shaders;

//private:
	char *readBinaryFile(const char *filename, size_t *psize);
	std::string readTextFile(const char *fileName);
	VkShaderModule _loadShader(const char *fileName, VkDevice& device);
	VkShaderModule _loadShaderGLSL(const char* fileName, VkDevice& device, VkShaderStageFlagBits type);


	vk::ShaderModule loadShaderGLSLXD(const char* fileName, vk::Device& device, VkShaderStageFlagBits type) {
		std::string shaderSrc = readTextFile(fileName);
		const char *shaderCode = shaderSrc.c_str();
		size_t size = strlen(shaderCode);
		assert(size > 0);

		auto moduleCreateInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(3 * sizeof(uint32_t)+size + 1);
		moduleCreateInfo.setPCode((uint32_t*)malloc(moduleCreateInfo.codeSize));

		// Magic SPV number
		((uint32_t *)moduleCreateInfo.pCode)[0] = 0x07230203;
		((uint32_t *)moduleCreateInfo.pCode)[1] = 0;
		((uint32_t *)moduleCreateInfo.pCode)[2] = type;
		memcpy(((uint32_t *)moduleCreateInfo.pCode + 3), shaderCode, size + 1);

		vk::ShaderModule shaderModule;
		vk::Result err  = device.createShaderModule(&moduleCreateInfo, nullptr, &shaderModule);
		assert(err == vk::Result::eSuccess);

		return shaderModule;
	}
};

#endif /* _VKSHADER_H_ */ 