// ===========================================================================
// Program.cpp // Thread Pool
// ===========================================================================

#pragma once

#include <iostream>
#include <memory>

class FunctionWrapper
{
    struct impl_base {
        virtual ~impl_base() {}
        virtual void call() = 0;
    };

    std::unique_ptr<impl_base> m_impl;

    template<typename F>
    struct impl_type : impl_base
    {
        F m_f;
        impl_type(F&& f) : m_f(std::move(f)) {}
        void call() { m_f(); }
    };

public:
    template<typename F>
    FunctionWrapper(F&& f) :
        m_impl(new impl_type<F>(std::move(f)))
    {}

    FunctionWrapper() = default;

    void operator()() { m_impl->call(); }

    void call() { m_impl->call(); }

    FunctionWrapper(FunctionWrapper&& other) noexcept :
        m_impl(std::move(other.m_impl))
    {}

    FunctionWrapper& operator=(FunctionWrapper&& other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;
};

// ===========================================================================
// End-of-File
// ===========================================================================
