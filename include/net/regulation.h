#ifndef REGULATION_H
#define REGULATION_H

#include <stdint.h>

class Regulation
{
  public:
    constexpr static uint32_t Broadcast_Seq = 0;
    constexpr static uint32_t kLogin = 1;
    constexpr static uint32_t kInfo = 2;
    constexpr static uint32_t kExit = 3;
    constexpr static uint32_t kChat = 4;
    static constexpr uint32_t kAdd = 5;     // username message to
    static constexpr uint32_t kAccept = 6;  // username message to
    static constexpr uint32_t kRefuse = 7;  // username message to
};

#endif  // REGULATION_H
