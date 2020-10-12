#pragma once

#include <memory>

namespace algue::utils {

template <typename TObject>
struct Ptr {
    using Object = TObject;

    Ptr() = default;
    template <typename... Args>
    Ptr(Args&&... args) :
        ptr_{std::make_unique<Object>(std::forward<Args>(args)...)}
    {}

    ~Ptr() = default;
    Ptr(const Ptr& other) :
        ptr_{std::make_unique<Object>(*other)}
    {}
    Ptr(Ptr&& other) noexcept = default;
    Ptr& operator=(const Ptr& other)
    {
        ptr_ = std::make_unique<Object>(*other);
        return *this;
    }
    Ptr& operator=(Ptr&& other) noexcept = default;

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
