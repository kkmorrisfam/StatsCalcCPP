#pragma once

#include <cstdint>
#include <string_view>
#include <optional>

struct MonthYear {int month; int year; };

class WindowClass
{
public:
    void Draw(std::string_view label);

private:
    void SaveToCsvFile(std::string_view filename);
    void LoadFromCsvFile(std::string_view filename);
    void DrawInputTextFields();
    void DrawMonthComboBox();
    void DrawYearComboBox();
    //optional because if there's no date in the field, we'll just skip that row
    std::optional<MonthYear> GetMonthYear(std::string_view date);
    std::string_view Trim(std::string_view str);
    bool CheckDigits(std::string_view str);
    int SelectedMonthNumber();
    int SelectedYearNumber();

    void TestFunction();


private:
    char matterFileNameBuffer[65]{};  //InputText expects char array
    char eventFileNameBuffer[65]{};
    const char* months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "Decmeber"};
    const char* years[13] = {"2018", "2019", "2020", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};

    int month_selected_idx = 0;
    int year_selected_idx = 0;
};

void render(WindowClass &window_obj);
