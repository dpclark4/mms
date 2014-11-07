#pragma once

#include <string>

namespace sim {

// The function returns the directory of the maze file.
// It takes in the name of the executed binary file for location reference.
std::string getMazeFileDirPath();

std::string getProjectDirectory();

int mazeFileWidth(std::string mazeFilePath);

int mazeFileHeight(std::string mazeFilePath);

bool checkValidMazeFile(std::string mazeFilePath); // TODO: Mack Ward

bool checkValidMazeFileTom(std::string mazeFilePath, int height, int width);

bool hasOnlyDigits(const std::string &str);

} // namespace sim
