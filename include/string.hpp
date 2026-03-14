/*
 * @Author: Zhang YuHua 1774630667@qq.com
 * @Date: 2026-03-14 18:31:36
 * @LastEditors: Zhang YuHua 1774630667@qq.com
 * @LastEditTime: 2026-03-14 18:54:00
 * @FilePath: /MyStl/include/string.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <cstddef>
#include <cstring>
#include "iterator.hpp"

 namespace MyStl {
    class string {
    public:
    using value_type = char;
    using iterator = char*;
    using const_iterator = const char*;
    using pointer = char*;
    using const_pointer = const char*;
    using reference = char&;
    using const_reference = const char&;

    private:
        // 阈值：本地最多存 15 个字符 + 1 个 '\0'
        static constexpr size_t SSO_CAPACITY = 15;

        size_t size_; // 无论 SSO 还是堆，都需要记录当前长度

        union {
            // 模式 1：短字符串模式 (存在栈上，不需要 capacity，因为上限固定是 15)
            char local_buf_[SSO_CAPACITY + 1]; 

            // 模式 2：长字符串模式 (存在堆上)
            struct {
                char* ptr_;
                size_t capacity_;
            } heap_data_;
        } data_;
        
        // 辅助函数：判断当前是否是 SSO 模式
        bool is_sso() const {
            return size_ <= SSO_CAPACITY;
        }

        pointer data_ptr() {
            return is_sso() ? data_.local_buf_ : data_.heap_data_.ptr_;
        }

        const_pointer data_ptr() const {
            return is_sso() ? data_.local_buf_ : data_.heap_data_.ptr_;
        }

    public:
        string() : size_(0) {
            data_.local_buf_[0] = '\0';
        }

        string(const char* str) {
            size_ = strlen(str);
            if (size_ <= SSO_CAPACITY) {
                strcpy(data_.local_buf_, str);
            } else {
                data_.heap_data_.ptr_ = new char[size_ + 1];
                data_.heap_data_.capacity_ = size_;
                strcpy(data_.heap_data_.ptr_, str);
            }
        }

        string(const string& other) {
            size_ = other.size_;
            if (size_ <= SSO_CAPACITY) {
                strcpy(data_.local_buf_, other.data_ptr());
            } else {
                data_.heap_data_.ptr_ = new char[size_ + 1];
                data_.heap_data_.capacity_ = size_;
                strcpy(data_.heap_data_.ptr_, other.data_ptr());
            }
        }

        string(string && other) {
            size_ = other.size_;
            if (size_ <= SSO_CAPACITY) {
                strcpy(data_.local_buf_, other.data_ptr());
            } else {
                data_.heap_data_.ptr_ = other.data_.heap_data_.ptr_;
                data_.heap_data_.capacity_ = other.data_.heap_data_.capacity_;
                other.data_.heap_data_.ptr_ = nullptr;
                other.data_.heap_data_.capacity_ = 0;
            }
        }

        string& operator=(const string& other) {
            if (this != &other) {
                if (!is_sso()) {
                    delete[] data_.heap_data_.ptr_;
                }
                size_ = other.size_;
                if (size_ <= SSO_CAPACITY) {
                    strcpy(data_.local_buf_, other.data_ptr());
                } else {
                    data_.heap_data_.ptr_ = new char[size_ + 1];
                    data_.heap_data_.capacity_ = size_;
                    strcpy(data_.heap_data_.ptr_, other.data_ptr());
                }
            }
            return *this;
        }

        string& operator=(string&& other) {
            if (this != &other) { 
                if (!is_sso()) {
                    delete[] data_.heap_data_.ptr_;
                }
                size_ = other.size_;
                if (size_ <= SSO_CAPACITY) {
                    strcpy(data_.local_buf_, other.data_ptr());
                } else {
                    data_.heap_data_.ptr_ = other.data_.heap_data_.ptr_;
                    data_.heap_data_.capacity_ = other.data_.heap_data_.capacity_;
                    other.data_.heap_data_.ptr_ = nullptr;
                    other.data_.heap_data_.capacity_ = 0;
                }
                other.size_ = 0;
                other.data_.local_buf_[0] = '\0';
            }
            return *this;
        }

        string& operator+=(const char* str) {
            size_t len = strlen(str);
            size_t old_size = size_;
            size_t new_size = old_size + len;
            if (new_size > capacity()) {
                reserve(new_size);
            }
            size_ = new_size; // 先更新 size_，确保 data_ptr() 获取到正确的联合体字段
            strcpy(data_ptr() + old_size, str);
            return *this;
        }

        string& operator+=(const string& other) {
            return *this += other.data_ptr();
        }

        bool operator==(const string& other) const {  
            return strcmp(data_ptr(), other.data_ptr()) == 0;  
        }

        bool operator!=(const string& other) const {  
            return !(*this == other);  
        }

        bool empty() const {
            return size_ == 0;
        }

        size_t size() const {
            return size_;
        }

        reference operator[](size_t index) {
            return data_ptr()[index];
        }
        
        iterator begin() {
            return data_ptr();
        }

        const_iterator begin() const {
            return data_ptr();
        }

        iterator end() {
            return data_ptr() + size_;
        }

        const_iterator end() const {
            return data_ptr() + size_;
        }

        const char* c_str() const {
            return data_ptr();
        }

        size_t capacity() const {
            return is_sso() ? SSO_CAPACITY : data_.heap_data_.capacity_;
        }

        void reserve(size_t n) {
            if (n > capacity()) {
                char* new_buf = new char[n + 1];
                strcpy(new_buf, data_ptr());
                if (!is_sso()) {
                    delete[] data_.heap_data_.ptr_;
                }
                data_.heap_data_.ptr_ = new_buf;
                data_.heap_data_.capacity_ = n;
            }
        }

        void push_back(char c) {
            if (size_ < SSO_CAPACITY) {
                data_.local_buf_[size_] = c;
                data_.local_buf_[size_ + 1] = '\0';
            } else {
                reserve(size_ + 1);
                data_.heap_data_.ptr_[size_] = c;
                data_.heap_data_.ptr_[size_ + 1] = '\0';
            }
            ++size_;
        }

        string& append(const char* str) {
            *this += str;
            return *this;
        }

        string& append(const string& other) {
            *this += other;
            return *this;
        }

        ~string() {
            if (!is_sso()) {
                delete[] data_.heap_data_.ptr_;
            }
        }
    };
}