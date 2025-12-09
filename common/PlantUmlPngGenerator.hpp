#ifndef A608D739_0E97_438E_AEA0_667A95CBD50B
#define A608D739_0E97_438E_AEA0_667A95CBD50B

/// @file PlantUmlPngGenerator.hpp
/// @brief Класс для генерации PNG-изображений UML-диаграмм из PlantUML-кода
///        и сохранения результата в SC-памяти.

#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <sstream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_result.hpp>

/// @class PlantUmlPngGenerator
/// @brief Генерирует PNG по PlantUML-коду и сохраняет его в виде sc-ссылки.
///
/// Класс инкапсулирует:
/// - создание временного .puml-файла;
/// - вызов внешнего PlantUML (через java -jar);
/// - чтение сгенерированного PNG;
/// - кодирование в Base64;
/// - запись данных в sc-ссылку и связывание с выходным узлом.
class PlantUmlPngGenerator
{
private:
  /// Контекст sc-памяти, используемый для создания узлов и ссылок.
  ScMemoryContext * context;

  /// Логгер для вывода диагностических сообщений.
  utils::ScLogger * m_logger;

  /// @brief Удаляет временные файлы .puml и .png.
  /// @param pumlPath Путь к временному файлу с PlantUML-диаграммой.
  /// @param pngPath Путь к сгенерированному PNG-файлу.
  void deleteTempFiles(
      const std::string & pumlPath,
      const std::string & pngPath);

  /// @brief Кодирует бинарные данные PNG в строку Base64.
  /// @param data Буфер с бинарными данными изображения.
  /// @return Строка с Base64-представлением данных.
  ///
  /// Реализация находится в .cpp или внешней утилите base64.
  static std::string base64Encode(const std::vector<unsigned char> & data);

public:
  /// @brief Конструктор генератора PNG.
  /// @param context Контекст sc-памяти.
  /// @param logger Логгер для отладки и ошибок.
  explicit PlantUmlPngGenerator(ScMemoryContext * context, utils::ScLogger * logger);

  /// @brief Виртуальный деструктор.
  virtual ~PlantUmlPngGenerator();

  /// @brief Генерирует PNG по PlantUML-коду и сохраняет его в SC-памяти.
  ///
  /// Алгоритм:
  /// 1. Создаёт временный .puml-файл и записывает туда PlantUML-код.
  /// 2. Запускает внешний PlantUML (java -jar ...) для генерации PNG.
  /// 3. Считывает PNG в память и кодирует в Base64.
  /// 4. Создаёт sc-ссылку и записывает в неё закодированные данные.
  /// 5. Связывает ссылку с выходным узлом и помечает формат как PNG.
  ///
  /// @param plantUML_code Исходный текст PlantUML-диаграммы.
  /// @param outputNode Узел, к которому будет присоединена ссылка с PNG.
  /// @return Тот же узел outputNode (для удобства чейнинга или проверок).
  ScAddr png_generator(const std::string & plantUML_code, ScAddr outputNode);

  /// @brief Доступ к контексту sc-памяти.
  /// @return Указатель на ScMemoryContext.
  ScMemoryContext * GetContext() const { return context; }

  /// @brief Доступ к логгеру.
  /// @return Указатель на utils::ScLogger.
  utils::ScLogger * GetLogger() const { return m_logger; }
};

#endif /* A608D739_0E97_438E_AEA0_667A95CBD50B */