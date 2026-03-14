#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>

namespace MyStl {
    template <size_t N>
    class bitset {
    private:
        // 使用 64 位的 unsigned long long 作为底层数据块
        using BlockType = unsigned long long;
        static constexpr size_t BitsPerBlock = 8 * sizeof(BlockType); // 64
        // 计算需要多少个 Block：(N + 63) / 64
        static constexpr size_t BlockCount = (N == 0) ? 0 : (N - 1) / BitsPerBlock + 1;

        BlockType blocks_[BlockCount];

        // 内部辅助函数：给定位索引，返回它属于哪个 Block
        constexpr size_t block_index(size_t pos) const { return pos / BitsPerBlock; }
        // 内部辅助函数：给定位索引，返回它在 Block 里的第几个 bit (0~63)
        constexpr size_t bit_index(size_t pos) const { return pos % BitsPerBlock; }
        // 内部辅助函数：生成一个只有第 bit_index 位是 1 的掩码
        constexpr BlockType bit_mask(size_t pos) const { return BlockType(1) << bit_index(pos); }

        constexpr void clean_unused_bits() {
            if constexpr (N == 0) return;
            constexpr size_t extra_bits = N % BitsPerBlock;
            if constexpr (extra_bits != 0) {
                BlockType mask = (BlockType(1) << extra_bits) - 1;
                blocks_[BlockCount - 1] &= mask;
            }
        }

    public:
        // ==========================================================
        // 核心难点：代理类 (Proxy Class)
        // 用于支持 bitset[i] = true; 的语法
        // ==========================================================
        class reference {
            friend class bitset;
        private:
            bitset* bitset_ptr; // 指向所属的 bitset
            size_t pos;         // 记录代表的是第几个 bit

            reference(bitset* b, size_t p) : bitset_ptr(b), pos(p) {}

        public:
            ~reference() = default;

            // 1. 当执行 my_bitset[5] = true; 时调用
            reference& operator=(bool val) {
                if (val) bitset_ptr->set(pos);
                else bitset_ptr->reset(pos);
                return *this;
            }

            // 2. 当执行 my_bitset[5] = my_bitset[6]; 时调用
            reference& operator=(const reference& other) {
                if (other.bitset_ptr->test(other.pos)) bitset_ptr->set(pos);
                else bitset_ptr->reset(pos);
                return *this;
            }

            // 3. 隐式转换为 bool，当执行 if(my_bitset[5]) 时调用
            operator bool() const {
                return bitset_ptr->test(pos);
            }

            // 4. 反转该位： ~my_bitset[5];
            bool operator~() const {
                return !bitset_ptr->test(pos);
            }
        };

        // ==========================================================
        // 你的 TODO：实现以下核心接口 (尽量使用位运算以追求极致性能)
        // ==========================================================
        
        // 默认构造：所有位清零
        bitset();

        // 状态操作
        bitset& set();               // 全部设为 1
        bitset& set(size_t pos);     // 将第 pos 位设为 1 (越界抛异常 out_of_range)
        
        bitset& reset();             // 全部设为 0
        bitset& reset(size_t pos);   // 将第 pos 位设为 0 (越界抛异常)
        
        bitset& flip();              // 全部反转 (0变1，1变0)
        bitset& flip(size_t pos);    // 反转第 pos 位 (越界抛异常)

        // 状态查询
        bool test(size_t pos) const; // 测试第 pos 位是否为 1 (越界抛异常)
        bool any() const;            // 是否有至少一个 1
        bool none() const;           // 是否全为 0
        bool all() const;            // 是否全为 1
        size_t count() const;        // 统计 1 的个数 (提示：可以使用 gcc 内置函数 __builtin_popcountll)
        constexpr size_t size() const { return N; }

        // 操作符重载
        reference operator[](size_t pos) { return reference(this, pos); }
        constexpr bool operator[](size_t pos) const { return (blocks_[block_index(pos)] & bit_mask(pos)) != 0; }
        
        // 进阶位运算 (可选挑战，能实现最好)
        bitset& operator&=(const bitset& other);
        bitset& operator|=(const bitset& other);
        bitset& operator^=(const bitset& other);
    };

    template <size_t N>
    bitset<N>::bitset() {
        reset();
    }

    template <size_t N>
    bitset<N>& bitset<N>::set() {
        for (size_t i = 0; i < BlockCount; ++i) {
            blocks_[i] = ~BlockType(0);
        }
        clean_unused_bits();
        return *this;
    }

    template <size_t N>
    bitset<N>& bitset<N>::set(size_t pos) {
        if (pos >= N) {
            throw std::out_of_range("Index out of range");
        }
        blocks_[block_index(pos)] |= bit_mask(pos);
        return *this;
    }

    template <size_t N>
    bitset<N>& bitset<N>::reset() {
        for (size_t i = 0; i < BlockCount; ++i) {
            blocks_[i] = 0;
        }
        return *this;
    }

    template <size_t N>
    bitset<N>& bitset<N>::reset(size_t pos) {
        if (pos >= N) {
            throw std::out_of_range("Index out of range");
        }
        blocks_[block_index(pos)] &= ~bit_mask(pos);
        return *this;
    }

    template <size_t N>
    bitset<N>& bitset<N>::flip() {
        for (size_t i = 0; i < BlockCount; ++i) {
            blocks_[i] = ~blocks_[i];
        }
        clean_unused_bits();
        return *this;
    }

    template <size_t N>
    bitset<N>& bitset<N>::flip(size_t pos) {
        if (pos >= N) throw std::out_of_range("Index out of range");
        blocks_[block_index(pos)] ^= bit_mask(pos);
        return *this;
    }

    template <size_t N>
    bool bitset<N>::test(size_t pos) const {
        if (pos >= N) throw std::out_of_range("Index out of range");
        return (blocks_[block_index(pos)] & bit_mask(pos)) != 0;
    }

    template <size_t N>
    bool bitset<N>::any() const {
        for (size_t i = 0; i < BlockCount; ++i) {
            if (blocks_[i] != 0) {
                return true;
            }
        }
        return false;
    }

    template <size_t N>
    bool bitset<N>::none() const {
        for (size_t i = 0; i < BlockCount; ++i) {
            if (blocks_[i] != 0) {
                return false;
            }
        }
        return true;
    }

    template <size_t N>
    bool bitset<N>::all() const {
        if constexpr (N == 0) return true;
        
        for (size_t i = 0; i < BlockCount - 1; ++i) {
            if (blocks_[i] != ~BlockType(0)) {
                return false;
            }
        }
        
        constexpr size_t extra_bits = N % BitsPerBlock;
        BlockType last_mask = (extra_bits == 0) ? ~BlockType(0) : ((BlockType(1) << extra_bits) - 1);
        
        return blocks_[BlockCount - 1] == last_mask;
    }

    template <size_t N>
    size_t bitset<N>::count() const {
        size_t count = 0;
        for (size_t i = 0; i < BlockCount; ++i) {
            count += __builtin_popcountll(blocks_[i]);
        }
        return count;
    }

    template <size_t N>
    bitset<N>& bitset<N>::operator&=(const bitset& other) {
        for (size_t i = 0; i < BlockCount; ++i) {
            blocks_[i] &= other.blocks_[i];
        }
        return *this;
    }

    template <size_t N>
    bitset<N>& bitset<N>::operator|=(const bitset& other) {
        for (size_t i = 0; i < BlockCount; ++i) {
            blocks_[i] |= other.blocks_[i];
        }
        return *this;
    }

    template <size_t N>
    bitset<N>& bitset<N>::operator^=(const bitset& other) {
        for (size_t i = 0; i < BlockCount; ++i) {
            blocks_[i] ^= other.blocks_[i];
        }
        return *this;
    }
}