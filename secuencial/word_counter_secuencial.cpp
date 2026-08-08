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
#include <vector>

#include "../common/word_utils.h"

int main(int argc, char* argv[]) {
    std::string rutaArchivo = (argc > 1) ? argv[1] : "../data/prueba_pequena.txt";

    std::ifstream entrada(rutaArchivo);
    if (!entrada.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo \"" << rutaArchivo << "\"\n";
        return 1;
    }

    // Lectura del archivo a memoria: NO se mide, para que el tiempo medido
    // corresponda unicamente al procesamiento (separar, normalizar, contar)
    // y sea comparable con la version paralela, que hace lo mismo.
    std::vector<std::string> lineas;
    std::string linea;
    while (std::getline(entrada, linea)) {
        lineas.push_back(linea);
    }
    entrada.close();

    auto inicio = std::chrono::steady_clock::now();

    std::map<std::string, int> frecuencia;  // map -> salida ordenada alfabeticamente
    long totalPalabras = 0;

    for (const auto& l : lineas) {
        std::istringstream streamLinea(l);
        std::string token;
        while (streamLinea >> token) {
            std::string palabra = normalizeWord(token);
            if (palabra.empty()) continue;
            ++frecuencia[palabra];
            ++totalPalabras;
        }
    }

    auto fin = std::chrono::steady_clock::now();
    double segundos = std::chrono::duration<double>(fin - inicio).count();

    std::cout << "palabra,frecuencia\n";
    for (const auto& [palabra, cantidad] : frecuencia) {
        std::cout << palabra << "," << cantidad << "\n";
    }

    std::cerr << "\n--- Resumen (secuencial) ---\n";
    std::cerr << "Total de palabras procesadas: " << totalPalabras << "\n";
    std::cerr << "Palabras distintas: " << frecuencia.size() << "\n";
    std::cerr << "Tiempo de conteo: " << segundos << " s\n";
    std::cerr << "TIEMPO_SEGUNDOS=" << segundos << "\n";  // linea facil de parsear para benchmarks

    return 0;
}
