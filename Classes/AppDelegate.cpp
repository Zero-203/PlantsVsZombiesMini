/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"
#include "./UI/MenuScene.h"
#include "./Game/GameManager.h"
#include "./Resources/ResourceLoader.h"
#include "./Resources/AudioManager.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(960, 640);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    // 清理单例实例
    GameManager::getInstance()->release();
    ResourceLoader::getInstance()->release();
    AudioManager::getInstance()->release();
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // 初始化导演类
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("PlantsVsZombiesMini",
            cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("PlantsVsZombiesMini");
#endif
        director->setOpenGLView(glview);
    }

    // 设置FPS显示
    director->setDisplayStats(true);

    // 设置FPS
    director->setAnimationInterval(1.0f / 60);

    // 设置设计分辨率
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
        ResolutionPolicy::SHOW_ALL);

    auto frameSize = glview->getFrameSize();

    // 根据设备屏幕尺寸调整设计分辨率
    if (frameSize.height > mediumResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height,
            largeResolutionSize.width / designResolutionSize.width));
    }
    else if (frameSize.height > smallResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height,
            mediumResolutionSize.width / designResolutionSize.width));
    }
    else
    {
        director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height,
            smallResolutionSize.width / designResolutionSize.width));
    }

    // 初始化游戏管理器
    GameManager::getInstance()->retain();

    // 初始化资源加载器
    ResourceLoader::getInstance()->retain();

    // 初始化音频管理器
    AudioManager::getInstance()->retain();
    AudioManager::getInstance()->init();

    // 创建并运行菜单场景
    auto scene = MenuScene::createScene();
    director->runWithScene(scene);

    return true;
}

void  AppDelegate::applicationDidEnterBackground() 
{
    Director::getInstance()->stopAnimation();

    // 暂停背景音乐
    AudioManager::getInstance()->pauseBackgroundMusic();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    // 恢复背景音乐
    AudioManager::getInstance()->resumeBackgroundMusic();
}