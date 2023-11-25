#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <variant>
#include <iterator>

struct Text {
    std::string before_curs_, after_curs_;
};

class Command {
public:
    virtual int Do(Text& text) = 0;
    virtual void Undo(Text& text) = 0;
    virtual ~Command() = default;
};

class TypeChar : public Command {
public:
    TypeChar(char c) : c_(c) {
    }

    int Do(Text& text) override {
        text.before_curs_.push_back(c_);
        return 0;
    }

    void Undo(Text& text) override {
        text.before_curs_.pop_back();
    }

private:
    char c_;
};

class MoveLeft : public Command {
public:
    int Do(Text& text) override {
        if (!text.before_curs_.empty()) {
            text.after_curs_.push_back(text.before_curs_.back());
            text.before_curs_.pop_back();
            return 0;
        } else {
            return 1;
        }
    }

    void Undo(Text& text) override {
        text.before_curs_.push_back(text.after_curs_.back());
        text.after_curs_.pop_back();
    }
};

class MoveRight : public Command {
public:
    int Do(Text& text) override {
        if (!text.after_curs_.empty()) {
            text.before_curs_.push_back(text.after_curs_.back());
            text.after_curs_.pop_back();
            return 0;
        } else {
            return 1;
        }
    }

    void Undo(Text& text) override {
        text.after_curs_.push_back(text.before_curs_.back());
        text.before_curs_.pop_back();
    }
};

class Erase : public Command {
public:
    int Do(Text& text) override {
        if (!text.before_curs_.empty()) {
            c_ = text.before_curs_.back();
            text.before_curs_.pop_back();
            return 0;
        } else {
            return 1;
        }
    }

    void Undo(Text& text) override {
        text.before_curs_.push_back(c_);
    }

private:
    char c_;
};

class Editor {
public:
    class TextView {
    public:
        TextView(std::string::const_iterator start, std::string::const_iterator end,
                 std::string::const_iterator cursor_prev,
                 std::string::const_reverse_iterator cursor_next)
            : start_(start), end_(end), cursor_prev_(cursor_prev), cursor_next_(cursor_next) {
        }

        TextView(std::string::const_iterator start, std::string::const_reverse_iterator end,
                 std::string::const_iterator cursor_prev,
                 std::string::const_reverse_iterator cursor_next)
            : start_(start), end_(end), cursor_prev_(cursor_prev), cursor_next_(cursor_next) {
        }

        TextView(std::string::const_reverse_iterator start, std::string::const_reverse_iterator end,
                 std::string::const_iterator cursor_prev,
                 std::string::const_reverse_iterator cursor_next)
            : start_(start), end_(end), cursor_prev_(cursor_prev), cursor_next_(cursor_next) {
        }

        class Iterator {
        public:
            Iterator(
                std::variant<std::string::const_iterator, std::string::const_reverse_iterator> cur,
                std::string::const_iterator cursor_prev,
                std::string::const_reverse_iterator cursor_next)
                : it_(cur), cursor_prev_(cursor_prev), cursor_next_(cursor_next) {
            }

            Iterator& operator++() {
                if (std::holds_alternative<std::string::const_iterator>(it_)) {
                    auto it = ++std::get<std::string::const_iterator>(it_);
                    if (it == cursor_prev_) {
                        it_ = cursor_next_;
                    } else {
                        it_ = it;
                    }
                } else {
                    it_ = ++std::get<std::string::const_reverse_iterator>(it_);
                }

                return *this;
            }

            char operator*() const {
                if (std::holds_alternative<std::string::const_iterator>(it_)) {
                    return *std::get<std::string::const_iterator>(it_);
                } else {
                    return *std::get<std::string::const_reverse_iterator>(it_);
                }
            }

            const char* operator->() const {
                if (std::holds_alternative<std::string::const_iterator>(it_)) {
                    return &(*std::get<std::string::const_iterator>(it_));
                } else {
                    return &(*std::get<std::string::const_reverse_iterator>(it_));
                }
            }

            bool operator==(const Iterator& other) const {
                return it_ == other.it_;
            }

        private:
            std::variant<std::string::const_iterator, std::string::const_reverse_iterator> it_;
            std::string::const_iterator cursor_prev_;
            std::string::const_reverse_iterator cursor_next_;
        };

        Iterator begin() const {
            return Iterator(start_, cursor_prev_, cursor_next_);
        }

        Iterator end() const {
            return Iterator(end_, cursor_prev_, cursor_next_);
        }

    private:
        std::variant<std::string::const_iterator, std::string::const_reverse_iterator> start_, end_;
        std::string::const_iterator cursor_prev_;
        std::string::const_reverse_iterator cursor_next_;
    };

    void Type(char symbol) {
        TypeChar op(symbol);
        if (!op.Do(text_)) {
            history_.push(std::make_unique<TypeChar>(op));
            trash_ = std::stack<std::unique_ptr<Command>>();
        }
    }

    void ShiftLeft() {
        MoveLeft op;
        if (!op.Do(text_)) {
            history_.push(std::make_unique<MoveLeft>(op));
            trash_ = std::stack<std::unique_ptr<Command>>();
        }
    }

    void ShiftRight() {
        MoveRight op;
        if (!op.Do(text_)) {
            history_.push(std::make_unique<MoveRight>(op));
            trash_ = std::stack<std::unique_ptr<Command>>();
        }
    }

    void Backspace() {
        Erase op;
        if (!op.Do(text_)) {
            history_.push(std::make_unique<Erase>(op));
            trash_ = std::stack<std::unique_ptr<Command>>();
        }
    }

    void Undo() {
        if (!history_.empty()) {
            history_.top()->Undo(text_);
            trash_.push(std::move(history_.top()));
            history_.pop();
        }
    }

    void Redo() {
        if (!trash_.empty()) {
            trash_.top()->Do(text_);
            history_.push(std::move(trash_.top()));
            trash_.pop();
        }
    }

    size_t Size() const {
        return text_.before_curs_.size() + text_.after_curs_.size();
    }

    TextView GetText(size_t pos, size_t count) const {
        const auto end = pos + count;
        if (pos < text_.before_curs_.size()) {
            if (end >= text_.before_curs_.size()) {
                return TextView(text_.before_curs_.begin() + pos,
                                text_.after_curs_.rbegin() + end - text_.before_curs_.size(),
                                text_.before_curs_.end(), text_.after_curs_.rbegin());
            } else {
                return TextView(text_.before_curs_.begin() + pos, text_.before_curs_.begin() + end,
                                text_.before_curs_.end(), text_.after_curs_.rbegin());
            }
        } else {
            return TextView(text_.after_curs_.rbegin() + pos - text_.before_curs_.size(),
                            text_.after_curs_.rbegin() + end - text_.before_curs_.size(),
                            text_.before_curs_.end(), text_.after_curs_.rbegin());
        }
    }

    size_t GetPosition() const {
        return text_.before_curs_.size();
    }

private:
    Text text_;
    std::stack<std::unique_ptr<Command>> history_, trash_;
};
