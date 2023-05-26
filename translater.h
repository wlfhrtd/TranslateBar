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

    const QString QUERY_FROM_EN_TO_RU = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=ru&dt=t&q=";
    const QString QUERY_FROM_RU_TO_EN = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=ru&tl=en&dt=t&q=";

    const static QRegularExpression regex_parsing_from_en_to_ru;
    const static QRegularExpression regex_parsing_from_ru_to_en;

    QString m_language_id;
    QString m_word_to_translate;
    QString m_translation;
    QString m_error_message;

    NetworkModule* m_network_module;
public:
    explicit Translater(QObject *parent = nullptr);


    inline QString translation() const { return m_translation; }
    inline QString errorMessage() const { return m_error_message; }

public slots:
    void doTranslation(QString languageId, QString input);

    void onNetworkResponseReady();
    void onNetworkErrorOccurred();

signals:
    void translationReady();
    void errorOccurred();

};

#endif // TRANSLATER_H
