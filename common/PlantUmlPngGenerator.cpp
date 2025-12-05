#include "PlantUmlPngGenerator.hpp"
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_type.hpp>
#include <sc-memory/sc_utils.hpp>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "../utils/base64.h"
// #include "utils/base64.h"

#ifdef _WIN32
// Для localtime_s на Windows
#define LOCALTIME(time_ptr, tm_ptr) localtime_s(tm_ptr, time_ptr)
#else
// Для localtime_r на Unix-подобных системах
#define LOCALTIME(time_ptr, tm_ptr) localtime_r(time_ptr, tm_ptr)
#endif



void PlantUmlPngGenerator::deleteTempFiles(const std::string& pumlPath,
    const std::string& pngPath)
{
if (std::remove(pumlPath.c_str()) == 0)
 m_logger->Debug("[Debug] Удалён временный puml: " + pumlPath + "\n");
else
 m_logger->Debug("[Warn] Не удалось удалить puml: " + pumlPath + "\n");

if (std::remove(pngPath.c_str()) == 0)
 m_logger->Debug("[Debug] Удалено временное png: " + pngPath + "\n");
else
 m_logger->Debug("[Warn] Не удалось удалить png: " + pngPath + "\n");
}
std::string generateUniquePngName()
{
    // Текущая дата и время
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
    LOCALTIME(&time, &tm); // Использование макроса для кроссплатформенности

    // Случайное число для уникальности
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(10000, 99999);

    // Генерация имени
    std::stringstream ss;
    // Форматирование: diagram_YYYY-M-D_H-M-S_RANDOM.base64
    ss << "diagram_"
       << (tm.tm_year + 1900) << "-"
       << (tm.tm_mon + 1) << "-"
       << tm.tm_mday << "_"
       << tm.tm_hour << "-"
       << tm.tm_min << "-"
       << tm.tm_sec << "_"
       << dist(gen) << ".base64";

    return ss.str();
}


PlantUmlPngGenerator::PlantUmlPngGenerator(ScMemoryContext * context, utils::ScLogger * logger)
: context(context)
, m_logger(logger)
{
}

PlantUmlPngGenerator::~PlantUmlPngGenerator()
{
}


ScAddr PlantUmlPngGenerator::png_generator(const std::string& plantUML_code, ScAddr outputNode)
{

    std::string pumlFile = "temp_diagram_" + std::to_string(std::rand()) + ".puml";
    std::ofstream out(pumlFile);
    if (!out.is_open()) {
        m_logger->Debug("Ошибка: не могу создать временный файл PUML");
        return outputNode;
    }
    out << plantUML_code;
    out.close();


    std::string pngFile = pumlFile.substr(0, pumlFile.size() - 5) + ".png";
    std::string cmd =
        "java -jar problem-solver/cxx/plantUML-submodule/utils/plantuml.1.2023.7.jar -tpng " + pumlFile;

    m_logger->Debug("Выполняю команду: " + cmd);

    int result = system(cmd.c_str());
    if (result != 0) {
        m_logger->Debug("Ошибка: plantuml.jar завершился с кодом: " + std::to_string(result));
        return outputNode;
    }

  
    std::ifstream png(pngFile, std::ios::binary);
    if (!png.is_open()) {
        m_logger->Debug("Ошибка: PNG файл не найден после генерации!");
        return outputNode;
    }

    std::vector<unsigned char> png_data(
        (std::istreambuf_iterator<char>(png)),
        std::istreambuf_iterator<char>()
    );
    png.close();

    if (png_data.empty()) {
        m_logger->Debug("Ошибка: PNG файл пустой!");
        return outputNode;
    }

 
    std::string encoded_data = base64_encode(
        reinterpret_cast<const unsigned char*>(png_data.data()),
        png_data.size()
    );
    
    if (encoded_data.empty()) {
        m_logger->Debug("Ошибка Base64-кодирования PNG!");
        return outputNode;
    }


    size_t size = encoded_data.size();
    sc_char *raw_data = new sc_char[size];
    std::copy(encoded_data.begin(), encoded_data.end(), raw_data);

    ScAddr link = context->GenerateNode(ScType::ConstNodeLink);

    ScStreamPtr stream = std::make_shared<ScStream>(
        raw_data,
        size,
        SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK | SC_STREAM_FLAG_TELL,
        false  
    );
    // MemoryBufferPtr buffer = std::make_shared<MemoryBuffer>(raw_data, size);
    // ScStreamPtr stream = std::make_shared<ScStream>(buffer);
    

    if (stream && stream->IsValid() && stream->HasFlag(SC_STREAM_FLAG_READ)) 
    {
        context->SetLinkContent(link, stream);
        context->GenerateConnector(ScType::ConstPermPosArc, outputNode, link);

        ScAddr format_png = context->ResolveElementSystemIdentifier("format_png");
        ScAddr nrel_format = context->ResolveElementSystemIdentifier("nrel_format");
        ScAddr edge = context->GenerateConnector(ScType::ConstCommonArc, link, format_png);
        ScAddr rel= context->GenerateConnector(ScType::ConstPermPosArc, nrel_format, edge);

        m_logger->Debug("PNG успешно сохранён как Base64-link в SC-памяти");
         deleteTempFiles(pumlFile, pngFile);
        return outputNode;
    }

    m_logger->Debug("Ошибка при создании SC-stream.");
    delete[] raw_data;
    return outputNode;
}
