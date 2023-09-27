#pragma once

#include <vector>
#include <string>
#include <stdexcept>

struct Student {
    std::string name, surname;
    int day, month, year;
};

enum class SortType { kByName, kByDate };

void SortStudents(std::vector<Student>* students, SortType sort_type) {
    throw std::runtime_error{"Not implemented"};
}
