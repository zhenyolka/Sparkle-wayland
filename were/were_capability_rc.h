#ifndef WERE_CAPABILITY_RC_H
#define WERE_CAPABILITY_RC_H

class were_capability_rc
{
public:
    virtual void reference() = 0;
    virtual void unreference() = 0;
    virtual int reference_count() const = 0;
};

#endif // WERE_CAPABILITY_RC_H
