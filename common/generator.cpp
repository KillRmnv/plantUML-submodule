#include <curl/curl.h>
#include <vector>
#include <fstream>
#include <chrono>
#include <random>
#include <sstream>
#include <iostream>


std::string generateUniquePngName()
{
    // Текущая дата и время
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    // Случайное число для уникальности
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(10000, 99999);

    // Генерация имени
    std::stringstream ss;
    ss << "diagram_"
       << (tm.tm_year + 1900) << "-"
       << (tm.tm_mon + 1) << "-"
       << tm.tm_mday << "_"
       << tm.tm_hour << "-"
       << tm.tm_min << "-"
       << tm.tm_sec << "_"
       << dist(gen) << ".png";

    return ss.str();
}

// Callback функция
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::vector<unsigned char> *data)
{
    size_t total = size * nmemb;
    data->insert(data->end(), (unsigned char*)contents, (unsigned char*)contents + total);
    return total;
}

// Запись строки
static size_t WriteString(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t total = size * nmemb;
    s->append((char*)contents, total);
    return total;
}

void png_generator(const std::string plantUML_code){
    CURL *curl = curl_easy_init();

    if(!curl) {
        //SC_LOG_INFO("Ошибка при получении png");
        //return SC_RESULT_ERROR;
        std::cerr << "CURL init failed\n";
        return;
    }

    // POST-запрос 
    std::string response;
    std::string url = "https://www.plantuml.com/plantuml/png";
    std::string postData = "text=" + plantUML_code;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L); // ВАЖНО: не следовать за 302

    CURLcode res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        //SC_LOG_INFO("Ошибка при получении png");
        //return SC_RESULT_ERROR;
        std::cerr << "POST error\n";
        return;
    }

    // Извлекаем заголовок "Location:"
    char *redirect_url_c = nullptr;
    curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirect_url_c);

    if (!redirect_url_c) {
        //SC_LOG_INFO("Ошибка при получении png");
        //return SC_RESULT_ERROR;
        std::cerr << "Redirect URL not found\n";
        return;
    }

    std::string redirect_url = redirect_url_c;

    std::cout << "Redirect URL: " << redirect_url << "\n";

    // PNG URL
    std::string png_url = redirect_url;
    size_t pos = png_url.find("/uml/");
    if (pos != std::string::npos)
        png_url.replace(pos, 5, "/png/");

    std::cout << "PNG URL: " << png_url << "\n";

    // GET PNG
    std::vector<unsigned char> png_data;

    curl_easy_setopt(curl, CURLOPT_URL, png_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &png_data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    if(res != CURLE_OK || png_data.empty()) {
        std::cerr << "Failed to download PNG\n";
        return;
    }

    // Сохранение в файл
    // Путь к папке
    std::string moduleDir = std::string(__FILE__);
    moduleDir = moduleDir.substr(0, moduleDir.find_last_of("/\\"));

    // Сохранение в нужную папку
    std::string fileName = generateUniquePngName();
    std::string outputPath = moduleDir + "/../png_results/" + fileName;

    // Запись
    //std::ofstream file(outputPath, std::ios::binary);
    std::ofstream file(fileName, std::ios::binary);
    file.write((char*)png_data.data(), png_data.size());
    file.close();

    std::cout << "PNG сохранён: " << outputPath << std::endl;


}

int main(){
    std::string plantUML_code = "@startuml\nAlice -> Bob: Hi\n@enduml";
    png_generator(plantUML_code);
    return 0;
}