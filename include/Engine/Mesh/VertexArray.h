#pragma once

#include <GL/glew.h>
#include <string>

// replacement for state vertex arrays
// handles vertex buffer structure
// ONLY USE FOR ONE VERTEX BUFFER UNLESS YOU KNOW WHAT YOU ARE DOING
class VertexArray
{
public:
	// need to use Construct()
	VertexArray();
	// if true auto Construct()
	VertexArray(bool construct);
	~VertexArray();

	void EnableAttribute(unsigned int index);
	void DisableAttribute(unsigned int index);
	void FormatAttribute(unsigned int index, int size, GLenum type, bool normalized, int stride, void* pointer);

	void Construct();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetID() const { return id; }

	static unsigned int currentlyBound;

private:
	unsigned int id = -1;
	bool constructed{};
};

