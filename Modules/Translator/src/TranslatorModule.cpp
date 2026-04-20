#include "TranslatorModule.h"
#include <fstream>
#include <algorithm>
#include "nlohmann/json.hpp"

#define CONFIG_PATH "config/language_config.json"
using json = nlohmann::json;
namespace fs = std::filesystem;

bool TranslatorModule::Init()
{
    m_supportedLangs = { LanguageType::zh_CN, LanguageType::en_US, LanguageType::ru_RU };
    m_bInited = LoadConfig();
    return m_bInited;
}

void TranslatorModule::Destroy()
{
    m_transData.clear();
    m_supportedLangs.clear();
    m_bInited = false;
}

std::string TranslatorModule::GetModuleName() const
{
    return "Translator";
}

bool TranslatorModule::IsInited() const
{
    return m_bInited;
}

void TranslatorModule::SubscribeUIMessage(const TopicMessagePtr& msg)
{
    if (!msg)
    {
        return;
    }

    if (msg->topic == TOPIC_TRANSLATOR_UI_SWITCH_LANGUAGE)
    {
        SwitchNextLanguage();
        PublishUIMessage(TOPIC_TRANSLATOR_MOUDLE_LANGUAGE_CHANGED);
    }
}

std::string TranslatorModule::GetTransText(const std::string& key)
{
    auto it = m_transData.find(key);
    if (it != m_transData.end())
    {
        return it->second;
    }
    return key;
}

void TranslatorModule::SwitchNextLanguage()
{
    if (m_supportedLangs.empty())
    {
        return;
    }

    auto it = std::find(m_supportedLangs.begin(), m_supportedLangs.end(), m_currentLang);
    if (it != m_supportedLangs.end())
    {
        size_t idx = std::distance(m_supportedLangs.begin(), it);
        idx = (idx + 1) % m_supportedLangs.size();
        m_currentLang = m_supportedLangs[idx];
    }

    LoadLanguage(m_currentLang);
    SaveCurrentLanguage();
}

bool TranslatorModule::LoadConfig()
{
    std::ifstream ifs(CONFIG_PATH);
    if (ifs.is_open())
    {
        try
        {
            json cfg = json::parse(ifs);

            // 1. 读取 supported 语言列表（完整保留）
            if (cfg.contains("supported") && cfg["supported"].is_array())
            {
                m_supportedLangs.clear();
                for (const auto& langStr : cfg["supported"])
                {
                    if (langStr == "zh_CN")
                    {
                        m_supportedLangs.push_back(LanguageType::zh_CN);
                    }
                    else if (langStr == "en_US")
                    {
                        m_supportedLangs.push_back(LanguageType::en_US);
                    }
                    else if (langStr == "ru_RU")
                    {
                        m_supportedLangs.push_back(LanguageType::ru_RU);
                    }
                }
            }

            // 2. 读取 default 和 current
            std::string current = cfg.value("current", "");
            std::string defaultLang = cfg.value("default", "zh_CN");

            // 3. 按你要求：current 为空用 default，不为空用 current
            std::string targetLang;
            if (current.empty())
            {
                targetLang = defaultLang;
            }
            else
            {
                targetLang = current;
            }

            // 4. 转换为枚举
            if (targetLang == "zh_CN")
            {
                m_currentLang = LanguageType::zh_CN;
            }
            else if (targetLang == "en_US")
            {
                m_currentLang = LanguageType::en_US;
            }
            else if (targetLang == "ru_RU")
            {
                m_currentLang = LanguageType::ru_RU;
            }
            else
            {
                m_currentLang = LanguageType::zh_CN;
            }
        }
        catch (...)
        {
            m_currentLang = LanguageType::zh_CN;
        }
    }

    return LoadLanguage(m_currentLang);
}

void TranslatorModule::SaveCurrentLanguage()
{
    json cfg;

    cfg["supported"] = json::array();
    for (const auto& lang : m_supportedLangs)
    {
        cfg["supported"].push_back(LangToString(lang));
    }

    cfg["default"] = "zh_CN";
    cfg["current"] = LangToString(m_currentLang);

    std::ofstream ofs(CONFIG_PATH);
    if (ofs.is_open())
    {
        ofs << cfg.dump(4);
        ofs.close();
    }
}

bool TranslatorModule::LoadLanguage(LanguageType type)
{
    std::string path = "translations/" + LangToString(type) + ".json";
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        return false;
    }

    json j = json::parse(ifs);
    m_transData.clear();

    for (auto& el : j.items())
    {
        m_transData[el.key()] = el.value();
    }

    return true;
}

std::string TranslatorModule::LangToString(LanguageType type)
{
    switch (type)
    {
    case LanguageType::zh_CN:
        return "zh_CN";
    case LanguageType::en_US:
        return "en_US";
    case LanguageType::ru_RU:
        return "ru_RU";
    default:
        return "zh_CN";
    }
}
