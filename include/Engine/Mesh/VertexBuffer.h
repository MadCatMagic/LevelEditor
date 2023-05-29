#pragma once

#include <GL/glew.h>

class VertexBuffer
{
public:
	enum class UsageHint {
		StreamDraw = GL_STREAM_DRAW, StreamRead = GL_STREAM_READ, StreamCopy = GL_STREAM_COPY,
		StaticDraw = GL_STATIC_DRAW, StaticRead = GL_STATIC_READ, StaticCopy = GL_STATIC_COPY,
		DynamicDraw = GL_DYNAMIC_DRAW, DynamicRead = GL_DYNAMIC_READ, DynamicCopy = GL_DYNAMIC_COPY
	};

	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size, UsageHint hint = UsageHint::StaticDraw);
	VertexBuffer(const VertexBuffer& obj);
	VertexBuffer(VertexBuffer&& obj) noexcept;
	~VertexBuffer();
	
	VertexBuffer& operator=(const VertexBuffer& other);
	VertexBuffer& operator=(VertexBuffer&& other) noexcept;

	void SetData(const void* data, unsigned int size, UsageHint hint = UsageHint::StaticDraw);
	void ModifyData(const void* data, unsigned int size, unsigned int offset);

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetID() const { return id; }

private:
	unsigned int id;
};

