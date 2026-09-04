#include "LevelExportData.hpp"
#include <Geode/Geode.hpp>
#include <sqlite3.h>
#include <fstream>
#include <sstream>

using namespace geode::prelude;

// Todos los archivos se guardan en la carpeta de datos persistentes del mod:
// %appdata%/GeometryDash/geode/mods/com.tuusuario.levelexporter/ (Windows)
// o el equivalente segun plataforma - Geode lo resuelve por nosotros.
static std::filesystem::path dataDir() {
    auto dir = Mod::get()->getSaveDir();
    std::filesystem::create_directories(dir);
    return dir;
}

// ---------------------------------------------------------------------------
// JSON
// ---------------------------------------------------------------------------
void LevelExporter::exportToJson(const LevelExportData& data) {
    auto path = dataDir() / "levels.json";

    matjson::Value root = matjson::Value::array();

    // Si ya existe el archivo, lo cargamos para ir agregando (append)
    if (std::filesystem::exists(path)) {
        std::ifstream in(path);
        std::stringstream buffer;
        buffer << in.rdbuf();
        auto parsed = matjson::parse(buffer.str());
        if (parsed.isOk() && parsed.unwrap().isArray()) {
            root = parsed.unwrap();
        }
    }

    // Evitar duplicados: si el ID ya existe, lo reemplazamos.
    auto& arr = root.asArray().unwrap();
    bool replaced = false;
    for (auto& item : arr) {
        if (item.contains("id") && item["id"].asInt().unwrapOr(0) == data.levelID) {
            item = matjson::makeObject({
                {"id", data.levelID},
                {"name", data.name},
                {"author", data.author},
                {"song_id", data.songID},
                {"song", data.songName},
                {"rated", data.rated},
                {"category", data.category},
                {"objects", data.objectCount},
                {"difficulty", data.difficulty},
                {"stars", data.stars},
            });
            replaced = true;
            break;
        }
    }

    if (!replaced) {
        arr.push_back(matjson::makeObject({
            {"id", data.levelID},
            {"name", data.name},
            {"author", data.author},
            {"song_id", data.songID},
            {"song", data.songName},
            {"rated", data.rated},
            {"category", data.category},
            {"objects", data.objectCount},
            {"difficulty", data.difficulty},
            {"stars", data.stars},
        }));
    }

    std::ofstream out(path, std::ios::trunc);
    out << root.dump(4);
    log::info("Nivel exportado a JSON: {}", path.string());
}

// ---------------------------------------------------------------------------
// TXT (append, un renglon legible por nivel)
// ---------------------------------------------------------------------------
void LevelExporter::exportToTxt(const LevelExportData& data) {
    auto path = dataDir() / "levels.txt";

    std::ofstream out(path, std::ios::app);
    out << "ID: " << data.levelID
        << " | Nombre: " << data.name
        << " | Autor: " << data.author
        << " | Cancion: " << data.songName << " (ID " << data.songID << ")"
        << " | Rated: " << (data.rated ? "true" : "false")
        << " | Categoria: " << data.category
        << " | Objetos: " << data.objectCount
        << " | Dificultad: " << data.difficulty
        << " | Estrellas: " << data.stars
        << "\n";

    log::info("Nivel exportado a TXT: {}", path.string());
}

// ---------------------------------------------------------------------------
// SQLite DB
// ---------------------------------------------------------------------------
void LevelExporter::exportToDb(const LevelExportData& data) {
    auto path = dataDir() / "levels.db";

    sqlite3* db = nullptr;
    if (sqlite3_open(path.string().c_str(), &db) != SQLITE_OK) {
        log::error("No se pudo abrir levels.db: {}", sqlite3_errmsg(db));
        return;
    }

    const char* createSql =
        "CREATE TABLE IF NOT EXISTS levels ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "author TEXT NOT NULL,"
        "song_id INTEGER,"
        "song TEXT,"
        "rated INTEGER NOT NULL DEFAULT 0,"
        "category TEXT,"
        "objects INTEGER DEFAULT 0,"
        "difficulty TEXT,"
        "stars INTEGER DEFAULT 0"
        ");";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, createSql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        log::error("Error creando tabla: {}", errMsg ? errMsg : "?");
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    const char* upsertSql =
        "INSERT INTO levels (id, name, author, song_id, song, rated, category, objects, difficulty, stars) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(id) DO UPDATE SET "
        "name=excluded.name, author=excluded.author, song_id=excluded.song_id, "
        "song=excluded.song, rated=excluded.rated, category=excluded.category, "
        "objects=excluded.objects, difficulty=excluded.difficulty, stars=excluded.stars;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, upsertSql, -1, &stmt, nullptr) != SQLITE_OK) {
        log::error("Error preparando statement: {}", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int64(stmt, 1, data.levelID);
    sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, data.author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, data.songID);
    sqlite3_bind_text(stmt, 5, data.songName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, data.rated ? 1 : 0);
    sqlite3_bind_text(stmt, 7, data.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, data.objectCount);
    sqlite3_bind_text(stmt, 9, data.difficulty.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 10, data.stars);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        log::error("Error insertando/actualizando nivel: {}", sqlite3_errmsg(db));
    } else {
        log::info("Nivel exportado a DB: {}", path.string());
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
