#pragma once
#include <stdexcept>
#include <functional>
#include "utility.hpp"
#include "type_traits.hpp"
#include "unique_ptr.hpp"

namespace MyStl {

    // 1. 主模板声明
    template<typename Signature>
    class function;

    // 2. 特化版本：Ret(Args...)
    template<typename Ret, typename... Args>
    class function<Ret(Args...)> {
    private:
        // --- 内部抽象基类 ---
        struct CallableBase {
            virtual ~CallableBase() = default;
            // 纯虚函数：负责调用的接口
            virtual Ret call(Args... args) = 0;

            virtual CallableBase* clone() const = 0;
        };

        // --- 内部具体实现类 (模板) ---
        template<typename F>
        struct CallableImpl : public CallableBase {
            F f; // 真正存储的函数对象 (Lambda, 指针等)

            CallableImpl(F fun) : f(MyStl::move(fun)) {}

            // 重写 call 方法
            Ret call(Args... args) override {
                // TODO 1: 在这里调用内部存储的 f
                // 提示：记得使用 std::forward<Args>(args)... 来完美转发参数
                // 形式大概是：return f( ... );
                return f(MyStl::forward<Args>(args)...);
            }

            CallableBase* clone() const override {
                return new CallableImpl(f);
            }
        };

        // 基类指针，持有具体实现
        MyStl::unique_ptr<CallableBase> base;

    public:
        // 默认构造：空函数
        function() = default;

        // --- 构造函数 (核心) ---
        // 这是一个模板构造函数，能接收 Lambda, 函数指针等任何 F
        template<typename F>
        function(F f) {
            // TODO 2: 将传入的 f 包装成 CallableImpl<F>
            // 提示：你需要 new 一个 CallableImpl<F>(std::move(f))
            // 然后把它交给 base 管理 (base.reset(...))
            CallableImpl<F>* Impl = new CallableImpl<F>(MyStl::move(f));
            base.reset(Impl);
        }

        function(const function& other) {
            if(other.base){
                base.reset(other.base->clone());
            }
        }

        void swap(function& other) {
            base.swap(other.base);
        }

        function& operator=(const function& other) {
            function tmp(other); // 过滤掉other为空的情况
            if(other.base){
                swap(tmp);
            }
            return *this;
        }

        // --- 调用操作符 ---
        Ret operator()(Args... args) const {
            // TODO 3: 转发调用
            // 1. 检查 base 是否为空 (如果为空，抛出 std::bad_function_call)
            // 2. 如果不为空，调用 base->call(...)
            // 提示：同样记得用 std::forward
            if (base) {
                return base->call(MyStl::forward<Args>(args)...);
            }
            throw std::bad_function_call();
        }

        // 判空支持 if(func)
        explicit operator bool() const {
            return base != nullptr;
        }
    };

}