#pragma once
#include <string>
#include <cstdint>

// Estructura con todos los campos que queremos capturar de un GJGameLevel.
struct LevelExportData {
    int64_t levelID = 0;
    std::string name;
    std::string author;
    int64_t songID = 0;
    std::string songName;
    bool rated = false;          // featured/rated
    std::string category;        // Rated / Unrated / Featured / Epic / Legendary / Mythic / Daily / Weekly, etc.
    int objectCount = 0;
    std::string difficulty;      // Easy, Normal, Hard, Harder, Insane, Demon (+ tipo de demon), N/A
    int stars = 0;
};

namespace LevelExporter {
    // Construye un LevelExportData a partir del nivel actualmente cargado en el PlayLayer.
    // Devuelve false si no hay nivel activo o no se pudo leer.
    bool captureCurrentLevel(LevelExportData& out);

    // Exporta a los 3 formatos. dbPath/txtPath/jsonPath son carpetas o rutas base;
    // internamente se generan/actualizan levels.db, levels.txt, levels.json
    // dentro de la carpeta de datos del mod.
    void exportToDb(const LevelExportData& data);
    void exportToTxt(const LevelExportData& data);
    void exportToJson(const LevelExportData& data);
}
