#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>


class NetworkModule : public QObject
{
private:
    Q_OBJECT


    QNetworkAccessManager* m_net_manager;
    QNetworkReply* m_net_reply;
    QByteArray* m_data_buffer;

    QNetworkRequest m_http_request;

    QMetaObject::Connection m_connection_net_reply_readyRead;
    QMetaObject::Connection m_connection_net_reply_finished;

    QString m_json_response;
    QString m_errorMessage;

    void data_ready_to_read();
    void data_read_finished();

    void cleanup();

public:
    explicit NetworkModule(QObject *parent = nullptr);
    ~NetworkModule();

    inline QString jsonResponse() const { return m_json_response; }
    inline QString errorMessage() const { return m_errorMessage; }

public slots:
    void httpGet(QString query);

signals:
    void networkResponseReady();
    void errorOccurred();

};

#endif // NETWORKMODULE_H
