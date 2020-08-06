// Copyright(c) 2020-present, JiaoMeng.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include "state_interface.h"

namespace file_pool {
/**
 * @brief 就绪状态实现类
 */
class CStateReady : public CStateInterface {
  public:
    CStateReady(const CStateInterface& preState);
    CStateReady(const std::string uri, const std::string filePath);
};
}  // namespace file_pool