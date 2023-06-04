#ifndef REGULATION_H
#define REGULATION_H

#include <stdint.h>

class Regulation
{
  public:
    constexpr static uint32_t Boradcast_Seq = 0;
    constexpr static uint32_t kLogin = 1;
    constexpr static uint32_t kInfo = 2;
    constexpr static uint32_t kExit = 3;
    constexpr static uint32_t kChat = 4;
};

#endif  // REGULATION_H
