#include "Level/Entity.h"
#include "Level/Level.h"
#include "Engine/SpriteRenderer.h"
#include "imgui.h"
#include "Editor/EditorGizmos.h"
#include "Compiler/LevelRenderer.h"

Entity* Entity::CreateEntityFromName(const std::string& name)
{
	if (name == "Entity")
		return new Entity();
	if (name == "Camera")
		return new Camera();
	return nullptr;
}

void Camera::UI(Level* l)
{
	bool previousPreview = preview;
	if (ImGui::InputFloat2("Dimensions", &dimensions.x))
		VariableHasChanged();
	if (ImGui::InputInt2("Pixel Size", &pixelSize.x))
		VariableHasChanged();

	ImGui::NewLine();
	ImGui::Checkbox("Enable Camera Preview", &preview);

	if (preview)
	{
		ImGui::SameLine();
		if (ImGui::Button("Force Update Preview"))
			cameraShouldUpdatePreview = true;
	}

	if (preview && !previousPreview)
		cameraShouldUpdatePreview = true;
	if (editorChangedVariable)
	{
		editorChangedVariable = false;
		cameraShouldUpdatePreview = true;
	}
	
	if (preview)
	{
		if (cameraShouldUpdatePreview)
		{
			cameraShouldUpdatePreview = false;
			// rendering
			LevelRenderer r = LevelRenderer(l);
			if (tex != nullptr)
				delete tex;
			tex = r.RenderCamera(*this);
		}
	}
	_UpdateRenderer(this, preview);
}

void Camera::Gizmos()
{
	EditorGizmos::SetColour(editorColour);
	EditorGizmos::DrawLine(position - dimensions * 0.5f, position + dimensions * 0.5f, 0.7f);
	EditorGizmos::DrawLine(position - v2(dimensions.x, -dimensions.y) * 0.5f, position + v2(dimensions.x, -dimensions.y) * 0.5f, 0.7f);
	EditorGizmos::DrawRectOutline(position - dimensions * 0.5f, position + dimensions * 0.5f, 1.0f);
}

std::vector<std::string> Camera::SaveData() const
{
	std::vector<std::string> o;
	o.push_back(std::to_string(dimensions.x));
	o.push_back(std::to_string(dimensions.y));
	o.push_back(std::to_string(pixelSize.x));
	o.push_back(std::to_string(pixelSize.y));
	return o;
}

void Camera::LoadData(std::vector<std::string> data)
{
	dimensions.x = std::stof(data[0]);
	dimensions.y = std::stof(data[1]);
	pixelSize.x = std::stof(data[2]);
	pixelSize.y = std::stof(data[3]);
}

void Camera::_UpdateRenderer(Camera* targetCam, bool rendering)
{
	if (targetCam == nullptr || !rendering)
		return _Release();

	if (renderer == nullptr)
		renderer = new SpriteRenderer(-10000);

	renderer->SetTexture(tex->GetTexture());
}

bool Camera::cameraShouldUpdatePreview = false;

void Camera::_Release()
{
	if (renderer != nullptr)
		delete renderer;
	if (tex != nullptr)
		delete tex;
	renderer = nullptr;
	tex = nullptr;
}

SpriteRenderer* Camera::renderer = nullptr;
PixelTexture2D* Camera::tex = nullptr;