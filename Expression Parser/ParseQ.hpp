/*
* ParseQ library is an expression parsing library, which provides various features:
*     Computing expressions according to the operator's precedence
*     Parenthesis support
*     Functions support
* TODO Items:
*     Unary operators
*     User-defined functions
*     More functions
*
* Developed by hikarideai
* 06.112016
*/

#pragma once
#include <map>
#include <deque>
#include <functional>

#define uint unsigned int

enum TokenType { 
	UNDEFINED, VARIABLE, NUMBER, FUNCTION, 
	OPENBRACE, CLOSEBRACE, ADDITION, 
	SUBTRACTION, MULTIPLICATION, DIVISION 
};

extern double pi;
extern std::map <char, std::function<double(double, double)>> operators;
extern std::map <std::string, std::function<double(double)>> functions;
//std::vector<std::string> func1 {'sqrt', 'sin', 'cos', 'tg'};

struct Token {
	Token() {
		type = UNDEFINED;
		data = "";
	}

	Token(TokenType t, std::string d)
	: type(t), data(d) {}

	TokenType type;
	std::string data;
};

class Equation {
public:
	Equation() {}
	Equation(std::string ifx) {
		//buildPrecedence();
		parse(ifx);
	}

	void parse(std::string ifx);
	double calc();

	void setVar(std::string id, double value) {
		vars[id] = value;
	}

	size_t varCount() {
		return vars.size();
	}

	double getVal(std::string id) {
		return vars[id];
	}

	std::map<std::string, double>& getVars() {
		return vars;
	}
	std::deque<std::string>& getVals() {
		return val_table;
	}

	bool failed()
	{
		return error;
	}

	std::string what()
	{
		return message;
	}

private:
	std::deque<Token> expression;
	std::map<std::string, double> vars;
	std::deque<std::string> val_table;
	void addValTab(std::string s) {
		for (auto &i : val_table)
			if (i == s) return;
		val_table.push_back(s);
	}
	/*std::map<std::string, uint> precedence;
	void buildPrecedence() {
	precedence["("] = 0;
	precedence["+"] = 1; precedence["-"] = 1;
	precedence["*"] = 2; precedence["/"] = 2;
	precedence["^"] = 3;
	}*/
	void parseInfix(std::string ifx, std::deque<Token> &exp);

	bool error = true;
	std::string message = "Equation have not initialized";
};
