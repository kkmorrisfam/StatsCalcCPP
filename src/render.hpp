#pragma once

#include <cstdint>
#include <string_view>

class WindowClass
{
public:
    void Draw(std::string_view label);

private:
    void SaveToCsvFile(std::string_view filename);
    void LoadFromCsvFile(std::string_view filename);
    void DrawInputTextFields();

private:
    char matterFileNameBuffer[65]{};  //InputText expects char array
    char eventFileNameBuffer[65]{};
};

void render(WindowClass &window_obj);
