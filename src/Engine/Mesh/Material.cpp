#include <GL/glew.h>
#include <vector>

#include "Engine/Mesh/Material.h"
#include "Engine/Texture/Texture2D.h"

Material::Material() { }

Material::Material(Shader* shader)
{
	this->shader = shader;
}


Material::Material(const Material& obj) {
	// Copy Constructor
	// copy of object is created
	this->shader = obj.shader;
	// Deep copying
}

Material::Material(Material&& obj) noexcept {
	// Move constructor
	// It will simply shift the resources,
	// without creating a copy.
	this->shader = obj.shader;
	obj.shader = nullptr;
}

void Material::SetShader(Shader* shader)
{
	this->shader = shader;
}

void Material::SetBool(const std::string& name, bool b)
{
	int location = GetUniformLocation(name);
	glUniform1i(location, b);
}

void Material::SetBoolArray(const std::string& name, const bool* bools, unsigned int count)
{
	int location = GetUniformLocation(name);
	glUniform1iv(location, count, (int*)bools);
}

void Material::SetInt(const std::string& name, int i)
{
	int location = GetUniformLocation(name);
	glUniform1i(location, i);
}

void Material::SetIntArray(const std::string& name, const int* ints, unsigned int count)
{
	int location = GetUniformLocation(name);
	glUniform1iv(location, count, ints);
}

void Material::SetFloat(const std::string& name, float f)
{
	int location = GetUniformLocation(name);
	glUniform1f(location, f);
}

void Material::SetFloatArray(const std::string& name, const float* floats, unsigned int count)
{
	int location = GetUniformLocation(name);
	glUniform1fv(location, count, floats);
}

void Material::SetVector2(const std::string& name, const v2& vector)
{
	int location = GetUniformLocation(name);
	glUniform2f(location, vector.x, vector.y);
}

void Material::SetVector3(const std::string& name, const v3& vector)
{
	int location = GetUniformLocation(name);
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void Material::SetVector4(const std::string& name, const v4& vector)
{
	int location = GetUniformLocation(name);
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Material::SetVector2Array(const std::string& name, const v2* vectors, unsigned int count)
{
	int location = GetUniformLocation(name);
	std::vector<float> floats = std::vector<float>(count * 2);
	for (unsigned int i = 0; i < count; i++)
	{
		floats[i * 2 + 0] = vectors[i].x;
		floats[i * 2 + 1] = vectors[i].y;
	}
	glUniform2fv(location, count, floats.data());
}

void Material::SetVector3Array(const std::string& name, const v3* vectors, unsigned int count)
{
	int location = GetUniformLocation(name);
	std::vector<float> floats = std::vector<float>(count * 3);
	for (unsigned int i = 0; i < count; i++)
	{
		floats[i * 3 + 0] = vectors[i].x;
		floats[i * 3 + 1] = vectors[i].y;
		floats[i * 3 + 2] = vectors[i].z;
	}
	glUniform3fv(location, count, floats.data());
}

void Material::SetVector4Array(const std::string& name, const v4* vectors, unsigned int count)
{
	int location = GetUniformLocation(name);
	std::vector<float> floats = std::vector<float>(count * 4);
	for (unsigned int i = 0; i < count; i++)
	{
		floats[i * 4 + 0] = vectors[i].x;
		floats[i * 4 + 1] = vectors[i].y;
		floats[i * 4 + 2] = vectors[i].z;
		floats[i * 4 + 3] = vectors[i].w;
	}
	glUniform4fv(location, count, floats.data());
}

void Material::SetMatrix4x4(const std::string& name, const mat4x4& matrix)
{
	int location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, true, matrix.m);
}

void Material::SetMatrix4x4Array(const std::string& name, const mat4x4* matrix, unsigned int count)
{
	int location = GetUniformLocation(name);
	std::vector<float> floats = std::vector<float>(count * 16);
	
	// assign function takes range such as array and adds to vector
	for (unsigned int i = 0; i < count; i++)
		for (unsigned int j = 0; j < 16; j++)
			floats[i * 16 + j] = matrix[i].m[j];
	
	glUniformMatrix4fv(location, count, true, floats.data());
}

void Material::SetTexture(const std::string& name, unsigned int texid)
{
	int location = GetUniformLocation(name);
	glUniform1i(location, texid);
}

void Material::SetTextures(const std::string& name, const int* texids, unsigned int count)
{
	int location = GetUniformLocation(name);
	glUniform1iv(location, count, texids);
}

void Material::QuickTexture(const std::string& name, const Texture2D& tex, int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	tex.Bind();
	SetTexture(name, slot);
}

Material& Material::operator=(const Material& other)
{
	this->shader = other.shader;
	return *this;
}

Material& Material::operator=(Material&& other) noexcept
{
	this->shader = other.shader;
	other.shader = nullptr;
	return *this;
}

void Material::Bind() const
{
	shader->Bind();
}

void Material::Unbind() const
{
	shader->Unbind();
}

int Material::GetUniformLocation(const std::string& name)
{
	if (uniformLocations.find(name) != uniformLocations.end())
		return uniformLocations[name];
	else
	{
		int location = glGetUniformLocation(shader->id, name.c_str());
		uniformLocations[name] = location;
		return location;
	}
}
