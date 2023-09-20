#include "Engine/Mesh/QuadRenderer.h"

QuadRenderer::QuadRenderer(VAStructure structure)
{
	vb = VertexBuffer(nullptr, 0);
	va = VertexArray();
	va.Construct();
	int cumulative = 0;
	for (int i = 0; i < structure.Size(); i++)
	{
		auto pair = structure.Get(i);
		va.EnableAttribute(i);
		va.FormatAttribute(i, pair.first * sizeof(float), pair.second, false, structure.Width(), (void*)cumulative);
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
