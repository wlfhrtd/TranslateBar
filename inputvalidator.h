#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QObject>
#include <QRegularExpression>

// RESULT: sanitized and validated input
class InputValidator : public QObject
{
private:
    Q_OBJECT

    Q_PROPERTY(QString result READ result CONSTANT)
    Q_PROPERTY(QString errorMessage READ errorMessage CONSTANT)

    const static QRegularExpression regex_input_sanitizer;

    QString m_result;
    QString m_errorMessage;

    bool input_is_valid(QString& raw_input);

public:
    explicit InputValidator(QObject *parent = nullptr);

    inline QString result() const { return m_result; }
    inline QString errorMessage() const { return m_errorMessage; }

public slots:
    void validate(QString userInput);

signals:
    void resultReady();
    void errorOccurred();

};

#endif // INPUTVALIDATOR_H
