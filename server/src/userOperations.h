#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <iterator>
#include "./globals.h"

std::string getFileContents(const std::filesystem::path &relativePath)
{
    std::filesystem::path fullPath = std::filesystem::path(env.config["homePath"]) / relativePath;
    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open())
        return "";

    std::string contents(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    return contents;
}