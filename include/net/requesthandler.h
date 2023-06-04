#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <functional>
#include <stdint.h>

class Response;

class RequestHandler
{
  public:
    RequestHandler(uint32_t seq);

  private:
    uint32_t seq_;
    std::function<void(const Response&)> hand_func_;
    std::function<void(const Response&)> err_func_;

  public:
    RequestHandler& then(std::function<void(const Response&)> handle_func);

    void err(std::function<void(const Response&)> err_func);
    std::function<void(const Response&)> get(uint32_t statu_code);
};

#endif  // REQUESTHANDLER_H
