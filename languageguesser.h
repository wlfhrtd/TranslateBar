#ifndef LANGUAGEGUESSER_H
#define LANGUAGEGUESSER_H

#include <QObject>
#include <QRegularExpression>


class LanguageGuesser : public QObject
{
private:
    Q_OBJECT

    Q_PROPERTY(QString result READ result CONSTANT)
    Q_PROPERTY(QString errorMessage READ errorMessage CONSTANT)
    Q_PROPERTY(QString input READ input CONSTANT)

    const static QRegularExpression regex_input_sanitizer;
    const static QRegularExpression regex_ru_symbols;

    QString m_result;
    QString m_errorMessage;
    QString m_sanitized_input;

    bool input_is_valid(QString& raw_input);

    void cleanup();

public:
    explicit LanguageGuesser(QObject *parent = nullptr);


    inline QString result() const { return m_result; }
    inline QString errorMessage() const { return m_errorMessage; }
    inline QString input() const { return m_sanitized_input; }

public slots:
    void doGuess(QString input);

signals:
    void resultReady();
    void errorOccurred();
};

#endif // LANGUAGEGUESSER_H
