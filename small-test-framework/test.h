#pragma once

#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() = default;
};

class TestCreator {
public:
    TestCreator(const std::string& class_name) : test_name_(class_name) {
    }

    const std::string& GetName() const {
        return test_name_;
    }

    virtual std::unique_ptr<AbstractTest> CreateTest() = 0;

    void RunTest() {
        test_ptr_ = CreateTest();
        test_ptr_->SetUp();
        try {
            test_ptr_->Run();
        } catch (...) {
            test_ptr_->TearDown();
            throw;
        }

        test_ptr_->TearDown();
    }

    virtual ~TestCreator() = default;

private:
    const std::string test_name_;
    std::unique_ptr<AbstractTest> test_ptr_ = nullptr;
};

template <class TestClass>
class ConcreteTestCreator : public TestCreator {
public:
    ConcreteTestCreator(const std::string& class_name) : TestCreator(class_name) {
    }

    std::unique_ptr<AbstractTest> CreateTest() override {
        return std::unique_ptr<AbstractTest>(new TestClass);
    }
};

class AlreadyExist : public std::runtime_error {
public:
    AlreadyExist(const std::string& msg) : std::runtime_error(msg) {
    }
};

class TestRegistry {
public:
    TestRegistry(const TestRegistry&) = delete;
    TestRegistry(TestRegistry&&) = delete;

    static TestRegistry& Instance() {
        if (!instance) {
            instance = std::unique_ptr<TestRegistry>(new TestRegistry);
        }

        return *instance;
    }

    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        for (const auto& test_creator : registry_) {
            const auto& test_creator_ref = *test_creator;
            if (test_creator->GetName() == class_name ||
                typeid(test_creator_ref) == typeid(ConcreteTestCreator<TestClass>)) {
                throw AlreadyExist("This test type or test name already exists");
            }
        }

        registry_.emplace_back(new ConcreteTestCreator<TestClass>(class_name));
        std::sort(registry_.begin(), registry_.end(),
                  [](const auto& a, const auto& b) { return a->GetName() < b->GetName(); });
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        for (const auto& test_creator : registry_) {
            if (test_creator->GetName() == class_name) {
                return test_creator->CreateTest();
            }
        }

        throw std::out_of_range("There is no test with the same name");
    }

    void RunTest(const std::string& test_name) {
        for (const auto& test_creator : registry_) {
            if (test_creator->GetName() == test_name) {
                test_creator->RunTest();
                return;
            }
        }

        throw std::out_of_range("There is no test with the same name");
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> res;
        for (const auto& test_creator : registry_) {
            const auto& test_name = test_creator->GetName();
            if (callback(test_name)) {
                res.push_back(test_name);
            }
        }

        return res;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> res;
        for (const auto& test_creator : registry_) {
            res.push_back(test_creator->GetName());
        }

        return res;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        for (const auto& test_creator : registry_) {
            if (callback(test_creator->GetName())) {
                test_creator->RunTest();
            }
        }
    }

    void Clear() {
        registry_.clear();
    }

private:
    TestRegistry() = default;

    std::vector<std::unique_ptr<TestCreator>> registry_;
    static std::unique_ptr<TestRegistry> instance;
};

std::unique_ptr<TestRegistry> TestRegistry::instance = nullptr;

class FullMatch {
public:
    FullMatch(const std::string& pattern) : pattern_(pattern) {
    }
    bool operator()(const std::string& s) {
        return s == pattern_;
    }

private:
    std::string pattern_;
};

class Substr {
public:
    Substr(const std::string& pattern) : pattern_(pattern) {
    }
    bool operator()(const std::string& s) {
        return s.find(pattern_) != std::string::npos;
    }

private:
    std::string pattern_;
};