#pragma once

#include <string>
#include <unordered_map>

#include "Shader.h"
#include "Matrix.h"

class Texture2D;

class Material
{
public:
	Material();
	Material(Shader* shader);
	Material(const Material& obj);
	Material(Material&& obj) noexcept;

	void SetShader(Shader* shader);

	void SetBool(const std::string& name, bool b);
	void SetBoolArray(const std::string& name, const bool* bools, unsigned int count);
	void SetInt(const std::string& name, int i);
	void SetIntArray(const std::string& name, const int* ints, unsigned int count);
	void SetFloat(const std::string& name, float f);
	void SetFloatArray(const std::string& name, const float* floats, unsigned int count);

	void SetVector2(const std::string& name, const v2& vector);
	void SetVector3(const std::string& name, const v3& vector);
	void SetVector4(const std::string& name, const v4& vector);
	void SetVector2Array(const std::string& name, const v2* vectors, unsigned int count);
	void SetVector3Array(const std::string& name, const v3* vectors, unsigned int count);
	void SetVector4Array(const std::string& name, const v4* vectors, unsigned int count);

	void SetMatrix4x4(const std::string& name, const mat4x4& matrix);
	void SetMatrix4x4Array(const std::string& name, const mat4x4* matrix, unsigned int count);

	void SetTexture(const std::string& name, unsigned int texid);
	void SetTextures(const std::string& name, const int* texids, unsigned int count);

	/* performs operations in this order:
	* binds the texture unit [slot]
	* binds the texture [tex]
	* sets the texture to the shader for [name] at the relevent slot */
	void QuickTexture(const std::string& name, const Texture2D& tex, int slot);

	Material& operator=(const Material& other);
	Material& operator=(Material&& other) noexcept;

	void Bind() const;
	void Unbind() const;

	inline Shader* GetShaderReference() { return shader; }

private:
	Shader* shader = nullptr;
	std::unordered_map <std::string, int> uniformLocations;

	int GetUniformLocation(const std::string& name);
};

