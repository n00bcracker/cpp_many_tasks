#pragma once

#include <list>
#include <string>
#include <unordered_map>

struct ListNode {
    ListNode* next;
    ListNode* prev;
    std::string key;
    std::string value;
};

class LruCache {
public:
    explicit LruCache(size_t max_size) : size_(), capacity_(max_size), head_(nullptr), cache_() {
    }

    LruCache(const LruCache&) = delete;
    LruCache& operator=(const LruCache&) = delete;

    void Set(const std::string& key, const std::string& value) {
        ListNode* node;
        if (cache_.contains(key)) {
            node = cache_[key];
            if (node != head_) {
                node->prev->next = node->next;
                if (node == head_->prev) {
                    head_->prev = node->prev;
                }
            } else {
                head_ = node->next;
            }

            if (node->next) {
                node->next->prev = node->prev;
            }

            cache_.erase(key);
            --size_;

            delete node;
        }

        node = new ListNode{head_, nullptr, key, value};
        if (head_) {
            node->prev = head_->prev;
            head_->prev = node;
        } else {
            node->prev = node;
        }

        head_ = node;
        cache_[key] = node;
        ++size_;

        if (size_ > capacity_) {
            node = head_->prev;
            node->prev->next = node->next;
            head_->prev = node->prev;
            cache_.erase(node->key);
            delete node;
            --size_;
        }
    }

    bool Get(const std::string& key, std::string* value) {
        if (cache_.contains(key)) {
            ListNode* node = cache_[key];
            if (node != head_) {
                node->prev->next = node->next;
                if (node == head_->prev) {
                    head_->prev = node->prev;
                }

                if (node->next) {
                    node->next->prev = node->prev;
                }

                node->prev = head_->prev;
                head_->prev = node;
                node->next = head_;
                head_ = node;
            }
            *value = node->value;
            return true;
        } else {
            return false;
        }
    }

    ~LruCache() {
        ListNode* node = head_;
        while (node) {
            ListNode* tmp = node;
            node = node->next;
            delete tmp;
        }

        head_ = nullptr;
        cache_.clear();
        size_ = 0;
    }

private:
    size_t size_, capacity_;
    ListNode* head_;
    std::unordered_map<std::string, ListNode*> cache_;
};
