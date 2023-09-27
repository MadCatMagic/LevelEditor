#include "Editor/GeometryEditor.h"
#include "Editor/GeometryTool.h"

#include "Editor/EditorGizmos.h"
#include "imgui.h"

#include "Compiler/LevelRenderer.h"
#include "Engine/SpriteRenderer.h"

GeometryEditor::~GeometryEditor()
{
    if (t != nullptr)
        delete t;
    if (rend != nullptr)
        delete rend;
}

void GeometryEditor::SetupTools()
{
    tools.push_back(new DrawGeometryTool(target, "geometry_draw_icon.png"));
    tools.push_back(new BoxGeometryTool(target, "geometry_boxfill_icon.png"));
    tools.push_back(new RotateGeometryTool(target, "geometry_boxfill_icon.png")); // need to add icon

    rend = new SpriteRenderer(100);
    rend->pixelScreenSize = v2i(640, 480);
}

void GeometryEditor::Render()
{
    // testing
    EditorGizmos::SetColour(v4(1.0f, 0.0f, 0.0f, 0.5f));
    float col1 = 0.0f;
    float col2 = 0.0f;
    for each (const ColliderSegment & segment in compiledGeometry)
    {
        col1 += 0.25f;
        if (col1 > 1.0f)
        {
            col1 = 0.0f;
            col2 += 0.25f;
            if (col2 > 1.0f)
                col2 = 0.0f;
        }

        EditorGizmos::SetColour(v4(1.0f, col1, col2, 0.5f));
        for (int i = 0; i < (int)segment.vec.size() - segment.isLoop * 3 - 1; i++)
            EditorGizmos::DrawLine(segment.vec[i], segment.vec[i + 1], 2.0f);
    }
    
    if (t != nullptr)
    {
        rend->SetTexture(t->GetTexture());
    }
}

void GeometryEditor::RenderUI()
{
    // testing
    if (ImGui::Button("test exporter"))
    {
        Compiler c = Compiler(target);
        compiledGeometry = c.CompileGeometry(0);
    }

    if (ImGui::Button("test renderer"))
    {
        LevelRenderer r = LevelRenderer(target);
        Camera c;
        c.centre = v2i(20, 20);
        c.dimensions = v2i(40, 30);
        c.pixelSize = v2i(640, 480);
        if (t != nullptr)
            delete t;
        t = r.RenderCamera(c);
    }
}

void GeometryEditor::OnReload()
{
    compiledGeometry.clear();
}