#include "Monkey.h"
#include "AppMacros.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

USING_NS_CC;
#define SPEED_LR 200
#define SPEED_UD 200
#define SPEED_JUMP 10
#define CONSTANT_G -0.8

bool Monkey::init()
{
	Sprite::init();
	
	Size visibleSize = Director::getInstance()->getVisibleSize();

	float space = visibleSize.width / 2560 * 110;
	float firstTreeX = visibleSize.width / 2560 * 522 - space;
	float firstTreeLongY = visibleSize.height / 1440 * 150;
	float firstTreeShortY = visibleSize.height / 1440 * 180;
	for (int i = 0; i < 6; i++)
	{
		firstTreeX = firstTreeX + space;
		if (i % 2 == 0)	//short tree
		{
			_TreePos.push_back(Vec2(firstTreeX, firstTreeShortY));
		}
		else
		{
			_TreePos.push_back(Vec2(firstTreeX, firstTreeLongY));
		}
	}
	firstTreeX = firstTreeX + visibleSize.width / 2560 * 392 - space;
	for (int i = 0; i < 6; i++)
	{
		firstTreeX = firstTreeX + space;
		if (i % 2 == 0)	//short tree
		{
			_TreePos.push_back(Vec2(firstTreeX, firstTreeLongY));
		}
		else
		{
			_TreePos.push_back(Vec2(firstTreeX, firstTreeShortY));
		}
	}


	_HolePos.push_back(Rect(CONVERSION_X(656), CONVERSION_Y(0),CONVERSION_X(112),CONVERSION_Y(85)));
	_HolePos.push_back(Rect(CONVERSION_X(936), CONVERSION_Y(0), CONVERSION_X(108), CONVERSION_Y(85)));
	_HolePos.push_back(Rect(CONVERSION_X(1212), CONVERSION_Y(0), CONVERSION_X(104), CONVERSION_Y(85)));
	_HolePos.push_back(Rect(CONVERSION_X(1484), CONVERSION_Y(0), CONVERSION_X(110), CONVERSION_Y(85)));
	_HolePos.push_back(Rect(CONVERSION_X(1762), CONVERSION_Y(0), CONVERSION_X(106), CONVERSION_Y(85)));

	_LandPos.push_back(Rect(CONVERSION_X(400), CONVERSION_Y(0), CONVERSION_X(256), CONVERSION_Y(85)));
	_LandPos.push_back(Rect(CONVERSION_X(768), CONVERSION_Y(0), CONVERSION_X(168), CONVERSION_Y(85)));
	_LandPos.push_back(Rect(CONVERSION_X(1044), CONVERSION_Y(0), CONVERSION_X(168), CONVERSION_Y(85)));
	_LandPos.push_back(Rect(CONVERSION_X(1316), CONVERSION_Y(0), CONVERSION_X(168), CONVERSION_Y(85)));
	_LandPos.push_back(Rect(CONVERSION_X(1594), CONVERSION_Y(0), CONVERSION_X(168), CONVERSION_Y(85)));
	_LandPos.push_back(Rect(CONVERSION_X(1868), CONVERSION_Y(0), CONVERSION_X(292), CONVERSION_Y(85)));
	_LandPos.push_back(Rect(CONVERSION_X(1160), CONVERSION_Y(356), CONVERSION_X(210), CONVERSION_Y(72)));
	_LandPos.push_back(Rect(CONVERSION_X(1160), CONVERSION_Y(624), CONVERSION_X(210), CONVERSION_Y(72)));

	return true;
}

void Monkey::reset()
{
	isLeft = false;
	isRunL = false;
	isRunR = false;
	isClimbU = false;
	isClimbD = false;
	isJumping = false;
	isUpLand = false;
	g = CONSTANT_G;
	moveStep = 0;
	_Status = Status::LAND;

	_CurTreeIdx = -1;
	_CurLandIdx = 0;
	_DelayCount = 0;
	idle(0);
}

int Monkey::judgeMonkeyPosWithTree()
{
	//判断monkey的X，如果在长树藤附近则切换为爬行状态
	Size size = Director::getInstance()->getVisibleSize();
	int i = 0;
	for (i = 0;i < _TreePos.size();i++)
	{
		if ((getPositionX() > (_TreePos[i].x - size.width / 2560 * 32) && getPositionX() < (_TreePos[i].x + size.width / 2560 * 32)))
		{
			if ((i < 6 && i % 2 != 0) || (i >= 6 && i % 2 == 0))
			{
				break;
			}
		}
	}
	return i;
}

void Monkey::update(float dt)
{
	if ((_Status == Status::LAND && (isRunL || isRunR) && !isUpLand) || (isDropDown && _Status != Status::HOLE))
	{
		for (size_t i = 0;i < _HolePos.size();i++)
		{
			if (getPositionX() > _HolePos[i].getMinX() && getPositionX() < _HolePos[i].getMaxX())
			{
				_Status = Status::HOLE;
				isDropDown = true;
				isRunR = isRunL = isClimbD = false;
				sp->stopAction(run);
				sp->stopAction(climb);
				vy = 0;
				oy = 0;
				cryAnimation();
				scheduleOnce(schedule_selector(Monkey::holeCallBack), 1);
			}
		}
	}
	if (moveStep == 1)
	{
		return;
	}

	if (isDropDown)
	{
		setPositionY(getPositionY() + vy);
		vy += g;

		if (getPositionY() < oy)
		{
			setPositionY(oy);
			isDropDown = false;
			if (oy != 0)
			{
				if (!isRunL && !isRunR)
					idle(0);
				else
					runAnimation();
				_Status = Status::LAND;
			}
			playEffect("sound/dang.wav");
		}
	}
	else if(isRunL)
	{
		if (_Status == Status::LAND)
		{
			if (isUpLand)
			{
				if (getPositionX() <= _LandPos[6].getMinX())
				{
					_CurTreeIdx = 5;
					setPositionX(_TreePos[5].x);
					_Status = Status::TREE;
					idle(1);
					isUpLand = false;
					isRunL = false;
					isJumping = false;
					return;
				}
			}
			runLeft(dt);
		}
		
	}
	else if (isRunR)
	{
		if (_Status == Status::LAND)
		{
			if (isUpLand)
			{
				if (getPositionX() >= _LandPos[6].getMaxX())
				{
					_CurTreeIdx = 6;
					setPositionX(_TreePos[6].x);
					_Status = Status::TREE;
					idle(1);
					isUpLand = false;
					isRunR = false;
					isJumping = false;
					return;
				}
			}
			runRight(dt);
		}
		
	}
	else if (isClimbU)
	{
		_DelayCount++;
		if (_DelayCount >= 18)
		{
			_DelayCount = 0;
			playEffect("sound/di.wav");
		}
		
		climbUp(dt);
	}
	else if (isClimbD)
	{
		_DelayCount++;
		if (_DelayCount >= 12)
		{
			_DelayCount = 0;
			playEffect("sound/di.wav");
		}
		climbDown(dt);
		if (_CurTreeIdx >= 0 && _CurTreeIdx < _TreePos.size())
		{
			if (getPositionY() <= _TreePos[_CurTreeIdx].y)
			{
				//切换为LAND状态
				_Status = Status::LAND;
				isClimbD = false;
				isDropDown = true;
				oy = _LandPos[0].getMaxY();
				vy = 0;
				idle(0);
				setLandIdx();
			}
		}
	}
	if (isJumping && !isDropDown)
	{
		setPositionY(getPositionY() + vy);
		vy += g;

		//判断monkey的X，如果在长树藤附近则切换为爬行状态
		int i = judgeMonkeyPosWithTree();
		if (i >= 0 && i < _TreePos.size())
		{
			_CurTreeIdx = i;
			setPosition(_TreePos[i]);
			isJumping = false;
			isClimbU = true;
			isRunL = isRunR = false;
			_DelayCount = 0;
			_Status = Status::TREE;
			idle(1);
			climbAnimation();
			return;
		}
		if (getPositionY() < oy)
		{
			setPositionY(oy);
			isJumping = false;
			playEffect("sound/dang.wav");
			if (isRunL || isRunR)
			{
				runAnimation();
				sp->setScaleX(isLeft ? -1 : 1);
			}
			else
			{
				idle(0);
			}
		}
	}
}
void  Monkey::idle(int type)
{
	this->removeAllChildren();
	Sprite * s;
	if (type == 0)
	{
		s = Sprite::create("monkey/idle.png");
	}
	else if (type == 1)
	{
		s = Sprite::create("monkey/climb/climb-1.png");
	}
	else if (type == 2)
	{
		s = Sprite::create("monkey/jump/jump.png");
	}
	else if (type == 3)
	{
		s = Sprite::create("monkey/move/move.png");
	}
	
	s->setAnchorPoint(Vec2(0.5,0));
	this->addChild(s);
	sp = s;
	sp->setScaleX(isLeft ? -1 : 1);
}

void Monkey::jump()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	if (_Status != Status::LAND)
	{
		return;
	}
	if (!isDropDown && !isJumping)
	{
		idle(2);
		oy = getPositionY();
		vy = CONVERSION_Y(SPEED_JUMP);
		g = CONVERSION_Y(CONSTANT_G);
		isJumping = true;
	}
}

void Monkey::doUpPressed()
{
	stopAnimationByStatus(Direction::UP);
	if (!isClimbU && _Status == Status::TREE)
	{
		isClimbU = true;
		_DelayCount = 0;
		if (!isJumping)
		{
			climbAnimation();
		}
	}
}

void Monkey::doUpReleased()
{
	stopAnimationByStatus(Direction::NONE);
}

void Monkey::doDownPressed()
{
	stopAnimationByStatus(Direction::DOWN);
	if (!isClimbD && _Status == Status::TREE)
	{
		isClimbD = true;
		if (!isJumping)
		{
			climbAnimation();
		}
	}
}

void Monkey::doDownReleased()
{
	if (!isClimbD)
	{
		return;
	}
	stopAnimationByStatus(Direction::NONE);
}

void Monkey::doLeftPressed()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	stopAnimationByStatus(Direction::LEFT);
	if (!isRunL)
	{
		if (!isJumping)
		{
			if (_Status == Status::LAND)
			{
				isRunL = true;
				isLeft = true;
				if(!isDropDown)
					runAnimation();
			}
			else if (_Status == Status::TREE && moveStep == 0)
			{
				if (_CurTreeIdx < 1)
				{
					return;
				}
				//第7根在一定高度时，不能向左。低于一定高度如果向左，应该掉到对应的陆地上
				if (_CurTreeIdx == 7 && getPositionY() > visibleSize.height / 1440 * 830)
				{
					return;
				}
				else if (_CurTreeIdx == 6 && getPositionY() <= visibleSize.height / 1440 * 830)
				{
					if (getPositionY() < _LandPos[7].getMaxY() && getPositionY() > (_LandPos[7].getMinY() - CONVERSION_Y(110)))
					{
						return;
					}
					else if (getPositionY() < _LandPos[6].getMaxY() && getPositionY() > (_LandPos[6].getMinY() - CONVERSION_Y(110)))
					{
						return;
					}
					else if (getPositionY() >= _LandPos[7].getMaxY())
					{
						_CurLandIdx = 7;
						setPositionX(_LandPos[7].getMaxX());
						idle(3);
						oy = _LandPos[7].getMaxY();
						isDropDown = true;
						isUpLand = true;
						vy = 0;
					}
					else if (getPositionY() >= _LandPos[6].getMaxY())
					{
						_CurLandIdx = 6;
						setPositionX(_LandPos[6].getMaxX());
						idle(3);
						oy = _LandPos[6].getMaxY();
						isDropDown = true;
						isUpLand = true;
						vy = 0;
					}
				}
				else
				{
					isRunL = false;
					isLeft = true;
					//1.设置精灵X坐标为两个树藤之间
					float leftX = _TreePos[_CurTreeIdx - 1].x;
					float curX = _TreePos[_CurTreeIdx].x;
					float midX = leftX + (curX - leftX) / 2;
					setPositionX(midX);
					//2.设置动画为切换状态
					idle(3);
					moveStep = 1;
					scheduleOnce(schedule_selector(Monkey::moveLeft), 0.1);
				}

			}
			sp->setScaleX(isLeft ? -1 : 1);
		}
	}
}

void Monkey::doLeftReleased()
{
	stopAnimationByStatus(Direction::NONE);
}

void Monkey::doRightPressed()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	stopAnimationByStatus(Direction::RIGHT);
	if (!isRunR)
	{
		if (!isJumping)
		{
			if (_Status == Status::LAND)
			{
				isRunR = true;
				isLeft = false;
				if (!isDropDown)
					runAnimation();
			}
			else if (_Status == Status::TREE && moveStep == 0)
			{
				if (_CurTreeIdx == _TreePos.size() - 1)
				{
					return;
				}
				if (_CurTreeIdx == 4 && getPositionY() > visibleSize.height / 1440 * 830)
				{
					return;
				}
				else if (_CurTreeIdx == 5 && getPositionY() <= visibleSize.height / 1440 * 830)
				{
					if (getPositionY() < _LandPos[7].getMaxY() && getPositionY() > (_LandPos[7].getMinY() - CONVERSION_Y(110)))
					{
						return;
					}
					else if (getPositionY() < _LandPos[6].getMaxY() && getPositionY() > (_LandPos[6].getMinY() - CONVERSION_Y(110)))
					{
						return;
					}
					else if (getPositionY() >= _LandPos[7].getMaxY())
					{
						setPositionX(_LandPos[7].getMinX());
						idle(3);
						oy = _LandPos[7].getMaxY();
						isDropDown = true;
						isUpLand = true;
						vy = 0;
					}
					else if (getPositionY() >= _LandPos[6].getMaxY())
					{
						setPositionX(_LandPos[6].getMinX());
						idle(3);
						oy = _LandPos[6].getMaxY();
						isDropDown = true;
						isUpLand = true;
						vy = 0;
					}
				}
				else
				{
					isRunR = false;
					isLeft = false;
					float rightX = _TreePos[_CurTreeIdx + 1].x;
					float curX = _TreePos[_CurTreeIdx].x;
					float midX = curX + (rightX - curX) / 2;
					setPositionX(midX);
					idle(3);
					moveStep = 1;
					scheduleOnce(schedule_selector(Monkey::moveRight), 0.1);
				}
			}
			sp->setScaleX(isLeft ? -1 : 1);
		}
	}
}

void Monkey::doRightReleased()
{
	stopAnimationByStatus(Direction::NONE);
}

void Monkey::setLandIdx()
{
	if (_CurTreeIdx == 2 || _CurTreeIdx == 4 || _CurTreeIdx == 7 || _CurTreeIdx == 9)
	{
		_CurLandIdx = -1;
	}
	else
	{
		_CurLandIdx = _CurTreeIdx / 2;
	}
}

void Monkey::holeCallBack(float dt)
{
	reset();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setPosition(visibleSize.width / 2560 * 400, visibleSize.height / 1440 * 85);
}

void Monkey::playEffect(const char* pszFilePath)
{
	if (_IsSoundOn)
	{
		SimpleAudioEngine::getInstance()->playEffect(pszFilePath);
	}
}

void Monkey::stopEffect()
{
	if (_RunSoundID != 0)
	{
		SimpleAudioEngine::getInstance()->stopEffect(_RunSoundID);
	}
}

void Monkey::runLeft(float dt)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	float d = dt * CONVERSION_X(SPEED_LR);
	float p = getPositionX() - d;
	float limitX = CONVERSION_X(400);
	p = (p < limitX) ? limitX : p;
	setPositionX(p);
	_DelayCount++;
	if (_DelayCount >= 15)
	{
		_DelayCount = 0;
		playEffect("sound/walk.wav");
	}
	
}
void Monkey::runRight(float dt)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	float d = dt * CONVERSION_X(SPEED_LR);
	float p = getPositionX() + d;
	float limitX = CONVERSION_X(2160);
	p = (p > limitX) ? limitX : p;
	setPositionX(p);
	_DelayCount++;
	if (_DelayCount >= 15)
	{
		_DelayCount = 0;
		playEffect("sound/walk.wav");
	}
}

void Monkey::moveLeft(float dt)
{
	if (_CurTreeIdx == 0)
	{
		return;
	}
	playEffect("sound/di.wav");
	float leftX = _TreePos[_CurTreeIdx - 1].x;
	//3.设置精灵X坐标为左侧树藤
	_CurTreeIdx -= 1;
	setPositionX(leftX);
	//4.设置动画为爬行
	idle(1);
	_Status = Status::TREE;
	isRunL = false;
	moveStep = 0;
}

void Monkey::moveRight(float dt)
{
	if (_CurTreeIdx == _TreePos.size() - 1)
	{
		return;
	}
	playEffect("sound/di.wav");
	float rightX = _TreePos[_CurTreeIdx + 1].x;
	_CurTreeIdx += 1;
	setPositionX(rightX);
	idle(1);
	_Status = Status::TREE;
	isRunR = false;
	moveStep = 0;
}

void Monkey::climbUp(float dt)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	float d = dt * CONVERSION_Y(SPEED_UD);
	float p = getPositionY() + d;
	float temp = (_CurTreeIdx == 5 || _CurTreeIdx == 6) ? 830 : 890;
	float limitY = CONVERSION_Y(temp);
	
	p = (p > limitY) ? limitY : p;
	setPositionY(p);
}

void Monkey::climbDown(float dt)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	float d = dt * CONVERSION_Y(SPEED_UD*2);
	float p = getPositionY() - d;
	float limitY = CONVERSION_Y(85);
	p = (p < limitY) ? limitY : p;
	setPositionY(p);
}

void Monkey::runAnimation()
{
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	Size visibleSize = Director::getInstance()->getVisibleSize();
	for (int i = 1; i < 6; i++)
	{
		sprintf(txt, "monkey/run/run_%d.png", i);
		SpriteFrame * sf = SpriteFrame::create(txt,Rect(0,0, visibleSize.width / 2560 * 104, visibleSize.width / 2560 * 110));
		allFrames.pushBack(sf);
	}
	Animation * ani = Animation::createWithSpriteFrames(allFrames,0.05);
	run = sp->runAction(RepeatForever::create(Animate::create(ani)));
}

void Monkey::climbAnimation()
{
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	Size visibleSize = Director::getInstance()->getVisibleSize();
	for (int i = 1; i < 3; i++)
	{
		sprintf(txt, "monkey/climb/climb-%d.png", i);
		SpriteFrame * sf = SpriteFrame::create(txt, Rect(0, 0, visibleSize.width / 2560 * 101, visibleSize.width / 2560 * 110));
		allFrames.pushBack(sf);
	}
	Animation * ani = Animation::createWithSpriteFrames(allFrames, 0.05);
	climb = sp->runAction(RepeatForever::create(Animate::create(ani)));
}


void Monkey::cryAnimation()
{
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	Size visibleSize = Director::getInstance()->getVisibleSize();
	for (int i = 1; i < 3; i++)
	{
		sprintf(txt, "monkey/cry/cry_%d.png", i);
		SpriteFrame * sf = SpriteFrame::create(txt, Rect(0, 0, visibleSize.width / 2560 * 89, visibleSize.width / 2560 * 110));
		allFrames.pushBack(sf);
	}
	Animation * ani = Animation::createWithSpriteFrames(allFrames, 0.05);
	cry = sp->runAction(RepeatForever::create(Animate::create(ani)));
}

void Monkey::stopAnimationByStatus(Direction d)
{
	if (isRunL && d != Direction::LEFT)
	{
		isRunL = false;
		if (!isJumping)
		{
			if (_Status == Status::LAND)
			{
				sp->stopAction(run);
				idle(0);
			}
		}
	}
	else if (isRunR && d != Direction::RIGHT)
	{
		isRunR = false;
		if (!isJumping)
		{
			if (_Status == Status::LAND)
			{
				sp->stopAction(run);
				idle(0);
			}
		}
	}
	else if (isClimbU && d != Direction::UP)
	{
		isClimbU = false;
		if (!isJumping)
		{
			sp->stopAction(climb);
			idle(1);
		}
	}
	else if (isClimbD && d != Direction::DOWN)
	{
		isClimbD = false;
		if (!isJumping)
		{
			sp->stopAction(climb);
			idle(1);
		}
	}
}