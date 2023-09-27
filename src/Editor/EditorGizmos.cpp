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
		GizmosObject obj = GizmosObject();
		obj.colour = colour;
		obj.p1 = v4(startPos.x, startPos.y, endPos.x, endPos.y);
		obj.p2 = v4(); obj.p2.x = thickness;
		obj.type = GizmosType::Line;
		gizmosToDraw.push_back(obj);
	}

	void DrawRect(const v2& centre, const v2& size)
	{
		GizmosObject obj = GizmosObject();
		obj.colour = colour;
		obj.p1 = v4(centre.x, centre.y, size.x, size.y);
		obj.p2 = v4();
		obj.type = GizmosType::Rect;
		gizmosToDraw.push_back(obj);
	}

	void DrawRectOutline(const v2& bottomRight, const v2& topLeft, float thickness)
	{
		DrawLine(bottomRight, v2(bottomRight.x, topLeft.y), thickness);
		DrawLine(v2(bottomRight.x, topLeft.y), topLeft, thickness);
		DrawLine(topLeft, v2(topLeft.x, bottomRight.y), thickness);
		DrawLine(v2(topLeft.x, bottomRight.y), bottomRight, thickness);
	}

	void DrawLineReal(const v2& startPos, const v2& endPos, float thickness)
	{
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

	void DrawRectReal(const v2& centre, const v2& size)
	{
		v2 s = size * 0.5f;

		renderer->SetVerts(
			editor->PixelToScreen(editor->WorldToPixel(centre + s)),
			editor->PixelToScreen(editor->WorldToPixel(centre + v2(s.x, -s.y))),
			editor->PixelToScreen(editor->WorldToPixel(centre + v2(-s.x, s.y))),
			editor->PixelToScreen(editor->WorldToPixel(centre - s))
		);
		renderer->Render();
	}

	void DrawAllGizmos()
	{
		blitMat->Bind();
		renderer->Bind();
		blitMat->SetVector4("tint", colour);

		for each (auto& gizmo in gizmosToDraw)
		{
			if (colour != gizmo.colour)
			{
				colour = gizmo.colour;
				blitMat->SetVector4("tint", colour);
			}

			if (gizmo.type == GizmosType::Line)
			{
				DrawLineReal(v2(gizmo.p1.x, gizmo.p1.y), v2(gizmo.p1.z, gizmo.p1.w), gizmo.p2.x);
			}
			else if (gizmo.type == GizmosType::Rect)
			{
				DrawRectReal(v2(gizmo.p1.x, gizmo.p1.y), v2(gizmo.p1.z, gizmo.p1.w));
			}
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
