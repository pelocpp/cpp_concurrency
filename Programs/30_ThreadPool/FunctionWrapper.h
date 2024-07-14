// ===========================================================================
// FunctionWrapper.h // Thread Pool
// ===========================================================================

#pragma once

#include <iostream>
#include <memory>

class FunctionWrapper
{
public:
    template<typename F>
    FunctionWrapper(F&& f) :
        m_wrappedObject{ new ObjectModel<F>(std::move(f)) }
    {}

    FunctionWrapper() = default;

    void operator()() { m_wrappedObject->call(); }

    void call() { m_wrappedObject->call(); }

    // move constructor
    FunctionWrapper(FunctionWrapper&& other) noexcept :
        m_wrappedObject(std::move(other.m_wrappedObject))
    {}

    // move assignment
    FunctionWrapper& operator=(FunctionWrapper&& other) noexcept
    {
        m_wrappedObject = std::move(other.m_wrappedObject);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;

private:
    struct ObjectConcept {
        virtual ~ObjectConcept() {}
        virtual void call() = 0;
    };

    std::unique_ptr<ObjectConcept> m_wrappedObject;

    template<typename F>
    class ObjectModel : public ObjectConcept
    {
    private:
        F m_f;

    public:
        ObjectModel(F&& f) : m_f(std::move(f)) {}
        void call() { m_f(); }
    };

};

// ===========================================================================

class PolymorphicObjectWrapper
{
public:
    template <typename T>
    PolymorphicObjectWrapper(const T& obj) :
         m_wrappedObject{ std::make_shared<ObjectModel<T>>(obj) }
    {}

    template<typename T>
    PolymorphicObjectWrapper(T&& obj) :
        m_wrappedObject{ std::make_shared<ObjectModel<T>>(std::move(obj)) }
    {}

    PolymorphicObjectWrapper() = default;

    // move constructor
    PolymorphicObjectWrapper(PolymorphicObjectWrapper&& other) noexcept
        : m_wrappedObject(std::move(other.m_wrappedObject))
    {}

    // move assignment
    PolymorphicObjectWrapper& operator= (PolymorphicObjectWrapper&& other) noexcept
    {
        m_wrappedObject = std::move(other.m_wrappedObject);
        return *this;
    }

    //PolymorphicObjectWrapper(const PolymorphicObjectWrapper&) = delete;
    //PolymorphicObjectWrapper(PolymorphicObjectWrapper&) = delete;
    //PolymorphicObjectWrapper& operator=(const PolymorphicObjectWrapper&) = delete;

    void operator()() { m_wrappedObject->call(); }

    void call() { m_wrappedObject->call(); }

private:
    struct ObjectConcept
    {
        virtual ~ObjectConcept() = default;
        virtual void call() = 0;
    };

    template < typename T>
    struct ObjectModel final : public ObjectConcept
    {
       // ObjectModel(const T& object) : m_object{ object } {}
        ObjectModel(T&& object) : m_object{ std::move(object) } {}
        void call() { m_object(); }

    private:
        T m_object;
    };

    std::shared_ptr<ObjectConcept> m_wrappedObject;
};


// ===========================================================================
// End-of-File
// ===========================================================================
