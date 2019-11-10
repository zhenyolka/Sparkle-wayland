#include "were_object_base.h"
#include "were_debug.h"

were_object_base::~were_object_base()
{
    were_debug::instance().remove_object(this);
}

were_object_base::were_object_base()
{
    were_debug::instance().add_object(this);
}
