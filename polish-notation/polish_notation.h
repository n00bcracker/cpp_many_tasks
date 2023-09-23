#pragma once

#include <string>
#include <stack>
#include <set>
#include <stdexcept>

std::vector<std::string> Split(const std::string& string, const std::string& delimiter = " ") {
    std::vector<std::string> res;
    if (!string.empty()) {
        int start_pos = 0;
        size_t fpos;
        while ((fpos = string.find(delimiter, start_pos)) != std::string::npos) {
            res.push_back(string.substr(start_pos, fpos - start_pos));
            start_pos = fpos + delimiter.size();
        }

        res.push_back(string.substr(start_pos));
    }

    return res;
}

bool IsOperation(char c) {
    std::set<char> opers = {'+', '-', '*'};
    if (opers.contains(c)) {
        return true;
    } else {
        return false;
    }
}

int EvaluateExpression(const std::string& expression) {
    std::vector<std::string> parts = Split(expression);
    std::stack<int> numbers;
    for (const auto& p : parts) {
        if (p.size() == 1 && IsOperation(p[0])) {
            int arg2 = numbers.top();
            numbers.pop();
            int arg1 = numbers.top();
            numbers.pop();

            int res;
            switch (p[0]) {
                case '+':
                    res = arg1 + arg2;
                    break;
                case '-':
                    res = arg1 - arg2;
                    break;;
                case '*':
                    res = arg1 * arg2;
                    break;
                default:
                    break;
            }

            numbers.push(res);
        } else {
            numbers.push(std::stoi(p));
        }
    }

    return numbers.top();
}
