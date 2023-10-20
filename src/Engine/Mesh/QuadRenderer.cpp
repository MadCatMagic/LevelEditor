#include "Engine/Mesh/QuadRenderer.h"

QuadRenderer::QuadRenderer(VAStructure structure)
{
	vb = VertexBuffer(nullptr, 0);
	va = VertexArray();
	va.Construct();
	int cumulative = 0;
	for (int i = 0; i < structure.Attributes(); i++)
	{
		auto pair = structure.Get(i);
		va.EnableAttribute(i);
		va.FormatAttribute(i, pair.first, pair.second, false, (int)structure.Width() * (int)sizeof(float), (void*)(cumulative * sizeof(float)));
		cumulative += pair.first;
	}
}

void QuadRenderer::SetData(void* data, int count)
{
	vb.SetData(data, sizeof(float) * count, VertexBuffer::UsageHint::StreamDraw);
}

// please bind me before running!
void QuadRenderer::Render()
{
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void QuadRenderer::Bind() const
{
	vb.Bind();
	va.Bind();
}

SimpleQuadRenderer::SimpleQuadRenderer()
	: QuadRenderer(VAStructure("2f2f"))
{ }

// arranged like this:
// a b
// c d
void SimpleQuadRenderer::SetVerts(const v2& a, const v2& b, const v2& c, const v2& d)
{
	float data[24];
	data[0] = a.x;  data[1] = a.y;  data[2] = quadData[0].z;  data[3] = quadData[0].w;
	data[4] = b.x;  data[5] = b.y;  data[6] = quadData[1].z;  data[7] = quadData[1].w;
	data[8] = c.x;  data[9] = c.y;  data[10] = quadData[2].z; data[11] = quadData[2].w;
	data[12] = c.x; data[13] = c.y; data[14] = quadData[3].z; data[15] = quadData[3].w;
	data[16] = b.x; data[17] = b.y; data[18] = quadData[4].z; data[19] = quadData[4].w;
	data[20] = d.x; data[21] = d.y; data[22] = quadData[5].z; data[23] = quadData[5].w;
	SetData(data, 24);
}
