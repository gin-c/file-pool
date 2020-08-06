// Copyright(c) 2020-present, JiaoMeng.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
#pragma once
#include <string>
#include "state_interface.h"

namespace file_pool
{
    /**
    * @brief 下载状态实现类
    */
    class CStateDownloading : public CStateInterface
    {
    public:
        CStateDownloading(const CStateInterface& preState);
        CStateDownloading(std::string uri);

    public:
        int downloadFile(long exceptSize);
    };
} // namespace file_pool