#include "ResourceLoader.h"
#include "AppDelegate.h"
#include <cocos/audio/include/SimpleAudioEngine.h>

USING_NS_CC;

ResourceLoader* ResourceLoader::_instance = nullptr;

ResourceLoader* ResourceLoader::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new ResourceLoader();
        if (_instance && _instance->init())
        {
            _instance->autorelease();
        }
        else
        {
            delete _instance;
            _instance = nullptr;
        }
    }
    return _instance;
}

ResourceLoader::ResourceLoader()
    : _isMenuResourcesLoaded(false)
    , _isGameResourcesLoaded(false)
{
}

ResourceLoader::~ResourceLoader()
{
    clearUnusedResources();
    _instance = nullptr;
}

bool ResourceLoader::init()
{
    loadResourceConfig();
    return true;
}

void ResourceLoader::loadResourceConfig()
{
    // 这里可以读取JSON或XML配置文件
    // 为简化，我们硬编码一些资源路径

    // 菜单资源
    _resourcePaths["menu_background"] = "Images/Backgrounds/menu_bg.png";
    _resourcePaths["start_button_normal"] = "Images/UI/start_btn_normal.png";
    _resourcePaths["start_button_pressed"] = "Images/UI/start_btn_pressed.png";
    _resourcePaths["menu_logo"] = "Images/UI/menu_logo.png";

    // 游戏资源占位符
    _resourcePaths["game_background"] = "Images/Backgrounds/game_bg.png";

    // 字体
    _resourcePaths["font_main"] = "Fonts/Marker Felt.ttf";
    _resourcePaths["font_score"] = "Fonts/arial.ttf";

    // 音效
    _resourcePaths["sound_button_click"] = "Sounds/SFX/button_click.mp3";
    _resourcePaths["sound_menu_bgm"] = "Sounds/BGM/menu_bgm.mp3";

    // 创建必要的目录（在真实项目中，这些目录应该已经存在）
    auto fileUtils = FileUtils::getInstance();
    std::vector<std::string> searchPaths = fileUtils->getSearchPaths();
    searchPaths.push_back("Resources/");
    fileUtils->setSearchPaths(searchPaths);
}

void ResourceLoader::preloadResources(LoadingPhase phase)
{
    switch (phase)
    {
    case LoadingPhase::MENU_RESOURCES:
        if (!_isMenuResourcesLoaded)
        {
            // 加载菜单图片
            std::vector<std::string> menuImages = {
                "menu_background",
                "start_button_normal",
                "start_button_pressed",
                "menu_logo"
            };
            loadImages(menuImages);

            // 加载字体
            std::vector<std::string> fonts = {
                "font_main",
                "font_score"
            };
            loadFonts(fonts);

            // 加载音效
            std::vector<std::string> sounds = {
                "sound_button_click",
                "sound_menu_bgm"
            };
            loadSoundEffects(sounds);

            _isMenuResourcesLoaded = true;
        }
        break;

    case LoadingPhase::GAME_RESOURCES:
        if (!_isGameResourcesLoaded)
        {
            // 游戏资源将在后续阶段实现
            _isGameResourcesLoaded = true;
        }
        break;

    case LoadingPhase::ALL_RESOURCES:
        preloadResources(LoadingPhase::MENU_RESOURCES);
        preloadResources(LoadingPhase::GAME_RESOURCES);
        break;
    }
}

void ResourceLoader::loadImages(const std::vector<std::string>& imageFiles)
{
    auto textureCache = Director::getInstance()->getTextureCache();

    for (const auto& imageName : imageFiles)
    {
        auto it = _resourcePaths.find(imageName);
        if (it != _resourcePaths.end())
        {
            textureCache->addImageAsync(it->second, [](Texture2D* texture) {
                // 纹理加载完成回调
                log("Texture loaded: %s", texture->getPath().c_str());
                });
        }
    }
}

void ResourceLoader::loadSpriteSheets(const std::vector<std::pair<std::string, std::string>>& spriteSheets)
{
    auto spriteFrameCache = SpriteFrameCache::getInstance();

    for (const auto& sheet : spriteSheets)
    {
        const std::string& plistPath = sheet.first;
        const std::string& texturePath = sheet.second;

        // 先加载纹理
        auto textureCache = Director::getInstance()->getTextureCache();
        textureCache->addImageAsync(texturePath, [plistPath, spriteFrameCache](Texture2D* texture) {
            // 然后加载plist文件
            spriteFrameCache->addSpriteFramesWithFile(plistPath, texture);
            log("Sprite sheet loaded: %s", plistPath.c_str());
            });
    }
}

void ResourceLoader::loadFonts(const std::vector<std::string>& fontFiles)
{
    // Cocos2d-x会自动加载系统字体
    // 这里主要是确保字体文件存在
    for (const auto& fontName : fontFiles)
    {
        auto it = _resourcePaths.find(fontName);
        if (it != _resourcePaths.end())
        {
            // 检查字体文件是否存在
            if (FileUtils::getInstance()->isFileExist(it->second))
            {
                log("Font found: %s", it->second.c_str());
            }
            else
            {
                log("Warning: Font not found: %s", it->second.c_str());
            }
        }
    }
}

void ResourceLoader::loadSoundEffects(const std::vector<std::string>& soundFiles)
{
    auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();

    for (const auto& soundName : soundFiles)
    {
        auto it = _resourcePaths.find(soundName);
        if (it != _resourcePaths.end())
        {
            // 预加载音效
            audioEngine->preloadEffect(it->second.c_str());
            log("Sound effect preloaded: %s", it->second.c_str());
        }
    }
}

void ResourceLoader::loadBackgroundMusic(const std::vector<std::string>& musicFiles)
{
    auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();

    for (const auto& musicName : musicFiles)
    {
        auto it = _resourcePaths.find(musicName);
        if (it != _resourcePaths.end())
        {
            // 预加载背景音乐
            audioEngine->preloadBackgroundMusic(it->second.c_str());
            log("Background music preloaded: %s", it->second.c_str());
        }
    }
}

cocos2d::SpriteFrame* ResourceLoader::getSpriteFrame(const std::string& name)
{
    auto it = _spriteFrames.find(name);
    if (it != _spriteFrames.end())
    {
        return it->second;
    }

    // 如果不在缓存中，尝试从SpriteFrameCache获取
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    auto spriteFrame = spriteFrameCache->getSpriteFrameByName(name);
    if (spriteFrame)
    {
        _spriteFrames[name] = spriteFrame;
    }

    return spriteFrame;
}

cocos2d::Animation* ResourceLoader::getAnimation(const std::string& name)
{
    auto it = _animations.find(name);
    if (it != _animations.end())
    {
        return it->second;
    }
    return nullptr;
}

std::string ResourceLoader::getFontPath(const std::string& fontName)
{
    auto it = _resourcePaths.find(fontName);
    if (it != _resourcePaths.end())
    {
        return it->second;
    }
    return "";
}

std::string ResourceLoader::getSoundEffectPath(const std::string& soundName)
{
    auto it = _resourcePaths.find(soundName);
    if (it != _resourcePaths.end())
    {
        return it->second;
    }
    return "";
}

std::string ResourceLoader::getBackgroundMusicPath(const std::string& musicName)
{
    auto it = _resourcePaths.find(musicName);
    if (it != _resourcePaths.end())
    {
        return it->second;
    }
    return "";
}

cocos2d::Texture2D* ResourceLoader::getTexture(const std::string& textureName)
{
    auto it = _resourcePaths.find(textureName);
    if (it != _resourcePaths.end())
    {
        return Director::getInstance()->getTextureCache()->getTextureForKey(it->second);
    }
    return nullptr;
}

void ResourceLoader::clearUnusedResources()
{
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
    SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();

    _spriteFrames.clear();
    _animations.clear();
}
