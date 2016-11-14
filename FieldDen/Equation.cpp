/*************************************************************************************
ParseQ library is an expression parsing library, which provides various features:
* Computing expressions according to the operator's precedence
* Parenthesis support
* Functions support
TODO Items:
* Unary operators
* User-defined functions
* More functions

hikarideai
06.11.2016
*************************************************************************************/


#include <string>
#include <stack>
#include <iostream>

//Tests
#include  <fstream>
#include "Equation.hpp"

#define DEBUG 0

double pi = 3.141592;

std::map <char, std::function<double(double, double)>> operators{
	{ '+', [](double a, double b) { return a + b; } },
	{ '-', [](double a, double b) { return a - b; } },
	{ '*', [](double a, double b) { return a * b; } },
	{ '/', [](double a, double b) { return a / b; } }
};

std::map <std::string, std::function<double(double)>> functions{
	{ "sqr", [](double a) { return a * a; } },
	{ "sqrt", [](double a) { return std::sqrt(a); } },
	{ "cos", [](double a) { return std::cos(a); } },
	{ "sin", [](double a) { return std::sin(a); } },
	{ "tan", [](double a) { return std::tan(a); } },
	{ "abs", [](double a) { return std::abs(a); } },
	{ "round", [](double a) { return std::round(a); } },
	{ "floor", [](double a) { return std::floor(a); } },
	{ "ceil", [](double a) { return std::ceil(a); } },
	{ "exp", [](double a) { return std::exp(a); } },
	{ "log", [](double a) { return std::log(a); } },
};
void Equation::addValTab(std::string s)
{
	if (std::find(ignore_list_.begin(), ignore_list_.end(), s) != ignore_list_.end())
		return;
	for (auto &i : val_table)
		if (i == s) return;
	val_table.push_back(s);
}
void Equation::parseInfix(std::string ifx, std::deque<Token> &exp) {
	if (DEBUG) std::cout << "Tokenizing";
	size_t i = 0;

	auto readNumber = [&]() -> std::string {
		std::string	num = "";

		size_t start = i;
		for (; i < ifx.size(); i++) {
			if (!(ifx[i] == '.' || (0 <= ifx[i] - '0' && ifx[i] - '0' < 10))) {
				i--;
				break;
			}
		}

		num = ifx.substr(start, i - start + 1);

		return num;
	};

	auto readWord = [&]() -> std::string {
		size_t start = i;
		for (; i < ifx.size(); i++) {
			if (ifx[i] == '('
				|| ifx[i] == ')'
				|| operators.find(ifx[i]) != operators.end()) {
				i--;
				break;
			}
		}
		return ifx.substr(start, i - start + 1);
	};

	for (; i < ifx.size(); i++) {
		if (0 <= ifx[i] - '0' && ifx[i] - '0' < 10) {
			exp.push_back({ NUMBER, readNumber() });
		}
		else if (ifx[i] == '+') {
			exp.push_back({ ADDITION, "+" });
		}
		else if (ifx[i] == '-') {
			exp.push_back({ SUBTRACTION, "-" });
		}
		else if (ifx[i] == '*') {
			exp.push_back({ MULTIPLICATION, "*" });
		}
		else if (ifx[i] == '/') {
			exp.push_back({ DIVISION, "/" });
		}
		else if (ifx[i] == '(') {
			exp.push_back({ OPENBRACE, "(" });
		}
		else if (ifx[i] == ')') {
			exp.push_back({ CLOSEBRACE, ")" });
		}
		else {
			std::string id = readWord();
			if (functions.find(id) == functions.end()) {
				vars[id] = 0;
				exp.push_back({ VARIABLE, id });
				addValTab(id);
			}
			else
			{
				exp.push_back({ FUNCTION, id });
			}
		}
	}
	if (DEBUG) std::cout << '.';
}

void Equation::parse(std::string ifx) {
	if (DEBUG) std::cout << "StringNotEmpty";
	if (ifx.empty())
	{
		error = true;
		message = "Passed empty string.";
		return;
	}

	if (DEBUG) std::cout << ".BraceBalance"; {
		int braces = 0;
		for (auto &i : ifx)
			if (i == '(') braces++;
			else if (i == ')') braces--;

		if (braces > 0) {
			error = true;
			message = "Brace balance corrupt(Maybe you forgot to put \')\'";
			return;
		}
		else if (braces < 0) {
			error = true;
			message = "Brace balance corrupt(Maybe you forgot to put \'(\'";
			return;
		}
	}

	if (DEBUG) std::cout << ".Clear";
	vars.clear();
	val_table.clear();
	if (DEBUG) std::cout << '.';

	std::deque<Token> tokens;
	parseInfix(ifx, tokens);

	expression.clear();
	error = false;


	std::stack<Token> token_stack;
	for (auto i : tokens) {
		TokenType t = i.type;
		if (t == NUMBER || t == VARIABLE) {
			expression.push_back(i);
		}
		else if (t == FUNCTION)
		{
			token_stack.push(i);
		}
		else if (t == OPENBRACE) {
			token_stack.push(i);
		}
		else if (t == CLOSEBRACE) {
			while (!token_stack.empty() && token_stack.top().type != OPENBRACE) {
				expression.push_back(token_stack.top());
				token_stack.pop();
			}

			//If we reached open para then pop it, else there are an error
			if (token_stack.top().type == OPENBRACE) {
				token_stack.pop();
				if (token_stack.top().type == FUNCTION)
				{
					expression.push_back(token_stack.top());
					token_stack.pop();
				}
			}
			else {
				error = true;
				message = "Scope violation";
				return;
			}
		}
		else if (ADDITION <= t && t <= DIVISION) {
			while (!token_stack.empty() && t <= token_stack.top().type) {
				expression.push_back(token_stack.top());
				token_stack.pop();
			}
			token_stack.push(i);
		}
	}

	while (!token_stack.empty()) {
		expression.push_back(token_stack.top());
		token_stack.pop();
	}

	if (DEBUG) std::cout << "OK\n";
}

double Equation::calc() {
	if (error) return 0;

	std::stack<double> numbers;
	for (auto &i : expression) {
		TokenType t = i.type;
		if (t == NUMBER) {
			numbers.push(std::stod(i.data, nullptr));
		}
		else if (t == VARIABLE) {
			numbers.push(vars[i.data]);
		}
		else if (ADDITION <= t && t <= DIVISION) {
			if (numbers.size() > 1) {
				double arg2 = numbers.top();
				numbers.pop();
				double arg1 = numbers.top();
				numbers.pop();
				numbers.push(operators[i.data[0]](arg1, arg2));
			}
			else
			{
				error = true;
				message = "Operator's argument flaw";
				return 0;
			}
		}
		else if (t == FUNCTION) {
			if (numbers.size() > 0) {
				double arg1 = numbers.top();
				numbers.pop();
				numbers.push(functions[i.data](arg1));
			}
			else
			{
				error = true;
				message = "Function's argument flaw";
				return 0;
			}
		}
		else {
			error = true;
			message = "Unknown error";
			return 0;
		}
	}

	return numbers.top();
}

/*
int main() {
	std::fstream tests;
	std::string path;
	std::cout << "Run ";
	std::cin >> path;
	tests.open(path, tests.in);

	int overall = 0, passed = 0;
	while (!tests.eof()) {
		overall++;

		std::string infix;
		tests >> infix;

		std::cout << "Test " << overall << "\n";
		std::cout << "Input: " << infix << "\n";

		Equation eqt(infix);
		for (int i = 0; i < eqt.varCount(); i++) {
			std::string id;
			double val;
			tests >> id >> val;
			eqt.setVar(id, val);
		}

		double answer, output;
		tests >> answer;
		output = eqt.calc();

		std::cout << "Answer: " << answer << "\n";
		std::cout << "Output: " << output << "\n";
		if (std::to_string(answer) == std::to_string(output)) {
			passed++;
			std::cout << "Test passed.\n\n";
		}
		else std::cout << "Test failed.\n\n";
	}
	tests.close();

	std::cout << "Test completed(" << overall << '/' << passed << ")\n";
	std::cout << "Success rate: " << passed * 100 / overall << "%\n";

	system("PAUSE");
	return 0;
}*/