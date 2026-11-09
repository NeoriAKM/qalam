#pragma once

#include <vector>
#include <string>

void help_msg();
void copyright_msg();
size_t buflen(size_t line, const std::vector<std::string>& lines);
std::string repeat(const std::string& str, size_t times);

void copy_string(std::vector<std::string>& lines, size_t strnum);
void copy_vec(std::vector<std::string>& lines);