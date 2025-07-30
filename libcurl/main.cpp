#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

//library for parsing json files
using json = nlohmann::json;

//function to write the results into a string variable
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    out->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    //create an easy handle
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        //set URL for the curl easy handle
        curl_easy_setopt(curl, CURLOPT_URL, "https://pokeapi.co/api/v2/pokemon/charmander");
        //set function for the curl easy handle
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        //set variable for the result of the curl easy handle
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        //perform a blocking network transfer
        CURLcode res = curl_easy_perform(curl);

        //free easy handle after use
        curl_easy_cleanup(curl);

        //check if response was successfully received
        if (res == CURLE_OK) {
            try {
                // JSON Parsing
                json j = json::parse(response);

                // Access via key and index notation
                auto ability = j["abilities"][1];
                std::string ability_name = ability["ability"]["name"];

                std::cout << "Nombre de la segunda habilidad: " << ability_name << std::endl;

                // Access the whole object
                std::cout << "Objeto ability completo: " << ability.dump(4) << std::endl;
            }
            catch (json::parse_error& e) {
                std::cerr << "Error al parsear JSON: " << e.what() << std::endl;
            }
            catch (json::type_error& e) {
                std::cerr << "Error de tipo JSON: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
        }
    }

    std::cout << "\nPresiona Enter para salir...";
    std::cin.get();

    return 0;
}