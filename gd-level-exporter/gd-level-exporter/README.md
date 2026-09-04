# Level Exporter (Geode Mod)

Mod para Geometry Dash (via [Geode SDK](https://geode-sdk.org)) que, al presionar
**E** mientras estás jugando un nivel, extrae sus metadatos y los guarda en:

- `levels.db` (SQLite)
- `levels.txt` (texto plano, una línea por nivel)
- `levels.json` (array JSON, se actualiza/agrega por ID)

Los archivos se guardan en la carpeta de datos del mod (accesible desde
Geode → Mods → Level Exporter → carpeta de datos).

## Campos capturados

| Campo       | Descripción                                  |
|-------------|-----------------------------------------------|
| id          | ID del nivel                                  |
| name        | Nombre del nivel                              |
| author      | Nombre del creador                            |
| song_id     | ID de la canción                              |
| song        | Nombre de la canción                          |
| rated       | `true`/`false` (tiene estrellas asignadas)    |
| category    | Unrated / Rated / Featured / Epic / Legendary / Mythic |
| objects     | Cantidad de objetos del nivel                 |
| difficulty  | Easy … Insane, Demon (con subtipo), Auto, N/A |
| stars       | Cantidad de estrellas                         |

## Configuración

Desde el menú de Geode se puede cambiar:
- **export-key**: la tecla que dispara la exportación (por defecto `E`).
- **export-format**: cuáles formatos generar (`db`, `txt`, `json`, o combinación separada por comas).

## Estructura del proyecto

```
gd-level-exporter/
├── mod.json                    # Manifiesto del mod (Geode)
├── CMakeLists.txt              # Build config (incluye SQLite via FetchContent)
├── src/
│   ├── main.cpp                 # Hook de PlayLayer::keyDown
│   ├── LevelExportData.hpp/.cpp # Extracción de datos del GJGameLevel
│   └── Exporters.cpp            # Escritura a .db / .txt / .json
└── .github/workflows/build.yml # CI: compila para Win/Mac/Android y sube el .geode
```

## Compilar localmente

1. Instalar el [Geode CLI](https://docs.geode-sdk.org/getting-started/):
   ```
   geode --version
   ```
2. Clonar este repo e instalar el SDK apuntado en `mod.json`:
   ```
   git clone <tu-repo>
   cd gd-level-exporter
   geode sdk install
   ```
3. Compilar:
   ```
   geode build
   ```
   Esto genera un `.geode` en `build/`.

También puedes usar CMake directamente si ya tienes `GEODE_SDK` en tu entorno:
```
cmake -B build
cmake --build build --config Release
```

## CI / GitHub Actions

El workflow `.github/workflows/build.yml`:
1. Compila el mod para **Windows**, **macOS**, **Android32** y **Android64** usando
   la action oficial [`geode-sdk/build-geode-mod`](https://github.com/geode-sdk/build-geode-mod).
2. Combina todos los binarios en un único `.geode` (job `package`).
3. Sube ese `.geode` como artifact descargable en cada push.
4. Si el push es un **tag** (ej. `v1.0.0`), crea automáticamente un GitHub Release
   con el `.geode` adjunto.

Para publicar una versión:
```bash
git tag v1.0.0
git push origin v1.0.0
```

## Instalar el mod compilado

1. Descargar el `.geode` desde el artifact de Actions o desde el Release.
2. Colocarlo en la carpeta `mods/` de Geode (o arrastrarlo sobre la ventana de Geode).
3. Abrir un nivel, presionar **E**, y revisar la carpeta de datos del mod para ver
   `levels.db`, `levels.txt` y `levels.json`.

## Notas técnicas

- El nivel se lee directamente del `PlayLayer::m_level` (un `GJGameLevel*`), por lo
  que **no** requiere conexión a internet ni a los servidores de GD — funciona con
  cualquier nivel que estés jugando localmente.
- SQLite se compila embebido (amalgamation descargado por CMake `FetchContent`),
  así que no depende de ninguna librería del sistema — funciona igual en Windows,
  macOS y Android.
- Los `INSERT` a la base usan `ON CONFLICT(id) DO UPDATE`, así que volver a exportar
  el mismo nivel actualiza el registro en vez de duplicarlo.
