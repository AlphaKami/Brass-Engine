#ifndef IRONBRASS_RESOURCEMANAGER_H
#define IRONBRASS_RESOURCEMANAGER_H

#include <filesystem>

#include <map>
#include <string>

#include "Animation/Animation.h"

#include "SFML/Graphics/Font.hpp"


class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Loaders

    // Getters
    sf::Font* getFont(const std::string& fontName) { return this->fonts[fontName]; }

    const sf::Texture& getTextureSheet(const std::string& sheetName) const { return this->textureSheets.at(sheetName).texture; }
    sf::IntRect getTextureRect(const std::string& sheetName, const std::string& tileName) const { return this->textureSheets.at(sheetName).rects.at(tileName); }

    sf::Shader& getFragShader(const std::string& shaderName) { return this->fragShaders.at(shaderName); }

    const sf::Texture& getAnimatedTextureSheet(const std::string& sheetName) const { return this->animationSheets.at(sheetName).texture; }
    const Animation& getAnimation(const std::string& sheetName, const std::string& animationName) const { return this->animationSheets.at(sheetName).animations.at(animationName); }

private:
    std::map<std::string, sf::Font*> fonts;

    struct TextureSheet {
        sf::Texture texture;
        std::unordered_map<std::string, sf::IntRect> rects;
    };
    std::unordered_map<std::string, TextureSheet> textureSheets;

    struct AnimationSheet {
        sf::Texture texture;
        std::unordered_map<std::string, Animation> animations;
    };
    std::unordered_map<std::string, AnimationSheet> animationSheets;

    std::unordered_map<std::string, sf::Shader> fragShaders;

    void loadFonts(const std::string& folderPath);
    void loadTextureSheet(const std::string& sheetName, const std::string& texturePath, const std::string& atlasDataPath);
    void loadAnimatedTextures(const std::string& sheetName, const std::string& texturePath, const std::string& atlasDataPath);
    void loadFragmentShader(const std::string& shaderName, const std::string& fragPath);
};

#endif //IRONBRASS_RESOURCEMANAGER_H
