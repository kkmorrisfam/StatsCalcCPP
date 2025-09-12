#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <filesystem>

//alias for maps structure
using Maps = std::map<std::string, std::string>;
struct MonthYear {int month; int year; };

class WindowClass
{
public:
    void Draw(std::string_view label);

private:
    void SaveToCsvFile(std::string_view filename);
    std::vector<Maps> LoadFromCsvFile(const std::filesystem::path& filename);
    std::vector<Maps> ReadCsvRows(const std::filesystem::path& filename);
    std::vector<Maps> FilterApcon(const std::vector<Maps>& rows);

    void DrawInputTextFields();
    void DrawMonthComboBox();
    void DrawYearComboBox();
    void DrawInputFieldForResultFile();
    void DrawWriteFileButton();

    //optional because if there's no date in the field, we'll just skip that row
    std::optional<MonthYear> GetMonthYear(std::string_view date);
    std::string_view Trim(std::string_view str);
    bool CheckDigits(std::string_view str);
    int SelectedMonthNumber();
    int SelectedYearNumber();

    void WriteToTextFile(std::string_view filename);
    void ToUpperCase(std::string& string);

    std::vector<Maps> GetClosedCases(std::vector<Maps> matters, MonthYear monthYear);
    std::vector<Maps> GetClosedEvents(std::vector<Maps> events, MonthYear monthYear);
    std::vector<Maps> GetOpenedCases(std::vector<Maps> matters, MonthYear monthYear);
    int16_t GetChargeCount(std::vector<Maps> matters, std::string charge);
    int16_t GetDispositionCount(std::vector<Maps> matters, std::string charge, std::string disposition);
    int16_t GetSubtotalHoursByCharges(std::vector<Maps> matters, std::string charge);




    void TestFunction();


private:
    char matterFileNameBuffer[512]{};  //InputText expects char array
    char eventFileNameBuffer[512]{};
    char resultsOutputFileNameBuffer[512]{};
    const char* months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "Decmeber"};
    const char* years[13] = {"2018", "2019", "2020", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};

    int month_selected_idx = 0;
    int year_selected_idx = 0;

};

void render(WindowClass &window_obj);
