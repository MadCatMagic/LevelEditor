#include "Editor/LevelMaterial.h"


void MaterialManager::PopulateMaterialList()
{
	// page 1
	pages.push_back(PageData("Basic Page", 3));
	materialList[0] = new LevelMaterial(0, v3(1.0f, 0.0f, 0.5f), "default");
	materialList[1] = new LevelMaterial(1, v3(0.5f, 1.0f, 0.5f), "material1");
	materialList[2] = new LevelMaterial(2, v3(0.0f, 1.0f, 1.0f), "material2");

	// page 2
	pages.push_back(PageData("Second Page", 1));
	materialList[16] = new LevelMaterial(16, v3(1.0f, 0.5f, 0.0f), "material3");
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

int MaterialManager::GetPageLength(int page)
{
	if (page < pages.size())
		return pages[page].length;
	return 0;
}

std::string MaterialManager::GetPageName(int page)
{
	if (page < pages.size())
		return pages[page].name;
	return "error_invalid_page";
}

bool MaterialManager::ValidMaterial(int id)
{
	int page = id / 16;
	return page < pages.size() && (id % 16) < pages[page].length;
}

LevelMaterial::LevelMaterial(int materialId, const v3& editorCol, const std::string& materialName)
	: id(materialId), editorColour(editorCol), name(materialName)
{ }

MaterialManager::PageData::PageData(const std::string& pageName, int pageLength)
	: name(pageName), length(pageLength)
{ }
