// Contador de palabras - version PARALELA (hilos)
//
// La lectura del archivo se mantiene secuencial (igual que en el algoritmo
// original), pero el trabajo repetitivo -separar en palabras, normalizar y
// contar- se reparte entre varios hilos. Cada hilo llena su propio
// diccionario local (sin memoria compartida durante el conteo, por lo que no
// se necesitan mutex ahi) y al final el hilo principal combina los
// resultados parciales en un diccionario global.

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../common/word_utils.h"

void procesarLineas(const std::vector<std::string>& lineas, size_t desde, size_t hasta,
                     std::unordered_map<std::string, int>& frecuenciaLocal, long& totalLocal) {
    for (size_t i = desde; i < hasta; ++i) {
        std::istringstream streamLinea(lineas[i]);
        std::string token;
        while (streamLinea >> token) {
            std::string palabra = normalizeWord(token);
            if (palabra.empty()) continue;
            ++frecuenciaLocal[palabra];
            ++totalLocal;
        }
    }
}

int main(int argc, char* argv[]) {
    std::string rutaArchivo = (argc > 1) ? argv[1] : "data/prueba_pequena.txt";
    unsigned int numHilos = (argc > 2) ? static_cast<unsigned int>(std::stoul(argv[2]))
                                        : std::thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 4;

    std::ifstream entrada(rutaArchivo);
    if (!entrada.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo \"" << rutaArchivo << "\"\n";
        return 1;
    }

    std::vector<std::string> lineas;
    std::string linea;
    while (std::getline(entrada, linea)) {
        lineas.push_back(linea);
    }
    entrada.close();

    auto inicio = std::chrono::steady_clock::now();

    size_t totalLineas = lineas.size();
    numHilos = static_cast<unsigned int>(std::min<size_t>(numHilos, std::max<size_t>(totalLineas, 1)));

    std::vector<std::thread> hilos;
    std::vector<std::unordered_map<std::string, int>> frecuenciasLocales(numHilos);
    std::vector<long> totalesLocales(numHilos, 0);

    size_t base = totalLineas / numHilos;
    size_t resto = totalLineas % numHilos;
    size_t cursor = 0;

    for (unsigned int i = 0; i < numHilos; ++i) {
        size_t tamanoChunk = base + (i < resto ? 1 : 0);
        size_t desde = cursor;
        size_t hasta = cursor + tamanoChunk;
        cursor = hasta;
        hilos.emplace_back(procesarLineas, std::cref(lineas), desde, hasta,
                            std::ref(frecuenciasLocales[i]), std::ref(totalesLocales[i]));
    }

    for (auto& h : hilos) h.join();

    // Combinar los diccionarios locales en uno global (unica seccion
    // secuencial de esta etapa, ya con los hilos terminados).
    std::map<std::string, int> frecuenciaGlobal;
    long totalPalabras = 0;
    for (unsigned int i = 0; i < numHilos; ++i) {
        for (const auto& [palabra, cantidad] : frecuenciasLocales[i]) {
            frecuenciaGlobal[palabra] += cantidad;
        }
        totalPalabras += totalesLocales[i];
    }

    auto fin = std::chrono::steady_clock::now();
    double segundos = std::chrono::duration<double>(fin - inicio).count();

    std::cout << "palabra,frecuencia\n";
    for (const auto& [palabra, cantidad] : frecuenciaGlobal) {
        std::cout << palabra << "," << cantidad << "\n";
    }

    std::cerr << "\n--- Resumen (paralelo) ---\n";
    std::cerr << "Total de palabras procesadas: " << totalPalabras << "\n";
    std::cerr << "Palabras distintas: " << frecuenciaGlobal.size() << "\n";
    std::cerr << "Hilos utilizados: " << numHilos << "\n";
    std::cerr << "Tiempo de ejecucion: " << segundos << " s\n";

    return 0;
}
