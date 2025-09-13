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

    std::vector<Maps> LoadFromCsvFile(const std::filesystem::path& filename);
    std::vector<Maps> ReadCsvRows(const std::filesystem::path& filename);
    std::vector<Maps> FilterApcon(const std::vector<Maps>& rows);

    void DrawInputTextFields();
    void DrawMonthComboBox();
    void DrawYearComboBox();
    void DrawInputFieldForResultFile();
    void DrawWriteFileButton();
    bool ReadyToGenerate() const;

    //optional because if there's no date in the field, we'll just skip that row
    std::optional<MonthYear> GetMonthYear(std::string_view date);
    std::string_view Trim(std::string_view str);
    bool CheckDigits(std::string_view str);
    int SelectedMonthNumber();
    int SelectedYearNumber();

    void WriteToTextFile(std::string_view filename);
    void ToUpperCase(std::string& string);

    std::vector<Maps> GetClosedCases(const std::vector<Maps>& list, int inputMonth, int inputYear);
    // std::vector<Maps> GetClosedEvents(const std::vector<Maps>& events, MonthYear monthYear);
    std::vector<Maps> GetOpenedCases(const std::vector<Maps>& matters, MonthYear monthYear);
    int16_t GetChargeCount(const std::vector<Maps>& matters, std::string_view charge);
    int16_t GetDispositionCount(const std::vector<Maps>& matters, std::string_view charge, std::string_view disposition);
    double GetSubtotalHoursByCharges(const std::vector<Maps>& events, std::string_view charge);

    std::filesystem::path ResolveCsvPath(std::string_view text) const;


    void TestFunction();
    void TestChargeOut(const std::vector<Maps>& rows);


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
