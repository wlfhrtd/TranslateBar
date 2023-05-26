#include "languageguesser.h"


const QRegularExpression LanguageGuesser::regex_input_sanitizer("[\\\\&_@/#,+()[\\]$~%.^'\":*?<>{ }]", QRegularExpression::CaseInsensitiveOption);
const QRegularExpression LanguageGuesser::regex_ru_symbols("([\u0400-\u04FF])", QRegularExpression::CaseInsensitiveOption);


LanguageGuesser::LanguageGuesser(QObject *parent)
    : QObject{parent}
    , m_result("")
    , m_errorMessage("")
    , m_sanitized_input("")
{

}


void LanguageGuesser::doGuess(QString input)
{
    cleanup();

    if(!input_is_valid(input)) {

        m_errorMessage = "Error! Invalid input: " + input; // sanitized one

        emit errorOccurred();

        return;
    }

    QRegularExpressionMatch match = regex_ru_symbols.match(input[0]);
    match.hasMatch()
        ? m_result = "ru"
        : m_result = "en";

    m_sanitized_input = input;

    emit resultReady();
}

bool LanguageGuesser::input_is_valid(QString &raw_input)
{
    if(raw_input.isEmpty()) return false;

    raw_input.replace(regex_input_sanitizer, ""); // AT PLACE AND BY REFERENCE!!!

    if(raw_input.size() < 3) return false;

    return true;
}

void LanguageGuesser::cleanup()
{
    m_result = "";
    m_errorMessage = "";
    m_sanitized_input = "";
}
