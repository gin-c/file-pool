#pragma once

#include <string>
#include <map>
#include <mutex>
#include "spdlog/spdlog.h"
#include "state_enum.h"

#define IN
#define OUT

namespace file_pool {

// ״̬��ǰ������
class CStateInterface;

/**
 * @brief �ļ��������ģ��̰߳�ȫ
 */
class CFilePoolContext {
  public:
    /**
     * @brief �޲ι��캯��
     */
    CFilePoolContext();
    /**
     * @brief ��������
     */
    ~CFilePoolContext();
    /**
     * @brief ��ʼ��
     * @param filePoolDir �ļ��������ļ���,��Ҫ�Ѵ���
     * @param userData �û�����
     * @param pLogger spdlog����
     */
    int init(IN const std::string& filePoolDir, void* userData, IN std::shared_ptr<spdlog::logger> pLogger);

  public:
    /**
     * @brief ��ȡ�ļ��������ļ�״̬����ʱʱ��30��
     * @return NONE �ļ���ȡʧ��
     * @return DOWNLOADING �ļ����س�ʱ
     * @return CHECKING �ļ�У�鳬ʱ
     * @return CHECKING �ļ�У�鳬ʱ
     */
    StateVale getFileBlock(IN const std::string& uri, IN long exceptSize, OUT std::string& filePath);

  public:
    // �л��ļ�״̬
    int changeFileState(IN const std::string& uri, IN CStateInterface* state);

    /**
     * @brief �����ļ�״̬���ļ�
     * @param state ״̬����
     */
    int saveFileState(IN CStateInterface* state);

    /**
     * @brief ���غ�����public����
     * @param downloadUri ����uri
     * @param filePath �ļ�����
     */
    bool DownloadFunction(IN const std::string& downloadUri, IN const std::string& filePath);

    /**
     * @brief ��ȡ��־����
     */
    std::shared_ptr<spdlog::logger> getLogger();
    /**
     * @brief ��ȡ�ļ���·��
     */
    std::string getFilePoolDir();

  protected:
    /**
     * @brief ��������ȡ�ļ��������ļ���ǰ״̬
     */
    StateVale getFileNoBlock(IN const std::string& uri, IN long exceptSize, OUT std::string& filePath);
    /**
     * @brief ���غ����ӿڣ���Ҫ�Զ���ʵ��
     * @param downloadUri ����uri
     * @param filePath ������ļ���
     * @param userData ��ʼ��ʱ������û�����
     */
    virtual bool downloadFunction(IN const std::string& downloadUri, IN const std::string& filePath, IN void* userData) = 0;

    /**
     * @brief ��ȡָ��uri���ļ�״̬����
     * @param uri ��ѯ����
     */
    CStateInterface* getFileState(IN const std::string& uri);
    /**
     * @brief ��ȡָ��uri���ļ�״ֵ̬
     * @param uri ��ѯ����
     */
    StateVale getFileStateValue(IN const std::string& uri);

    /**
     * @brief ��ȡ�û�����
     */
    void* getUserData();

  private:
    /**
     * @brief �ļ�����
     * @param uri
     * @param exceptSize �ļ�������С
     */
    int downloadFile(IN const std::string& uri, IN long exceptSize);

  private:
    // �ļ�״̬map ������
    std::mutex m_fileStateMutex;
    // �ļ�״̬map
    std::map<std::string, CStateInterface*> m_fileState;
    // �ļ�״̬���浽���̵��ļ���
    std::string m_recordFilePath;
    // �ļ�����ԴĿ¼
    std::string m_filePoolDir;
    // ��־����
    std::shared_ptr<spdlog::logger> m_pLogger;
    // �û�����
    void* m_userData;
};
}  // namespace file_pool