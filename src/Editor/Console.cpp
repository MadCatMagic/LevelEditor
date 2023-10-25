#include "Editor/Console.h"
#include "imgui.h"

Console* Console::instance = nullptr;

Console::Console()
{
	instance = this;
    history.push_back("");

    AddCommand(&HelpCallback, "help");
}

void Console::Log(const std::string& data)
{
	Entry e;
	e.type = Entry::Type::Info;
	e.text = "<Info> " + data;
	instance->AddLog(e);
}

void Console::LogWarn(const std::string& data)
{
	Entry e;
	e.type = Entry::Type::Warning;
	e.text = "<Warning> " + data;
	instance->AddLog(e);
}

void Console::LogErr(const std::string& data)
{
	Entry e;
	e.type = Entry::Type::Error;
	e.text = "<Error> " + data;
	instance->AddLog(e);
}

void Console::Execute(const std::string& command)
{
    instance->ExecuteCommand(command);
}

void Console::AddCommand(ConsoleCommandCallback callback, const std::string& name)
{
    Command c;
    c.callback = callback;
    c.name = name;
    instance->commands.push_back(c);
}

// largely taken from the ImGui example console window demo
// https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
void Console::GUI()
{
	if (!enabled)
		return;

	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Console", &enabled))
	{
		ImGui::End();
		return;
	}

    if (ImGui::SmallButton("Clear")) { ClearLog(); }
    ImGui::SameLine();
    bool copyToClipboard = ImGui::SmallButton("Copy");

    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copyToClipboard)
            ImGui::LogToClipboard();
        for (const Entry& entry : entries)
        {
            // display each entry
            ImVec4 color;
            bool has_color = false;
            if (entry.type == Entry::Type::Error) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (entry.type == Entry::Type::Warning) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(entry.text.c_str());
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copyToClipboard)
            ImGui::LogFinish();
        
        // apply our scrolling settings
        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaimFocus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    char inputBuffer[256] = "";
    if (ImGui::InputText("Input", inputBuffer, 256, input_text_flags, &TextEditCallback, (void*)this))
    {
        if (inputBuffer[0] != '\0')
            ExecuteCommand(std::string(inputBuffer));
        strcpy_s(inputBuffer, "");
        reclaimFocus = true;
    }

    // Auto-focus on window apparition
    // who knows what this does :)
    ImGui::SetItemDefaultFocus();
    if (reclaimFocus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void Console::AddRawText(const std::string& text)
{
    Entry e;
    e.type = Entry::Type::Info;
    e.text = text;
    instance->AddLog(e);
}

int Console::TextEditCallback(ImGuiInputTextCallbackData* data)
{
    Console* console = (Console*)data->UserData;
    return console->TextEditEvent(data);
}

int Console::TextEditEvent(ImGuiInputTextCallbackData* data)
{
    if (historyPos == 0)
    {
        history[0] = std::string(data->Buf);
        cursorPosInCurrent = data->CursorPos;
    }

    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
    {
        // locate beginning of current word
        const char* wordEnd = data->Buf + data->CursorPos;
        const char* wordStart = wordEnd;
        while (wordStart > data->Buf)
        {
            const char c = wordStart[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            wordStart--;
        }

        // create list of possible completions
        std::vector<Command> possibilities;
        for (size_t i = 0; i < commands.size(); i++)
            if (strnicmp(commands[i].name.c_str(), wordStart, (int)(wordEnd - wordStart)) == 0)
                possibilities.push_back(commands[i]);

        if (possibilities.size() == 0)
            AddRawText("Could not find a match for " + std::string(wordStart, (size_t)(wordEnd - wordStart)));
        else if (possibilities.size() == 1)
        {
            // single match, replace entire thing with completion
            data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
            data->InsertChars(data->CursorPos, commands[0].name.c_str());
            data->InsertChars(data->CursorPos, " ");
        }
        else 
        {
            // multiple cases, complete as much as possible and then print the possibilities
            int matchLength = (int)(wordEnd - wordStart);
            while (true)
            {
                int c = 0;
                bool allPossibilityMatches = true;
                for (int i = 0; i < (int)possibilities.size() && allPossibilityMatches; i++)
                    if (i == 0)
                        c = toupper(possibilities[i].name[matchLength]);
                    else if (c == 0 || c != toupper(possibilities[i].name[matchLength]))
                        allPossibilityMatches = false;
                if (!allPossibilityMatches)
                    break;
                matchLength++;
            }

            if (matchLength > 0)
            {
                data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
                data->InsertChars(data->CursorPos, possibilities[0].name.c_str(), possibilities[0].name.c_str() + matchLength);
            }

            // List matches
            AddRawText("Possible matches:");
            for (int i = 0; i < (int)possibilities.size(); i++)
                AddRawText("- " + possibilities[i].name);
        }

        break; 
    }
    case ImGuiInputTextFlags_CallbackHistory:
    {
        // todo
        break;
    }
    }
    return 0;
}

void Console::ExecuteCommand(const std::string& command)
{
    // get the length of the first word
    int wordLength = 0;
    while (true)
    {
        char c = command[wordLength];
        if (c == '\0' || c == ' ' || c == ',' || c == ';' || c == '\t')
            break;
        wordLength++;
    }

    // split up into command/args
    std::string commandWord = command.substr(0, wordLength);
    std::string commandArgs;
    if (wordLength < (int)command.size())
        std::string commandArgs = command.substr(wordLength + 1);

    // split up args into individual strings
    std::vector<std::string> args;
    args.push_back("");
    for (size_t i = 0; i < commandArgs.size(); i++)
    {
        char c = commandArgs[i];
        if (c == ' ' || c == ',' || c == ';' || c == '\t')
            args.push_back("");
        else
            args[args.size() - 1].push_back(c);
    }

    // actually try and run the command
    for (size_t i = 0; i < commands.size(); i++)
        if (commands[i].name == commandWord)
        {
            commands[i].callback(args);
            return;
        }
    LogErr("That was not a real command. (" + commandWord + ")");
}

void Console::HelpCallback(std::vector<std::string> arguments)
{
    AddRawText("Help!");
}
