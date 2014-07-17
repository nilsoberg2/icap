#ifndef LINKLIST_H__
#define LINKLIST_H__


#include "link.h"


namespace geometry
{
    class LinkList
    {
    public:
        virtual int count() = 0;
        virtual std::shared_ptr<Link> get(int index) = 0;
        virtual id_type id(int index) = 0;
    };

    class LinkList_helper : public LinkList
    {
    public:
        virtual int link_count() = 0;
        virtual int count() { return link_count(); }

        virtual std::shared_ptr<Link> link_get(int index) = 0;
        virtual std::shared_ptr<Link> get(int index) { return link_get(index); }

        virtual id_type link_id(int index) = 0;
        virtual id_type id(int index) { return link_id(index); }
    };
}


#endif//LINKLIST_H__
