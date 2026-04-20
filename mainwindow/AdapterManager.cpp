#include "AdapterManager.h"

#include "TranslatorAdapter.h"
#include "ModuleManager.h"

#include "LogManager.h"
#include "LogDefine.h"

void AdapterManager::init()
{
    // 1.初始化日志
    LogManager::GetInstance().Init("./config/log_config.json");

    // 注册翻译模块
    ModuleManager::GetInstance().RegisterModule("Translator");

    // 初始化业务模块
    if (!ModuleManager::GetInstance().InitAllModules())
    {
        LOG_ERROR("模块初始化失败");
        return ;
    }
    LOG_INFO("---------所有模块初始化完成----------");

    // 自动绑定所有适配器
    auto moduleNames = ModuleManager::GetInstance().GetAllModuleNames();
    for (const auto& name : moduleNames)
    {
        auto it = m_adapterMap.find(name);
        if (it == m_adapterMap.end())
            continue;

        IAdapter* adapter = it->second();
        auto module = ModuleManager::GetInstance().GetModule(name);

        if (adapter && module)
            adapter->bindModule(module);
    }
    LOG_INFO("---------所有适配器初始化完成----------");
}

void AdapterManager::destroy()
{
    m_adapters.clear();
    ModuleManager::GetInstance().DestroyAllModules();
    LogManager::GetInstance().Shutdown();
}

QObject* AdapterManager::getTranslatorAdapter()
{
    auto it = m_adapters.find("Translator");
    if (it != m_adapters.end())
    {
        return it->second;
    }

    auto* adapter = new TranslatorAdapter(this);
    m_adapters["Translator"] = adapter;
    return adapter;
}

AdapterManager::AdapterManager(QObject *parent) : QObject(parent)
{
    // 新增模块需要添加
    m_adapterMap =
        {
            { "Translator", [this](){return qobject_cast<IAdapter*>(getTranslatorAdapter());}
            }
        };
}
