#include "ResourceManager.h"

#include "fstream"
#include <ranges>

ResourceManager::ResourceManager() {
    this->loadFonts("Resources/Assets/Fonts/");
    this->loadTextureSheet("TestTilesheet", "Resources/Assets/Textures/TileSheets/TestTileset.png", "Resources/Data/TilemapData/factory_atlas_data");
    this->loadTextureSheet("Cube", "Resources/Assets/Textures/Sprites/Cube.png", "Resources/Data/SpriteData/sprite_atlas_data");
    this->loadFragmentShader("UpscalingShader", "Resources/Assets/Shaders/upscaling.frag");
    this->loadAnimatedTextures("KineticSet", "Resources/Assets/Textures/Sprites/KineticSet_Sheet3.png", "Resources/Data/SpriteData/kineticSet_Sheet_Data");
}

ResourceManager::~ResourceManager() {
    for (const auto &font: fonts | std::views::values) {
        delete font;
    }
    this->fonts.clear();
}

void ResourceManager::loadFonts(const std::string& folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (!entry.is_regular_file()) continue;

        const std::filesystem::path& filePath = entry.path();
        if (filePath.extension() != ".ttf") continue;

        std::string fontName = filePath.stem().string();

        auto* font = new sf::Font(filePath);
        this->fonts[fontName] = font;

    }

}

void ResourceManager::loadTextureSheet(const std::string &sheetName, const std::string &texturePath, const std::string &atlasDataPath) {
    TextureSheet sheet;
    if (!sheet.texture.loadFromFile(texturePath)) {
        std::cerr << "[RESOURCEMANAGER] Error: cannot load " << texturePath << std::endl;
        return;
    }

    std::ifstream ifs(atlasDataPath);
    if (!ifs.is_open()) {
        std::cerr << "[RESOURCEMANAGER] Error: cannot open " << atlasDataPath << std::endl;
        return;
    }
    std::string name;
    int x, y, w, h;
    while (ifs >> name >> x >> y >> w >> h) {
        sheet.rects[name] = sf::IntRect({x, y}, {w, h});
    }

    this->textureSheets[sheetName] = std::move(sheet);
}

void ResourceManager::loadAnimatedTextures(const std::string &sheetName, const std::string &texturePath, const std::string &atlasDataPath) {
    AnimationSheet sheet;
    sheet.texture.setSmooth(false);
    if (!sheet.texture.loadFromFile(texturePath)) {
        std::cerr << "[RESOURCEMANAGER] Error: cannot load " << texturePath << std::endl;
        return;
    }
    std::ifstream ifs(atlasDataPath);
    if (!ifs.is_open()) {
        std::cerr << "[RESOURCEMANAGER] Error: cannot open " << atlasDataPath << std::endl;
        return;
    }
    std::string name;
    int x, y, w, h, frames;
    float baseDuration;
    bool loop = false;

    while (ifs >> name >> x >> y >> w >> h >> frames >> baseDuration >> loop) {
        sheet.animations[name].frames.reserve(frames);
        for (int i = 0; i < frames; i++) {
            sheet.animations[name].frames.push_back(sf::IntRect({x + i * w, y}, {w, h}));
        }
        sheet.animations[name].duration = baseDuration;
        sheet.animations[name].loop = loop;
    }
    this->animationSheets[sheetName] = std::move(sheet);
}

void ResourceManager::loadFragmentShader(const std::string &shaderName, const std::string &fragPath) {
    sf::Shader shader;
    if (!shader.loadFromFile(fragPath, sf::Shader::Type::Fragment)) {
        std::cerr << "[RESOURCEMANAGER] Error: cannot load shader " << fragPath << std::endl;
        return;
    }
    this->fragShaders[shaderName] = std::move(shader);
}

