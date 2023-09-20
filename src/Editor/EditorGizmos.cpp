#include "Editor/EditorGizmos.h"

#include "Engine/Mesh/QuadRenderer.h"

#include "Engine/Mesh/Material.h"
#include "Engine/Mesh/Shader.h"

namespace EditorGizmos
{
	static SimpleQuadRenderer* renderer;
	static Shader* blitShader;
	static Material* blitMat;

	static Editor* editor;
	static v4 colour;

	void Initialize(Editor* e)
	{
		editor = e;

		// setup renderer
		renderer = new SimpleQuadRenderer();

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
		
		// draw it
		renderer->SetVerts(p1, p2, p3, p4);
		renderer->Render();
	}

	void DrawAllGizmos()
	{
		blitMat->Bind();
		renderer->Bind();

		for each (auto & gizmo in gizmosToDraw)
		{
			colour = gizmo.colour;
			DrawLineReal(gizmo.startPos, gizmo.endPos, gizmo.thickness);
		}

		gizmosToDraw.clear();
	}

	void Release()
	{
		delete renderer;
		delete blitShader;
		delete blitMat;
	}
}
