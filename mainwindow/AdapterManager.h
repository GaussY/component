#pragma once
#include"IAdapter.h"

#include <QObject>
#include <unordered_map>

class AdapterManager : public QObject
{
    Q_OBJECT
public:
    static AdapterManager& Instance()
    {
        static AdapterManager manager;
        return manager;
    }

    void init();
    void destroy();

    Q_INVOKABLE QObject* getTranslatorAdapter();

private:
    explicit AdapterManager(QObject *parent = nullptr);
    ~AdapterManager()=default;
    AdapterManager(const AdapterManager&) = delete;
    AdapterManager& operator=(const AdapterManager&) = delete;

private:
    std::unordered_map<std::string, std::function<IAdapter*()>> m_adapterMap;  //创建映射
    std::unordered_map<std::string, QObject*> m_adapters;
};
