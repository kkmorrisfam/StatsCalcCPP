#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>
#include <chrono>
#include <iomanip>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "render.hpp"
#include <charconv>



/**
 * draw window
 * call any class functions that are drawn in the window
 *
 *
 *
 */
void WindowClass::Draw(std::string_view label)
{
    constexpr static auto window_flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    constexpr static auto window_size = ImVec2(1280.0F, 720.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    ImGui::Begin(label.data(), nullptr, window_flags);

    //draw other boxes/buttons
    DrawInputTextFields();
    DrawMonthComboBox();
    DrawYearComboBox();
    DrawInputFieldForResultFile();


    // TestFunction();
    static bool didTest = false;
    if (!didTest) { TestFunction(); didTest = true; }

    ImGui::End();  //must close what is opened
}

/**
 *1. need to have a two text boxes to import csv files;
    collect month/year for stats
 * or one which will accept multiple csv files
 *2. read in csv file
    store contents into data structure
    variables: month & year
    read by row matters file, store in map? add to vector? filter?
    read by row events file, store in map? add to vector? filter?
 *3. write results to text file
    if file is not there, creates it, if it is there, writes over the old one
    write header with current date/time; month date of stats

 *4.Get list of opened cases within month/year input
    Get subtotal by charge
 *5.Get list of closed cases withing month/year
    Get subtotal of cases by charge
    Get subtotal by charge and by plea
*6. Get subtotal of hours by charge closed
*7. Helper: filter matters by APCON in map?
*8. Helper: filter events by APCON
*9. Helper: get get closed matters and events
*10.Helper: get opened cases
*11.Helper: get charge count
*12.Helper: get dispostion count of closed cases
*13.Helper: get subtotal hours by charges of closed cases
 */

void WindowClass::DrawInputTextFields()
{
    ImGui::Text("Matter File Name");
    ImGui::SameLine();
    ImGui::InputText("###MatterFileName: ", matterFileNameBuffer, sizeof(matterFileNameBuffer));

    ImGui::Text("Events File Name");
    ImGui::SameLine();
    ImGui::InputText("###EventsFileName: ", eventFileNameBuffer, sizeof(eventFileNameBuffer));


}
    //add input fields for month, year
void WindowClass::DrawMonthComboBox()
{
        //from https://github.com/ocornut/imgui/blob/02af06ea5f57696b93f0dfe77a9e2525522ba76e/imgui_demo.cpp#L1313C1-L1331C10
        // const char* months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "Decmeber"};
        // static int month_selected_idx = 0; // Here we store our selection data as an index. January = 0.
        // monthPtr = &month_selected_idx;

        // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from months[])
        const char* combo_preview_value = months[month_selected_idx];
        if (ImGui::BeginCombo("Month", combo_preview_value))
        {
            for (int n = 0; n < IM_ARRAYSIZE(months); n++)
            {
                const bool is_selected = (month_selected_idx == n);
                if (ImGui::Selectable(months[n], is_selected))  //need flags?
                    month_selected_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
}

void WindowClass::DrawYearComboBox()
{
        //from https://github.com/ocornut/imgui/blob/02af06ea5f57696b93f0dfe77a9e2525522ba76e/imgui_demo.cpp#L1313C1-L1331C10
        // const char* years[] = {"2018", "2019", "2020", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
        // static int year_selected_idx = 0; // Here we store our selection data as an index. January = 0.
        // yearPtr = &year_selected_idx;

        // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from months[])
        const char* combo_preview_value = years[year_selected_idx];
        if (ImGui::BeginCombo("Year", combo_preview_value))
        {
            for (int n = 0; n < IM_ARRAYSIZE(years); n++)
            {
                const bool is_selected = (year_selected_idx == n);
                if (ImGui::Selectable(years[n], is_selected))  //need flags?
                    year_selected_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
}

//add field for name of text file that program will create
void WindowClass::DrawInputFieldForResultFile()
{
    ImGui::Text("File name for results output (don't include file extension):");
    ImGui::SameLine();
    ImGui::InputText("###ResultsOutputFileName: ", resultsOutputFileNameBuffer, sizeof(resultsOutputFileNameBuffer));

}




//helper functions
//uses string_view to avoid making a copy of the string
std::string_view WindowClass::Trim(std::string_view str)
{
    //moves the string view starting point to appear to remove whitespace from front of the string
    //advances one char at a time in the while loop, static_cast for platforms where char is signed
    //does not modify the original string.
    while (!str.empty() && std::isspace(static_cast<unsigned char>(str.front())))
        str.remove_prefix(1);

    //same for the end of the string, takes white space out of the string_view at back
    while (!str.empty() && std::isspace(static_cast<unsigned char>(str.back())))
        str.remove_suffix(1);

        return str;
}

//check to see if the values in the string are numbers
bool WindowClass::CheckDigits(std::string_view str)
{
    if(str.empty()) return false;  //if string is empty, return false

    //for each character in str
    for (char c : str)
    {
        //if the char value is less than 0 or more than 9, it's not a digit but an alpha char
        //char data type is an int representing a character value
        if (c < '0' || c > '9')
        return false;
    }

    //else return true
    return true;
}

int WindowClass::SelectedMonthNumber()
{
   //convert selected month index to month of year (Jan index = 0, Jan month num = 1)
    return month_selected_idx + 1 ;
}

int WindowClass::SelectedYearNumber()
{
    //convert selected year string value to number
    return std::atoi(years[year_selected_idx]);
}

std::optional<MonthYear> WindowClass::GetMonthYear(std::string_view date)
{
    //changes string_view to view string without leading and ending whitespace
    date = Trim(date);

    //get position of first and second '/' in field
    auto s1 = date.find('/');
    //if s1 has no position or not found; lets std::optional know that value not found
    //if no '/' found, return from function with no value
    if (s1 == std::string_view::npos)
        return std::nullopt;
    //searches for the next '/' in date and starts right after the first instance at s1: s1+1 is start position
    auto s2 = date.find('/', s1 + 1);
    //if there's not a 2nd slash or there's more than 2, return
    if (s2 == std::string_view::npos || date.find('/', s2+1) != std::string_view::npos)
        return std::nullopt;

    //get substrings from date positions for month, day and year
    //substr takes starting and ending position
    auto monthStr = Trim(date.substr(0,s1));
    auto dayStr = Trim(date.substr(s1+1, s2-s1-1)); //not used, but may need it later
    auto yearStr = Trim(date.substr(s2+1));

    //if substrings are not digits, return
    if (!CheckDigits(monthStr) || !CheckDigits(dayStr) || !CheckDigits(yearStr))
        return std::nullopt;

    int monthInt = 0;
    int yearInt = 0;

    //get data from string view, convert to int
    std::from_chars(monthStr.data(), monthStr.data() + monthStr.size(), monthInt);
    std::from_chars(yearStr.data(), yearStr.data() + yearStr.size(), yearInt);

    //if year is in 2 digit format, convert to 4 digit
    if (yearStr.size() <= 2)
    {
        yearInt = yearInt + 2000;  //the only dates I will have are in the 2000's
    }

    //check and see if the month value is valid
    if (monthInt < 1 || monthInt > 12)
        return std::nullopt;

    //return a MonthYear structure with month and year values.
    return MonthYear{monthInt, yearInt};
}




void WindowClass::TestFunction()
{
    // std::cout << "This is a test output. addresses: \n" <<  << " / " << yearPtr << std::endl;
    // std::cout << "This is a test output. values:  \n" << *monthPtr << " / " << *yearPtr << std::endl;

    //works with both 2 and 4 year dates
    std::string_view testDate = "01/01/21";
    auto testResult = WindowClass::GetMonthYear(testDate);
    std::cout << "This is testing my GetMonthYear function with string '01/01/21':  " << testResult->month
    << " & "    << testResult->year << std::endl;

    if (WindowClass::SelectedMonthNumber() == testResult->month)
    {
        std::cout << "Selected Month: "
        << WindowClass::SelectedMonthNumber() << "\n"
        << "Test month: "
        << testResult->month << "\n";
    }

    std::cout << "Current Working Directory: " << std::filesystem::current_path() << "\n";

    // const std::filesystem::path src = R"(C:\Users\Kerri\Documents\BYU 310\StatsCalcCPP\matters-list.csv)";
//    auto apcon = LoadFromCsvFile(src);

    std::strncpy(matterFileNameBuffer,
                R"(C:\Users\Kerri\Documents\BYU 310\StatsCalcCPP\matters-list.csv)",
                sizeof(matterFileNameBuffer)-1);
    matterFileNameBuffer[sizeof(matterFileNameBuffer)-1] = '\0';

    WriteToTextFile(R"(C:\Users\Kerri\Documents\BYU 310\StatsCalcCPP\matters_summary)");

}

void WindowClass::SaveToCsvFile(std::string_view filename)
{

}

//ImGui text fields uses string buffers, so char*, string_view or path need to be used
std::vector<Maps> WindowClass::LoadFromCsvFile(const std::filesystem::path& filename)
{
    //test - print file and path to terminal
    std::cout << "Load File, Current Working Directory: " << std::filesystem::current_path() << "\n";
    std::cout << "Load File, Filename: " << filename << std::endl;

    //open file
    auto rows = ReadCsvRows(filename);
    auto apcon = FilterApcon(rows);
    return apcon;

}

//structure row data into a vector of maps/dictionary
std::vector<Maps> WindowClass::ReadCsvRows(const std::filesystem::path& filename)
{
    //structure to fill and return
    std::vector<Maps> data;
    //start a file stream with filename
    std::ifstream file(filename);

    //double check
    if(!file.is_open())
    {
        std::cerr << "Could not open: " << filename << "\n";
        return data;
    }

    //collect column header row
    std::string line;
    std::vector<std::string> headers;

    //read header row
    if(std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string header;
        while (std::getline(ss, header, ','))
        {
            headers.push_back(header);
        }
    }
    else
    {
        std::cerr << "Error: CSV file is empty or header cannot be read." << std::endl;
        return data;
    }

    //collect rows
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;
        Maps row;
        int columnIndex = 0;

        while(std::getline(ss, cell, ',') && columnIndex < headers.size())
        {
            row[headers[columnIndex]] = cell;
            columnIndex++;
        }

        //add row to data
        data.push_back(row);

    }

    file.close();
    return data;
}

//filter vector of maps into values with just "APCON" in column
std::vector<Maps> WindowClass::FilterApcon(const std::vector<Maps>& rows)
{
    //structure for return value
    std::vector<Maps> data;
    data.reserve(rows.size());  //pre-allocate memory for data before loop

    for (const auto& r : rows)
    {
        auto it = r.find("Casecode");
        if (it != r.end() && !it -> second.empty()) {
            std::string val = it->second;
            WindowClass::ToUpperCase(val);
            if (val == "APCON")
            {
                data.push_back(r);
            }
        }
    }
    return data;

}

//change string to uppercase characters
void WindowClass::ToUpperCase(std::string& string)
{

    for (char& c : string)
    {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
}

void WindowClass::WriteToTextFile(std::string_view filename)
{
    //to test - read and filter file
    auto rows = ReadCsvRows(matterFileNameBuffer);
    auto apcon = FilterApcon(rows);

    std::filesystem::path filePath = filename;
    if (filePath.extension() != ".txt")
    {
        filePath.replace_extension(".txt");
    }

    //create parent directory if none given
    if (!filePath.parent_path().empty())
    {
        std::error_code code;
        std::filesystem::create_directories(filePath.parent_path(), code);
    }

    //open new file
    std::ofstream out(filePath);
    if(!out)
    {
        std::cerr << "Failed to open for writing new file: " << filePath << std::endl;
        return;
    }

    //create timestamp
    const auto now = std::chrono::system_clock::now();
    const auto tt = std::chrono::system_clock::to_time_t(now);
    out << "Summary generated: " << std::put_time(std::localtime(&tt), "%Y-%m-%d%H:%M:%S") << "\n";

    //create temp report
    std::filesystem::path matterPath{matterFileNameBuffer};


    out << "Matters CSV: " << matterFileNameBuffer << std::endl;
    out << "Matters csv full path: " << std::filesystem::absolute(matterPath) << std::endl << std::endl;

    out << "Total data rows: " << rows.size()  << std::endl;
    out << "Filtered by APCON size: " << apcon.size() << std::endl;

    if (!apcon.empty())
    {
        out << "\nFirst APCON row keys:\n";
        for (const auto& kv: apcon.front())
        {
            out << " - " << kv.first << std::endl;
        }
    }

    out << std::flush;
    std::cout << "Wrote Summary to: " << filePath << std::endl;

}

std::vector<Maps> WindowClass::GetClosedCases(std::vector<Maps> matters, MonthYear monthYear)
{

}
std::vector<Maps> WindowClass::GetClosedEvents(std::vector<Maps> events, MonthYear monthYear){}
std::vector<Maps> WindowClass::GetOpenedCases(std::vector<Maps> matters, MonthYear monthYear){}
int16_t WindowClass::GetChargeCount(std::vector<Maps> matters, std::string charge){}
int16_t WindowClass::GetDispositionCount(std::vector<Maps> matters, std::string charge, std::string disposition){}
int16_t WindowClass::GetSubtotalHoursByCharges(std::vector<Maps> matters, std::string charge){}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
