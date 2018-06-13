#include "HomeScene.h"
#include "GameScene.h" 
#include "SettingScene.h" 
#include "AboutScene.h" 
#include "AppMacros.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

Scene * HomeScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HomeScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool HomeScene::init()
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

#if 1
	auto startButton = ui::Button::create("start_normal.png", "start_selected.png", "start_disabled.png");
	auto settingButton = ui::Button::create("setting_normal.png", "setting_selected.png", "setting_disabled.png");
	auto aboutButton = ui::Button::create("about_normal.png", "about_selected.png", "about_disabled.png");

	auto margin = (visibleSize.width - startButton->getContentSize().width * 3) / 4;

	auto hbox = ui::HBox::create();

	auto lp1 = ui::LinearLayoutParameter::create();
	startButton->setLayoutParameter(lp1);
	lp1->setGravity(ui::LinearGravity::CENTER_HORIZONTAL);
	lp1->setMargin(ui::Margin(margin, 0, 0, 0));
	hbox->addChild(startButton);

	auto lp2 = ui::LinearLayoutParameter::create();
	settingButton->setLayoutParameter(lp2);
	lp2->setGravity(ui::LinearGravity::CENTER_HORIZONTAL);
	lp2->setMargin(ui::Margin(margin, 0, 0, 0));
	hbox->addChild(settingButton);

	auto lp3 = ui::LinearLayoutParameter::create();
	aboutButton->setLayoutParameter(lp3);
	lp3->setGravity(ui::LinearGravity::CENTER_HORIZONTAL);
	lp3->setMargin(ui::Margin(margin, 0, 0, 0));
	hbox->addChild(aboutButton);
	
	hbox->setPosition(Vec2(0, startButton->getContentSize().height));
	addChild(hbox);

	startButton->addTouchEventListener(CC_CALLBACK_2(HomeScene::startButtonTouchEvent, this));
	settingButton->addTouchEventListener(CC_CALLBACK_2(HomeScene::settingButtonTouchEvent, this));
	aboutButton->addTouchEventListener(CC_CALLBACK_2(HomeScene::aboutButtonTouchEvent, this));
#endif 

	//背景音乐
	auto audioengine = SimpleAudioEngine::getInstance();
	audioengine->preloadBackgroundMusic("music/start.wav");
	//音效
	audioengine->preloadEffect("sound/run.mp3");
	audioengine->preloadEffect("sound/di.wav");
	audioengine->preloadEffect("sound/dang.wav");

	if (UserDefault::getInstance()->getBoolForKey(MUSIC_KEY))
	{
		audioengine->playBackgroundMusic("music/start.wav", true);
	}

#if 1

	auto listener = EventListenerKeyboard::create();
	listener->onKeyReleased = CC_CALLBACK_2(HomeScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

#endif

	return true;
}

void HomeScene::startButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		auto scene = GameScene::createScene();
		Director::getInstance()->replaceScene(TransitionCrossFade::create(0.1,scene));
	}
}

void HomeScene::settingButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		auto scene = SettingScene::createScene();
        Director::getInstance()->pushScene(scene);
	}
}

void HomeScene::aboutButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		auto scene = AboutScene::createScene();
        Director::getInstance()->pushScene(scene);
	}
}

void HomeScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
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
