#include "TranslatorAdapter.h"
#include "TopicMessage.h"

TranslatorAdapter::TranslatorAdapter(QObject* parent)
    : IAdapter(parent)
{
}

void TranslatorAdapter::switchLanguage()
{
    auto module = getModule();
    if (!module)
    {
        return;
    }

    TopicMessagePtr msg = CreateMessage(TOPIC_TRANSLATOR_UI_SWITCH_LANGUAGE);
    module->SubscribeUIMessage(msg);
}

QString TranslatorAdapter::getText(const QString& key)
{
    auto module = getModule();
    if (!module)
        return key;

    return QString::fromStdString(module->GetTransText(key.toStdString()));
}

void TranslatorAdapter::handleModuleMessage(const TopicMessagePtr& msg)
{
    if (!msg)
        return;

    if (msg->topic == TOPIC_TRANSLATOR_MOUDLE_LANGUAGE_CHANGED)
    {
        emit languageChanged();
    }
}
