#include "SettingScene.h"
#include "HomeScene.h"
#include "SimpleAudioEngine.h"
#include "AppMacros.h"
using namespace CocosDenshion;


Scene * SettingScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SettingScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool SettingScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	
	Sprite *sprite = Sprite::create("background.png");
	sprite->setAnchorPoint(Vec2(0, 0));
	addChild(sprite);
	Sprite *setting = Sprite::create("setting_bg.png");
	setting->setAnchorPoint(Vec2(-0.5, -0.5));
	addChild(setting);
	auto closeButton = ui::Button::create("close_normal.png", "close_selected.png", "close_disabled.png");
	closeButton->setAnchorPoint(Vec2(-0.5, -0.5));
	setting->addChild(closeButton);
	closeButton->setPosition(Vec2(setting->getBoundingBox().size.width*0.7, setting->getBoundingBox().size.height*0.64));

	bool musicOn = UserDefault::getInstance()->getBoolForKey(MUSIC_KEY);
	bool souncOn = UserDefault::getInstance()->getBoolForKey(SOUND_KEY);

	ui::CheckBox* music = ui::CheckBox::create(	"music_N.png","music_N.png","music_Y.png","music_Y.png","music_Y.png");
	music->setPosition(Vec2(setting->getBoundingBox().size.width*0.5, setting->getBoundingBox().size.height*0.62));
	music->setSelected(musicOn);
	setting->addChild(music);

	ui::CheckBox* sound = ui::CheckBox::create("sound_N.png", "sound_N.png", "sound_Y.png", "sound_Y.png", "sound_Y.png");
	sound->setPosition(Vec2(setting->getBoundingBox().size.width*0.5, setting->getBoundingBox().size.height*0.38));
	sound->setSelected(souncOn);
	setting->addChild(sound);

	closeButton->addTouchEventListener(CC_CALLBACK_2(SettingScene::closeButtonTouchEvent, this));
	music->addEventListener(CC_CALLBACK_2(SettingScene::selectedMusic, this));
	sound->addEventListener(CC_CALLBACK_2(SettingScene::selectedSound, this));


#if 1

	auto listener = EventListenerKeyboard::create();
	listener->onKeyReleased = CC_CALLBACK_2(SettingScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

#endif

	return true;
}

void SettingScene::closeButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Director::getInstance()->popScene();
	}
}

void SettingScene::selectedMusic(Ref *pSender, ui::CheckBox::EventType type)
{
	auto audioengine = SimpleAudioEngine::getInstance();
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
		audioengine->playBackgroundMusic("music/start.wav",true);
		UserDefault::getInstance()->setBoolForKey(MUSIC_KEY, true);
		break;
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		audioengine->stopBackgroundMusic();
		UserDefault::getInstance()->setBoolForKey(MUSIC_KEY, false);
		break;
	default:
		break;
	}
}

void SettingScene::selectedSound(Ref *pSender, ui::CheckBox::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
		UserDefault::getInstance()->setBoolForKey(SOUND_KEY, true);
		break;
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		UserDefault::getInstance()->setBoolForKey(SOUND_KEY, false);
		break;
	default:
		break;
	}
}

void SettingScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		//Director::getInstance()->popScene();
		Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
#endif
	}
}
