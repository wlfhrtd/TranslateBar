#include "networkmodule.h"


NetworkModule::NetworkModule(QObject *parent)
    : QObject{parent}
    , m_net_manager(new QNetworkAccessManager(this))
    , m_net_reply(nullptr)
    , m_data_buffer(new QByteArray)
    , m_json_response("")
{

}

NetworkModule::~NetworkModule()
{
    delete m_data_buffer;
}


void NetworkModule::httpGet(QString query)
{
    cleanup();

    m_http_request.setUrl(query);

    m_net_reply = m_net_manager->get(m_http_request);

    m_connection_net_reply_readyRead = connect(m_net_reply, &QIODevice::readyRead, this, [=](){ data_ready_to_read(); });
    m_connection_net_reply_finished = connect(m_net_reply, &QNetworkReply::finished, this, [=](){ data_read_finished(); });
}

void NetworkModule::data_ready_to_read()
{
    m_data_buffer->append(m_net_reply->readAll());
}

void NetworkModule::data_read_finished()
{
    if(m_net_reply->error()) {
        m_errorMessage = m_net_reply->errorString();

        cleanup();

        emit errorOccurred();

        return;
    }

    m_json_response = QString(*m_data_buffer);

    emit networkResponseReady();
}

void NetworkModule::cleanup()
{
    m_data_buffer->clear();
    m_json_response = "";

    disconnect(m_connection_net_reply_readyRead);
    disconnect(m_connection_net_reply_finished);
}
