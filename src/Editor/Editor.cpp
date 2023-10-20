#include "Editor/Editor.h"
#include "imgui.h"
#include "Engine/Renderer.h"
#include "Engine/Texture/Texture2D.h"
#include "Engine/SpriteRenderer.h"

#include "Engine/Input.h"
#include <iostream>

#include "Editor/EditorGizmos.h"

#include "Editor/GeometryEditor.h"
#include "Editor/MaterialEditor.h"
#include "Editor/LogicEditor.h"
#include "Editor/EffectEditor.h"

#include "tracy/Tracy.hpp"

Editor::Editor()
{
}

Editor::~Editor()
{
    if (initialized)
    {
        delete whiteTex;
        delete slantTex;
        delete[] spriteRenderers;

        for (SpecificEditor* editor : editors)
            delete editor;

        EditorGizmos::Release();

        delete level;
    }
}

void Editor::Initialize(Level* target, const v2i& windowSize)
{
    if (initialized)
        std::cout << "already initialized, what are you doing you silly girl" << std::endl;
    initialized = true;

    winSize = windowSize;
    whiteTex = new Texture2D("res/white.png");
    slantTex = new Texture2D("res/slant.png");
    slantTex->sampling = Texture::Sampling::Nearest;
    slantTex->ApplyFiltering();

    // setup editors
    editors.push_back(new GeometryEditor(level, this));
    editors.push_back(new MaterialEditor(level, this));
    editors.push_back(new LogicEditor(level, this));
    editors.push_back(new EffectEditor(level, this));
    ChangeEditor(EditorMode::Geometry);

    for (size_t i = 0; i < editors.size(); i++)
        editors[i]->SetupTools();

    ReloadLevel(target);

    // setup gizmos
    EditorGizmos::Initialize(this);
}

void Editor::Update()
{
    ZoneScoped;

    // translate back from screen space to tile space
    v2 pixelPos = v2(Input::cursorPosX, winSize.y - Input::cursorPosY);
    v2 pos = PixelToWorld(pixelPos);

    if (Input::GetKeyDown(Input::Key::COMMA))
        currentLayer--;
    else if (Input::GetKeyDown(Input::Key::DOT))
        currentLayer++;

    if (currentLayer < 0)
        currentLayer = 0;
    else if (currentLayer >= TILE_CHUNK_LAYERS)
        currentLayer = TILE_CHUNK_LAYERS - 1;

    // do tools stuff first
    for each (auto & tool in editors[mode]->tools)
        tool->SetLayer(currentLayer);

    if (selectedTool != nullptr)
    {
        bool shift = Input::GetKey(Input::Key::LSHIFT);
        bool ctrl = Input::GetKey(Input::Key::LCONTROL);
        if (Input::GetMouseButtonDown(0))
        {
            selectedTool->OnClick(shift, ctrl, v2i(pos));
            selectedTool->OnClick(shift, ctrl, pos);
        }
        else if (Input::GetMouseButton(0))
        {
            selectedTool->OnHoldClick(shift, ctrl, v2i(pos));
            selectedTool->OnHoldClick(shift, ctrl, pos);
        }
        else if (Input::GetMouseButtonUp(0))
        {
            selectedTool->OnReleaseClick(shift, ctrl, v2i(pos));
            selectedTool->OnReleaseClick(shift, ctrl, pos);
        }
    }

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

    // do editor stuff
    for each (SpecificEditor * editor in editors)
        editor->Update();
}

void Editor::Render(RenderTexture* target)
{
    ZoneScoped;
    for (int x = 0; x < level->dimensions.x; x++)
        for (int y = 0; y < level->dimensions.y; y++)
        {
            v2 targetPos = WorldToPixel((v2)v2i(x, y));
            float vc = (x + y) % 2 == 0 ? 0.9f : 0.85f;
            for (int l = 0; l < TILE_CHUNK_LAYERS; l++)
            {
                // translate from tile space to pixel space
                int i = GetIndex(x, y, l);
                v2 targetPos = WorldToPixel((v2)v2i(x, y));
                spriteRenderers[i].SetPixelPos(targetPos);
                spriteRenderers[i].SetScale(viewerScale);

                // make checkerboard pattern
                float v = vc * (float)(TILE_CHUNK_LAYERS - abs(l - currentLayer)) / (float)TILE_CHUNK_LAYERS;
                spriteRenderers[i].tint = v4(v, v, v, v);

                // ensure it renders properly
                auto tile = level->GetTile(v2i(x, y), l);
                if (tile->solid)
                    spriteRenderers[i].render = true;
                else
                    spriteRenderers[i].render = false;

                if (tile->slant != 0)
                {
                    spriteRenderers[i].SetRotation(tile->slant - 1);
                    spriteRenderers[i].SetTexture(slantTex);
                }
                else
                    spriteRenderers[i].SetTexture(whiteTex);
            }
        }

    editors[mode]->Render();
}

void Editor::RenderUI(ImGuiIO* io)
{
    ZoneScoped;
    ImGui::Begin("Editor");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);

    ImGui::Text(("viewerPosition: " + viewerPosition.ToString() + " viewerScale: " + viewerScale.ToString()).c_str());
    ImGui::Text(("layer: " + std::to_string(currentLayer)).c_str());

    ImGui::Text("Editor modes:");
    if (ImGui::Button("Geometry Editor"))
        ChangeEditor(EditorMode::Geometry);
    ImGui::SameLine();
    if (ImGui::Button("Material Editor"))
        ChangeEditor(EditorMode::Material);
    ImGui::SameLine();
    if (ImGui::Button("Logic Editor"))
        ChangeEditor(EditorMode::Logic);
    ImGui::SameLine();
    if (ImGui::Button("Effect Editor"))
        ChangeEditor(EditorMode::Effect);

    // tool option menu
    for (int i = 0; i < (int)editors[mode]->tools.size(); i++)
    {
        ImGui::PushID(i); // probably not that necessary
        unsigned int targetTex = editors[mode]->tools[i]->GetTextureID();

        // tint it if it is disabled
        float disabledColour = 0.5f + 0.5f * (float)editors[mode]->tools[i]->enabled;
           ImGui::BeginDisabled(!editors[mode]->tools[i]->enabled);
        if (ImGui::ImageButton(
            "",
            (void*)targetTex,//(void*)tools[i]->GetTextureID(),
            ImVec2(32.0f, 32.0f),           // size of the image
            ImVec2(0.0f, 0.0f),             // uv coordinates for lower left
            ImVec2(1.0f, 1.0f),             // uv coordinates for top right
            ImVec4(0.0f, 0.0f, 0.0f, 1.0f), // black background
            ImVec4(disabledColour, disabledColour, disabledColour, 1.0f)  // no tint
        ))
        {
            selectedTool = editors[mode]->tools[i];
            editors[mode]->selectedTool = i;
        }
        ImGui::EndDisabled();

        ImGui::PopID();
        ImGui::SameLine();
    }
    ImGui::NewLine();

    // tool gui options tab
    if (selectedTool != nullptr)
    {
        if (ImGui::CollapsingHeader("Tool options"))
        {
            ImGui::Indent();
            selectedTool->OnGUI();
            ImGui::Unindent();
        }
        ImGui::NewLine();
    }

    // level managing tab
    if (ImGui::CollapsingHeader("Level Saving/Loading"))
    {
        ImGui::Indent();
        static char fname[64] = "level0";
        ImGui::InputText("File name", fname, 64);
        if (ImGui::Button("Save Level"))
        {
            FileManager fm = FileManager();
            fm.SaveLevel(level, std::string(fname));
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Level"))
        {
            FileManager fm = FileManager();
            Level* l = fm.LoadLevel(std::string(fname));
            if (l != nullptr)
                ReloadLevel(l);
        }
        ImGui::Unindent();
    }
    ImGui::NewLine();

    // editor gui
    if (ImGui::CollapsingHeader("Editor"))
    {
        ImGui::Indent();
        editors[mode]->RenderUI();
        ImGui::Unindent();
    }
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
    return GetIndex(pos.x, pos.y, currentLayer);
}

int Editor::GetIndex(int x, int y, int layer) const
{
    return x + y * level->dimensions.x + layer * level->dimensions.x * level->dimensions.y;
}

void Editor::ReloadLevel(Level* l)
{
    if (level != nullptr)
        delete level;
    level = l;

    // setup renderers
    if (spriteRenderers != nullptr)
        delete[] spriteRenderers;

    spriteRenderers = new SpriteRenderer[level->dimensions.x * level->dimensions.y * TILE_CHUNK_LAYERS];
    for (int l = TILE_CHUNK_LAYERS - 1; l >= 0; l--)
    {
        for (int y = 0; y < level->dimensions.y; y++)
            for (int x = 0; x < level->dimensions.x; x++)
            {
                int i = GetIndex(x, y, l);
                // set stuff
                spriteRenderers[i].SetTexture(whiteTex);
                spriteRenderers[i].SetLayer(l);
            }
    }

    // reload tools
    for (unsigned int i = 0; i < editors.size(); i++)
    {
        for each (EditorTool* t in editors[i]->tools)
            t->SetLevel(level);

        editors[i]->target = level;
        editors[i]->OnReload();
    } 
    
}

void Editor::ChangeEditor(EditorMode newMode)
{
    editors[mode]->OnEditorInactive();
    mode = newMode;
    if (editors[mode]->tools.size() != 0)
        selectedTool = editors[mode]->tools[editors[mode]->selectedTool];
    else
        selectedTool = nullptr;
    editors[mode]->OnEditorActive();
}
