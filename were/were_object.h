#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

class were_object;

void were_debug_add_object(were_object *object__);
void were_debug_remove_object(were_object *object__);
int were_debug_object_count();
void were_debug_print_objects();

class were_object
{
public:
    virtual ~were_object()
    {
        were_debug_remove_object(this);
    }

    were_object() : reference_count_(0), collapsed_(false)
    {
        were_debug_add_object(this);
    }

    void increment_reference_count()
    {
        reference_count_ += 1;
    }

    void decrement_reference_count()
    {
        reference_count_ -= 1;
    }

    int reference_count() const
    {
        return reference_count_;
    }

    bool collapsed() const
    {
        return collapsed_;
    }

    virtual void collapse()
    {
        collapsed_ = true;
    }

private:
    int reference_count_;
    bool collapsed_;
};

#endif // WERE_OBJECT_H
