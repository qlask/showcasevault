#include "LevelExportData.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/PlayLayer.hpp>

using namespace geode::prelude;

// Traduce el enum interno de dificultad + demon a texto legible.
static std::string difficultyToString(GJGameLevel* lvl) {
    if (!lvl) return "N/A";

    // isDemon / demonDifficulty cubren los distintos niveles de demon.
    if (lvl->m_demon) {
        switch (lvl->m_demonDifficulty) {
            case 3: return "Easy Demon";
            case 4: return "Medium Demon";
            case 0: return "Hard Demon";   // valor por defecto en muchas versiones
            case 5: return "Insane Demon";
            case 6: return "Extreme Demon";
            default: return "Demon";
        }
    }

    if (lvl->m_autoLevel) return "Auto";

    switch (lvl->m_difficulty) {
        case GJDifficulty::Easy: return "Easy";
        case GJDifficulty::Normal: return "Normal";
        case GJDifficulty::Hard: return "Hard";
        case GJDifficulty::Harder: return "Harder";
        case GJDifficulty::Insane: return "Insane";
        default: return "N/A";
    }
}

static std::string categoryFromLevel(GJGameLevel* lvl) {
    if (!lvl) return "Unrated";

    if (lvl->m_isEpic == 3) return "Mythic";
    if (lvl->m_isEpic == 2) return "Legendary";
    if (lvl->m_isEpic == 1) return "Epic";
    if (lvl->m_featured > 0) return "Featured";
    if (lvl->m_stars > 0) return "Rated";
    return "Unrated";
}

bool LevelExporter::captureCurrentLevel(LevelExportData& out) {
    auto playLayer = PlayLayer::get();
    if (!playLayer) {
        log::warn("captureCurrentLevel: no hay PlayLayer activo");
        return false;
    }

    auto level = playLayer->m_level;
    if (!level) {
        log::warn("captureCurrentLevel: PlayLayer sin nivel asociado");
        return false;
    }

    out.levelID = level->m_levelID.value();
    out.name = level->m_levelName;
    out.author = level->m_creatorName.empty() ? "-" : std::string(level->m_creatorName);
    out.songID = level->m_songID;
    out.songName = level->m_songName.empty() ? "Unknown" : std::string(level->m_songName);
    out.stars = level->m_stars;
    out.rated = level->m_stars > 0;
    out.category = categoryFromLevel(level);
    out.objectCount = level->m_objectCount;
    out.difficulty = difficultyToString(level);

    return true;
}
