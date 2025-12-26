// ResourceLoader.cpp - 简化版本，移除JSON解析
#include "ResourceLoader.h"
#include "AppDelegate.h"
#include <cocos/audio/include/SimpleAudioEngine.h>
//#include <Utils/AnimationHelper.h>

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

    // 加载硬编码的动画配置（不使用JSON文件）
    loadSimpleAnimationConfig();

    return true;
}

void ResourceLoader::loadResourceConfig()
{
    // 菜单资源
    _resourcePaths["menu_background"] = "Images/Backgrounds/menu_bg.png";
    _resourcePaths["start_button_normal"] = "Images/UI/start_btn_normal.png";
    _resourcePaths["start_button_pressed"] = "Images/UI/start_btn_pressed.png";

    _resourcePaths["menu_logo"] = "Images/UI/menu_logo.png";

    // 游戏资源
    _resourcePaths["game_background"] = "Images/Backgrounds/game_bg_1.png";
    //_resourcePaths["grid_cell"] = "Images/UI/grid_cell.png";
    _resourcePaths["card_bar_bg"] = "Images/UI/card_bar_bg.png";

    // 字体
    _resourcePaths["font_main"] = "Fonts/Marker Felt.ttf";
    _resourcePaths["font_score"] = "Fonts/arial.ttf";

    // 音效
    _resourcePaths["sound_button_click"] = "Sounds/SFX/button_click.mp3";
    _resourcePaths["sound_menu_bgm"] = "Sounds/BGM/menu_bgm.mp3";
    _resourcePaths["sound_shoot"] = "Sounds/SFX/shoot.ogg";
    _resourcePaths["sound_sun_produced"] = "Sounds/SFX/sun_produced.mp3";
    _resourcePaths["sound_sun_collected"] = "Sounds/SFX/sun_collected.mp3";
    _resourcePaths["sound_plant_planted"] = "Sounds/SFX/plant_planted.ogg";

    // 创建必要的目录
    auto fileUtils = FileUtils::getInstance();
    std::vector<std::string> searchPaths = fileUtils->getSearchPaths();
    searchPaths.push_back("Resources/");
    fileUtils->setSearchPaths(searchPaths);
}

// 简化的动画配置 - 硬编码在代码中
void ResourceLoader::loadSimpleAnimationConfig()
{
    log("ResourceLoader::loadSimpleAnimationConfig: Starting animation loading");

    // 向日葵空闲动画
    std::vector<std::string> sunflowerIdleFrames = {
        "Images/Plants/Sunflower/sunflower_idle_01.png",
        "Images/Plants/Sunflower/sunflower_idle_02.png",
        "Images/Plants/Sunflower/sunflower_idle_03.png",
        "Images/Plants/Sunflower/sunflower_idle_04.png",
        "Images/Plants/Sunflower/sunflower_idle_05.png",
        "Images/Plants/Sunflower/sunflower_idle_06.png",
        "Images/Plants/Sunflower/sunflower_idle_07.png",
        "Images/Plants/Sunflower/sunflower_idle_08.png",
        "Images/Plants/Sunflower/sunflower_idle_09.png",
        "Images/Plants/Sunflower/sunflower_idle_10.png",
        "Images/Plants/Sunflower/sunflower_idle_11.png",
        "Images/Plants/Sunflower/sunflower_idle_12.png",
        "Images/Plants/Sunflower/sunflower_idle_13.png",
        "Images/Plants/Sunflower/sunflower_idle_14.png",
        "Images/Plants/Sunflower/sunflower_idle_15.png",
        "Images/Plants/Sunflower/sunflower_idle_16.png",
        "Images/Plants/Sunflower/sunflower_idle_17.png",
        "Images/Plants/Sunflower/sunflower_idle_18.png",
    };
    loadAnimationFrames("sunflower_idle", sunflowerIdleFrames, 0.15f);

    // 向日葵生产动画
    std::vector<std::string> sunflowerProduceFrames = {
        "Images/Plants/Sunflower/sunflower_produce_01.png",
        "Images/Plants/Sunflower/sunflower_produce_02.png",
        "Images/Plants/Sunflower/sunflower_produce_03.png",
        "Images/Plants/Sunflower/sunflower_produce_04.png"
    };
    loadAnimationFrames("sunflower_produce", sunflowerProduceFrames, 0.1f);

    // 豌豆射手空闲动画
    std::vector<std::string> peashooterIdleFrames = {
        "Images/Plants/Peashooter/peashooter_idle_01.png",
        "Images/Plants/Peashooter/peashooter_idle_02.png",
        "Images/Plants/Peashooter/peashooter_idle_03.png",
        "Images/Plants/Peashooter/peashooter_idle_04.png",
        "Images/Plants/Peashooter/peashooter_idle_05.png",
        "Images/Plants/Peashooter/peashooter_idle_06.png",
        "Images/Plants/Peashooter/peashooter_idle_07.png",
        "Images/Plants/Peashooter/peashooter_idle_08.png",
        "Images/Plants/Peashooter/peashooter_idle_09.png",
        "Images/Plants/Peashooter/peashooter_idle_10.png",
        "Images/Plants/Peashooter/peashooter_idle_11.png",
        "Images/Plants/Peashooter/peashooter_idle_12.png",
        "Images/Plants/Peashooter/peashooter_idle_13.png"
    };
    loadAnimationFrames("peashooter_idle", peashooterIdleFrames, 0.2f);

    // 豌豆射手攻击动画
    std::vector<std::string> peashooterAttackFrames = {
        "Images/Plants/Peashooter/peashooter_attack_01.png",
        "Images/Plants/Peashooter/peashooter_attack_02.png",
        "Images/Plants/Peashooter/peashooter_attack_03.png",
        "Images/Plants/Peashooter/peashooter_attack_04.png",
        "Images/Plants/Peashooter/peashooter_attack_05.png",
        "Images/Plants/Peashooter/peashooter_attack_06.png"
    };
    loadAnimationFrames("peashooter_attack", peashooterAttackFrames, 0.08f);

    // 豌豆飞行动画
    std::vector<std::string> peaFlyFrames = {
        "Images/Projectiles/Pea/pea_01.png",
        "Images/Projectiles/Pea/pea_02.png",
        "Images/Projectiles/Pea/pea_03.png",
        "Images/Projectiles/Pea/pea_04.png"
    };
    loadAnimationFrames("pea_fly", peaFlyFrames, 0.1f);

    // 豌豆击中动画
    std::vector<std::string> peaHitFrames = {
        "Images/Projectiles/Pea/pea_hit_01.png",
        "Images/Projectiles/Pea/pea_hit_02.png",
        "Images/Projectiles/Pea/pea_hit_03.png",
        "Images/Projectiles/Pea/pea_hit_04.png"
    };
    loadAnimationFrames("pea_hit", peaHitFrames, 0.07f);

    // 坚果墙空闲动画
    std::vector<std::string> wallnutIdleFrames = {
        "Images/Plants/WallNut/wallnut_idle_01.png",
        "Images/Plants/WallNut/wallnut_idle_02.png",
        "Images/Plants/WallNut/wallnut_idle_03.png",
        "Images/Plants/WallNut/wallnut_idle_04.png",
        "Images/Plants/WallNut/wallnut_idle_05.png",
        "Images/Plants/WallNut/wallnut_idle_06.png",
        "Images/Plants/WallNut/wallnut_idle_07.png",
        "Images/Plants/WallNut/wallnut_idle_08.png",
        "Images/Plants/WallNut/wallnut_idle_09.png",
        "Images/Plants/WallNut/wallnut_idle_10.png",
        "Images/Plants/WallNut/wallnut_idle_11.png",
        "Images/Plants/WallNut/wallnut_idle_12.png",
        "Images/Plants/WallNut/wallnut_idle_13.png",
        "Images/Plants/WallNut/wallnut_idle_14.png",
        "Images/Plants/WallNut/wallnut_idle_15.png",
        "Images/Plants/WallNut/wallnut_idle_16.png",
    };
    loadAnimationFrames("wallnut_idle", wallnutIdleFrames, 0.3f);

    // 坚果墙受损动画1
    std::vector<std::string> wallnutDamaged1Frames = {
        "Images/Plants/WallNut/wallnut_damaged1_01.png",
        "Images/Plants/WallNut/wallnut_damaged1_02.png",
        "Images/Plants/WallNut/wallnut_damaged1_03.png"
    };
    loadAnimationFrames("wallnut_damaged1", wallnutDamaged1Frames, 0.2f);

    // 坚果墙受损动画2
    std::vector<std::string> wallnutDamaged2Frames = {
        "Images/Plants/WallNut/wallnut_damaged2_01.png",
        "Images/Plants/WallNut/wallnut_damaged2_02.png",
        "Images/Plants/WallNut/wallnut_damaged2_03.png"
    };
    loadAnimationFrames("wallnut_damaged2", wallnutDamaged2Frames, 0.2f);

    // 阳光漂浮动画
    std::vector<std::string> sunFloatingFrames = {
        "Images/UI/Sun/sun_01.png",
        "Images/UI/Sun/sun_02.png",
        "Images/UI/Sun/sun_03.png",
        "Images/UI/Sun/sun_04.png",
        "Images/UI/Sun/sun_05.png",
        "Images/UI/Sun/sun_06.png",
        "Images/UI/Sun/sun_07.png",
        "Images/UI/Sun/sun_08.png",
        "Images/UI/Sun/sun_09.png",
        "Images/UI/Sun/sun_10.png",
        "Images/UI/Sun/sun_11.png",
        "Images/UI/Sun/sun_12.png",
        "Images/UI/Sun/sun_13.png",
        "Images/UI/Sun/sun_14.png",
        "Images/UI/Sun/sun_15.png",
        "Images/UI/Sun/sun_16.png",
        "Images/UI/Sun/sun_17.png",
        "Images/UI/Sun/sun_18.png",
        "Images/UI/Sun/sun_19.png",
        "Images/UI/Sun/sun_20.png",
        "Images/UI/Sun/sun_21.png",
        "Images/UI/Sun/sun_22.png",
        "Images/UI/Sun/sun_23.png",
        "Images/UI/Sun/sun_24.png",
        "Images/UI/Sun/sun_25.png",
        "Images/UI/Sun/sun_26.png",
        "Images/UI/Sun/sun_27.png",
        "Images/UI/Sun/sun_28.png",
        "Images/UI/Sun/sun_29.png"
    };
    loadAnimationFrames("sun_floating", sunFloatingFrames, 0.2f);

    // 阳光收集动画
    std::vector<std::string> sunCollectFrames = {
        "Images/UI/Sun/sun_collect_01.png",
        "Images/UI/Sun/sun_collect_02.png",
        "Images/UI/Sun/sun_collect_03.png"
    };
    loadAnimationFrames("sun_collect", sunCollectFrames, 0.1f);

    log("Simple animation config loaded with %d animations", (int)_animations.size());
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
            // 加载游戏图片
            std::vector<std::string> gameImages = {
                "game_background",
                "grid_cell"
            };
            loadImages(gameImages);

            // 加载游戏音效
            std::vector<std::string> gameSounds = {
                "sound_shoot",
                "sound_sun_produced",
                "sound_sun_collected",
                "sound_plant_planted"
            };
            loadSoundEffects(gameSounds);

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

SpriteFrame* ResourceLoader::getSpriteFrame(const std::string& name)
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

Animation* ResourceLoader::getAnimation(const std::string& name)
{
    return getCachedAnimation(name);
}

Animation* ResourceLoader::getPNGAnimation(const std::string& name)
{
    return getCachedAnimation(name);
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

Texture2D* ResourceLoader::getTexture(const std::string& textureName)
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

    // 清理动画缓存
    for (auto& pair : _animations)
    {
        CC_SAFE_RELEASE(pair.second);
    }
    _animations.clear();
}

void ResourceLoader::loadAnimationFrames(const std::string& animationName,
    const std::vector<std::string>& framePaths,
    float delayPerFrame)
{
    if (framePaths.empty())
    {
        log("Error: No frames for animation %s", animationName.c_str());
        return;
    }

    // 如果没有AnimationHelper，直接创建
    cocos2d::Vector<cocos2d::SpriteFrame*> spriteFrames;
    auto textureCache = Director::getInstance()->getTextureCache();

    for (const auto& framePath : framePaths)
    {
        // 同步加载纹理
        auto texture = textureCache->addImage(framePath);
        if (texture)
        {
            auto spriteFrame = SpriteFrame::createWithTexture(
                texture,
                Rect(0, 0, texture->getContentSize().width, texture->getContentSize().height)
            );
            if (spriteFrame)
            {
                spriteFrames.pushBack(spriteFrame);
            }
        }
    }

    if (!spriteFrames.empty())
    {
        auto animation = Animation::createWithSpriteFrames(spriteFrames, delayPerFrame);
        cacheAnimation(animationName, animation);
        log("PNG sequence animation created: %s with %d frames",
            animationName.c_str(), (int)spriteFrames.size());
    }
    
}

void ResourceLoader::loadPNGFramesToTextureCache(const std::vector<std::string>& framePaths)
{
    auto textureCache = Director::getInstance()->getTextureCache();

    for (const auto& framePath : framePaths)
    {
        // 异步加载纹理
        if (!textureCache->getTextureForKey(framePath))
        {
            textureCache->addImageAsync(framePath, [framePath](Texture2D* texture) {
                if (texture)
                {
                    log("PNG frame loaded: %s, size: %.0fx%.0f",
                        framePath.c_str(), texture->getContentSize().width, texture->getContentSize().height);
                }
                else
                {
                    log("Error: Failed to load PNG frame: %s", framePath.c_str());
                }
                });
        }
    }
}

Animation* ResourceLoader::getCachedAnimation(const std::string& name)
{
    auto it = _animations.find(name);
    if (it != _animations.end())
    {
        return it->second;
    }
    return nullptr;
}

void ResourceLoader::cacheAnimation(const std::string& name, Animation* animation)
{
    if (animation)
    {
        animation->retain();
        _animations[name] = animation;
    }
}

void ResourceLoader::loadAnimations(const std::vector<std::pair<std::string, std::string>>& animations)
{
    auto spriteFrameCache = SpriteFrameCache::getInstance();

    for (const auto& anim : animations)
    {
        const std::string& plistPath = anim.first;
        const std::string& texturePath = anim.second;

        // 先加载纹理
        auto textureCache = Director::getInstance()->getTextureCache();
        textureCache->addImageAsync(texturePath, [plistPath, spriteFrameCache, this](Texture2D* texture) {
            // 加载plist文件
            spriteFrameCache->addSpriteFramesWithFile(plistPath, texture);
            log("Animation resource loaded: %s", plistPath.c_str());

            // 自动创建动画并缓存
            createAndCacheAnimations(plistPath);
            });
    }
}

void ResourceLoader::createAndCacheAnimations(const std::string& plistPath)
{
    // 根据plist路径自动创建动画
    std::string baseName = plistPath.substr(plistPath.find_last_of("/") + 1);
    baseName = baseName.substr(0, baseName.find_last_of("."));

    auto spriteFrameCache = SpriteFrameCache::getInstance();

    // 尝试创建各种可能的动画
    std::vector<std::string> animNames = { "idle", "attack", "walk", "damage", "death", "fly", "hit" };

    for (const auto& animName : animNames)
    {
        cocos2d::Vector<SpriteFrame*> frames;

        // 尝试加载帧序列
        for (int i = 1; i <= 30; i++) // 假设最多30帧
        {
            char frameName[256];
            sprintf(frameName, "%s_%s_%02d.png", baseName.c_str(), animName.c_str(), i);

            auto frame = spriteFrameCache->getSpriteFrameByName(frameName);
            if (frame)
            {
                frames.pushBack(frame);
            }
            else
            {
                if (i == 1) // 第一帧都没有，说明这个动画不存在
                {
                    break;
                }
            }
        }

        if (!frames.empty())
        {
            float delay = 0.1f; // 默认延迟
            if (animName == "idle") delay = 0.2f;
            else if (animName == "attack") delay = 0.05f;
            else if (animName == "walk") delay = 0.15f;
            else if (animName == "fly") delay = 0.1f;
            else if (animName == "hit") delay = 0.07f;

            auto animation = Animation::createWithSpriteFrames(frames, delay);
            std::string cacheKey = baseName + "_" + animName;
            cacheAnimation(cacheKey, animation);
            log("Animation created and cached: %s", cacheKey.c_str());
        }
    }
}

void ResourceLoader::printCachedAnimations()
{
    log("=== Cached Animations ===");
    log("Total animations: %d", (int)_animations.size());

    for (const auto& pair : _animations)
    {
        log("  Animation: %s", pair.first.c_str());
    }
    log("=========================");
}

bool ResourceLoader::hasAnimation(const std::string& name)
{
    return _animations.find(name) != _animations.end();
}