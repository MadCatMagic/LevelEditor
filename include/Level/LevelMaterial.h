#pragma once
#include <vector>
#include "Vector.h"

struct LevelMaterial
{
	LevelMaterial(int materialId, const v3& editorCol, const std::string& materialName);

	inline virtual v4 GetDataAtPoint(const v2& worldPos) { return editorColour; }

	int id;
	v3 editorColour;
	std::string name;
};

class MaterialManager
{
public:
	void PopulateMaterialList();

	v3 GetMaterialColour(int id);
	std::string GetMaterialName(int id);

	inline int GetNumPages() const { return (int)pages.size(); };
	int GetPageLength(int page) const;
	std::string GetPageName(int page) const;

	inline void SetSelectedOnPage(int page, int selected) { pages[page].currentSelection = selected; }
	inline int GetSelectedOnPage(int page) { return pages[page].currentSelection; }
	
	struct PageData
	{
		PageData(const std::string& pageName, int pageLength);
		std::string name;
		int length;
		int currentSelection = 0;
	};

	LevelMaterial* GetMaterialFromId(int id) const;

	static MaterialManager* currentInstance;

private:
	bool ValidMaterial(int id) const;

	// (pageName, numberOfMaterials)
	std::vector<PageData> pages;
	std::vector<LevelMaterial*> materialList = std::vector<LevelMaterial*>(256);
};