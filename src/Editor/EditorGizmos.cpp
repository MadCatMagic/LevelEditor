#include "Editor/EditorGizmos.h"
#include "Engine/SpriteRenderer.h"

#include "Engine/Mesh/Material.h"
#include "Engine/Mesh/Shader.h"
#include "Engine/Mesh/VertexArray.h"
#include "Engine/Mesh/VertexBuffer.h"

namespace EditorGizmos
{
	static VertexBuffer* blitVB;
	static VertexArray* blitVA;
	static Shader* blitShader;
	static Material* blitMat;

	static Editor* editor;
	static v4 colour;

	void Initialize(Editor* e)
	{
		editor = e;

		// setup renderer
		blitVB = new VertexBuffer(nullptr, 0);
		blitVA = new VertexArray();
		blitVA->Construct();
		blitVA->EnableAttribute(0);
		blitVA->FormatAttribute(0, 2, GL_FLOAT, false, 0, 0);
		blitVA->DisableAttribute(0);
		// Create and compile our GLSL program from the shaders
		blitShader = new Shader("res/shaders/EditorGizmos.shader");
		blitMat = new Material(*blitShader);
	}

	void SetColour(const v4& col)
	{
		colour = col;
	}

	void DrawLine(const v2& startPos, const v2& endPos, float thickness)
	{
		GizmosLine obj;
		obj.colour = colour;
		obj.startPos = startPos;
		obj.endPos = endPos;
		obj.thickness = thickness;
		gizmosToDraw.push_back(obj);
	}

	void DrawLineReal(const v2& startPos, const v2& endPos, float thickness)
	{
		// deprecated glLineWidth(thickness);

		// Use the shader
		blitMat->SetVector4("tint", colour);

		// create the vertex data
		v2 norm = endPos - startPos;
		norm = v2::Normalize(v2(-norm.y, norm.x)); // rotate 90 degrees
		v2 p1 = editor->PixelToScreen(editor->WorldToPixel(startPos + norm * thickness * 0.05f));
		v2 p2 = editor->PixelToScreen(editor->WorldToPixel(endPos + norm * thickness * 0.05f));
		v2 p3 = editor->PixelToScreen(editor->WorldToPixel(startPos - norm * thickness * 0.05f));
		v2 p4 = editor->PixelToScreen(editor->WorldToPixel(endPos - norm * thickness * 0.05f));

		float quad[] {
			p1.x, p1.y, p2.x, p2.y, p3.x, p3.y,
			p3.x, p3.y, p2.x, p2.y, p4.x, p4.y
		};

		// 1rst attribute buffer : vertices
		blitVB->SetData(quad, sizeof(float) * 12, VertexBuffer::UsageHint::StreamDraw);

		// Draw the lines !
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void DrawAllGizmos()
	{
		blitMat->Bind();
		blitVB->Bind();
		blitVA->Bind();
		blitVA->EnableAttribute(0);
		for each (auto & gizmo in gizmosToDraw)
		{
			colour = gizmo.colour;
			DrawLineReal(gizmo.startPos, gizmo.endPos, gizmo.thickness);
		}
		blitVA->DisableAttribute(0);

		gizmosToDraw.clear();
	}

	void Release()
	{
		delete blitVB;
		delete blitVA;
		delete blitShader;
		delete blitMat;
	}
}
