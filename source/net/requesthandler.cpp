#include "net/requesthandler.h"
#include <stdint.h>

RequestHandler::RequestHandler(uint32_t seq)
  : seq_(seq)
{
}

RequestHandler& RequestHandler::then(std::function<void(const Response&)> handle_func)
{
    this->hand_func_ = handle_func;
    return *this;
}

void RequestHandler::err(std::function<void(const Response&)> err_func)
{
    this->err_func_ = err_func;
}

std::function<void(const Response&)> RequestHandler::get(uint32_t statu_code)
{
    return statu_code == 200 ? hand_func_ : err_func_;
}