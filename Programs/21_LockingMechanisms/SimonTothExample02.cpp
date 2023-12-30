#include <mutex>
#include <thread>
#include <chrono>
#include <span>
#include <algorithm>
#include <iostream>

// https://medium.com/@simontoth/daily-bit-e-of-c-std-recursive-mutex-dd9b84f38f8d

struct NonRecursive {
    void push_back(int value) {
        std::unique_lock lock(mux_);
        // We already hold mux_, so we couldn't call reserve()
        if (size_ == capacity_)
            reserve_impl(capacity_ == 0 ? 64 : capacity_ * 2);
        data_[size_++] = value;
    }
    void reserve(size_t cnt) {
        std::unique_lock lock(mux_);
        reserve_impl(cnt);
    }
private:
    // reserve_impl expects mux_ to be held by the caller
    void reserve_impl(size_t cnt) {
        auto new_data = std::make_unique<int[]>(cnt);
        std::ranges::copy(std::span(data_.get(), size_), new_data.get());
        data_ = std::move(new_data);
        capacity_ = cnt;
        size_ = std::min(size_, capacity_);
    }
    std::mutex mux_;
    std::unique_ptr<int[]> data_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

struct Recursive {
    void push_back(int value) {
        std::unique_lock lock(mux_);
        // holding a recursive mutex multiple times is fine
        if (size_ == capacity_)
            reserve(capacity_ == 0 ? 64 : capacity_ * 2);
        data_[size_++] = value;
    }
    void reserve(size_t cnt) {
        std::unique_lock lock(mux_);
        auto new_data = std::make_unique<int[]>(cnt);
        std::ranges::copy(std::span(data_.get(), size_), new_data.get());
        data_ = std::move(new_data);
        capacity_ = cnt;
        size_ = std::min(size_, capacity_);
    }
private:
    std::recursive_mutex mux_;
    std::unique_ptr<int[]> data_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};


void simon_toth_main_02() {

    NonRecursive non;
    for (int i = 0; i < 200; i++) {
        non.push_back(i);
    }
    non.reserve(2);
    Recursive rec;
    for (int i = 0; i < 200; i++) {
        rec.push_back(i);
    }
    rec.reserve(2);
}