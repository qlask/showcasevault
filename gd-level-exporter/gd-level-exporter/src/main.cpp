#include <Geode/Geode.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "LevelExportData.hpp"

using namespace geode::prelude;

// Convierte el string de configuracion ("E") a un enums::KeyCode de cocos/geode.
// Por simplicidad este mod usa cocos2d::CCKeyboardManager, comparando el char.
class $modify(LevelExporterPlayLayer, PlayLayer) {

    // keyDown se llama cada vez que se presiona una tecla mientras el PlayLayer
    // tiene el foco. Aqui interceptamos "E" (o la tecla configurada) y disparamos
    // la exportacion sin bloquear el comportamiento normal del juego.
    void keyDown(cocos2d::enumKeyCodes key) {
        // Dejamos que el juego procese la tecla normalmente primero.
        PlayLayer::keyDown(key);

        std::string configuredKey = Mod::get()->getSettingValue<std::string>("export-key");
        if (configuredKey.empty()) configuredKey = "E";

        // Mapeo simple: solo soportamos letras A-Z por ahora (cubre el caso pedido, "E").
        char target = std::toupper(configuredKey[0]);
        cocos2d::enumKeyCodes expected =
            static_cast<cocos2d::enumKeyCodes>(cocos2d::KEY_A + (target - 'A'));

        if (key != expected) return;

        LevelExportData data;
        if (!LevelExporter::captureCurrentLevel(data)) {
            log::warn("No se pudo capturar el nivel actual para exportar");
            return;
        }

        std::string formats = Mod::get()->getSettingValue<std::string>("export-format");
        if (formats.empty()) formats = "db,txt,json";

        if (formats.find("db") != std::string::npos) LevelExporter::exportToDb(data);
        if (formats.find("txt") != std::string::npos) LevelExporter::exportToTxt(data);
        if (formats.find("json") != std::string::npos) LevelExporter::exportToJson(data);

        // Notificacion visual en pantalla para confirmar que se exporto.
        Notification::create(
            fmt::format("Nivel '{}' exportado", data.name),
            NotificationIcon::Success
        )->show();
    }
};
