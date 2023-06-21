#include "Engine/Mesh/VertexBuffer.h"
#include "Engine/Renderer.h"

VertexBuffer::VertexBuffer()
{
	id = -1;
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size, UsageHint hint)
{
	glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, (int)hint);
}

VertexBuffer::VertexBuffer(const VertexBuffer& obj) {
	// Copy Constructor
	// copy of object is created
	this->id = obj.id;
	// Deep copying
}

VertexBuffer::VertexBuffer(VertexBuffer&& obj) noexcept {
	// Move constructor
	// It will simply shift the resources,
	// without creating a copy.
	this->id = obj.id;
	obj.id = -1;
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &id);
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& other)
{
	this->id = other.id;
	return *this;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
	this->id = other.id;
	other.id = -1;
	return *this;
}

void VertexBuffer::SetData(const void* data, unsigned int size, UsageHint hint)
{
	if (id == -1)
		glGenBuffers(1, &id);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, (int)hint);
}

void VertexBuffer::ModifyData(const void* data, unsigned int size, unsigned int offset)
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}