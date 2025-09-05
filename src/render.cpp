#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "render.hpp"


/**
 * draw window
 * call any class functions that are drawn in the window
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
void WindowClass::SaveToCsvFile(std::string_view filename)
{

}
void WindowClass::LoadFromCsvFile(std::string_view filename)
{

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


void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
