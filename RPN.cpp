#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include "RPN_util.h"

// Error code, used in the if (ERR) portion of eval.
int ERR = 0;

// In verbose mode, the current instruction and stack are printed each tick.
bool verbose = false;

// Variables. Note that variables do not have scope, they are all global.
std::unordered_map<std::string, int> var_values{};

// Functions. Functions are executed as their own program on a separate stack.
std::unordered_map<std::string, std::pair<int, std::vector<std::vector<std::string>>>> functions{};

// Binary operations.
const std::vector<std::string> bin_ops = {"+", "-", "*", "/", "%", "=", ">", "<", "<=", ">=", "!=", "^", "max", "min"};

// Unary operations. ! and : have to be handled outside the unary_op function.
const std::vector<std::string> unary_ops = {"|", "~", "!", ":"};

int bin_op(std::string op, int a, int b)
{
	if (op.size() == 1)
	{
		switch (op[0])
		{
			case '+': return a + b;
			case '-': return a - b;
			case '*': return a * b;
			case '/': return a / b;
			case '%': return a % b;
			case '>': return a > b;
			case '<': return a < b;
			case '^': return exp(a, b);
			case '=': return a == b;
		}
	}
	else if (op == "<=")
	{
		return a <= b;
	}
	else if (op == ">=")
	{
		return a >= b;
	}
	else if (op == "!=")
	{
		return a != b;
	}
	else if (op == "max")
	{
		return (a > b) ? a : b;
	}
	else if (op == "min")
	{
		return (a < b) ? a : b;
	}
}

int unary_op(std::string op, int a)
{
	if (op.size() == 1)
	{
		switch (op[0])
		{
			case '|': return (a > 0) ? a : -a;
			case '~': return -a;
		}
	}
}

// Increment the Instruction Pointer.
void incr_IP(std::vector<std::vector<std::string>> commands, std::vector<int>& IP)
{
	++IP[1];
	while (IP[1] >= commands[IP[0]].size())
	{
		IP[1] = 0;
		++IP[0];
	}
}

// Forward declaration of eval allows us to use it to parse functions.
void eval(std::vector<std::vector<std::string>> code, std::vector<int>& stack);

// Evaluate the result of a single word.
void parse(std::vector<int>& stack, std::vector<std::vector<std::string>> commands, std::vector<int>& IP, std::string word)
{
	if (verbose) std::cout << "Parsing " << word << " with IP " << IP[0] << ", " << IP[1] << std::endl;
	if (is_numerical(word))
	{
		stack.push_back(std::stoi(word, nullptr, 10));
	}
	else if (is_elem(word, bin_ops))
	{
		if (stack.size() < 2)
		{
			ERR = 2;
			return;
		}
		int a, b;
		b = stack.back();
		stack.pop_back();
		a = stack.back();
		stack.pop_back();
		stack.push_back(bin_op(word, a, b));
	}
	else if (is_elem(word, unary_ops))
	{
		if (stack.size() < 1)
		{
			ERR = 2;
			return;
		}
		int a = stack.back();
		stack.pop_back();
		if (word == ":")
		{
			stack.push_back(a);
			stack.push_back(a);
		}
		else if (word != "!")
		{
			stack.push_back(unary_op(word, a));
		}
	}
	else if (word == "sum")
	{
		if (stack.size() < 1)
		{
			ERR = 2;
			return;
		}
		int sum = 0, count = stack.back();
		stack.pop_back();
		if (stack.size() < count)
		{
			ERR = 2;
			return;
		}
		while (count)
		{
			sum += stack.back();
			stack.pop_back();
			--count;
		}
		stack.push_back(sum);
	}
	else if (word == "prod")
	{
		if (stack.size() < 1)
		{
			ERR = 2;
			return;
		}
		int prod = 1, count = stack.back();
		stack.pop_back();
		if (stack.size() < count)
		{
			ERR = 2;
			return;
		}
		while (count)
		{
			prod *= stack.back();
			stack.pop_back();
			--count;
		}
		stack.push_back(prod);
	}
	else if (word == "size")
	{
		stack.push_back(stack.size());
	}
	else if (word == "jump")
	{
		if (stack.size() < 2)
		{
			ERR = 2;
			return;
		}
		int a = stack.back(), b;
		stack.pop_back();
		b = stack.back();
		stack.pop_back();
		if (b >= commands.size() || a >= commands[b].size())
		{
			ERR = 1;
			return;
		}
		IP[0] = b;
		IP[1] = --a;
	}
	else if (word == "prn")
	{
		if (stack.size() < 1)
		{
			ERR = 2;
			return;
		}
		int a = stack.back();
		stack.pop_back();
		std::cout << a;
	}
	else if (word == "prc")
	{
		if (stack.size() < 1)
		{
			ERR = 2;
			return;
		}
		int a = stack.back();
		stack.pop_back();
		std::cout << char(a);
	}
	else if (word == "in")
	{
		int a;
		std::cin >> a;
		stack.push_back(a);
	}
	else if (word == "if")
	{
		if (stack.size() < 1)
		{
			ERR = 2;
			return;
		}
		int a = stack.back();
		stack.pop_back();
		if (a == 0)
		{
			int loop_depth = 1;
			while (loop_depth)
			{
				incr_IP(commands, IP);
				if (IP[0] > commands.size())
				{
					ERR = 3;
					return;
				}
				if (commands[IP[0]][IP[1]] == "if")
				{
					++loop_depth;
				}
				else if (commands[IP[0]][IP[1]] == "func")
				{
					++loop_depth;
				}
				else if (commands[IP[0]][IP[1]] == "let")
				{
					++loop_depth;
				}
				else if (commands[IP[0]][IP[1]] == "end")
				{
					--loop_depth;
				}
				else if (commands[IP[0]][IP[1]] == "else" && loop_depth == 1)
				{
					--loop_depth;
				}
			}
		}
	}
	else if (word == "else")
	{
		int loop_depth = 1;
		while (loop_depth)
		{
			incr_IP(commands, IP);
			if (IP[0] > commands.size())
			{
				ERR = 3;
				return;
			}
			if (commands[IP[0]][IP[1]] == "if")
			{
				++loop_depth;
			}
			else if (commands[IP[0]][IP[1]] == "let")
			{
				++loop_depth;
			}
			else if (commands[IP[0]][IP[1]] == "func")
			{
				++loop_depth;
			}
			else if (commands[IP[0]][IP[1]] == "end")
			{
				--loop_depth;
			}
		}
	}
	else if (word == "rev")
	{
		std::reverse(stack.begin(), stack.end());
	}
	else if (word == "get")
	{
		if (stack.size() < 1)
		{
			ERR = 2;
			return;
		}
		int a = stack.back();
		stack.pop_back();
		if (stack.size() < a)
		{
			ERR = 2;
			return;
		}
		int b = stack[a];
		stack.erase(stack.begin() + a);
		stack.push_back(b);
	}
	else if (word == "$")
	{
		if (stack.size() < 2)
		{
			ERR = 2;
			return;
		}
		int a = stack.back(), b;
		stack.pop_back();
		b = stack.back();
		stack.pop_back();
		stack.push_back(a);
		stack.push_back(b);
	}
	else if (functions.count(word))
	{
		int arg_c = functions[word].first;
		if (arg_c > stack.size())
		{
			ERR = 2;
			return;
		}
		std::vector<int> func_stack{};
		while (arg_c)
		{
			func_stack.push_back(stack.back());
			stack.pop_back();
			--arg_c;
		}
		eval(functions[word].second, func_stack);
		for (std::vector<int>::iterator it = func_stack.begin(); it != func_stack.end(); ++it)
		{
			stack.push_back(*it);
		}
	}
	else if (var_values.count(word))
	{
		stack.push_back(var_values[word]);
	}
	if (verbose)
	{
		std::cout << "Stack is now ";
		vector_print(stack);
	}
	return;
}

void eval(std::vector<std::vector<std::string>> code, std::vector<int>& stack)
{
	std::vector<int> IP{0, 0};
	while (IP[0] < code.size() && !ERR)
	{
		// Make sure the IP points inside the code.
		if (code[IP[0]].size() <= IP[1])
		{
			incr_IP(code, IP);
		}
		// Function definition.
		if (code[IP[0]][IP[1]] == "func")
		{
			if (verbose) std::cout << "Begining function definition:\n";
			incr_IP(code, IP);
			// Function definition must be of the form: func arg_count function_name function body end
			if (!is_numerical(code[IP[0]][IP[1]]))
			{
				ERR = 4;
				break;
			}
			int arg_count = std::stoi(code[IP[0]][IP[1]], nullptr, 10);
			if (verbose) std::cout << "\targc = " << arg_count << "\n";
			incr_IP(code, IP);
			// Get function name
			std::string func_name = code[IP[0]][IP[1]];
			if (verbose) std::cout << "\tname = " << func_name << "\n";
			// Identify function body.
			incr_IP(code, IP);
			std::vector<std::vector<std::string>> func_spec{};
			int loop_depth = 1;
			if (verbose) std::cout << "\tbody =\n";
			while (loop_depth)
			{
				if (verbose) std::cout << "\t\t";
				std::vector<std::string> line{};
				while (IP[1] < code[IP[0]].size() && loop_depth)
				{
					if (code[IP[0]][IP[1]] == "if")
					{
						++loop_depth;
					}
					else if (code[IP[0]][IP[1]] == "end")
					{
						--loop_depth;
					}
					else if (code[IP[0]][IP[1]] == "let")
					{
						++loop_depth;
					}
					if (verbose) std::cout << code[IP[0]][IP[1]] << " ";
					line.push_back(code[IP[0]][IP[1]]);
					++IP[1];
				}
				if (verbose) std::cout << "\n";
				IP[1] = 0;
				++IP[0];
				func_spec.push_back(line);
			}
			functions[func_name] = std::make_pair(arg_count, func_spec);
			if (verbose) std::cout << "Successfully defined function." << std::endl;
		}
		// Macros. The redefined word is replaced with the given phrase everywhere it is found. Must be on its own line.
		if (code[IP[0]].front() == "def")
		{
			if (verbose) std::cout << "Begining macro definition:\n";
			std::string macro_name = code[IP[0]][1];
			if (verbose) std::cout << "\tname = " << macro_name << "\n";
			std::vector<std::string> macro_body = code[IP[0]];
			macro_body.erase(macro_body.begin(), macro_body.begin() + 2);
			for (std::vector<std::vector<std::string>>::iterator jt = code.begin() + IP[0] + 1; jt != code.end(); ++jt)
			{
				replace(*jt, macro_name, macro_body);
			}
			IP[1] = code[IP[0]].size();
			if (verbose) std::cout << "Successful macro definition." << std::endl;
		}
		// Variables. All variables are global.
		else if (code[IP[0]][IP[1]] == "let")
		{
			if (verbose)
			{
				std::cout << "Begining variable definition:\n";
			}
			incr_IP(code, IP);
			// Make sure assignment is not empty (note that no error will be thrown if it is).
			if (code[IP[0]][IP[1]] != "end")
			{
				std::string var_name = code[IP[0]][IP[1]];
				if (verbose)
				{
					std::cout << "\tname = " << var_name << "\n";
				}
				incr_IP(code, IP);
				int loop_depth = 1;
				// Execute code in assignment body on the MAIN stack, util an 'end' not bound to an 'if' is encountered.
				while (loop_depth)
				{
					parse(stack, code, IP, code[IP[0]][IP[1]]);
					if (code[IP[0]][IP[1]] == "if")
					{
						++loop_depth;
					}
					else if (code[IP[0]][IP[1]] == "end")
					{
						--loop_depth;
					}
					else if (code[IP[0]][IP[1]] == "let")
					{
						++loop_depth;
					}
					if (loop_depth)
					{
						incr_IP(code, IP);
					}
				}
				if (stack.size() == 0)
				{
					ERR = 2;
					break;
				}
				// Variable is then assigned to the back of the stack.
				int var_val = stack.back();
				stack.pop_back();
				if (verbose)
				{
					std::cout << "\tval = " << var_val << std::endl;
				}
				var_values[var_name] = var_val;
				if (verbose) std::cout << "Successful variable definition." << std::endl;
			}
			else if (verbose)
			{
				std::cout << "\tEmpty definition." << std::endl;
			}
		}
		else
		{
			parse(stack, code, IP, code[IP[0]][IP[1]]);
		}
		incr_IP(code, IP);
	}
	if (ERR)
	{
		// Decrement IP, since it is incremented after parsing regardless of error.
		IP[1]--;
		if (IP[1] < 0)
		{
			IP[0]--;
			IP[1] = code[IP[0]].size() - 1;
		}
		std::cout << "Recieved error signal with IP " << IP[0] << ", " << IP[1] << " :\n";
		switch (ERR)
		{
			case 1: std::cout << "Out of range.\n";
				break;
			case 2: std::cout << "Not enough values on stack to perform operation.\n";
				break;
			case 3: std::cout << "Incomplete if/else statement.\n";
				break;
			case 4: std::cout << "Improper function definition.\n";
				break;
			default: std::cout << "Unknown error signal.\n";
		}
	}
	return;
}

int main(int argc, char* argv[])
{
	std::vector<std::vector<std::string>> code_words{};
	std::vector<int> program_stack{};
	if (argc < 3)
	{
		goto usage;
	}
	if (argc > 3)
	{
		for (int i = 3; i < argc; ++i)
		{
			if (std::string(argv[i]) == "verbose")
			{
				verbose = true;
			}
		}
	}
	if (std::string(argv[1]) == "file")
	{
		std::string file_name = argv[2];
		std::ifstream file(file_name);
		if (!file.is_open())
		{
			std::cout << "File error: could not open " << file_name;
			std::cout << ", try using a complete path." << std::endl;
			return -1;
		}
		// Prepare file for the eval function by splitting it into a 2D array of words.
		std::string line_raw{};
		while (getline(file, line_raw))
		{
			std::vector<std::string> line = split(line_raw);
			code_words.push_back(line);
		}
	}
	else if (std::string(argv[1]) == "string")
	{
		// Prepare code for the eval function by splitting it into a 2D array of words.
		std::string code = argv[2];
		std::vector<std::string> lines = split_lines(code);
		for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
		{
			code_words.push_back(split(*it));
		}
	}
	else
	{
		goto usage;
	}
	eval(code_words, program_stack);
	return 0;

/*  Usage:
 *  RPN file filename
 *  OR
 *  RPN string 'code to execute'
 */
usage:
	std::cout << "Usage: " << argv[0] << " [file / string] [filename / code string]\n";
	std::cout << "Example:\n" << argv[0] << " file codefile.txt\nor\n";
	std::cout << argv[0] << " string \"1 1 + prn\"" << std::endl;
	return -1;
}
