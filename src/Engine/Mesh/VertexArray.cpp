#include "Engine/Mesh/VertexArray.h"

VertexArray::VertexArray() { }

VertexArray::VertexArray(bool construct)
{
	if (construct)
		Construct();
}

VertexArray::~VertexArray()
{
	if (constructed)
		glDeleteVertexArrays(1, &id);
}

void VertexArray::EnableAttribute(unsigned int index)
{
	glEnableVertexAttribArray(index);
}

void VertexArray::DisableAttribute(unsigned int index)
{
	glDisableVertexAttribArray(index);
}

void VertexArray::FormatAttribute(unsigned int index, int size, GLenum type, bool normalized, int stride, void* pointer)
{
	glVertexAttribPointer(index, size, GL_FLOAT, false, stride, pointer);
}

void VertexArray::Construct()
{
	glGenVertexArrays(1, &id);
	Bind();
	constructed = true;
}

void VertexArray::Bind() const
{
	currentlyBound = id;
	glBindVertexArray(id);
}

void VertexArray::Unbind() const
{
	currentlyBound = 0;
	glBindVertexArray(0);
}

unsigned int VertexArray::currentlyBound = 0;