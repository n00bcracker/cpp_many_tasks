#pragma once

#include <stdexcept>
#include <map>
#include <string>
#include <vector>
#include <tuple>

struct StudentName {
    std::string name, surname;

    bool operator<(const StudentName& rhs) const {
        return std::tie(surname, name) < std::tie(rhs.surname, rhs.name);
    }
};

struct Date {
    int year, month, day;

    bool operator<(const Date& rhs) const {
        return std::tie(year, month, day) < std::tie(rhs.year, rhs.month, rhs.day);
    }
};

struct Student {
    StudentName student_name;
    Date bday;
    int exam_score;
    std::vector<std::string> preferences;
};

class CompareStudents {

public:
    bool operator()(const Student& s1, const Student& s2) const {
        return std::tuple(-s1.exam_score, s1.bday, s1.student_name) <
               std::tuple(-s2.exam_score, s2.bday, s2.student_name);
    }
};

std::map<std::string, std::vector<StudentName>> GetStudents(
    const std::vector<std::pair<std::string, int>>& universities_info,
    const std::vector<std::tuple<StudentName, Date, int, std::vector<std::string>>>&
        students_info) {
    std::map<std::string, int> num_places;
    std::map<std::string, std::vector<StudentName>> univer_students;
    for (const auto& [univer_name, n_places] : universities_info) {
        num_places[univer_name] = n_places;
        univer_students[univer_name];
    }

    std::vector<Student> students;
    for (const auto& [student_name, bday, exam_score, preferences] : students_info) {
        students.emplace_back(student_name, bday, exam_score, preferences);
    }

    std::ranges::sort(students.begin(), students.end(), CompareStudents());

    for (const auto& student : students) {
        for (const auto& preference : student.preferences) {
            if (num_places.contains(preference) && num_places[preference] > 0) {
                univer_students[preference].push_back(student.student_name);
                num_places[preference] -= 1;
                break;
            }
        }
    }

    for (auto& [_, students_names] : univer_students) {
        std::ranges::sort(students_names,
                          [](StudentName sn1, StudentName sn2) { return sn1 < sn2; });
    }

    return univer_students;
}
