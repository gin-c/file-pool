// Copyright(c) 2020-present, JiaoMeng.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <iostream>
#include "filepool_context.h"

namespace file_pool {

/**
 * @brief 状态抽象类
 */
class CStateInterface {
  public:
    /**
     * @brief 下载文件动作 接口
     */
    virtual int downloadFile(long exceptSize) {
        getLogger()->critical("下载文件不支持");
        return 1;
    };
    /**
     * @brief 检验文件动作 接口
     */
    virtual int checkFileSize(long exceptSize) {
        getLogger()->critical("文件校验不支持");
        return 1;
    }

  public:
    /**
     * @brief 拷贝构造
     */
    CStateInterface(const CStateInterface& other);
    /**
     * @brief 无参构造
     */
    CStateInterface();

    /**
     * @brief 成员Getter/Setter
     */
    void              setContext(CFilePoolContext* context);
    CFilePoolContext* getContext();
    std::string       getUri();
    void              setUri(std::string);
    std::string       getFilePath();
    void              setFilePath(std::string);
    StateVale         getStateValue();
    void              setStateValue(StateVale stateValue);

  protected:
    /**
     * @brief 切换到下一状态
     */
    int changeFileState(CStateInterface* nextState);

    /**
     * @brief 保存文件状态到磁盘
     */
    int saveFileState();

    /**
     * @brief 获取日志对象
     */
    std::shared_ptr<spdlog::logger> getLogger();

  private:
    CFilePoolContext* m_context;
    StateVale         m_state;
    std::string       m_uri;
    std::string       m_filePath;
};
}  // namespace file_pool