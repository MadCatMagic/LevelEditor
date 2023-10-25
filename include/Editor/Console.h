#pragma once
#include "Vector.h"
#include <vector>

class Console
{
public:
	Console();

	static void Log(const std::string& data);
	static void LogWarn(const std::string& data);
	static void LogErr(const std::string& data);

	void GUI();

	bool enabled = false;

private:
	bool autoScroll = true;

	struct Entry
	{
		enum Type { Info, Warning, Error };

		std::string text;
		Type type;
	};
	static Console* instance;

	std::vector<Entry> entries;

	inline void AddLog(const Entry& e) { entries.push_back(e); }
	inline void ClearLog() { entries.clear(); }
};