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

	struct GizmosLine : public GizmosObject {
		v2 startPos;
		v2 endPos;
		float thickness = 0.0f;
	};
	struct GizmosRect : public GizmosObject {
		v2 centre;
		v2 size;
	};

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
		GizmosLine* obj = new GizmosLine();
		obj->colour = colour;
		obj->startPos = startPos;
		obj->endPos = endPos;
		obj->thickness = thickness;
		gizmosToDraw.push_back(obj);
	}

	void DrawRect(const v2& centre, const v2& size)
	{
		GizmosRect* obj = new GizmosRect();
		obj->colour = colour;
		obj->centre = centre;
		obj->size = size;
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

	void DrawRectReal(const v2& centre, const v2& size)
	{
		blitMat->SetVector4("tint", colour);

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

		for each (auto gizmo in gizmosToDraw)
		{
			colour = gizmo->colour;
			if (dynamic_cast<GizmosLine*>(gizmo) != nullptr)
			{
				GizmosLine* obj = static_cast<GizmosLine*>(gizmo);
				DrawLineReal(obj->startPos, obj->endPos, obj->thickness);
			}
			else if (dynamic_cast<GizmosRect*>(gizmo) != nullptr)
			{
				GizmosRect* obj = static_cast<GizmosRect*>(gizmo);
				DrawRectReal(obj->centre, obj->size);
			}
		}

		for (int i = 0; i < gizmosToDraw.size(); i++)
			delete gizmosToDraw[i];
		gizmosToDraw.clear();
	}

	void Release()
	{
		delete renderer;
		delete blitShader;
		delete blitMat;
	}
}
