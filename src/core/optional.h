/*
MIT License

Copyright (c) 2019 ZhuQian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include "core/narukami.h"

class bad_optional_access : public std::exception
{

};

//简易实现C++17中的std::optional
template <class T>
class optional
{
private:
    bool _set = false;
    T _value;

public:
    constexpr optional() = default;
    constexpr optional(const optional &other) : _set(other._set), _value(*other) {}
    constexpr optional(const optional &&other) : _set(other._set), _value(std::move(*other)) {}
    template <class U>
    constexpr optional(U &&value) : _set(true), _value(static_cast<T>(std::forward<U>(value))) {}
    constexpr optional &operator=(const optional &other)
    {
        _set = other._set;
        _value = *other;
        return *this;
    }
    constexpr optional &operator=(const optional &&other)
    {
        _set = other._set;
        _value = std::move(*other);
        return *this;
    }

    T *operator->() { return &_value; }
    const T *operator->() const { return &_value; }
    constexpr T &operator*() & { return _value; }
    constexpr const T &operator*() const & { return _value; }
    constexpr T &&operator*() && { return std::move(_value); }
    constexpr const T &&operator*() const && { return std::move(_value); }

    constexpr operator bool() const { return _set; }
    constexpr bool has_value() const { return _set; }

    constexpr T &value() &
    {
        if (has_value())
            return _value;
        else
            throw std::exception();
    }
    constexpr const T &value() const &
    {
        if (has_value())
            return _value;
        else
            throw std::exception();
    }
    constexpr T &&value() &&
    {
        if (has_value())
            return std::move(_value);
        else
            throw bad_optional_access();
    }
    constexpr const T &&value() const &&
    {
        if (has_value())
            return std::move(_value);
        else
            throw bad_optional_access();
    }

    template <class U>
    constexpr T value_or(U &&default_value) const &
    {
        return has_value() ? _value : static_cast<T>(std::forward<U>(default_value));
    }
    template <class U>
    constexpr T value_or(U &&default_value) &&
    {
        return has_value() ? std::move(_value) : static_cast<T>(std::forward<U>(default_value));
    }

    void swap(optional &other)
    {
        if (has_value() && !other.has_value())
        {
            other = std::move(*this);
            reset();
        }
        else if (!has_value() && other.has_value())
        {
            (*this) = std::move(other);
            other.reset();
        }
        else
        {
            std::swap(**this, *other);
        }
    }

    void reset()
    {
        if (has_value())
        {
            (**this).T::~T();
            _set = false;
        }
    }

    //TODO impl emplace
};
//TODO impl operator ==,!= .etc
