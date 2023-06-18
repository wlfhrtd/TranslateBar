#ifndef TRANSLATER_H
#define TRANSLATER_H

#include <QObject>
#include <QRegularExpression>

#include "networkmodule.h"


class Translater : public QObject
{
private:
    Q_OBJECT

    Q_PROPERTY(QString translation READ translation CONSTANT)
    Q_PROPERTY(QString errorMessage READ errorMessage CONSTANT)


    QString m_word_to_translate;
    QString m_translation;
    QString m_error_message;

    NetworkModule* m_network_module;
public:
    explicit Translater(QObject *parent = nullptr);


    inline QString translation() const { return m_translation; }
    inline QString errorMessage() const { return m_error_message; }

public slots:
    void doTranslation(QString languageFromId, QString languageToId, QString input);

    void onNetworkResponseReady();
    void onNetworkErrorOccurred();

signals:
    void translationReady();
    void errorOccurred();

};

#endif // TRANSLATER_H
