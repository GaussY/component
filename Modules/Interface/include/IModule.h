#pragma once
#include "TopicMessage.h"
#include <string>
#include <memory>
#include <any>
#include <functional>

// 模块通信消息（同步/异步）
struct AsyncMessage
{
    std::string task_name;   // 任务名称
    bool success = false;    // 执行结果
    std::string msg;         // 提示信息
    std::any data;           // 任意自定义数据/结构体
};
using AsyncMessagePtr = std::shared_ptr<AsyncMessage>;

class IModule {
public:
    // 虚析构函数：确保子类析构时能正确调用
    virtual ~IModule() = default;

    // 模块初始化（返回bool表示是否初始化成功）
    virtual bool Init() = 0;

    // 模块销毁（释放资源）
    virtual void Destroy() = 0;

    // 获取模块名称（唯一标识）
    virtual std::string GetModuleName() const = 0;

    // 检查模块是否已初始化
    virtual bool IsInited() const = 0;

    // 跨线程异步回调（模块重写此函数接收消息，线程池触发）
    virtual void OnAsyncCallback(const AsyncMessagePtr&) {}

    // 模块间同步回调（模块重写此函数接收消息，直接调用）
    virtual void OnSyncCallback(const AsyncMessagePtr&) {}

public:
    //翻译模块需重写接口
    virtual std::string GetTransText(const std::string& ){return std::string();}

public:
    // UI回调类型定义：模块 → UI 发送消息
    using UiMessageCallback = std::function<void(const TopicMessagePtr&)>;

    // 适配器注册UI消息回调（仅注册一次）
    void RegisterUiCallback(const UiMessageCallback& callback)
    {
        m_uiCallback = callback;
    }

protected:
    // 模块发布消息给UI
    void PublishUIMessage(const std::string& topic, const std::any& data = {})
    {
        if (m_uiCallback)
        {
            TopicMessagePtr msg = std::make_shared<TopicMessage>();
            msg->topic = topic;
            msg->data = data;
            m_uiCallback(msg);
        }
    }

public:
    //订阅UI消息
    virtual void SubscribeUIMessage(const TopicMessagePtr& msg){}

protected:
    // 模块初始化状态（子类可直接使用）
    bool m_bInited = false;

    // UI 消息回调句柄
    UiMessageCallback m_uiCallback;
};

// 模块指针类型定义（简化代码）
using IModulePtr = std::shared_ptr<IModule>;
