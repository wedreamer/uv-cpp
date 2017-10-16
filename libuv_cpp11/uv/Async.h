/*
   Copyright 2017, object_he@yeah.net  All rights reserved.

   Author: object_he@yeah.net

   Last modified: 2017-10-16

   Description:
*/

#include <memory>
#include <functional>

#include "uv/EventLoop.h"

#ifndef UV_ASYNC_H
#define UV_ASYNC_H

namespace uv
{


template<typename ValueType>
class Async  : public std::enable_shared_from_this<Async<ValueType>>
{
public:
    using AsyncCallback = std::function<void(Async<ValueType>*, ValueType*)>;


    Async<ValueType>(EventLoop* loop,AsyncCallback callback)
        : handle_(new uv_async_t),
        callback_(callback),
        valuePtr_(nullptr)
    {
        ::uv_async_init(loop->hanlde(), handle_, Async<ValueType>::process);
        handle_->data = static_cast<void*>(this);
    }

    void setData(ValueType* value)
    {
        valuePtr_ = value;
    }

    void runInLoop()
    {
        ::uv_async_send(handle_);
    }


    void close()
    {
        ::uv_close((uv_handle_t*)handle_, [](uv_handle_t* handle)
        {
            delete (uv_async_t*)handle;
        });
    }
private:
    uv_async_t* handle_;
    AsyncCallback callback_;
    ValueType* valuePtr_;

    static void process(uv_async_t* handle)
    {
        auto async = static_cast<Async*>(handle->data);
		if(async->callback_)
			async->callback_(async,async->valuePtr_);
    }
};


template<typename ValueType>
using AsyncPtr = std::shared_ptr<Async<ValueType>>;
}
#endif