#include "AboutScene.h"
#include "HomeScene.h"

Scene * AboutScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = AboutScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool AboutScene::init()
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

	Sprite *about = Sprite::create("about_bg.png");
	about->setAnchorPoint(Vec2(-0.5, -0.5));
	addChild(about);

	auto closeButton = ui::Button::create("close_normal.png", "close_selected.png", "close_disabled.png");
	closeButton->setAnchorPoint(Vec2(-0.5, -0.5));
	about->addChild(closeButton);
	closeButton->setPosition(Vec2(about->getBoundingBox().size.width*0.8, about->getBoundingBox().size.height*0.58));

	closeButton->addTouchEventListener(CC_CALLBACK_2(AboutScene::closeButtonTouchEvent, this));

	return true;
}

void AboutScene::closeButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		//TODO �л�Scene
		//auto scene = HomeScene::createScene();
		//Director::getInstance()->replaceScene(TransitionCrossFade::create(0.5,scene));
		Director::getInstance()->popScene();
	}
}
