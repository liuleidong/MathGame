#include "Number.h"
USING_NS_CC;

bool Number::init()
{
	Sprite::init();
	_Num = 0;
	sp = NULL;
	return true;
}

void Number::update(float dt)
{
}

void Number::flickAnimation()
{
	if (sp)
	{
		removeChild(sp, true);
		sp = NULL;
	}
	
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	for (int i = 0; i < 8; i++)
	{
		sprintf(txt, "number/%d-%d.png", _Num,i);
		SpriteFrame * sf = SpriteFrame::create(txt, Rect(0, 0, visibleSize.width / 2560 * 56, visibleSize.height / 1440 * 56));
		allFrames.pushBack(sf);
	}
	Animation * ani = Animation::createWithSpriteFrames(allFrames, 0.2);
	sp = Sprite::create();
	sp->runAction(RepeatForever::create(Animate::create(ani)));
	sp->setAnchorPoint(Vec2(0.5, 0));
	addChild(sp);
}

void Number::setNumber(int num)
{
	_Num = num;
	_Avai = false;
	if (num < 10)
	{
		flickAnimation();
	}
	else
	{
		if (sp)
		{
			removeChild(sp, true);
			sp = NULL;
		}
		sp = Sprite::create("number/prop.png");
		sp->setAnchorPoint(Vec2(0.5, 0));
		addChild(sp);
	}
}

bool Number::getAvai()
{
	return _Avai;
}

void Number::setAvai(bool isAvai)
{
	_Avai = isAvai;
}
