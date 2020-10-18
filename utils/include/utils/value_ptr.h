#pragma once

#include <memory>

namespace algue::utils {

template <typename TObject>
struct ValuePtr {
    using Object = TObject;

    ValuePtr() = default;
    template <typename... Args>
    ValuePtr(Args&&... args) :
        ptr_{std::make_unique<Object>(std::forward<Args>(args)...)}
    {}

    ~ValuePtr() = default;
    ValuePtr(const ValuePtr& other) :
        ptr_{std::make_unique<Object>(*other)}
    {}
    ValuePtr(ValuePtr&& other) noexcept = default;
    ValuePtr& operator=(const ValuePtr& other)
    {
        ptr_ = std::make_unique<Object>(*other);
        return *this;
    }
    ValuePtr& operator=(ValuePtr&& other) noexcept = default;

    bool operator==(const ValuePtr& other) const
    {
        return *ptr_ == *other.ptr_;
    }

    const Object& operator*() const
    {
        return *ptr_;
    }
    Object& operator*()
    {
        return *ptr_;
    }

    const Object* operator->() const
    {
        return ptr_.get();
    }
    Object* operator->()
    {
        return ptr_.get();
    }

    const Object* get() const
    {
        return ptr_.get();
    }
    Object* get()
    {
        return ptr_.get();
    }

private:
    std::unique_ptr<Object> ptr_;
};

}  // namespace algue::utils
