#include <vector>
#include <string>

std::vector<std::string> split(std::string sentence);

std::vector<std::string> split_lines(std::string lines_raw);

void vector_print(std::vector<int> vect);

void replace(std::vector<std::string>& words, std::string target, std::vector<std::string> replacement);

int exp(int b, int p);

bool is_elem(std::string arg, std::vector<std::string> array);

bool is_numerical(std::string arg);
