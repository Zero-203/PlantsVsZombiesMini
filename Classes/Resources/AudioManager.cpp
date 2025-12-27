#include "AudioManager.h"

USING_NS_CC;
using namespace CocosDenshion;

AudioManager* AudioManager::_instance = nullptr;

AudioManager* AudioManager::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new AudioManager();
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

AudioManager::AudioManager()
    : _audioEngine(nullptr)
    , _isMuted(false)
    , _isBackgroundMusicPlaying(false)
    , _backgroundMusicVolume(1.0f)
    , _soundEffectsVolume(1.0f)
{
}

AudioManager::~AudioManager()
{
    stopAllSoundEffects();
    stopBackgroundMusic();

    // 清除预加载的音效
    _audioEngine->unloadEffect(_currentBackgroundMusic.c_str());

    _instance = nullptr;
}

bool AudioManager::init()
{
    _audioEngine = SimpleAudioEngine::getInstance();

    // 检查音频引擎是否初始化成功
    if (!_audioEngine)
    {
        log("AudioManager::init - Failed to get audio engine instance!");
        return false;
    }

    log("AudioManager::init - Audio engine initialized");

    auto userDefaults = UserDefault::getInstance();
    _isMuted = userDefaults->getBoolForKey("audio_muted", false);
    _backgroundMusicVolume = userDefaults->getFloatForKey("bgm_volume", 0.5f);
    _soundEffectsVolume = userDefaults->getFloatForKey("sfx_volume", 0.6f);

    log("AudioManager::init - muted: %d, bgm volume: %.2f, sfx volume: %.2f",
        _isMuted, _backgroundMusicVolume, _soundEffectsVolume);

    _audioEngine->setBackgroundMusicVolume(_isMuted ? 0.0f : _backgroundMusicVolume);
    _audioEngine->setEffectsVolume(_isMuted ? 0.0f : _soundEffectsVolume);

    return true;
}

void AudioManager::playBackgroundMusic(const std::string& filePath, bool loop)
{
    if (_isMuted || filePath.empty())
    {
        log("AudioManager::playBackgroundMusic - muted or empty path");
        return;
    }

    // 保存当前播放的音乐路径
    _currentBackgroundMusic = filePath;
    _isBackgroundMusicPlaying = true;

    // 只预加载一次
    if (!filePath.empty())
    {
        _audioEngine->preloadBackgroundMusic(filePath.c_str());
    }

    log("AudioManager::playBackgroundMusic - path: %s, loop: %d", filePath.c_str(), loop);

    // 设置音量后播放
    if (!_isMuted)
    {
        _audioEngine->setBackgroundMusicVolume(_backgroundMusicVolume);
    }

    // 播放背景音乐
    _audioEngine->playBackgroundMusic(filePath.c_str(), loop);
}

void AudioManager::stopBackgroundMusic()
{
    _audioEngine->stopBackgroundMusic();
    _isBackgroundMusicPlaying = false;
    _currentBackgroundMusic.clear();
}

void AudioManager::pauseBackgroundMusic()
{
    if (_isBackgroundMusicPlaying)
    {
        _audioEngine->pauseBackgroundMusic();
    }
}

void AudioManager::resumeBackgroundMusic()
{
    if (_isBackgroundMusicPlaying)
    {
        _audioEngine->resumeBackgroundMusic();
    }
}

void AudioManager::setBackgroundMusicVolume(float volume)
{
    _backgroundMusicVolume = volume;
    if (!_isMuted)
    {
        _audioEngine->setBackgroundMusicVolume(volume);
    }

    // 保存设置
    UserDefault::getInstance()->setFloatForKey("bgm_volume", volume);
}

float AudioManager::getBackgroundMusicVolume() const
{
    return _backgroundMusicVolume;
}

unsigned int AudioManager::playSoundEffect(const std::string& filePath, bool loop, float pitch, float pan, float gain)
{
    if (_isMuted || filePath.empty())
        return 0;

    // 预加载音效
    preloadSoundEffect(filePath);

    // 播放音效
    unsigned int soundId = _audioEngine->playEffect(filePath.c_str(), loop, pitch, pan, gain);

    // 保存音效ID
    if (soundId > 0)
    {
        _soundEffectIds[filePath] = soundId;
    }

    return soundId;
}

void AudioManager::stopSoundEffect(unsigned int soundId)
{
    _audioEngine->stopEffect(soundId);

    // 从映射中移除
    for (auto it = _soundEffectIds.begin(); it != _soundEffectIds.end(); ++it)
    {
        if (it->second == soundId)
        {
            _soundEffectIds.erase(it);
            break;
        }
    }
}

void AudioManager::pauseSoundEffect(unsigned int soundId)
{
    _audioEngine->pauseEffect(soundId);
}

void AudioManager::resumeSoundEffect(unsigned int soundId)
{
    _audioEngine->resumeEffect(soundId);
}

void AudioManager::stopAllSoundEffects()
{
    _audioEngine->stopAllEffects();
    _soundEffectIds.clear();
}

void AudioManager::pauseAllSoundEffects()
{
    _audioEngine->pauseAllEffects();
}

void AudioManager::resumeAllSoundEffects()
{
    _audioEngine->resumeAllEffects();
}

void AudioManager::setSoundEffectsVolume(float volume)
{
    _soundEffectsVolume = volume;
    if (!_isMuted)
    {
        _audioEngine->setEffectsVolume(volume);
    }

    // 保存设置
    UserDefault::getInstance()->setFloatForKey("sfx_volume", volume);
}

float AudioManager::getSoundEffectsVolume() const
{
    return _soundEffectsVolume;
}

void AudioManager::setMuted(bool muted)
{
    _isMuted = muted;

    if (muted)
    {
        _audioEngine->setBackgroundMusicVolume(0.0f);
        _audioEngine->setEffectsVolume(0.0f);
    }
    else
    {
        _audioEngine->setBackgroundMusicVolume(_backgroundMusicVolume);
        _audioEngine->setEffectsVolume(_soundEffectsVolume);
    }

    // 保存设置
    UserDefault::getInstance()->setBoolForKey("audio_muted", muted);
}

void AudioManager::preloadSoundEffect(const std::string& filePath)
{
    if (!filePath.empty())
    {
        _audioEngine->preloadEffect(filePath.c_str());
    }
}

void AudioManager::preloadBackgroundMusic(const std::string& filePath)
{
    if (!filePath.empty())
    {
        _audioEngine->preloadBackgroundMusic(filePath.c_str());
    }
}

void AudioManager::unloadSoundEffect(const std::string& filePath)
{
    if (!filePath.empty())
    {
        _audioEngine->unloadEffect(filePath.c_str());
    }
}

void AudioManager::unloadBackgroundMusic(const std::string& filePath)
{
    // SimpleAudioEngine没有提供卸载背景音乐的方法
    // 我们只需停止播放即可
    if (!filePath.empty() && _currentBackgroundMusic == filePath)
    {
        stopBackgroundMusic();
    }
}
