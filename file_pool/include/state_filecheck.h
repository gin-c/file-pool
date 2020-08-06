#pragma once
#include <string>
#include "state_interface.h"

namespace file_pool {
/**
 * @brief У��״̬ʵ����
 */
class CStateFileCheck : public CStateInterface {
  public:
    CStateFileCheck(const CStateInterface& preState);
    int checkFileSize(long exceptSize);
};
}  // namespace file_pool