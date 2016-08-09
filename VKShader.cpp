#include "VKShader.h"
#include <fstream>
#include <iostream>
#include <assert.h>

VKShader::VKShader(void) {
}

VKShader::~VKShader(void) {
	//destroy(); TODO
	/*for (auto& shaderModule : shaderModules)
	{
		vkDestroyShaderModule(device, shaderModule, nullptr);
	}*/
}

void VKShader::loadAsGLSL(const std::string& fileName, VkShaderStageFlagBits type, VkDevice& device) {
	auto shaderModule = _loadShaderGLSL(fileName.c_str(), device, type);

	VkPipelineShaderStageCreateInfo shaderStage = {};
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage = type;
	shaderStage.module = shaderModule;
	//shaderStage.pName = "main"; // todo : make param
	assert(shaderStage.module != NULL);
	shaders.push_back(shaderStage);
}
void VKShader::loadAsGLSL(const std::string& vsFile, const std::string& fgFile, VkDevice& device) {
	loadAsGLSL(vsFile, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, device);
	loadAsGLSL(fgFile, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, device);
}

void VKShader::load(const std::string& fileName, VkShaderStageFlagBits type, VkDevice& device) {
	VkPipelineShaderStageCreateInfo shaderStage = {};
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage = type;
	shaderStage.module = _loadShader(fileName.c_str(), device);
	//shaderStage.pName = "main"; // todo : make param
	assert(shaderStage.module != NULL);
	shaders.push_back(shaderStage);
}
void VKShader::load(const std::string& vsFile, const std::string& fgFile, VkDevice& device) {
	load(vsFile, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, device);
	load(fgFile, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, device);
}

uint8_t* VKShader::send_uniform(VkDevice& device, VkDeviceMemory& memory, dummy* ubo) {
	uint8_t *pData;
	VkResult err = vkMapMemory(device, memory, 0, sizeof(*ubo), 0, (void **)&pData);
	assert(!err);
	//memcpy(pData, ubo, sizeof(*ubo));
	return pData;
}
void VKShader::unmap(VkDevice& device, VkDeviceMemory& memory) {
	vkUnmapMemory(device, memory);
}

char *VKShader::readBinaryFile(const char *filename, size_t *psize) {
	long int size;
	size_t retval;
	void *shader_code;

	FILE *fp = fopen(filename, "rb");
	if (!fp) return NULL;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	shader_code = malloc(size);
	retval = fread(shader_code, size, 1, fp);
	assert(retval == 1);

	*psize = size;

	return (char*)shader_code;
}

std::string VKShader::readTextFile(const char *fileName) {
	std::string fileContent;
	std::ifstream fileStream(fileName, std::ios::in);
	if (!fileStream.is_open()) {
		printf("File %s not found\n", fileName);
		return "";
	}
	std::string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		fileContent.append(line + "\n");
	}
	fileStream.close();
	return fileContent;
}

VkShaderModule VKShader::_loadShader(const char *fileName, VkDevice& device) {
	size_t size = 0;
	const char *shaderCode = readBinaryFile(fileName, &size);
	assert(size > 0);

	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo;
	VkResult err;

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;

	moduleCreateInfo.codeSize = size;
	moduleCreateInfo.pCode = (uint32_t*)shaderCode;
	moduleCreateInfo.flags = 0;
	err = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule);
	assert(!err);

	return shaderModule;
}

VkShaderModule VKShader::_loadShaderGLSL(const char* fileName, VkDevice& device, VkShaderStageFlagBits type) {
	std::string shaderSrc = readTextFile(fileName);
	const char *shaderCode = shaderSrc.c_str();
	size_t size = strlen(shaderCode);
	assert(size > 0);

	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo;
	VkResult err;

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;

	moduleCreateInfo.codeSize = 3 * sizeof(uint32_t) + size + 1;
	moduleCreateInfo.pCode = (uint32_t*)malloc(moduleCreateInfo.codeSize);
	moduleCreateInfo.flags = 0;

	// Magic SPV number
	((uint32_t *)moduleCreateInfo.pCode)[0] = 0x07230203;
	((uint32_t *)moduleCreateInfo.pCode)[1] = 0;
	((uint32_t *)moduleCreateInfo.pCode)[2] = type;
	memcpy(((uint32_t *)moduleCreateInfo.pCode + 3), shaderCode, size + 1);

	err = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule);
	assert(!err);

	return shaderModule;
}