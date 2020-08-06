#pragma once

#include <string>
#include <map>
#include <mutex>
#include "spdlog/spdlog.h"
#include "state_enum.h"

#define IN
#define OUT

namespace file_pool {

// 状态类前向声明
class CStateInterface;

/**
 * @brief 文件池上下文，线程安全
 */
class CFilePoolContext {
  public:
    /**
     * @brief 无参构造函数
     */
    CFilePoolContext();
    /**
     * @brief 析构函数
     */
    ~CFilePoolContext();
    /**
     * @brief 初始化
     * @param filePoolDir 文件池所在文件夹,需要已存在
     * @param userData 用户数据
     * @param pLogger spdlog对象
     */
    int init(IN const std::string& filePoolDir, void* userData, IN std::shared_ptr<spdlog::logger> pLogger);

  public:
    /**
     * @brief 获取文件，返回文件状态，超时时间30秒
     * @return NONE 文件获取失败
     * @return DOWNLOADING 文件下载超时
     * @return CHECKING 文件校验超时
     * @return CHECKING 文件校验超时
     */
    StateVale getFileBlock(IN const std::string& uri, IN long exceptSize, OUT std::string& filePath);

  public:
    // 切换文件状态
    int changeFileState(IN const std::string& uri, IN CStateInterface* state);

    /**
     * @brief 保存文件状态到文件
     * @param state 状态对象
     */
    int saveFileState(IN CStateInterface* state);

    /**
     * @brief 下载函数，public访问
     * @param downloadUri 下载uri
     * @param filePath 文件名称
     */
    bool DownloadFunction(IN const std::string& downloadUri, IN const std::string& filePath);

    /**
     * @brief 获取日志对象
     */
    std::shared_ptr<spdlog::logger> getLogger();
    /**
     * @brief 获取文件池路径
     */
    std::string getFilePoolDir();

  protected:
    /**
     * @brief 非阻塞获取文件，返回文件当前状态
     */
    StateVale getFileNoBlock(IN const std::string& uri, IN long exceptSize, OUT std::string& filePath);
    /**
     * @brief 下载函数接口，需要自定义实现
     * @param downloadUri 下载uri
     * @param filePath 保存的文件名
     * @param userData 初始化时传入的用户数据
     */
    virtual bool downloadFunction(IN const std::string& downloadUri, IN const std::string& filePath, IN void* userData) = 0;

    /**
     * @brief 获取指定uri的文件状态对象
     * @param uri 查询条件
     */
    CStateInterface* getFileState(IN const std::string& uri);
    /**
     * @brief 获取指定uri的文件状态值
     * @param uri 查询条件
     */
    StateVale getFileStateValue(IN const std::string& uri);

    /**
     * @brief 获取用户数据
     */
    void* getUserData();

  private:
    /**
     * @brief 文件下载
     * @param uri
     * @param exceptSize 文件期望大小
     */
    int downloadFile(IN const std::string& uri, IN long exceptSize);

  private:
    // 文件状态map 互斥锁
    std::mutex m_fileStateMutex;
    // 文件状态map
    std::map<std::string, CStateInterface*> m_fileState;
    // 文件状态保存到磁盘的文件名
    std::string m_recordFilePath;
    // 文件池资源目录
    std::string m_filePoolDir;
    // 日志对象
    std::shared_ptr<spdlog::logger> m_pLogger;
    // 用户数据
    void* m_userData;
};
}  // namespace file_pool