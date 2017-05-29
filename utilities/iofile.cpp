//
// Created by Simon on 29.05.2017.
//

#include "iofile.hpp"
#include <fstream>
#include <iostream>

namespace drdemo {

    bool ReadFile(std::string const &file_name, std::vector<std::string> &lines) {
        // Create buffer to old the current line
        char buffer[1024];

        // Output file we are reading
        std::cout << "Reading file: " << file_name << std::endl;

        // Open file
        std::ifstream file(file_name, std::ifstream::in);
        // Check that file was open
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << file_name << "!" << std::endl;

            return false;
        }

        // Loop over all lines and add them
        while (!file.eof()) {
            file.getline(buffer, 1024);
            lines.push_back(std::string(buffer));
        }

        // Close file
        file.close();
        std::cout << "Done reading file!" << std::endl;

        return true;
    }

} // drdemo namespace