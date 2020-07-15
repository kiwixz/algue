#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

namespace algue::utils {

template <typename TContainer>
struct IndexSpan {
    using Container = TContainer;
    using Element = std::remove_reference_t<decltype(*std::declval<Container>().data())>;

    constexpr IndexSpan() = default;
    constexpr IndexSpan(Container* container, size_t offset, size_t size) :
        container_{container}, offset_{offset}, size_{size}
    {
        assert(container->size() >= offset_ + size_);
    }

    constexpr Element& operator[](size_t index) const
    {
        assert(container_);
        return (*container_)[offset_ + index];
    }

    constexpr Element* begin() const
    {
        assert(container_);
        assert(container_->size() >= offset_ + size_);
        return container_->data() + offset_;
    }

    constexpr Element* end() const
    {
        assert(container_);
        assert(container_->size() >= offset_ + size_);
        return container_->data() + offset_ + size_;
    }

    constexpr Element* data() const
    {
        assert(container_);
        assert(container_->size() >= offset_ + size_);
        return container_->data() + offset_;
    }

    constexpr size_t size() const
    {
        return size_;
    }

    constexpr IndexSpan<Element> subspan(size_t offset, size_t size) const
    {
        return {offset_ + offset, size};
    }

private:
    Container* container_ = nullptr;
    size_t offset_ = 0;
    size_t size_ = 0;
};

}  // namespace algue::utils
