#pragma once
#include "Vector.h"
#include <vector>

typedef void (*ConsoleCommandCallback)(std::vector<std::string> arguments);

class Console
{
public:
	Console();

	static void Log(const std::string& data);
	static void LogWarn(const std::string& data);
	static void LogErr(const std::string& data);
	static void Execute(const std::string& command);

	static void AddCommand(ConsoleCommandCallback callback, const std::string& name);

	void GUI();

	bool enabled = false;

private:
	static Console* instance;

	struct Entry
	{
		enum Type { Info, Warning, Error };

		std::string text;
		Type type = Type::Info;
	};

	struct Command
	{
		ConsoleCommandCallback callback;
		std::string name;
	};

	bool autoScroll = true;
	bool scrollToBottom = false;

	// first element of history is always the current text which has been entered
	std::vector<std::string> history;
	size_t historyPos = 0;
	int cursorPosInCurrent = 0;

	// actual entries in the console
	std::vector<Entry> entries;
	
	// array of command callbacks
	std::vector<Command> commands;

	inline void AddLog(const Entry& e) { entries.push_back(e); }
	inline void ClearLog() { entries.clear(); }
	static void AddRawText(const std::string& text);

	static int TextEditCallback(struct ImGuiInputTextCallbackData* data);
	int TextEditEvent(ImGuiInputTextCallbackData* data);

	void ExecuteCommand(const std::string& command);
	static void HelpCallback(std::vector<std::string> arguments);
};