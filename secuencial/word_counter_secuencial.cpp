// Contador de palabras - version SECUENCIAL
//
// Sigue el algoritmo descrito en la Practica Guiada 01 - Parte 01:
// abre el archivo, lee linea por linea, separa cada linea en palabras,
// normaliza cada palabra y actualiza su frecuencia en un diccionario.

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "../common/word_utils.h"

int main(int argc, char* argv[]) {
    std::string rutaArchivo = (argc > 1) ? argv[1] : "data/prueba_pequena.txt";

    std::ifstream entrada(rutaArchivo);
    if (!entrada.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo \"" << rutaArchivo << "\"\n";
        return 1;
    }

    auto inicio = std::chrono::steady_clock::now();

    std::map<std::string, int> frecuencia;  // map -> salida ordenada alfabeticamente
    std::string linea;
    long totalPalabras = 0;

    while (std::getline(entrada, linea)) {
        std::istringstream streamLinea(linea);
        std::string token;
        while (streamLinea >> token) {
            std::string palabra = normalizeWord(token);
            if (palabra.empty()) continue;
            ++frecuencia[palabra];
            ++totalPalabras;
        }
    }
    entrada.close();

    auto fin = std::chrono::steady_clock::now();
    double segundos = std::chrono::duration<double>(fin - inicio).count();

    std::cout << "palabra,frecuencia\n";
    for (const auto& [palabra, cantidad] : frecuencia) {
        std::cout << palabra << "," << cantidad << "\n";
    }

    std::cerr << "\n--- Resumen (secuencial) ---\n";
    std::cerr << "Total de palabras procesadas: " << totalPalabras << "\n";
    std::cerr << "Palabras distintas: " << frecuencia.size() << "\n";
    std::cerr << "Tiempo de ejecucion: " << segundos << " s\n";

    return 0;
}
