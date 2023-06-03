#include "inputvalidator.h"


const QRegularExpression InputValidator::regex_input_sanitizer("[\\\\&_@/#,+()[\\]$~%.^'\":*?<>{ }]", QRegularExpression::CaseInsensitiveOption);


InputValidator::InputValidator(QObject *parent)
    : QObject{parent}
    , m_result("")
    , m_errorMessage("")
{

}

void InputValidator::validate(QString userInput)
{
    if(!input_is_valid(userInput)) {

        m_errorMessage = "Error! Invalid input: " + userInput; // sanitized one

        emit errorOccurred();

        return;
    }

    m_result = userInput; // sanitized one

    emit resultReady();
}

bool InputValidator::input_is_valid(QString &raw_input)
{
    if(raw_input.isEmpty()) return false;

    raw_input.replace(regex_input_sanitizer, ""); // AT PLACE AND BY REFERENCE!!!

    if(raw_input.size() < 3) return false;

    return true;
}
