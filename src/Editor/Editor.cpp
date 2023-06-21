#include "Editor/Editor.h"
#include "imgui.h"
#include "Engine/Renderer.h"
#include "Engine/Texture/Texture2D.h"
#include "Engine/SpriteRenderer.h"

#include "Engine/Input.h"
#include <iostream>

#include "Editor/EditorGizmos.h"

Editor::Editor()
{
}

Editor::~Editor()
{
    if (initialized)
    {
        delete whiteTex;
        delete[] spriteRenderers;

        for (GeometryTool* tool : tools)
            delete tool;

        EditorGizmos::Release();
    }
}

void Editor::Initialize(Level* target, const v2i& windowSize)
{
    if (initialized)
        std::cout << "already initialized, what are you doing you silly girl" << std::endl;
    initialized = true;

    winSize = windowSize;
    level = target;
    whiteTex = new Texture2D("res/white.png");

    // setup renderers
    spriteRenderers = new SpriteRenderer[level->dimensions.x * level->dimensions.y];
    for (int y = 0; y < level->dimensions.y; y++)
        for (int x = 0; x < level->dimensions.x; x++)
        {
            int i = GetIndex(x, y);
            // set stuff
            spriteRenderers[i].SetTexture(whiteTex);
            // make checkerboard pattern
            float v = (x + y) % 2 == 0 ? 0.9f : 0.85f;
            spriteRenderers[i].tint = v4(v, v, v, 1.0f);
        }

    // setup tools
    tools.push_back(new DrawGeometryTool(target, "geometry_draw_icon.png"));
    tools.push_back(new BoxGeometryTool(target, "geometry_boxfill_icon.png"));

    // setup gizmos
    EditorGizmos::Initialize(this);
}

void Editor::Update()
{
    // translate back from screen space to tile space
    v2 pixelPos = v2(Input::cursorPosX, winSize.y - Input::cursorPosY);
    v2 pos = PixelToWorld(pixelPos);

    // do tools stuff first
    bool shift = Input::GetKey(Input::Key::LSHIFT);
    bool ctrl = Input::GetKey(Input::Key::LCONTROL);
    if (Input::GetMouseButtonDown(0))
        tools[selectedTool]->OnClick(shift, ctrl, v2i(pos));
    else if (Input::GetMouseButton(0))
        tools[selectedTool]->OnHoldClick(shift, ctrl, v2i(pos));
    else if (Input::GetMouseButtonUp(0))
        tools[selectedTool]->OnReleaseClick(shift, ctrl, v2i(pos));

    // now do translation and zooming
    if (Input::GetMouseButton(1))
    {
        // i hope this works
        viewerPosition += v2::Scale(v2(-Input::diffX, Input::diffY) / 16.0f, v2::Reciprocal(viewerScale));
    }

    zoomLevel += (int)Input::scrollDiff;
    // clamp(zoomLevel, 0, 15) inclusive
    zoomLevel = zoomLevel >= 0 ? (zoomLevel < 16 ? zoomLevel : 15) : 0;
    // 1.1 ^ -7
    float z = 0.51315811823f;
    for (int i = 0; i < zoomLevel; i++)
        z *= 1.1f;
    v2 prevViewerScale = viewerScale;
    viewerScale = z;
    // want to scale at cursor -> pos beforehand should equal pos after i.e. solve for viewerPosition2
    // pixelPos / viewerScale1 / 16 + viewerPosition1 = pixelPos / viewerScale2 / 16 + viewerPosition2
    // rearranged:
    // viewerPosition 2 = pixelPos * (1 / (viewerScale1) - 1 / (viewerScale2)) / 16 + viewerPosition1
    viewerPosition = v2::Scale(pixelPos, v2::Reciprocal(prevViewerScale) - v2::Reciprocal(viewerScale)) / 16.0f + viewerPosition;
}

void Editor::Render(RenderTexture* target)
{
    for (int x = 0; x < level->dimensions.x; x++)
        for (int y = 0; y < level->dimensions.y; y++)
        {
            // translate from tile space to pixel space
            int i = GetIndex(x, y);
            v2 targetPos = WorldToPixel(v2(x, y));
            spriteRenderers[i].SetPixelPos(targetPos);
            spriteRenderers[i].SetScale(viewerScale);

            // ensure it renders properly
            if (level->GetTile(v2i(x, y))->solid)
                spriteRenderers[GetIndex(x, y)].render = true;
            else
                spriteRenderers[GetIndex(x, y)].render = false;
        }
}

void Editor::RenderUI(ImGuiIO* io)
{
    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too
    ImGui::Text(("viewerPosition: " + viewerPosition.ToString() + " viewerScale: " + viewerScale.ToString()).c_str());
    //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    //ImGui::Checkbox("Another Window", &show_another_window);

    //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //    counter++;
    //ImGui::SameLine();
    //ImGui::Text("counter = %d", counter);

    // tool option menu
    for (int i = 0; i < tools.size(); i++)
    {
        ImGui::PushID(i); // probably not that necessary
        if (ImGui::ImageButton(
            "",
            (void*)tools[i]->GetTextureID(),//(void*)tools[i]->GetTextureID(),
            ImVec2(16.0f, 16.0f),           // size of the image
            ImVec2(0.0f, 0.0f),             // uv coordinates for lower left
            ImVec2(1.0f, 1.0f),             // uv coordinates for top right
            ImVec4(0.0f, 0.0f, 0.0f, 1.0f), // black background
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f)  // no tint
        ))
            selectedTool = i;
        ImGui::PopID();
        ImGui::SameLine();
    }
    ImGui::NewLine();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
    ImGui::End();
}

v2 Editor::PixelToWorld(const v2& p) const
{
    return v2::Scale(p, v2::Reciprocal(viewerScale)) / 16.0f + viewerPosition;
}

v2 Editor::WorldToPixel(const v2& p) const
{
    return v2::Scale((p - viewerPosition) * 16.0f, viewerScale);
}

v2 Editor::PixelToScreen(const v2& p) const
{
    return v2::Scale(p, v2::Reciprocal(winSize)) * 2.0f - v2::one;
}

v2 Editor::ScreenToPixel(const v2& p) const
{
    return v2::Scale((p + v2::one) * 0.5f, winSize);
}

int Editor::GetIndex(const v2i& pos) const
{
    return pos.x + pos.y * level->dimensions.x;
}

int Editor::GetIndex(int x, int y) const
{
    return x + y * level->dimensions.x;
}