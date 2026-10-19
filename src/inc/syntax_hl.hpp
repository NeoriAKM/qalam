#pragma once
#include <string>

std::string get_ext(std::string filename);

int is_comment(std::string line, std::string filename);
int is_label(std::string line, std::string filename);