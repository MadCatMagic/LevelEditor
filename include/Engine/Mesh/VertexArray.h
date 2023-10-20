#pragma once

#include <GL/glew.h>
#include <string>

class VAStructure
{
public:
	// init token can be of the format
	// "1f2i3u" etc where the number is the width of that attribute
	// and the characture is the type - f=float, i=int, u=uint.
	VAStructure(const std::string& initToken);

	inline size_t Attributes() const { return numAttributes; }
	inline size_t Width() const { return width; }
	std::pair<int, GLenum> Get(int i);

private:
	// stored as pairs of characters
	// (2i, 2i+1) for i attributes
	// first value is the width of that attribute
	// second value is the type - 0=float, 1=int, 2=uint
	int* data;
	size_t numAttributes;
	size_t width = 0;
};

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
	void FormatAttribute(unsigned int index, size_t size, GLenum type, bool normalized, int stride, void* pointer);

	void Construct();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetID() const { return id; }

	static unsigned int currentlyBound;

private:
	unsigned int id = -1;
	bool constructed{};
};

