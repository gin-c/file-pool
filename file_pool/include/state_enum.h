// Copyright(c) 2020-present, JiaoMeng.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

namespace file_pool {

    /**
     * @brief 文件状态枚举
     */
    enum StateVale {
        // 初始状态
        NONE,
        // 下载中
        DOWNLOADING,
        // 检验中
        CHECKING,
        // 文件就绪
        READY
    };

}