#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <optional>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "render.hpp"
#include <charconv>
#include <array>


static constexpr std::array<std::string_view, 7> CHARGES_LIST
{"FELONY","MISDO","PV","PRCS","PRF","JUV","OTHER"};

static constexpr std::array<std::string_view, 5> PLEA_LIST
{"DISMIS","DIVERSION","PLEA","TRIAL","OTHER"};

/**
 * draw window
 * call any class functions that are drawn in the window
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
    DrawWriteFileButton();


    ImGui::Spacing();
    ImGui::Separator();
    if (!SummaryPath.empty())
    {
        ImGui::TextWrapped("Wrote Summary to: %s", SummaryPath.c_str());
        ImGui::SameLine();

    } else {
        ImGui::TextDisabled("No report generated yet.");
    }

    // Not working yet.
    // Should print error if csv file not found or opened
    // if (!Alert.empty())
    // {
    //     for (auto msg : Alert)
    //     {
    //         ImGui::TextWrapped(msg.c_str());
    //     }

    // }

    // TestFunction();
    //static bool didTest = false;
    //if (!didTest) { TestFunction(); didTest = true; }

    ImGui::End();  //must close what is opened
}

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

void WindowClass::DrawWriteFileButton()
{
    //check to see if all file fields are filled
    const bool ready = ReadyToGenerate();
    //if not ready, then disable text on button
    if (!ready)
    {
        ImGui::TextDisabled("Select both CSV files and an output name.");
    }

    //start the text disabled block
    ImGui::BeginDisabled(!ready);
    //generate button to generate report
    if(ImGui::Button("Generate Report", ImVec2(200, 0)))
    {
        WriteToTextFile(resultsOutputFileNameBuffer);
        // WriteToTextFile(R"(C:\Users\Kerri\Documents\BYU 310\StatsCalcCPP\matters_summary)");
    }
    //need to end text disabled block.
    ImGui::EndDisabled();
}

bool WindowClass::ReadyToGenerate() const
{
    //check to see if there are values in the buffers
    //check buffer at index 0 does not equal to null
    //return true or false
    return matterFileNameBuffer[0]!= '\0'
    && eventFileNameBuffer[0]!= '\0'
    && resultsOutputFileNameBuffer[0]!= '\0';
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

    std::strncpy(eventFileNameBuffer,
                R"(C:\Users\Kerri\Documents\BYU 310\StatsCalcCPP\events-list.csv)",
                sizeof(eventFileNameBuffer)-1);
    eventFileNameBuffer[sizeof(eventFileNameBuffer)-1] = '\0';

    //WriteToTextFile(R"(C:\Users\Kerri\Documents\BYU 310\StatsCalcCPP\matters_summary)");

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
        //get path for filename, add to alert vector
        const std::string message = filename.string();
        Alert.push_back(message);
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

    //iterate through each map to filter on column "APCON"
    for (const auto& r : rows)
    {
        //lookup on key "Casecode"
        auto it = r.find("Casecode");
        // if no "Casecode" key found or no key "Casecode" value found, continue
        if (it != r.end() && !it -> second.empty()) {
            std::string val = it->second; //get the value
            WindowClass::ToUpperCase(val);  //normalize to uppercase
            if (val == "APCON") //if value matches
            {
                data.push_back(r);  //add that "row" or map to vector
            }
        }
    }
    return data;  //return all matching "rows" as maps in vector

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
    auto matterRows = ReadCsvRows(ResolveCsvPath(matterFileNameBuffer));
    auto eventRows = ReadCsvRows(ResolveCsvPath(eventFileNameBuffer));
    auto filteredMatters = FilterApcon(matterRows);
    auto filteredEvents = FilterApcon(eventRows);

    int inputMonth = SelectedMonthNumber();
    int inputYear = SelectedYearNumber();
    auto closedMatters = GetClosedCases(filteredMatters, inputMonth, inputYear);
    auto closedEvents = GetClosedCases(filteredEvents, inputMonth, inputYear);


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
/*    std::filesystem::path matterPath{matterFileNameBuffer};


    out << "Matters CSV: " << matterFileNameBuffer << std::endl;
    out << "Matters csv full path: " << std::filesystem::absolute(matterPath) << std::endl << std::endl;

    out << "Total data rows: " << matterRows.size()  << std::endl;
    out << "Filtered by APCON size: " << filteredMatters.size() << std::endl;

    if (!filteredMatters.empty())
    {
        out << "\nFirst APCON row keys:\n";
        for (const auto& kv: filteredMatters.front())
        {
            out << " - " << kv.first << std::endl;
        }
    }
    std::cout << "matterRows=" << matterRows.size()
          << " filteredMatters=" << filteredMatters.size()
          << " closedMatters=" << closedMatters.size() << "\n";

    TestChargeOut(closedMatters);
*/
    //get data for report

    //print open cases count (subtotaled by charge type) for the selected month/year
    out << "\n" << "Opened Cases for " << inputMonth << "/" << inputYear << "\n";
//*************print opened cases count by charges



    //print closed cases count (subtotaled by charge type) for selected month/year
    out << "\n\n-------------------------------------------------------\n";
    out << "\n" << "Closed Cases for " << inputMonth << "/" << inputYear << "\n";
    //iterate through Charges array to print subtotals
    for (auto charges : CHARGES_LIST)
    {
        int16_t closedChargesCount = GetChargeCount(closedMatters, charges);
        out << charges << ":  " << closedChargesCount << '\n';
    }

    //print subtotals of dispostion on closed cases by type
    out << "\n\n-------------------------------------------------------\n";
    out << "\n" << "Subtotals by Disposition" << "\n";
    for (auto charges : CHARGES_LIST)
    {
        for (auto plea : PLEA_LIST)
        {
            int16_t dispositionCount = GetDispositionCount(closedMatters, charges, plea);
            //only print if value > 0
            if (dispositionCount > 0)
            {
                out << charges << " and " << plea << ":   " << dispositionCount << "\n";
            }
        }
    }

    //print totals for duration of time spent on cases by charges
    out << "\n\n-------------------------------------------------------\n";
    out << "\n" << "Total Time Spent On Cases (by Charges)" << "\n";
    for (auto charges : CHARGES_LIST)
    {
        double subtotal = GetSubtotalHoursByCharges(closedEvents, charges);
        if (subtotal > 0.0)
        {
            out << charges << ": " << subtotal << '\n';
        }
    }

    out << std::flush;
//    std::cout << "Wrote Summary to: " << filePath << std::endl;

    const auto absPath = std::filesystem::absolute(filePath);
    SummaryPath = absPath.string();
}


std::vector<Maps> WindowClass::GetClosedCases(const std::vector<Maps>& list, int inputMonth, int inputYear)
{
    //parameter: matters already filtered by APCON and input month and year
    //iterarates throught the vector of maps and appends to a new vector of maps where
    //the selected month and year is same as in the column for closed
    //returns a filtered vector of maps

    //create an empty vector of maps to fill and return
    std::vector<Maps> data;
    //pre-allocate memory for data before loop
    data.reserve(list.size());

    //iterate through each map to filter on month and year
    for (const auto&r : list)
    {
        //lookup on key "Closed" - column header
        auto itClosed = r.find("Closed");

        // if Closed" key not found & key "Closed" value empty, skip to next iteration
        if(itClosed == r.end() || itClosed->second.empty()) continue;

        //check for truthy - if there's a value and can parse into MonthYear struct, then
        //get the value from "Closed" key, put results in MonthYear structure variable
        if(auto monYear = GetMonthYear(itClosed->second))
        {

            //get value from parameter and pass into monYear
            //compare input with column data
            if (monYear->month == inputMonth && monYear->year == inputYear)
            {
                data.push_back(r); //add "row" or map to vector
            }

        } else
        {
            //don't print to screen because there are cells with "bad data",
            //it's ok, just move to next row
            std::cerr << "Bad Closed date: " <<itClosed->second<< std::endl;

        }

    }

    return data;  //return all matching "rows" as maps in vector
}

std::vector<Maps> WindowClass::GetOpenedCases(const std::vector<Maps>& list, int inputMonth, int inputYear)
{
    //create an empty vector of maps to fill and return
    std::vector<Maps> data;

    //pre-allocate memory for data before loop
    data.reserve(list.size());

    //iterate through each map to filter on month and year
    for (const auto&r : list)
    {
        //look up on key "Opened" - column header
        auto itOpened = r.find("Opened");

        //if "Opened" key not found & key "Opened" value empty, skip to next iteration
        if (itOpened == r.end() || itOpened->second.empty()) continue;

        //check for truthy - if there's a value and can parse into MonthYear struct, then
        //get value from "Opened"
        if(auto monYear = GetMonthYear(itOpened->second))
        {
            //get value from parameter and pass into monYear
            //compare input with column data
            if (monYear->month == inputMonth && monYear->year == inputYear)
            {
                //if all true - add "row"
                data.push_back(r);
            }
        } else{
            //this prints for each row that has bad data.  change to something else?
            std::cerr << "Bad Opened date: " <<itOpened->second<< std::endl;
        }
    }


    return data;  //return all matching "rows" as maps in vector
}

int16_t WindowClass::GetChargeCount(const std::vector<Maps>& matters, std::string_view charge)
{
    //create an count variable to return
    int16_t count = 0;

    //make string_view into a string for comparison; normalize
    std::string compare(charge);
    ToUpperCase(compare);

    //iterate through each "row" in matters
    for (const auto& r : matters)
    {
        auto itCharges = r.find("Charges"); //find "Charges" key in row
        // if "Charges" key found & key "Charges" value not empty, continue
        if (itCharges != r.end() && !itCharges -> second.empty())
        {
            std::string value = itCharges->second; //get value at key
            ToUpperCase(value); //normalize
            if(value == compare)
            {
                count++;
            }
        }
    }
    return count;
}

int16_t WindowClass::GetDispositionCount(const std::vector<Maps>& matters, std::string_view charge_sv, std::string_view disposition_sv)
{
    //create an count variable to return
    int16_t count = 0;

    //make a string from a string_view
    std::string charge(charge_sv);
    std::string disposition(disposition_sv);

    //iterate through each "row" in matters
    for (const auto& row : matters)
    {
        //look up both columns
        auto chargesPair = row.find("Charges");
        auto dispoPair = row.find("Disposition");
        //if either pair has no valid element (row.end()), continue
        if (chargesPair == row.end() || dispoPair == row.end()) continue;
        //if either pair has value = is empty, continue
        if (chargesPair->second.empty() || dispoPair->second.empty()) continue;

        //trim views, then copy to strings so we can uppercase
        auto svC = Trim(chargesPair->second);
        auto svD = Trim(dispoPair->second);
        //check again for empty
        if (svC.empty() || svD.empty()) continue;

        //make string from string_view
        std::string cellCharges(svC);
        std::string cellDispo(svD);
        //normalize
        ToUpperCase(cellCharges);
        ToUpperCase(cellDispo);

        //check for matching charge and dispo to increment count
        if (cellCharges==charge && cellDispo == disposition)
        {
            ++count;
        }
    }
    return count;
}

double WindowClass::GetSubtotalHoursByCharges(const std::vector<Maps>& events, std::string_view charge_sv)
{
    //create a count variable to return
    double subtotal = 0;

    //make a string from a string_view
    std::string charge(charge_sv);

    // iterate through each "row" in matters
    for (const auto& row : events)
    {
        auto itCharges = row.find("Charges"); //find "Charges" in row
        auto itDuration = row.find("Duration");  //find "Duration" in row

        // if "Charges" key found & key "Charges" value not empty, continue
        if (itCharges == row.end() || itDuration == row.end()) continue;
        if (itCharges->second.empty() || itDuration->second.empty()) continue;


        // normalize charge cell
        auto svCharge = Trim(itCharges->second);
        std::string cellCharge(svCharge);
        ToUpperCase(cellCharge);

        //if charges don't match with filter, continue to next row
        if (cellCharge != charge) continue;

        //for rows where charges match
        // trim and check or digits
        auto svDur = Trim(itDuration->second);
        //if (!CheckDigits(svD)) continue; //Can't use now because of "1.5" type numbers
        //if it's a number, make it a number then add it to the subtotal
        try
        {
            double duration = std::stod(std::string(svDur));
            subtotal += duration;
        }
        catch (const std::invalid_argument&)
        {
            //not a number
            std::cerr << "Bad Duration: '" << std::string(svDur) << "'\n";
        }

    }
    return subtotal;
}

void WindowClass::TestChargeOut(const std::vector<Maps>& rows)
{
    std::map<std::string,int> check;
    for (auto& r : rows)
    {
        auto pair = r.find("Charges");
        if (pair == r.end() || pair->second.empty())
        {
            continue;
        }
        auto strView = WindowClass::Trim(pair->second);
        std::string value(strView);  //create string from string view
        WindowClass::ToUpperCase(value);
        ++check[value];
    }
    for (auto& [k,v]: check)
    {
        std::cout << k << " : " << v << "\n";
    }

}

std::filesystem::path WindowClass::ResolveCsvPath(std::string_view text) const
{
    //had help with this, but needed something!!!
    //If the user only enters a filename, tries a few option:
    //Current Working Directory (CWD), sibling's Csv folder, parents of CWD
    using std::filesystem::path;
    using std::filesystem::exists;
    path myPath{std::string(text)};

    //check for absolute or already resolvable relative to CWD
    if (myPath.is_absolute() && exists(myPath))
    {
        return myPath;
    }
    if (!myPath.is_absolute() && exists(myPath))
    {
        return std::filesystem::absolute(myPath);
    }

    //try sibling of the other file (events or matters)
    //lambda [&] = capture by reference
    //buffer to hold the "other" file path to compare to
    auto try_base = [&](const char* buffer)->std::optional<path>
    {
        //make sure string is not empty/nullptr
        if (buffer && buffer[0])
        {
            //convert buffer into file system path, then just get directory
            path base = path(buffer).parent_path();
            if (!base.empty())
            {
                //join base and directory of sibling
                path fullPath = base / myPath;
                //if it exists, return fullPath
                if (exists(fullPath))
                {
                    //base + path of sibling
                    return fullPath;
                }
            }
        }
        //else signal "not found"
        return std::nullopt;
    };

    //use lambda function on both buffers
    if (auto x = try_base(eventFileNameBuffer))  return *x;
    if (auto x = try_base(matterFileNameBuffer)) return *x;


    //walk up a few parents from CWD
    path dir = std::filesystem::current_path();
    for (int i=0; i < 3 && dir.has_parent_path(); ++i)
    {
        dir = dir.parent_path();
        path fullPath = dir / myPath;  //add directory to myPath
        if (exists(fullPath))
        {
            return fullPath;
        }
    }

    //give back whatever we have;
    return myPath;
}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
