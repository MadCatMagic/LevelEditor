#include "Level/LevelMaterial.h"
#include "Level/Level.h"
#include "Level/SpriteSheet.h"

void MaterialManager::PopulateMaterialList()
{
	// page 1
	pages.push_back(PageData("Basic Page", 4));
	materialList[0] = (new LevelMaterial(v3(1.0f, 0.0f, 0.5f), "default"))->Init(0);
	materialList[1] = (new LevelMaterial(v3(0.5f, 1.0f, 0.5f), "material1"))->Init(1);
	materialList[2] = (new LevelMaterial(v3(0.0f, 1.0f, 1.0f), "material2"))->Init(2);
	materialList[3] = (new SpriteSheetMaterial("basic.mat"))->Init(3);

	// page 2
	pages.push_back(PageData("Second Page", 1));
	materialList[16] = (new LevelMaterial(v3(1.0f, 0.5f, 0.0f), "material3"))->Init(16);

	currentInstance = this;
}

v3 MaterialManager::GetMaterialColour(int id)
{
	if (ValidMaterial(id))
		return materialList[id]->editorColour;
	return v3(1.0f, 1.0f, 1.0f);
}

std::string MaterialManager::GetMaterialName(int id)
{
	if (ValidMaterial(id))
		return materialList[id]->name;
	return "error_invalid_id";
}

int MaterialManager::GetPageLength(int page) const
{
	if (page < pages.size())
		return pages[page].length;
	return 0;
}

std::string MaterialManager::GetPageName(int page) const
{
	if (page < pages.size())
		return pages[page].name;
	return "error_invalid_page";
}

MaterialManager* MaterialManager::currentInstance = nullptr;

LevelMaterial* MaterialManager::GetMaterialFromId(int id) const
{
	if (ValidMaterial(id))
		return materialList[id];
	return nullptr;
}

bool MaterialManager::ValidMaterial(int id) const
{
	int page = id / 16;
	return page < pages.size() && (id % 16) < pages[page].length;
}

LevelMaterial::LevelMaterial(const v3& editorCol, const std::string& materialName)
	: editorColour(editorCol), name(materialName)
{ }

LevelMaterial* LevelMaterial::Init(int materialId)
{
	id = materialId;
	return this;
}

MaterialManager::PageData::PageData(const std::string& pageName, int pageLength)
	: name(pageName), length(pageLength)
{ }
