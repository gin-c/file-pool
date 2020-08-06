#pragma once
#include <string>
#include "state_interface.h"

namespace file_pool {
/**
 * @brief ��ʼ״̬ʵ����
 */
class CStateNone : public CStateInterface {
  public:
    CStateNone(const CStateInterface& preState);
    CStateNone(std::string uri);
    int downloadFile(long exceptSize);
};
}  // namespace file_pool