#pragma once
#include "IModule.h"
#include <string>
#include <vector>
#include <unordered_map>

// 翻译模块
class TranslatorModule : public IModule
{
public:
    // 语言类型枚举
    enum class LanguageType
    {
        zh_CN,
        en_US,
        ru_RU
    };

public:
    // 模块初始化（返回bool表示是否初始化成功）
    bool Init() override;

    // 模块销毁（释放资源）
    void Destroy() override;

    // 获取模块名称（唯一标识）
    std::string GetModuleName() const override;

    // 检查模块是否已初始化
    bool IsInited() const override;

    // 订阅UI消息
    void SubscribeUIMessage(const TopicMessagePtr& msg) override;

    // 获取翻译文本
    std::string GetTransText(const std::string& key) override;

private:
    // 获取翻译文本
    // std::string GetText(const std::string& key);

    // 切换下一种语言
    void SwitchNextLanguage();

private:
    // 加载语言配置
    bool LoadConfig();

    // 加载指定语言
    bool LoadLanguage(LanguageType type);

    // 语言枚举转字符串
    std::string LangToString(LanguageType type);

    // 保存当前语言到配置文件
    void SaveCurrentLanguage();

private:
    // 翻译键值对
    std::unordered_map<std::string, std::string> m_transData;

    // 当前语言
    LanguageType m_currentLang = LanguageType::zh_CN;

    // 支持的语言列表
    std::vector<LanguageType> m_supportedLangs;
};
