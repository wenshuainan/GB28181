#ifndef RECORD_QUERY_INTERFACE_H
#define RECORD_QUERY_INTERFACE_H

#include <memory>
#include "MANSCDP/A.2.4Query.h"

class RecordQuery
{
public:
    class Handle
    {
    protected:
        int32_t m_ch;

    public:
        Handle(int32_t ch) : m_ch(ch) {}

    public:
        virtual int32_t querySumNum() = 0;
        virtual bool queryRecordInfo(int32_t num, std::vector<itemFileType>& items) = 0;
    };

public:
    virtual std::unique_ptr<Handle> createHandle(const RecordInfoQuery::Request &req) = 0;
};

#endif