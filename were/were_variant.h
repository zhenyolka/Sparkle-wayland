#ifndef WERE_VARIANT_H
#define WERE_VARIANT_H

#include "were_exception.h"
#include <typeinfo>
#include <typeindex>

class were_variant_base
{
public:

    virtual ~were_variant_base()
    {
    }

    were_variant_base(const std::type_index &type) :
        type_(type)
    {
    }

    const std::type_index &type() const
    {
        return type_;
    }

    virtual were_variant_base *clone() const = 0;

private:
    std::type_index type_;
};

template <typename T>
class were_variant_typed : public were_variant_base
{
public:

    ~were_variant_typed()
    {
    }

    were_variant_typed(const T &value) :
        were_variant_base(std::type_index(typeid(T))), value_(value)
    {
    }

    were_variant_typed(const were_variant_typed &other) :
        were_variant_base(std::type_index(typeid(T))), value_(other.value_)
    {
    }

    T &get()
    {
        return value_;
    }

    were_variant_base *clone() const
    {
        return new were_variant_typed(*this);
    }

private:
    T value_;
};

class were_variant
{
public:

    ~were_variant()
    {
        delete base_;
    }

    template <typename T>
    were_variant(const T &value)
    {
        base_ = new were_variant_typed<T>(value);
    }

    were_variant(const were_variant &other)
    {
        base_ = other.base_->clone();
    }

    template <typename T>
    T &get()
    {
        if (base_->type() == std::type_index(typeid(T)))
            return (static_cast< were_variant_typed<T> * >(base_))->get();
        else
            throw were_exception(WE_SIMPLE);
    }

private:
    were_variant_base *base_;
};


#endif // WERE_VARIANT_H
