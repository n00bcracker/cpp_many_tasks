#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

struct Student {
    std::string name, surname;
    int day, month, year;
};

enum class SortType { kByName, kByDate };

class CompareStudents {
    SortType sort_type_;

public:
    CompareStudents(SortType sort_type) : sort_type_(sort_type) {
    }

    bool operator()(const Student& s1, const Student& s2) const {
        int64_t bday_student1, bday_student2;
        bday_student1 = GetBday(s1);
        bday_student2 = GetBday(s2);
        std::string fullname_s1, fullname_s2;
        fullname_s1 = s1.surname + s1.name;
        fullname_s2 = s2.surname + s2.name;

        if (sort_type_ == SortType::kByDate) {
            if (bday_student1 == bday_student2) {
                return fullname_s1 < fullname_s2;
            } else {
                return bday_student1 < bday_student2;
            }
        } else if (sort_type_ == SortType::kByName) {
            if (fullname_s1 == fullname_s2) {
                return bday_student1 < bday_student2;
            } else {
                return fullname_s1 < fullname_s2;
            }
        } else {
            return false;
        }
    }

    static int64_t GetBday(const Student& student) {
        return static_cast<int64_t>(student.year) * 10000 +
               static_cast<int64_t>(student.month) * 100 + student.day;
    }
};

void SortStudents(std::vector<Student>* students, SortType sort_type) {
    std::ranges::sort(*students, CompareStudents(sort_type));
}
