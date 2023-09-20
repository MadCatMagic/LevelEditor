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

VAStructure::VAStructure(const std::string& initToken)
{
	length = initToken.size();
	data = new int[length];
	for (int i = 0; i < length; i++)
	{
		if (i % 2 == 0)
		{
			data[i] = std::stoi(std::string(&initToken[i], 1));
			width += data[i];
		}
		else
		{
			if (initToken[i] == 'f')
				data[i] = GL_FLOAT;
			else if (initToken[i] == 'i')
				data[i] = GL_INT;
			else if (initToken[i] == 'u')
				data[i] = GL_UNSIGNED_INT;
			else
				exit(-1);
		}
	}
}

std::pair<int, GLenum> VAStructure::Get(int i)
{
	return std::pair<int, GLenum>(data[i * 2], data[i * 2 + 1]);
}
