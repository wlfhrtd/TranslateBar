#include "translater.h"


const QRegularExpression Translater::regex_parsing_from_en_to_ru("\"([- \u0400-\u04FF]+)\"", QRegularExpression::CaseInsensitiveOption);
const QRegularExpression Translater::regex_parsing_from_ru_to_en("\"([- a-z]+)\"", QRegularExpression::CaseInsensitiveOption);


Translater::Translater(QObject *parent)
    : QObject{parent}
    , m_language_id("")
    , m_word_to_translate("")
    , m_translation("")
    , m_error_message("")
    , m_network_module(new NetworkModule(this))
{
    connect(m_network_module, &NetworkModule::networkResponseReady, this, [=](){ onNetworkResponseReady(); });
    connect(m_network_module, &NetworkModule::errorOccurred, this, [=](){ onNetworkErrorOccurred(); });
}


void Translater::doTranslation(QString languageId, QString input)
{
    m_language_id = languageId;

    QString query;
    m_language_id == "en"
        ? query = QUERY_FROM_EN_TO_RU + input
        : query = QUERY_FROM_RU_TO_EN + input;

    m_network_module->httpGet(query);
}

void Translater::onNetworkResponseReady()
{
    QString json = m_network_module->jsonResponse();

    QRegularExpressionMatchIterator it;
    m_language_id == "en"
        ? it = regex_parsing_from_en_to_ru.globalMatch(json)
        : it = regex_parsing_from_ru_to_en.globalMatch(json);

    QStringList words;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString word = match.captured(1);
        words.emplace_back(word);
    }

    if(words.size() == 0) {
        m_error_message = QString::asprintf("file:///%s:%i: %s", __FILE__, __LINE__, "JSON PARSE ERROR! OUTPUT IS EMPTY");

        emit errorOccurred();

        return;
    }

    m_translation = words[0];

    emit translationReady();
}

void Translater::onNetworkErrorOccurred()
{
    m_error_message = m_network_module->errorMessage();

    emit errorOccurred();
}