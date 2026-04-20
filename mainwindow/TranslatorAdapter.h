#pragma once
#include "IAdapter.h"

// 翻译模块适配器
class TranslatorAdapter : public IAdapter
{
    Q_OBJECT
public:
    explicit TranslatorAdapter(QObject* parent = nullptr);
    ~TranslatorAdapter() override = default;

    // 供QML调用的接口
    Q_INVOKABLE void switchLanguage();
    Q_INVOKABLE QString getText(const QString& key);

signals:
    // 通知QML语言已切换，刷新界面
    void languageChanged();

private:
    // 处理业务模块发来的消息
    void handleModuleMessage(const TopicMessagePtr& msg) override;
};
