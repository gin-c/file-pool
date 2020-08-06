// Copyright(c) 2020-present, JiaoMeng.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

namespace file_pool {

    /**
     * @brief �ļ�״̬ö��
     */
    enum StateVale {
        // ��ʼ״̬
        NONE,
        // ������
        DOWNLOADING,
        // ������
        CHECKING,
        // �ļ�����
        READY
    };

}