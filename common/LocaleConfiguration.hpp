#pragma once

#include <string>
#include <unordered_map>

class LocaleConfiguration
{
public:
    // Тип для хранения пары: идентификатор -> перевод
    using TranslationMap = std::unordered_map<std::string, std::string>;

    // Конструктор с выбором языка
    LocaleConfiguration(const std::string& language);

    // Получение перевода по идентификатору
    std::string getTranslation(const std::string& key) const;
    LocaleConfiguration();

private:
    TranslationMap translations;
    void loadLanguage(const std::string& language);

    // Вспомогательная функция для удаления пробелов с начала и конца строки
    static void trim(std::string& s);
};
