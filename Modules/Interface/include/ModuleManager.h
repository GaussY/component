#pragma once
#include "IModule.h"
#include "TranslatorModule.h"
#include <unordered_map>
#include <mutex>
#include <stdexcept>

/**
 * @brief 模块管理器：统一管理所有模块的生命周期
 * @details 单例模式实现，线程安全，提供模块注册、初始化、销毁、获取及消息分发功能
 */
class ModuleManager
{
public:
    /**
     * @brief 获取模块管理器单例实例（全局唯一）
     * @return 模块管理器单例引用
     */
    static ModuleManager& GetInstance()
    {
        static ModuleManager instance;
        return instance;
    }

    // 禁止拷贝和赋值，确保单例唯一性
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;

    /**
     * @brief 注册模块（自动创建模块实例）
     * @tparam T 模块类型，必须继承自IModule
     * @param moduleName 模块唯一名称
     * @return true-注册成功，false-模块已存在
     * @throw 模块已注册时抛出运行时异常
     */
    template <typename T>
    bool RegisterModule(const std::string& moduleName)
    {
        static_assert(std::is_base_of<IModule, T>::value,
                      "T must inherit from IModule!");

        std::lock_guard<std::mutex> lock(m_mutex);

        // 检查模块是否已注册
        if (m_modules.find(moduleName) != m_modules.end())
        {
            throw std::runtime_error("Module already registered: " + moduleName);
            return false;
        }

        m_modules[moduleName] = std::shared_ptr<T>(new T());
        return true;
    }

    /**
     * @brief 注册模块（自动创建模块实例）
     * @param moduleName 模块唯一名称
     * @return true-注册成功，false-模块已存在
     */
    bool RegisterModule(const std::string& moduleName)
    {
        try
        {
            // 内部根据名字自动创建，外部完全不用传类型！
            if (moduleName == "Translator")
            {
                return RegisterModule<TranslatorModule>(moduleName);
            }

            // 以后加模块，只在这里加 else if
            // else if (moduleName == "XXX")
            // {
            //     return RegisterModule<XXXModule>(moduleName);
            // }
        }
        catch (...)
        {
        }

        return false;
    }

    /**
     * @brief 获取已注册的模块实例
     * @tparam T 目标模块类型
     * @param moduleName 模块名称
     * @return 模块智能指针，未找到返回nullptr
     */
    template <typename T = IModule>
    std::shared_ptr<T> GetModule(const std::string& moduleName)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_modules.find(moduleName);
        if (it == m_modules.end())
            return nullptr;

        return std::dynamic_pointer_cast<T>(it->second);
    }

    /**
     * @brief 获取所有模块名
     * @return 模块名容器
     */
    std::vector<std::string> GetAllModuleNames()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> names;
        names.reserve(m_modules.size());
        for (const auto& pair : m_modules)
        {
            names.push_back(pair.first);
        }
        return names;
    }

    /**
     * @brief 初始化所有已注册的模块
     * @return true-全部初始化成功，false-存在初始化失败的模块
     * @throw 模块初始化失败时抛出运行时异常
     */
    bool InitAllModules()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& [name, module] : m_modules)
        {
            if (!module->Init())
            {
                throw std::runtime_error("Failed to init module: " + name);
                return false;
            }
        }
        return true;
    }

    /**
     * @brief 销毁所有模块并清空模块列表
     */
    void DestroyAllModules()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& [name, module] : m_modules)
        {
            module->Destroy();
        }
        m_modules.clear();
    }

    /**
     * @brief 获取已注册的模块总数量
     * @return 模块数量
     */
    size_t GetModuleCount() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_modules.size();
    }

    /**
     * @brief 检查指定模块是否已注册
     * @param moduleName 待检查的模块名称
     * @return true-已注册，false-未注册
     */
    bool IsModuleRegistered(const std::string& moduleName)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_modules.find(moduleName) != m_modules.end();
    }

    /**
     * @brief 异步消息分发（跨线程/线程池专用）
     * @param msg 待分发的异步消息指针
     */
    void DispatchAsyncMessage(const AsyncMessagePtr& msg)
    {
        if (!msg)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        // 分发给所有已初始化的模块
        for (auto& [name, module] : m_modules)
        {
            if (module && module->IsInited())
            {
                module->OnAsyncCallback(msg);
            }
        }
    }

    /**
     * @brief 同步消息分发（模块间直接调用，无线程池）
     * @param msg 待分发的同步消息指针
     */
    void DispatchSyncMessage(const AsyncMessagePtr& msg)
    {
        if (!msg)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        // 当前线程直接执行同步回调，无延迟、无跨线程
        for (auto& [name, module] : m_modules) {
            if (module && module->IsInited())
            {
                module->OnSyncCallback(msg);
            }
        }
    }

private:
    // 私有构造/析构，禁止外部实例化
    ModuleManager() = default;
    ~ModuleManager() = default;

    // 模块存储容器：key-模块名称，value-模块实例指针
    std::unordered_map<std::string, IModulePtr> m_modules;
    // 线程安全互斥锁
    mutable std::mutex m_mutex;
};
