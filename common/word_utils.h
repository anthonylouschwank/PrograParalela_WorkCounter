#pragma once

#include <cctype>
#include <string>

// Un byte pertenece a una palabra si es alfanumerico ASCII o si es parte de
// una secuencia UTF-8 multibyte (vocales acentuadas, "ñ", etc. en español).
inline bool isWordByte(unsigned char c) {
    if (std::isalnum(c)) return true;
    if (c >= 0x80) return true;
    return false;
}

// Pasa a minusculas un string UTF-8: ASCII con tolower normal, y las letras
// acentuadas / "Ñ" del bloque Latin-1 Supplement (byte lider 0xC3) mediante
// el desplazamiento fijo de 0x20 que separa mayuscula de minuscula en ese bloque.
inline void toLowerUtf8(std::string& s) {
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) {
            s[i] = static_cast<char>(std::tolower(c));
        } else if (c == 0xC3 && i + 1 < s.size()) {
            unsigned char siguiente = static_cast<unsigned char>(s[i + 1]);
            if (siguiente >= 0x80 && siguiente <= 0x9E && siguiente != 0x97) {
                s[i + 1] = static_cast<char>(siguiente + 0x20);
            }
            ++i;
        }
    }
}

// Normaliza un token: quita puntuacion en los extremos (comas, puntos,
// comillas, signos, etc.) y convierte a minusculas.
inline std::string normalizeWord(const std::string& raw) {
    size_t inicio = 0, fin = raw.size();
    while (inicio < fin && !isWordByte(static_cast<unsigned char>(raw[inicio]))) ++inicio;
    while (fin > inicio && !isWordByte(static_cast<unsigned char>(raw[fin - 1]))) --fin;

    std::string palabra = raw.substr(inicio, fin - inicio);
    toLowerUtf8(palabra);
    return palabra;
}
