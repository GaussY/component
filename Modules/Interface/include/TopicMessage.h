#pragma once
#include <string>
#include <any>
#include <memory>
#include <functional>

// ---------------------- 通用 Topic ----------------------
#define TOPIC_INIT_SUCCESS    "InitSuccess"

// ---------------------- 翻译模块 (Translator) ----------------------
#define TOPIC_TRANSLATOR_UI_SWITCH_LANGUAGE         "UISwitchLanguage"
#define TOPIC_TRANSLATOR_MOUDLE_LANGUAGE_CHANGED    "ModuleLanguageChanged"

// ---------------------- 配置模块 (Config) ----------------------

// ---------------------- 其它模块 (PointCloud) ----------------------

// 主题消息
struct TopicMessage
{
    std::string topic;
    std::any data;
};

// 智能指针别名
using TopicMessagePtr = std::shared_ptr<TopicMessage>;

// 回调函数类型
using MessageCallback = std::function<void(const TopicMessagePtr&)>;

// 无数据的消息
inline TopicMessagePtr CreateMessage(const std::string& topic)
{
    auto msg = std::make_shared<TopicMessage>();
    msg->topic = topic;
    return msg;
}

// 带任意数据的消息
template<typename T>
inline TopicMessagePtr CreateMessage(const std::string& topic, T&& data)
{
    auto msg = std::make_shared<TopicMessage>();
    msg->topic = topic;
    msg->data = std::forward<T>(data);
    return msg;
}

// 安全读取数据
template<typename T>
inline bool GetData(const TopicMessagePtr& msg, T& outValue)
{
    if (!msg) return false;
    if (msg->data.has_value() && msg->data.type() == typeid(T))
    {
        outValue = std::any_cast<T>(msg->data);
        return true;
    }
    return false;
}
