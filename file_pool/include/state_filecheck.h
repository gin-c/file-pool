#pragma once
#include <string>
#include "state_interface.h"

namespace file_pool {
/**
 * @brief 校验状态实现类
 */
class CStateFileCheck : public CStateInterface {
  public:
    CStateFileCheck(const CStateInterface& preState);
    int checkFileSize(long exceptSize);
};
}  // namespace file_pool