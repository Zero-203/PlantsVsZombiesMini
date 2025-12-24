// ResourceLoader.h - 简化版本，移除JSON相关声明
#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include "cocos2d.h"
#include <string>
#include <map>
#include <vector>

class ResourceLoader : public cocos2d::Ref
{
public:
    // 单例访问
    static ResourceLoader* getInstance();

    // 调试：打印所有缓存的动画
    void printCachedAnimations();

    // 检查动画是否存在
    bool hasAnimation(const std::string& name);

    // 资源类型
    enum class ResourceType
    {
        IMAGE,
        PLIST,
        FONT,
        SOUND_EFFECT,
        BACKGROUND_MUSIC
    };

    // 加载阶段
    enum class LoadingPhase
    {
        MENU_RESOURCES,
        GAME_RESOURCES,
        ALL_RESOURCES
    };

    // 预加载资源
    void preloadResources(LoadingPhase phase);

    // 获取精灵帧
    cocos2d::SpriteFrame* getSpriteFrame(const std::string& name);

    // 获取动画
    cocos2d::Animation* getAnimation(const std::string& name);

    // 获取PNG序列帧动画
    cocos2d::Animation* getPNGAnimation(const std::string& name);

    // 获取字体路径
    std::string getFontPath(const std::string& fontName);

    // 获取音效路径
    std::string getSoundEffectPath(const std::string& soundName);

    // 获取背景音乐路径
    std::string getBackgroundMusicPath(const std::string& musicName);

    // 获取纹理
    cocos2d::Texture2D* getTexture(const std::string& textureName);

    // 清理未使用资源
    void clearUnusedResources();

    // 加载PNG序列帧动画
    void loadAnimationFrames(const std::string& animationName,
        const std::vector<std::string>& framePaths,
        float delayPerFrame = 0.1f);

    // 加载PNG序列帧到纹理缓存
    void loadPNGFramesToTextureCache(const std::vector<std::string>& framePaths);

    // 获取缓存的动画
    cocos2d::Animation* getCachedAnimation(const std::string& name);

    // 缓存动画
    void cacheAnimation(const std::string& name, cocos2d::Animation* animation);

    // 加载动画资源
    void loadAnimations(const std::vector<std::pair<std::string, std::string>>& animations);

private:
    ResourceLoader();
    virtual ~ResourceLoader();

    // 禁止拷贝
    ResourceLoader(const ResourceLoader&) = delete;
    ResourceLoader& operator=(const ResourceLoader&) = delete;

    // 初始化
    bool init();

    // 加载资源配置
    void loadResourceConfig();

    // 加载简化的动画配置（硬编码）
    void loadSimpleAnimationConfig();

    // 加载图片资源
    void loadImages(const std::vector<std::string>& imageFiles);

    // 加载精灵表
    void loadSpriteSheets(const std::vector<std::pair<std::string, std::string>>& spriteSheets);

    // 加载字体
    void loadFonts(const std::vector<std::string>& fontFiles);

    // 加载音效
    void loadSoundEffects(const std::vector<std::string>& soundFiles);

    // 加载背景音乐
    void loadBackgroundMusic(const std::vector<std::string>& musicFiles);

    // 根据plist路径自动创建动画并缓存
    void createAndCacheAnimations(const std::string& plistPath);

private:
    static ResourceLoader* _instance;

    // 资源路径映射
    std::map<std::string, std::string> _resourcePaths;

    // 已加载的精灵帧缓存
    std::map<std::string, cocos2d::SpriteFrame*> _spriteFrames;

    // 已加载的动画缓存
    std::map<std::string, cocos2d::Animation*> _animations;

    // 资源加载状态
    bool _isMenuResourcesLoaded;
    bool _isGameResourcesLoaded;
};

#endif // RESOURCE_LOADER_H