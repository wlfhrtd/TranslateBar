#include "translater.h"


Translater::Translater(QObject *parent)
    : QObject{parent}
    , m_word_to_translate("")
    , m_translation("")
    , m_error_message("")
    , m_network_module(new NetworkModule(this))
{
    connect(m_network_module, &NetworkModule::networkResponseReady, this, [=](){ onNetworkResponseReady(); });
    connect(m_network_module, &NetworkModule::errorOccurred, this, [=](){ onNetworkErrorOccurred(); });
}


void Translater::doTranslation(QString languageFromId, QString languageToId, QString input)
{
    QString query = "https://translate.googleapis.com/translate_a/single?client=gtx&sl="
                    + languageFromId + "&tl=" + languageToId + "&dt=t&q=" + input;

    m_network_module->httpGet(query);
}

void Translater::onNetworkResponseReady()
{
    QString json = m_network_module->jsonResponse();

    QStringList words = json.split('"'); // [[["кот","cat",null,null,10]],null,"en",null,null,null,null,[]]

    if(words.size() == 0) {
        m_error_message = QString::asprintf("file:///%s:%i: %s", __FILE__, __LINE__, "JSON PARSE ERROR! OUTPUT IS EMPTY");

        emit errorOccurred();

        return;
    }

    m_translation = words[1];

    emit translationReady();
}

void Translater::onNetworkErrorOccurred()
{
    m_error_message = m_network_module->errorMessage();

    emit errorOccurred();
}
