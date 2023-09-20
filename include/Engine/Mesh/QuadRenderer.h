#pragma once
#include "Vector.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

class QuadRenderer
{
public:
	QuadRenderer(VAStructure structure);

	void SetData(void* data, int count);
	void Render();

	void Bind() const;
	
private:
	float data[12]{};

	VertexBuffer vb;
	VertexArray va;
};