#pragma once
#include "Vector.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

// (x, y, u, v)
const v4 quadData[6]{
	v4(-1.0f, -1.0f, 0.0f, 1.0f),
	v4(1.0f, -1.0f, 1.0f, 1.0f),
	v4(-1.0f,  1.0f, 0.0f, 0.0f),
	v4(-1.0f,  1.0f, 0.0f, 0.0f),
	v4(1.0f, -1.0f, 1.0f, 1.0f),
	v4(1.0f,  1.0f, 1.0f, 0.0f)
};

class QuadRenderer
{
public:
	QuadRenderer(VAStructure structure);

	void SetData(void* data, int count);
	void Render();

	void Bind() const;
	
private:
	VertexBuffer vb;
	VertexArray va;
};

// has a structure of (x, y) (u, v) and the setverts command lets you set the four vertices.
class SimpleQuadRenderer : public QuadRenderer
{
public:
	SimpleQuadRenderer();

	void SetVerts(const v2& a, const v2& b, const v2& c, const v2& d);
};