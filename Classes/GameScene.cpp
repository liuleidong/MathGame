#include "GameScene.h"
#include "AppMacros.h"
#include "HomeScene.h"
#include "SettingScene.h"
#include "SimpleAudioEngine.h"
#include "RandUtil.h"

#include <sstream>

using namespace CocosDenshion;

Scene * GameScene::createScene()
{
	Scene *scene = Scene::create();

	Layer *layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	Sprite *background = Sprite::create("GameScene_Background.png");
	background->setAnchorPoint(Vec2(0, 0));
	addChild(background);

	_Monkey = Monkey::create();
	_Monkey->setPosition(visibleSize.width / 2560 * 400, visibleSize.height / 1440 * 85);
	addChild(_Monkey);

	initLabels();
	setStars();
	initNums(visibleSize);
	initOperators();

	_Board = Sprite::create("board-back.png");
	_Board->setPosition(Vec2(visibleSize.width / 2560 * 1160, visibleSize.height / 1440 * 1270));
	addChild(_Board);

	auto timer = ProgressTimer::create(Sprite::create("board.png"));
	timer->setPosition(Vec2(visibleSize.width / 2560 * 1160, visibleSize.height / 1440 * 1270));
	timer->setPercentage(0);
	timer->setTag(100);
	addChild(timer);

	_UButton = ui::Button::create("up_normal.png", "up_selected.png", "up_disabled.png");
	_UButton->setAnchorPoint(Vec2(0,0));
	_UButton->setPosition(Vec2(CONVERSION_X(0), CONVERSION_Y(650)));
	addChild(_UButton);
	_UButton->addTouchEventListener(CC_CALLBACK_2(GameScene::upButtonTouchEvent, this));

	_DButton = ui::Button::create("down_normal.png", "down_selected.png", "down_disabled.png");
	_DButton->setAnchorPoint(Vec2(0, 0));
	_DButton->setPosition(Vec2(CONVERSION_X(0), CONVERSION_Y(200)));
	addChild(_DButton);
	_DButton->addTouchEventListener(CC_CALLBACK_2(GameScene::downButtonTouchEvent, this));

	_LButton = ui::Button::create("left_normal.png", "left_selected.png", "left_disabled.png");
	_LButton->setAnchorPoint(Vec2(0, 0));
	_LButton->setPosition(Vec2(CONVERSION_X(2160), CONVERSION_Y(650)));
	addChild(_LButton);
	_LButton->addTouchEventListener(CC_CALLBACK_2(GameScene::leftButtonTouchEvent, this));

	_RButton = ui::Button::create("right_normal.png", "right_selected.png", "right_disabled.png");
	_RButton->setAnchorPoint(Vec2(0, 0));
	_RButton->setPosition(Vec2(CONVERSION_X(2160), CONVERSION_Y(200)));
	addChild(_RButton);
	_RButton->addTouchEventListener(CC_CALLBACK_2(GameScene::rightButtonTouchEvent, this));

	_WinBackSprite = Sprite::create("gameWin_bg.png");
	_WinBackSprite->setPosition(Vec2(CONVERSION_X(600), CONVERSION_Y(260)));
	_WinBackSprite->setVisible(false);
	_WinBackSprite->setAnchorPoint(Vec2(0, 0));
	addChild(_WinBackSprite);

	_LostBackSprite = Sprite::create("gameOver_bg.png");
	_LostBackSprite->setPosition(Vec2(CONVERSION_X(600), CONVERSION_Y(260)));
	_LostBackSprite->setVisible(false);
	_LostBackSprite->setAnchorPoint(Vec2(0, 0));
	addChild(_LostBackSprite);

	 _QuitButton = ui::Button::create("quit_normal.png", "quit_selected.png", "quit_disabled.png");
	_QuitButton->setAnchorPoint(Vec2(-0.5, -0.5));
	_QuitButton->setPosition(Vec2(CONVERSION_X(720), CONVERSION_Y(330)));
	_QuitButton->addTouchEventListener(CC_CALLBACK_2(GameScene::quitButtonTouchEvent, this));
	_QuitButton->setVisible(false);
	addChild(_QuitButton);

	_ContinueButton = ui::Button::create("continue_normal.png", "continue_selected.png", "continue_disabled.png");
	_ContinueButton->setAnchorPoint(Vec2(-0.5, -0.5));
	_ContinueButton->setPosition(Vec2(CONVERSION_X(720), CONVERSION_Y(450)));
	_ContinueButton->addTouchEventListener(CC_CALLBACK_2(GameScene::continueButtonTouchEvent, this));
	_ContinueButton->setVisible(false);
	addChild(_ContinueButton);

	scheduleUpdate();

	return true;
}

void GameScene::onEnter()
{
	Layer::onEnter();

	Size visibleSize = Director::getInstance()->getVisibleSize();

	_LButton->onTouchEnded(new Touch(), new Event((cocos2d::Event::Type)ui::Widget::TouchEventType::ENDED));
	_RButton->onTouchEnded(new Touch(), new Event((cocos2d::Event::Type)ui::Widget::TouchEventType::ENDED));
	_UButton->onTouchEnded(new Touch(), new Event((cocos2d::Event::Type)ui::Widget::TouchEventType::ENDED));
	_DButton->onTouchEnded(new Touch(), new Event((cocos2d::Event::Type)ui::Widget::TouchEventType::ENDED));

	_MathStep = MathStep::FIRSTNUM;
	_FirstNum = _SecondNum = _Operator = 0;
	int level = UserDefault::getInstance()->getIntegerForKey(USER_LEVEL,1);
	int sublevel = UserDefault::getInstance()->getIntegerForKey(USER_SUBLEVEL);
	_TotalTime = 131 - level * 10 - sublevel * 1;
	_LeftTime = _TotalTime;
	_PropIndex = -1;
	_IsPropGen = false;
	_IsWinOrLose = false;

	std::string str;
	genTargetString(str);
	_LeftLabel->setString("");
	_MiddleLabel->setString(str);
	
	resetOpPos();

	//考虑加上小Task Get Extra Star
	//1.计算过程中出现指定数字
	//2.计算过程中使用n次指定的数字
	//3.计算过程中使用n次指定运算符
	//4.计算过程中不使用指定的数字
	//5.计算过程中不使用指定运算符
	//6.在指定时间内完成计算
	//7.只进行一次计算就得出正确结果

	int arr[18] = { 0 };
	Randomizer(arr);
	for (size_t i = 0; i < _Nums.size(); i++)
	{
		_Nums[i]->setNumber(arr[i]);
		_Nums[i]->setAvai(true);
	}

	_Monkey->reset();
	_Monkey->setPosition(visibleSize.width / 2560 * 400, visibleSize.height / 1440 * 85);

	SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);
	_Monkey->_IsSoundOn = UserDefault::getInstance()->getBoolForKey(SOUND_KEY);

	_UpdateTimer = schedule_selector(GameScene::updateTimer);
	schedule(_UpdateTimer, 1, CC_REPEAT_FOREVER, 0);

#if 1
	auto listener = EventListenerKeyboard::create();
	listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
}


void GameScene::initLabels()
{
	std::string str;
	genTargetString(str);

	Size visibleSize = Director::getInstance()->getVisibleSize();

	TTFConfig labelConfig;
	labelConfig.fontFilePath = "fonts/Marker Felt.ttf";
	labelConfig.fontSize = CONVERSION_X(100);
	labelConfig.glyphs = GlyphCollection::DYNAMIC;
	labelConfig.outlineSize = 0;
	labelConfig.customGlyphs = nullptr;
	labelConfig.distanceFieldEnabled = false;
	_MiddleLabel = Label::createWithTTF(labelConfig, str);
	_MiddleLabel->setLocalZOrder(1);
	_MiddleLabel->setPosition(CONVERSION_X(1160), CONVERSION_Y(1270));
	_MiddleLabel->setTextColor(Color4B::BLACK);
	addChild(_MiddleLabel);

	_LeftLabel = Label::createWithTTF(labelConfig, "");
	_LeftLabel->setPosition(CONVERSION_X(480), CONVERSION_Y(1310));
	_LeftLabel->setTextColor(Color4B::BLACK);
	addChild(_LeftLabel);

	_LeftLabel1 = Label::createWithTTF(labelConfig, "");
	_LeftLabel1->setPosition(CONVERSION_X(480), CONVERSION_Y(1310));
	_LeftLabel1->setTextColor(Color4B::BLACK);
	_LeftLabel1->setVisible(false);
	addChild(_LeftLabel1);

	labelConfig.fontSize = CONVERSION_X(60);
	_RightLabel = Label::createWithTTF(labelConfig, "");
	_RightLabel->setPosition(CONVERSION_X(1915), CONVERSION_Y(1320));
	_RightLabel->setTextColor(Color4B::BLACK);
	addChild(_RightLabel);

	for (size_t i = 0; i < 4; i++)
	{
		Operator *temp = Operator::create();
		temp->setType(i);
		temp->setPosition(CONVERSION_X(480), CONVERSION_Y(1315));
		temp->setVisible(false);
		addChild(temp);
		_ShowOps.push_back(temp);
	}
}

void GameScene::setStars()
{
	//sublevel 范围为0-5 0没有星星 5有5颗星
	int level = UserDefault::getInstance()->getIntegerForKey(USER_LEVEL, 1);
	std::stringstream ss;
	ss << "Stars: " << level;
	//TODO LEVEL
	_RightLabel->setString(ss.str());
	int sublevel = UserDefault::getInstance()->getIntegerForKey(USER_SUBLEVEL);
	for (int i = 0; i < 5; i++)
	{
		doAnimationWithStar("level/star1.png", i, 98 - i);
	}
	for (int i = 0; i < sublevel; i++)
	{
		doAnimationWithStar("level/star.png", i, 93 - (i + 1));
	}

}

void GameScene::initNums(Size & visibleSize)
{
	int n = 0;
	float numPos_x = visibleSize.width / 2560 * 525;
	float numPos_y = visibleSize.height / 1440 * 818;

	for (int i = 0; i < 2; i++)
	{
		float offset_x = visibleSize.width / 2560 * 1040 * i;
		for (int j = 0; j < 3; j++)
		{
			float offse_xx = visibleSize.width / 2560 * 218 * j;
			for (int k = 0; k < 3; k++)
			{
				float offse_y = -(visibleSize.height / 1440 * 223 * k);
				Number * number = Number::create();
				number->setPosition(numPos_x + offset_x + offse_xx, numPos_y + offse_y);
				addChild(number);
				_Nums.push_back(number);
				n++;
			}
		}
	}
}

void GameScene::upButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (_IsWinOrLose)
	{
		return;
	}
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		if (_Monkey->_Status == Monkey::Status::TREE)
		{
			if (_Monkey->isDropDown)
				return;
			if(_Monkey->moveStep == 1)
				scheduleOnce(schedule_selector(GameScene::doUpPressedWrapper), 0.1);
			else
				_Monkey->doUpPressed();
		}
		else if (_Monkey->_Status == Monkey::Status::LAND)
		{
			_Monkey->jump();
		}
	}
	else if (type == ui::Widget::TouchEventType::ENDED || type == ui::Widget::TouchEventType::CANCELED)
	{
		if (_Monkey->_Status == Monkey::Status::TREE)
		{
			_Monkey->doUpReleased();
		}
		
	}
}

void GameScene::downButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (_IsWinOrLose)
	{
		return;
	}
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		if (_Monkey->isDropDown)
			return;
		if (_Monkey->moveStep == 1)
			scheduleOnce(schedule_selector(GameScene::doDownPressedWrapper), 0.1);
		else
			_Monkey->doDownPressed();
	}
	else if (type == ui::Widget::TouchEventType::ENDED || type == ui::Widget::TouchEventType::CANCELED)
	{
		_Monkey->doDownReleased();
	}
}

void GameScene::leftButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (_IsWinOrLose)
	{
		return;
	}
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		_Monkey->doLeftPressed();
	}
	else if (type == ui::Widget::TouchEventType::ENDED || type == ui::Widget::TouchEventType::CANCELED)
	{
		if (!_Monkey->isClimbD && !_Monkey->isClimbU)
			_Monkey->doLeftReleased();
	}
}

void GameScene::rightButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (_IsWinOrLose)
	{
		return;
	}
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		_Monkey->doRightPressed();
	}
	else if (type == ui::Widget::TouchEventType::ENDED || type == ui::Widget::TouchEventType::CANCELED)
	{
		if(!_Monkey->isClimbD && !_Monkey->isClimbU)
			_Monkey->doRightReleased();
	}
}

void GameScene::update(float dt)
{
	_Monkey->update(dt);

	doProps();

	if (_MathStep == MathStep::FIRSTNUM)
	{
		int sIdx = 0;
		int fIdx = 0;
		if (getCurTreeIdx(sIdx, fIdx) == 1)
			return;
		for (size_t i = sIdx; i < fIdx; i++)
		{
			Number *num = _Nums[i];
			if (!num->getAvai())
			{
				continue;
			}
			if (num->getPositionY() >= _Monkey->getBoundingBox().origin.y && num->getPositionY() <= _Monkey->getBoundingBox().origin.y + _Monkey->sp->getBoundingBox().size.height)
			{
				int on = num->getNumber();
				{
					_MathStep = MathStep::OPERATOR;
					_FirstNum = on;
					std::stringstream ss;
					ss << on;
					_LeftLabel->setString(ss.str());

					if (!_IsPropGen)
					{
						on = Randomizer();
						if ((on % 1) == 0)
						{
							for (size_t i = 0; i < _Nums.size(); i++)
							{
								if (on == _Nums[i]->getNumber())
								{
									on = 10 + Randomizer();
									_IsPropGen = true;
								}
							}
						}
					}
					
				}
				num->setVisible(false);
				num->setNumber(on);
				auto blink = Blink::create(2, 10);
				auto action = Sequence::create(DelayTime::create(2), blink, CallFunc::create(CC_CALLBACK_0(GameScene::numVisibleCallBack, this, num)), nullptr);
				num->runAction(action);
			}
		}
	}
	else if (_MathStep == MathStep::OPERATOR)
	{
		dealOperator();
	}
	else if (_MathStep == MathStep::SECONDNUM)
	{
		int sIdx = 0;
		int fIdx = 0;
		if (getCurTreeIdx(sIdx, fIdx) == 1)
			return;
		for (size_t i = sIdx; i < fIdx; i++)
		{
			Number *num = _Nums[i];
			if (!num->getAvai())
			{
				continue;
			}
			if (num->getPositionY() >= _Monkey->getBoundingBox().origin.y && num->getPositionY() <= _Monkey->getBoundingBox().origin.y + _Monkey->sp->getBoundingBox().size.height)
			{
				int on = num->getNumber();
				{
					_MathStep = MathStep::RESULT;
					_SecondNum = on;
					std::stringstream ss;
					ss << on;
					
					_LeftLabel1->setVisible(true);
					_LeftLabel1->setPositionX(_ShowOps[_Operator]->getBoundingBox().getMaxX() + 25);
					_LeftLabel1->setString(ss.str());

					if (!_IsPropGen)
					{
						on = Randomizer();
						if ((on % 1) == 0)
						{
							for (size_t i = 0; i < _Nums.size(); i++)
							{
								if (on == _Nums[i]->getNumber())
								{
									on = 10 + Randomizer();
									_IsPropGen = true;
								}
							}
						}
					}

				}
				num->setVisible(false);
				num->setNumber(on);
				auto blink = Blink::create(2, 10);
				auto action = Sequence::create(DelayTime::create(2), blink, CallFunc::create(CC_CALLBACK_0(GameScene::numVisibleCallBack, this, num)), nullptr);
				num->runAction(action);
		}
	}
	}
	else if (_MathStep == MathStep::RESULT)
	{
		switch (_Operator)
		{
		case 0:
			_Result = _FirstNum + _SecondNum;
			break;
		case 1:
			_Result = _FirstNum - _SecondNum;
			break;
		case 2:
			_Result = _FirstNum * _SecondNum;
			break;
		case 3:
			_Result = _FirstNum / _SecondNum;
			break;
		default:
			break;
		}
		_MathStep = MathStep::NONE;
		scheduleOnce(schedule_selector(GameScene::updateResultCallBack), 1);
	}
}

void GameScene::Randomizer(int * arr)
{
	time_t t1;
	time(&t1);
	clock_t t2 = clock();
	unsigned seed = t1 * 1000 + t2 % 1000;
	srand(seed);
	
	int max = 17;
	int min = 0;
	for (int i = 0; i < 9; i++)
	{
		int value = (rand() % (max - min)) + min;
		while (arr[value]>0)
		{
			value = (rand() % (max - min)) + min;
		}
		arr[value] = i + 1;
	}
	
	max = 9;
	min = 1;
	for (int i = 0; i< 18; i++)
	{
		if (arr[i]==0)
		{
			arr[i] =  (rand() % (max - min)) + min;
		}
	}
}

int GameScene::Randomizer()
{
	time_t t1;
	time(&t1);
	clock_t t2 = clock();
	unsigned seed = t1 * 1000 + t2 % 1000;
	srand(seed);
	
	int max = 9;
	int min = 1;
	int value = (rand() % (max - min)) + min;
	return value;
}

void GameScene::dealOperator()
{
	if (_Monkey->_Status == Monkey::Status::LAND && !_Monkey->isUpLand)
	{
		if (_Monkey->_CurLandIdx < 0 || _Monkey->_CurLandIdx > _Monkey->_LandPos.size() - 1)
		{
			return;
		}
		Operator * curOp = NULL;
		int curOpIdx = -1;
		float minX = _Monkey->_LandPos[_Monkey->_CurLandIdx].getMinX();
		float maxX = _Monkey->_LandPos[_Monkey->_CurLandIdx].getMaxX();
		for (size_t i = 0; i < _Ops.size(); i++)
		{
			if (_Ops[i]->getPositionX() > minX && _Ops[i]->getPositionX() < maxX)
			{
				curOp = _Ops[i];
				break;
			}
		}
		if (curOp == NULL)
			return;

		_MathStep = MathStep::SECONDNUM;
		time_t t = time(NULL);
		srand(t);
		int posIdx = rand() % 2;
		curOp->setPositionX(_Monkey->_LandPos[_FreeOpPos[posIdx]].getMidX());
		_FreeOpPos[posIdx] = _Monkey->_CurLandIdx;
		_Operator = curOp->getType();
		showOperator();
#if 0
		std::string str = _LeftLabel->getString();
		switch (_Operator)
		{
		case 0:
			str.append(" + ");
			break;
		case 1:
			str.append(" - ");
			break;
		case 2:
			str.append(" x ");
			break;
		case 3:
			str.append(" ÷ ");
			break;
		default:
			break;
		}
		CCLOG("dealOperator %d %s", _Operator, str.c_str());
		_LeftLabel->setString(str);
#endif
	}
}

void GameScene::doProps()
{
	int sIdx = 0;
	int fIdx = 0;

	if (getCurTreeIdx(sIdx, fIdx) == 1)
		return;
	for (size_t i = sIdx; i < fIdx; i++)
	{
		Number *num = _Nums[i];
		if (!num->getAvai())
		{
			continue;
		}
		if (num->getPositionY() >= _Monkey->getBoundingBox().origin.y && num->getPositionY() <= _Monkey->getBoundingBox().origin.y + _Monkey->sp->getBoundingBox().size.height)
		{
			int on = num->getNumber();
			if (on > 10)
			{
				dealProps(on, Vec2(num->getPositionX(), num->getPositionY()));
				_IsPropGen = false;
				on = Randomizer();
				num->setVisible(false);
				num->setNumber(on);
				auto blink = Blink::create(2, 10);
				auto action = Sequence::create(DelayTime::create(2), blink, CallFunc::create(CC_CALLBACK_0(GameScene::numVisibleCallBack, this, num)), nullptr);
				num->runAction(action);
			}
		}
	}
}

void GameScene::numVisibleCallBack(Node* sender)
{
	Number *num = (Number *)sender;
	num->setAvai(true);
	num->setVisible(true);
}

void GameScene::updateResultCallBack(float dt)
{ 
	std::stringstream ss;
	ss << _Result;
	showOperator(false);
	_LeftLabel1->setVisible(false);
	_LeftLabel->setString(ss.str());
	_FirstNum = _Result;
	_MathStep = MathStep::OPERATOR;

	if (_Result == _Target)
	{
		int sublevel = UserDefault::getInstance()->getIntegerForKey(USER_SUBLEVEL);
		sublevel += 1;
		//TODO::add star
		if (sublevel > 5)
		{
			sublevel = 0;
			int level = UserDefault::getInstance()->getIntegerForKey(USER_LEVEL,1);
			UserDefault::getInstance()->setIntegerForKey(USER_LEVEL,level+1);
			//TODO::LEVEL
			std::stringstream ss;
			ss << "Stars: " << level+1;
			//TODO LEVEL
			_RightLabel->setString(ss.str());
			removeAllStar();
		}
		else
		{
			doAnimationWithStar("level/star.png", sublevel - 1, 93 - sublevel);
		}
		UserDefault::getInstance()->setIntegerForKey(USER_SUBLEVEL, sublevel);
		//TODO SUCCESS
		CCLOG("you won");
		showWinOrLost(true);
#if 0
		GameOverScene* layer = GameOverScene::create();
		layer->setWin(true);
		auto scene = Scene::create();
		scene->addChild(layer);
		Director::getInstance()->pushScene(scene);
#endif
	}
}

void GameScene::updateTimer(float dt)
{
	_LeftTime--;
	ProgressTimer *timer = (ProgressTimer *)getChildByTag(100);
	timer->setPercentage((1 - (float)_LeftTime / (float)_TotalTime) * 100);
	if (_LeftTime <= 0)
	{
		int sublevel = UserDefault::getInstance()->getIntegerForKey(USER_SUBLEVEL);
		if (sublevel > 0)
		{
			//TODO::remove one star
			removeOneStar(93 - sublevel);
			doAnimationWithStar1("level/star.png", sublevel - 1, 93 - sublevel);
			sublevel -= 1;
		}
		else
		{
			sublevel = 5;
			int level = UserDefault::getInstance()->getIntegerForKey(USER_LEVEL);
			if (level > 1)
			{
				level -= 1;
				UserDefault::getInstance()->setIntegerForKey(USER_LEVEL,level);
				std::stringstream ss;
				ss << "Stars: " << level;
				//TODO LEVEL
				_RightLabel->setString(ss.str());
			}
#if 0
			for (int i = 0; i < 5; i++)
			{
				doAnimationWithStar("level/star1.png", i, 98 - i);
			}
#endif
			for (int i = 0; i < sublevel; i++)
			{
				doAnimationWithStar("level/star.png", i, 93 - (i+1));
			}
		}
		UserDefault::getInstance()->setIntegerForKey(USER_SUBLEVEL, sublevel);
		unschedule(_UpdateTimer);
		CCLOG("you lost");
		showWinOrLost(false);
#if 0
		GameOverScene* layer = GameOverScene::create();
		layer->setWin(false);
		auto scene = Scene::create();
		scene->addChild(layer);
		Director::getInstance()->pushScene(scene);
#endif
	}
	
}

void GameScene::pageAnimation()
{
	Vector<SpriteFrame*> allFrames;
	char txt[100] = {};
	for (int i = 1; i < 7; i++)
	{
		sprintf(txt, "page/page-%d.png", i);
		SpriteFrame * sf = SpriteFrame::create(txt, Rect(0, 0, _Board->getBoundingBox().size.width, _Board->getBoundingBox().size.height));
		allFrames.pushBack(sf);
	}
	Animation * ani = Animation::createWithSpriteFrames(allFrames, 0.1);
	_Board->runAction(RepeatForever::create(Animate::create(ani)));
}

void GameScene::genTargetString(std::string & str)
{
	time_t t1 = time(NULL);
	clock_t t2 = clock();
	unsigned seed = t1 * 1000 + t2 % 1000;
	srand(seed);
	_Target = rand() % 99 + 10;
	std::stringstream ss;
	ss << _Target;
	str = ss.str();
}


void GameScene::initOperators()
{
	int *pRandom = RandUtil::rand_N(4);
	for (size_t i = 0; i < 4; i++)
	{
		Operator *temp = Operator::create();
		temp->setType(pRandom[i]-1);
		temp->setPosition(_Monkey->_LandPos[i + 1].getMidX(), _Monkey->_LandPos[i + 1].getMaxY());
		addChild(temp);
		_Ops.push_back(temp);
	}
	delete[]pRandom;
}

void GameScene::resetOpPos()
{
	_FreeOpPos.clear();
	_FreeOpPos.push_back(0);
	_FreeOpPos.push_back(5);
	for (size_t i = 0; i < _Ops.size(); i++)
	{
		_Ops[i]->setPosition(_Monkey->_LandPos[i + 1].getMidX(), _Monkey->_LandPos[i + 1].getMaxY());
	}
}

int GameScene::getCurTreeIdx(int & sIdx, int & fIdx)
{
	if (_Monkey->_CurTreeIdx == 1 || _Monkey->_CurTreeIdx == 3 || _Monkey->_CurTreeIdx == 5
		|| _Monkey->_CurTreeIdx == 6 || _Monkey->_CurTreeIdx == 8 || _Monkey->_CurTreeIdx == 10)
	{
		return 1;
	}
	if (_Monkey->_CurTreeIdx == 0)
	{
		sIdx = 0;
		fIdx = 3;
	}
	else if (_Monkey->_CurTreeIdx == 2)
	{
		sIdx = 3;
		fIdx = 6;
	}
	else if (_Monkey->_CurTreeIdx == 4)
	{
		sIdx = 6;
		fIdx = 9;
	}
	else if (_Monkey->_CurTreeIdx == 7)
	{
		sIdx = 9;
		fIdx = 12;
	}
	else if (_Monkey->_CurTreeIdx == 9)
	{
		sIdx = 12;
		fIdx = 15;
	}
	else if (_Monkey->_CurTreeIdx == 11)
	{
		sIdx = 15;
		fIdx = 18;
	}
	return 0;
}

void GameScene::dealProps(int propType,const Vec2 pos)
{
	CCLOG("dealProps %d", propType);
	switch (propType)
	{
	case 11:
		doAnimationWithLeftTime("number/prop11.png",pos);
		break;
	case 12:
		doAnimationWithLeftTime("number/prop12.png", pos, propType);
		break;
	case 13:
	case 17:
		doAnimationWithNums(pos, propType);
		break;
	case 14:
	case 18:
		doAnimationWithNums(pos, propType);
		break;
	case 15:
	case 19:
		doAnimationWithNums(pos, propType);
		break;
	case 16:
	{
#if 1
		int arr[18] = { 0 };
		Randomizer(arr);
		for (size_t i = 0; i < _Nums.size(); i++)
		{
			_Nums[i]->setAvai(false);
			doAnimationWithAllNum(arr[i], _Nums[i]->getPosition(), pos, i);
		}
#endif
	}
		break;
	default:
		break;
	}
}

void GameScene::doAnimationWithLeftTime(const std::string & filePath,const Vec2 pos,int type)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto flyTime = Sprite::create(filePath);
	addChild(flyTime,1, type);
	flyTime->setPosition(pos);

	auto pMove = MoveTo::create(2, Vec2(CONVERSION_X(1160), CONVERSION_Y(1270)));
	auto pRepeatMove = RepeatForever::create(pMove);
	pRepeatMove->setTag(type);
	auto pScale1 = ScaleBy::create(1, 1.5f);
	auto pScale2 = ScaleBy::create(1, 1.0f / 1.5f);
	auto pSequenceScale = Sequence::createWithTwoActions(pScale1, pScale2);
	pSequenceScale->setTag(type);
	auto pRepeatScale = RepeatForever::create(pSequenceScale);
	auto pRotate = RotateBy::create(2, 360);
	auto pRepeatRotate = RepeatForever::create(pRotate);
	pRepeatRotate->setTag(type);

	flyTime->runAction(pMove);
	flyTime->runAction(pScale1);
	flyTime->runAction(pScale2);
	flyTime->runAction(pRotate);
	//this->scheduleOnce((SEL_SCHEDULE)&GameScene::stopLeftTimeAction, 2);
	scheduleOnce(CC_CALLBACK_1(GameScene::stopLeftTimeAction, this,type), 2,"flyTime");
}

void GameScene::stopLeftTimeAction(float time,int type)
{
	if (type == 11)
	{
		_LeftTime += 10;
	}
	else
	{
		if (_LeftTime > 10)
		{
			_LeftTime -= 10;
		}
		else
		{
			_LeftTime = 5;
		}
	}

	auto sprite = getChildByTag(type);
	if (sprite)
	{
		sprite->stopAllActionsByTag(type);
		removeChild(sprite, true);
	}
}

void GameScene::doAnimationWithNums(const Vec2 pos, int type)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	std::string str;
	Vec2 dstPos;
	if (type == 13 || type == 17)
	{
		dstPos = Vec2(CONVERSION_X(1160), CONVERSION_Y(1270));
		genTargetString(str);
	}
	else if (type == 14 || type == 18)
	{
		std::stringstream ss;
		_PropNum = Randomizer() * 10;
		ss << _PropNum;
		str = ss.str();
		dstPos = Vec2(CONVERSION_X(480), CONVERSION_Y(1310));
	}
	else if (type == 15 || type == 19)
	{
		if (_MathStep != MathStep::SECONDNUM)
		{
			type = 14;
			std::stringstream ss;
			_PropNum = Randomizer() * 10;
			ss << _PropNum;
			str = ss.str();
			dstPos = Vec2(CONVERSION_X(480), CONVERSION_Y(1310));
		}
		else
		{
			dstPos = _ShowOps[_Operator]->getPosition();
			int *pRandom = RandUtil::rand_N(4);
			for (size_t i = 0; i < 4; i++)
			{
				if ((pRandom[i] - 1) != _Operator)
				{
					_Operator = pRandom[i] - 1;
					break;
				}
			}
			delete[] pRandom;
		}
	}

	if (type != 15 && type != 19)
	{
		TTFConfig labelConfig;
		labelConfig.fontFilePath = "fonts/Marker Felt.ttf";
		labelConfig.fontSize = CONVERSION_X(100);
		labelConfig.glyphs = GlyphCollection::DYNAMIC;
		labelConfig.outlineSize = 0;
		labelConfig.customGlyphs = nullptr;
		labelConfig.distanceFieldEnabled = false;

		auto label = Label::createWithTTF(labelConfig, str);
		label->setPosition(pos);
		label->setTextColor(Color4B::BLACK);
		addChild(label, 1, type);

		auto pMove = MoveTo::create(2, dstPos);
		auto pRepeatMove = RepeatForever::create(pMove);
		pRepeatMove->setTag(type);
		auto pScale1 = ScaleBy::create(1, 1.5f);
		auto pScale2 = ScaleBy::create(1, 1.0f / 1.5f);
		auto pSequenceScale = Sequence::createWithTwoActions(pScale1, pScale2);
		pSequenceScale->setTag(type);
		auto pRepeatScale = RepeatForever::create(pSequenceScale);
		auto pRotate = RotateBy::create(2, 360);
		auto pRepeatRotate = RepeatForever::create(pRotate);
		pRepeatRotate->setTag(type);

		label->runAction(pMove);
		label->runAction(pScale1);
		label->runAction(pScale2);
		label->runAction(pRotate);
		scheduleOnce(CC_CALLBACK_1(GameScene::stopNumsAction, this, type), 2, "label");
	}
	else
	{
		_ShowOps[_Operator]->setVisible(true);
		_ShowOps[_Operator]->setPosition(pos);

		auto pMove = MoveTo::create(2, dstPos);
		auto pRepeatMove = RepeatForever::create(pMove);
		pRepeatMove->setTag(type);
		auto pScale1 = ScaleBy::create(1, 1.5f);
		auto pScale2 = ScaleBy::create(1, 1.0f / 1.5f);
		auto pSequenceScale = Sequence::createWithTwoActions(pScale1, pScale2);
		pSequenceScale->setTag(type);
		auto pRepeatScale = RepeatForever::create(pSequenceScale);
		auto pRotate = RotateBy::create(2, 360);
		auto pRepeatRotate = RepeatForever::create(pRotate);
		pRepeatRotate->setTag(type);

		_ShowOps[_Operator]->runAction(pMove);
		_ShowOps[_Operator]->runAction(pScale1);
		_ShowOps[_Operator]->runAction(pScale2);
		_ShowOps[_Operator]->runAction(pRotate);
		scheduleOnce(CC_CALLBACK_1(GameScene::stopNumsAction, this, type), 2, "label");
	}
}

void GameScene::stopNumsAction(float time, int type)
{
	auto label = getChildByTag(type);
	if (label)
	{
		label->stopAllActionsByTag(type);
		removeChild(label, true);
	}
	if (type == 13 || type == 17)
	{
		std::stringstream ss;
		ss << _Target;
		_MiddleLabel->setString(ss.str());
	}
	else if (type == 14 || type == 18)
	{
		std::string str;
		std::stringstream ss;
		_FirstNum = _PropNum;
		ss << _FirstNum;
		str = ss.str();
		_LeftLabel->setString(str);
		if (_MathStep == MathStep::SECONDNUM)
		{
			showOperator(true);
		}
	}
	else if (type == 15 || type == 19)
	{
		showOperator();
	}
}

void GameScene::doAnimationWithAllNum(int num,const Vec2 dstPos, const Vec2 propPos,int numIdx)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	std::string str;
	std::stringstream ss;
	ss << num;
	str = ss.str();

	TTFConfig labelConfig;
	labelConfig.fontFilePath = "fonts/Marker Felt.ttf";
	labelConfig.fontSize = CONVERSION_X(100);
	labelConfig.glyphs = GlyphCollection::DYNAMIC;
	labelConfig.outlineSize = 0;
	labelConfig.customGlyphs = nullptr;
	labelConfig.distanceFieldEnabled = false;

	auto label = Label::createWithTTF(labelConfig, str);
	label->setPosition(propPos);
	label->setTextColor(Color4B::ORANGE);
	addChild(label, 1, 16);

	auto pMove = MoveTo::create(2, dstPos);
	auto pRepeatMove = RepeatForever::create(pMove);
	pRepeatMove->setTag(16);
	auto pScale1 = ScaleBy::create(1, 1.5f);
	auto pScale2 = ScaleBy::create(1, 1.0f / 1.5f);
	auto pSequenceScale = Sequence::createWithTwoActions(pScale1, pScale2);
	pSequenceScale->setTag(16);
	auto pRepeatScale = RepeatForever::create(pSequenceScale);
	auto pRotate = RotateBy::create(2, 360);
	auto pRepeatRotate = RepeatForever::create(pRotate);
	pRepeatRotate->setTag(16);

	label->runAction(pMove);
	label->runAction(pScale1);
	label->runAction(pScale2);
	label->runAction(pRotate);

	ss.str("");
	ss << numIdx;
	scheduleOnce(CC_CALLBACK_1(GameScene::stopAllNumAnimation, this,num, numIdx), 2, ss.str());
}

void GameScene::stopAllNumAnimation(float time, int num,int numIdx)
{
	auto label = getChildByTag(16);
	if (label)
	{
		label->stopAllActionsByTag(16);
		removeChild(label, true);
	}
	
	_Nums[numIdx]->setNumber(num);
	_Nums[numIdx]->setAvai(true);
}

void GameScene::doAnimationWithTitle(int level, int type)
{
	if (type>1)
	{
		removeOneLevel(type);
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	if (level == 0)
		level = 1;
	std::string filePath = StringUtils::format("level/level-%d.png", level);
	auto title = Sprite::create(filePath);
	title->setAnchorPoint(Vec2(0, 1));
	addChild(title,1, type);
	title->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));

	Vec2 desPos = Vec2(CONVERSION_X(1800), CONVERSION_Y(1370));
	auto pMove = MoveTo::create(2, desPos);
	auto pRepeatMove = RepeatForever::create(pMove);
	pRepeatMove->setTag(type);
	auto pScale1 = ScaleBy::create(1, 1.5f);
	auto pScale2 = ScaleBy::create(1, 1.0f / 1.5f);
	auto pSequenceScale = Sequence::createWithTwoActions(pScale1, pScale2);
	pSequenceScale->setTag(type);
	auto pRepeatScale = RepeatForever::create(pSequenceScale);
	auto pRotate = RotateBy::create(2, 360);
	auto pRepeatRotate = RepeatForever::create(pRotate);
	pRepeatRotate->setTag(type);

	title->runAction(pMove);
	title->runAction(pScale1);
	title->runAction(pScale2);
	title->runAction(pRotate);
	//this->scheduleOnce((SEL_SCHEDULE)&GameScene::stopLeftTimeAction, 2);
	scheduleOnce(CC_CALLBACK_1(GameScene::stopTitleAnimation, this,type, desPos, filePath), 2, StringUtils::format("title-%d", type));
}

void GameScene::stopTitleAnimation(float time, int type,Vec2 dstPos, std::string filepath)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto sprite = getChildByTag(type);
	if (sprite)
	{
		sprite->stopAllActionsByTag(type);
		removeChild(sprite, true);
	}
	auto title = Sprite::create(filepath);
	title->setAnchorPoint(Vec2(0, 1));
	title->setPosition(dstPos);
	addChild(title, 1, type);
}

void GameScene::removeOneLevel(int type)
{
	auto sprite = getChildByTag(type);
	if (sprite)
	{
		sprite->stopAllActionsByTag(type);
		removeChild(sprite, true);
	}
}

void GameScene::doAnimationWithStar(const std::string filePath, int sublevel, int type)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto star = Sprite::create(filePath);
	star->setAnchorPoint(Vec2(0, 1));
	addChild(star, 1, type);
	star->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	Vec2 dstPos = Vec2(visibleSize.width / 2560 * 2050 + star->getBoundingBox().size.width*sublevel, visibleSize.height / 1440 * 1350);

	auto pMove = MoveTo::create(2, dstPos);
	auto pRepeatMove = RepeatForever::create(pMove);
	pRepeatMove->setTag(type);
	auto pScale1 = ScaleBy::create(1, 1.5f);
	auto pScale2 = ScaleBy::create(1, 1.0f / 1.5f);
	auto pSequenceScale = Sequence::createWithTwoActions(pScale1, pScale2);
	pSequenceScale->setTag(type);
	auto pRepeatScale = RepeatForever::create(pSequenceScale);
	auto pRotate = RotateBy::create(2, 360);
	auto pRepeatRotate = RepeatForever::create(pRotate);
	pRepeatRotate->setTag(type);

	star->runAction(pMove);
	star->runAction(pScale1);
	star->runAction(pScale2);
	star->runAction(pRotate);
	//this->scheduleOnce((SEL_SCHEDULE)&GameScene::stopLeftTimeAction, 2);
	scheduleOnce(CC_CALLBACK_1(GameScene::stopStarAnimation, this, type,dstPos, filePath), 2, StringUtils::format("star-%d", type));
}

void GameScene::stopStarAnimation(float time, int type, Vec2 dstPos,std::string filepath)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto sprite = getChildByTag(type);
	if (sprite)
	{
		sprite->stopAllActionsByTag(type);
		removeChild(sprite, true);
	}
	if (type != (93-6))
	{
		auto star = Sprite::create(filepath);
		star->setAnchorPoint(Vec2(0, 1));
		star->setPosition(dstPos);
		addChild(star, 1, type);
	}
}

void GameScene::doAnimationWithStar1(const std::string filePath, int sublevel, int type)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto star = Sprite::create(filePath);
	star->setAnchorPoint(Vec2(0, 1));
	addChild(star, 1, type);
	star->setPosition(Vec2(visibleSize.width / 2560 * 2050 + star->getBoundingBox().size.width*sublevel, visibleSize.height / 1440 * 1350));
	Vec2 dstPos = Vec2(visibleSize.width / 2560 * 2050 + star->getBoundingBox().size.width*sublevel, 0);

	auto pMove = MoveTo::create(2, dstPos);
	auto pRepeatMove = RepeatForever::create(pMove);
	pRepeatMove->setTag(type);
	auto pScale1 = ScaleBy::create(1, 1.5f);
	auto pScale2 = ScaleBy::create(1, 1.0f / 1.5f);
	auto pSequenceScale = Sequence::createWithTwoActions(pScale1, pScale2);
	pSequenceScale->setTag(type);
	auto pRepeatScale = RepeatForever::create(pSequenceScale);
	auto pRotate = RotateBy::create(2, 360);
	auto pRepeatRotate = RepeatForever::create(pRotate);
	pRepeatRotate->setTag(type);

	star->runAction(pMove);
	star->runAction(pScale1);
	star->runAction(pScale2);
	star->runAction(pRotate);
	//this->scheduleOnce((SEL_SCHEDULE)&GameScene::stopLeftTimeAction, 2);
	scheduleOnce(CC_CALLBACK_1(GameScene::stopStarAnimation1, this, type, dstPos, filePath), 2, StringUtils::format("star-%d", type));
}

void GameScene::stopStarAnimation1(float time, int type, Vec2 dstPos, std::string filepath)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto sprite = getChildByTag(type);
	if (sprite)
	{
		sprite->stopAllActionsByTag(type);
		removeChild(sprite, true);
	}
}

void GameScene::removeAllStar()
{
	int type = 0;
	for (int i = 0; i < 5; i++)
	{
		type = 93 - (i + 1);
		CCLOG("removeAllStar %d", type);
		auto sprite = getChildByTag(type);
		if (sprite)
		{
			sprite->stopAllActionsByTag(type);
			removeChild(sprite, true);
		}
	}
}

void GameScene::removeOneStar(int type)
{
	CCLOG("removeOneStar %d", type);
	auto sprite = getChildByTag(type);
	if (sprite)
	{
		sprite->stopAllActionsByTag(type);
		removeChild(sprite, true);
	}
}

void GameScene::doUpPressedWrapper(float dt)
{
	_Monkey->doUpPressed();
}

void GameScene::doDownPressedWrapper(float dt)
{
	_Monkey->doDownPressed();
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
#if 0
	auto scene = HomeScene::createScene();
	Director::getInstance()->replaceScene(TransitionCrossFade::create(0.1, scene));
#else
	auto scene = SettingScene::createScene();
	Director::getInstance()->pushScene(scene);
#endif
}

void GameScene::quitButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		//Director::getInstance()->popScene();
		Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
#endif
	}
}

void GameScene::continueButtonTouchEvent(Ref * pSender, ui::Widget::TouchEventType type)
{
	_WinBackSprite->setVisible(false);
	_LostBackSprite->setVisible(false);
	_QuitButton->setVisible(false);
	_ContinueButton->setVisible(false);
	onEnter();
}

void GameScene::showWinOrLost(bool isWin)
{
	if (isWin)
		_WinBackSprite->setVisible(true);
	else
		_LostBackSprite->setVisible(true);
	_QuitButton->setVisible(true);
	_ContinueButton->setVisible(true);
	for (size_t i = 0; i < _Nums.size(); i++)
	{
		_Nums[i]->setAvai(false);
	}
	unschedule(_UpdateTimer);
	_Monkey->stopAnimationByStatus(Monkey::Direction::NONE);
	_IsWinOrLose = true;
}

void GameScene::showOperator(bool isShow)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	for (size_t i = 0; i < _ShowOps.size(); i++)
	{
		_ShowOps[i]->setVisible(false);
	}

	_ShowOps[_Operator]->setPositionX(_LeftLabel->getBoundingBox().getMaxX() + 20);
	_ShowOps[_Operator]->setVisible(isShow);
}
