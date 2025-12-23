#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include <string>
#include <map>

class AudioManager : public cocos2d::Ref
{
public:
    // 单例访问点
    static AudioManager* getInstance();

    // 音效类型枚举
    enum class SoundType
    {
        EFFECT,
        BACKGROUND_MUSIC
    };

    // 初始化
    bool init();

    // 背景音乐控制
    void playBackgroundMusic(const std::string& filePath, bool loop = true);
    void stopBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();
    void setBackgroundMusicVolume(float volume);
    float getBackgroundMusicVolume() const;

    // 音效控制
    unsigned int playSoundEffect(const std::string& filePath, bool loop = false, float pitch = 1.0f, float pan = 0.0f, float gain = 1.0f);
    void stopSoundEffect(unsigned int soundId);
    void pauseSoundEffect(unsigned int soundId);
    void resumeSoundEffect(unsigned int soundId);
    void stopAllSoundEffects();
    void pauseAllSoundEffects();
    void resumeAllSoundEffects();
    void setSoundEffectsVolume(float volume);
    float getSoundEffectsVolume() const;

    // 全局音频控制
    void setMuted(bool muted);
    bool isMuted() const { return _isMuted; }

    // 预加载音效
    void preloadSoundEffect(const std::string& filePath);
    void preloadBackgroundMusic(const std::string& filePath);

    // 卸载音效
    void unloadSoundEffect(const std::string& filePath);
    void unloadBackgroundMusic(const std::string& filePath);

private:
    AudioManager();
    virtual ~AudioManager();

    // 禁止拷贝
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    static AudioManager* _instance;

    // 音频引擎
    CocosDenshion::SimpleAudioEngine* _audioEngine;

    // 音频状态
    bool _isMuted;
    bool _isBackgroundMusicPlaying;
    float _backgroundMusicVolume;
    float _soundEffectsVolume;

    // 当前播放的背景音乐文件
    std::string _currentBackgroundMusic;

    // 音效ID映射
    std::map<std::string, unsigned int> _soundEffectIds;
};

#endif // AUDIO_MANAGER_H
