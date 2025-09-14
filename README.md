# Stats Report

## Overview

This is a project I added to my portfolio to accomplish two things.  (1) I wanted to learn C++ through practice and (2) I wanted to create a program to help me accomplish an assignment that I have at work.  By solving a problem I already had, I was able to push through hurdles to get the desired results.  Also, the solution made more sense than a theoretical problem because I could visualize it.

This project creates a GUI interface that enables a user to enter two CSV files with data in it, choose a month and year for the report, create a name for the output file and create a text file with statistical information printed out.  Within the code, it reads column data.  I have functions to read from and write to files.  I have functions to filter the data based on values in the column. I have functions to count column contents when they meet criteria and print values for display.  

I work for a law office and need to report certain data to the county each month.  I need to report how many cases were appointed by the court in criminal matters, what the charges were, and report the count on those types.  I need to do something similar for cases that have closed in the reporting month but also include totals of how many total hours spent on felony cases, and how many felonies were closed that month as an example.  These are some of the values that I needed to calculate.  In doing so I handled date fields "xx/xx/xxxx" and time or duration amounts that needed to be tallied along with strings that needed a total count done.


{Provide a link to your YouTube demonstration. It should be a 4-5 minute demo of the software running and a walkthrough of the code. Focus should be on sharing what you learned about the language syntax.}

[Software Demo Video](http://youtube.link.goes.here)

## Development Environment

C++ standard library
ImGui
CMake
VSCode

## Useful Websites

Helpful websites:
- [cplusplus](https://cplusplus.com)
- [Gormanalysis](https://www.gormanalysis.com/blog/eading-and-writing-csv-files-with-cpp/)
- [ImGui Library](https://github.com/ocornut/imgui)
- [Studyplan](https://www.studyplan.dev/pro-cpp)
- [Microsoft](https://learn.microsoft.com/en-us/cpp)
- [Udemy - Interactive GUI with C++ and ImGui (practical examples)](https://www.udemy.com/course/interactive-gui-with-c-and-imgui-practical-examples/ )


## Future Work

- Add an alert vector to display if input file(s) not found
- Add better error handling
- Refactor code into different classes/files
