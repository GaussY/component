#pragma once
#include "TopicMessage.h"
#include "IModule.h"

#include <QObject>

// 所有业务适配器的基类
class IAdapter : public QObject
{
    Q_OBJECT
public:
    explicit IAdapter(QObject *parent = nullptr) : QObject(parent) {setParent(nullptr);}
    virtual ~IAdapter() = default;

    // 处理业务模块发来的消息
    virtual void handleModuleMessage(const TopicMessagePtr& msg) = 0;

    // 模块与适配器建立关系
    void bindModule(std::shared_ptr<IModule> module)
    {
        m_module = module;
        if (m_module)
        {
            // 基类统一注册回调
            m_module->RegisterUiCallback([this](const TopicMessagePtr& msg)
                                         {
                                             this->handleModuleMessage(msg);
                                         });
        }
    }

protected:
    // 获取绑定的Module
    std::shared_ptr<IModule> getModule() const { return m_module; }

private:
    std::shared_ptr<IModule> m_module; // 基类统一保存 Module 指针
};
