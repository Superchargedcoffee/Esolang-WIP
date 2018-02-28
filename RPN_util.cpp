#include <vector>
#include <string>
#include <iostream>
#include "RPN_util.h"

std::vector<std::string> split(std::string sentence)
{
	std::vector<std::string> words{};
	std::string word{};
	for (std::string::iterator it = sentence.begin(); it != ++sentence.end(); ++it)
	{
		if (*it == '\t' || *it == ' ' || it == sentence.end())
		{
			if (word.size())
			{
				words.push_back(word);
				word = "";
			}
		}
		else
		{
			word.push_back(*it);
		}
	}
	return words;
}

std::vector<std::string> split_lines(std::string lines_raw)
{
	std::vector<std::string> lines{};
	std::string line{};
	for (std::string::iterator it = lines_raw.begin(); it != ++lines_raw.end(); ++it)
	{
		if (*it == '\n' || it == lines_raw.end())
		{
			if (line.size())
			{
				lines.push_back(line);
				line = "";
			}
		}
		else
		{
			line.push_back(*it);
		}
	}
	return lines;
}

void vector_print(std::vector<int> vect)
{
	for (std::vector<int>::iterator it = vect.begin(); it != vect.end(); ++it)
	{
		std::cout << *it << " ";
	}
	std::cout << std::endl;
}

void replace(std::vector<std::string>& words, std::string target, std::vector<std::string> replacement)
{
	for (int i = 0; i < words.size(); ++i)
	{
		if (words[i] == target)
		{
			words.erase(words.begin() + i);
			words.insert(words.begin() + i, replacement.begin(), replacement.end());
			i += replacement.size();
		}
	}
}

int exp(int b, int p)
{
	int x = 1, y = 1;
	while (p)
	{
		if (p & 1)
		{
			y *= x;
			x *= x;
			p >>= 1;
		}
		else
		{
			x *= x;
			p >>= 1;
		}
	}
	return x * y;
}

bool is_elem(std::string arg, std::vector<std::string> array)
{
	for (std::vector<std::string>::iterator it = array.begin(); it != array.end(); ++it)
	{
		if (arg == *it)
		{
			return true;
		}
	}
	return false;
}

bool is_numerical(std::string arg)
{
	for (auto it = arg.begin(); it != arg.end(); ++it)
	{
		if (int(*it) > 57 || int(*it) < 48)
		{
			return false;
		}
	}
	return true;
}
