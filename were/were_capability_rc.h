#ifndef WERE_CAPABILITY_RC_H
#define WERE_CAPABILITY_RC_H

template <typename T>
class were_pointer;

template <typename T, typename ...Args>
were_pointer<T> were_new(Args &&...args);

class were_capability_rc
{

    template <typename T>
    friend class were_pointer;

    template <typename T, typename ...Args>
    friend were_pointer<T> were_new(Args &&...args);

public:
    virtual int reference_count() const = 0;

private:
    virtual void reference() = 0;
    virtual void unreference() = 0;
};

#endif // WERE_CAPABILITY_RC_H
