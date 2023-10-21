#pragma once

#include <string>
#include <cstddef>
#include <vector>

struct State {
    size_t ref_count;
    std::vector<std::string> vector;

    State() : ref_count(1) {
    }

    State(const std::vector<std::string>& other_vector) : ref_count(1), vector(other_vector) {
    }
};

class COWVector {
public:
    COWVector() {
        cow_vector_ = new State();
    }

    ~COWVector() {
        if (--cow_vector_->ref_count == 0) {
            delete cow_vector_;
        }
    }

    COWVector(const COWVector& other) {
        cow_vector_ = other.cow_vector_;
        ++cow_vector_->ref_count;
    }

    COWVector& operator=(const COWVector& other) {
        if (cow_vector_ != other.cow_vector_) {
            if (--cow_vector_->ref_count == 0) {
                delete cow_vector_;
            }

            cow_vector_ = other.cow_vector_;
            ++cow_vector_->ref_count;
        }

        return *this;
    }

    size_t Size() const {
        return cow_vector_->vector.size();
    }

    void Resize(size_t size) {
        if (size != Size()) {
            if (cow_vector_->ref_count > 1) {
                --cow_vector_->ref_count;
                cow_vector_ = new State(cow_vector_->vector);
            }

            cow_vector_->vector.resize(size);
        }
    }

    const std::string& Get(size_t at) const {
        return cow_vector_->vector.at(at);
    }

    const std::string& Back() const {
        return cow_vector_->vector.back();
    }

    void PushBack(const std::string& value) {
        if (cow_vector_->ref_count > 1) {
            --cow_vector_->ref_count;
            cow_vector_ = new State(cow_vector_->vector);
        }

        cow_vector_->vector.push_back(value);
    }

    void Set(size_t at, const std::string& value) {
        if (cow_vector_->ref_count > 1) {
            --cow_vector_->ref_count;
            cow_vector_ = new State(cow_vector_->vector);
        }

        cow_vector_->vector[at] = value;
    }

private:
    State* cow_vector_;
};
