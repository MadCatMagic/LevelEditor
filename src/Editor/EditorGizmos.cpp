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
		blitVA->FormatAttribute(0, 3, GL_FLOAT, false, 0, 0);
		blitVA->DisableAttribute(0);
		// Create and compile our GLSL program from the shaders
		blitShader = new Shader("res/shaders/EditorGizmos.shader");
		blitMat = new Material(*blitShader);

		// line aliasing
		glEnable(GL_LINE_SMOOTH);
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
		glLineWidth(thickness);

		// Use the shader
		blitMat->SetVector4("tint", colour);

		// create the vertex data
		v2 p1 = editor->PixelToScreen(editor->WorldToPixel(startPos));
		v2 p2 = editor->PixelToScreen(editor->WorldToPixel(endPos));
		float line[] {
			p1.x, p1.y, 0.0f,
			p2.x, p2.y, 0.0f
		};

		// 1rst attribute buffer : vertices
		blitVB->SetData(line, sizeof(float) * 6, VertexBuffer::UsageHint::StreamDraw);

		// Draw the lines !
		glDrawArrays(GL_LINES, 0, 2);
	}

	void DrawAllGizmos()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
