#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cstdint>
#include <algorithm>
#include <source_location>
#include <cmath>
#include <chrono>
#include <stacktrace>
#include <iomanip>
void InvalidInputMessage(const std::string &details, std::source_location location)
{
    std::cout << std::stacktrace::current() << std::endl;
    std::cout << "Filename:" << location.file_name() << std::endl;
    std::cout << "Function:" << location.function_name() << std::endl;
    std::cout << "Line:" << location.line() << std::endl;
    std::cout << "Column:" << location.column() << std::endl;
    if (!details.empty())
    {
        std::cout << "Details provided:" << details << std::endl;
    }
    exit(1);
}
