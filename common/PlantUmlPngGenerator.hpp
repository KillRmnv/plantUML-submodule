#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <sstream>

// Включаем необходимые заголовки Sc-Memory для контекста и логирования
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_result.hpp>

// Прототипы callback-функций (они будут приватными методами, но для чистоты
// мы можем их оставить здесь или просто определить их в .cpp)

class PlantUmlPngGenerator {
private:
    ScMemoryContext * context;
    utils::ScLogger * m_logger;
    
    void deleteTempFiles(const std::string& pumlPath,
        const std::string& pngPath);
    static std::string base64Encode(const std::vector<unsigned char>& data);

public:

    explicit PlantUmlPngGenerator(ScMemoryContext * context, utils::ScLogger * logger);

    virtual ~PlantUmlPngGenerator();

     ScAddr png_generator(const std::string& plantUML_code, ScAddr outputNode);

    // Методы доступа к контексту и логгеру (аналогично примеру)
    ScMemoryContext* GetContext() const { return context; }
    utils::ScLogger* GetLogger() const { return m_logger; }
};