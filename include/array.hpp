namespace MyStl {
    template<typename T, bool IsConst = false> 
    class arrayIterator {
        public: 
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = MyStl::conditional_t<IsConst, const T*, T*>;
        using reference = MyStl::conditional_t<IsConst, const T&, T&>;
        using const_reference = const T&;
        using const_pointer = const T*;


        private:
        pointer ptr;

        public:
        arrayIterator(pointer p = nullptr) : ptr(p) {}
        pointer operator->() const { return ptr; }
        reference operator*() const { return *ptr; }
        arrayIterator& operator++() { ++ptr; return *this; }
        arrayIterator operator++(int) { arrayIterator temp = *this; ++ptr; return temp; }
        arrayIterator& operator--() { --ptr; return *this; }
        arrayIterator operator--(int) { arrayIterator temp = *this; --ptr; return temp; }
        arrayIterator& operator+=(difference_type n) { ptr += n; return *this; }
        arrayIterator operator+(difference_type n) const { return arrayIterator(ptr + n); }
        arrayIterator& operator-=(difference_type n) { ptr -= n; return *this; }
        arrayIterator operator-(difference_type n) const { return arrayIterator(ptr - n); }
        difference_type operator-(const arrayIterator& other) const { return ptr - other.ptr; }
        bool operator==(const arrayIterator& other) const { return ptr == other.ptr; }
        bool operator!=(const arrayIterator& other) const { return ptr != other.ptr; }
        bool operator<(const arrayIterator& other) const { return ptr < other.ptr; }
        bool operator>(const arrayIterator& other) const { return ptr > other.ptr; }
        bool operator<=(const arrayIterator& other) const { return ptr <= other.ptr; }
        bool operator>=(const arrayIterator& other) const { return ptr >= other.ptr; }
    };

    template<typename T, int N>
    class array {
        public:
        using iterator = arrayIterator<T>;
        using const_iterator = arrayIterator<T, true>;
        using pointer = T*;
        using reference = T&;
        using const_reference = const T&;

        private:
        T elements[N];

        public:

        // 修复3：补回默认构造函数，并确保零初始化
        array() : elements{} {}

        array(std::initializer_list<T> list) {
            int i = 0;
            for (const auto& item : list) {
                if (i < N) {
                    elements[i++] = item;
                } else {
                    break; // 超出数组大小，停止赋值
                }
            }
            // 修复1：填充剩余元素为默认值（如 int 为 0）
            while (i < N) {
                elements[i++] = T();
            }
        }

        array& operator=(std::initializer_list<T> list) {
            int i = 0;
            for (const auto& item : list) {
                if (i < N) {
                    elements[i++] = item;
                } else {
                    break; // 超出数组大小，停止赋值
                }
            }
            // 修复2：赋值时也应该重置剩余元素，避免保留旧数据
            while (i < N) {
                elements[i++] = T();
            }
            return *this;
        }

        iterator begin() noexcept { return iterator(elements); }
        iterator end() noexcept { return iterator(elements + N); }
        const_iterator begin() const noexcept { return const_iterator(elements); }
        const_iterator end() const noexcept { return const_iterator(elements + N); }
        reference operator[](int index) { return elements[index]; }
        const_reference operator[](int index) const { return elements[index]; }
        const_reference at(size_t index) { 
            if (index >= N) {
                throw std::out_of_range("Index out of range");
            }
            return elements[index]; 
        }
        reference at(size_t index) const { 
            if (index >= N) {
                throw std::out_of_range("Index out of range");
            }
            return elements[index]; 
        }
        reference front() { return elements[0]; }
        const_reference front() const { return elements[0]; }
        reference back() { return elements[N - 1]; }
        const_reference back() const { return elements[N - 1]; }
        size_t size() const { return N; }
        pointer data() { return elements; }
        bool empty() const { return N == 0; }
    };
}