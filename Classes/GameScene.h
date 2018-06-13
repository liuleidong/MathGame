#ifndef GameScene_h__
#define GameScene_h__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Monkey.h"
#include "Number.h"
#include "Operator.h"

USING_NS_CC;
class GameScene : public Layer
{
public:
	enum class MathStep {
		NONE,
		FIRSTNUM,
		OPERATOR,
		SECONDNUM,
		RESULT
	};

public:
	static Scene *createScene();
	bool init();
	void onEnter();
	CREATE_FUNC(GameScene);

	void initLabels();
	void setStars();
	void initNums(Size &visibleSize);

	void upButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
	void downButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
	void leftButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
	void rightButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);

	void update(float dt);
	void Randomizer(int * arr);
	int	 Randomizer();

	void dealOperator();
	void doProps();

	void numVisibleCallBack(Node* sender);
	void updateResultCallBack(float dt);
	void updateTimer(float dt);

	void pageAnimation();
	void genTargetString(std::string &str);
	
	void initOperators();
	void resetOpPos();

	int getCurTreeIdx(int &sIdx, int &fIdx);
	void dealProps(int propType,const Vec2 pos);
	//type-11 时间增长10s 12-时间减少10s
	void doAnimationWithLeftTime(const std::string &filePath,const Vec2 pos,int type = 11);
	void stopLeftTimeAction(float time,int type);

	//type 13-目标结果改变 14-已经吃的数字变一下 15-已经吃的运算符变一下
	void doAnimationWithNums(const Vec2 pos, int type);
	void stopNumsAction(float time, int type);

	void doAnimationWithAllNum(int num,const Vec2 dstPos, const Vec2 propPos,int numIdx);
	void stopAllNumAnimation(float time, int num,int numIdx);

	void doAnimationWithTitle(int level, int type = 99);
	void stopTitleAnimation(float time, int type, Vec2 dstPos, std::string filepath);
	void removeOneLevel(int type);
	void doAnimationWithStar(const std::string filePath, int sublevel, int type);
	void stopStarAnimation(float time, int type, Vec2 dstPos, std::string filepath);
	void doAnimationWithStar1(const std::string filePath, int sublevel, int type);
	void stopStarAnimation1(float time, int type, Vec2 dstPos, std::string filepath);
	void removeAllStar();
	void removeOneStar(int type);
	void doUpPressedWrapper(float dt);
	void doDownPressedWrapper(float dt);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	void quitButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
	void continueButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);

	void showWinOrLost(bool isWin);
	void showOperator(bool isShow = true);
private:
	Operator *_PlusOp;
	Operator *_MinusOp;
	Operator *_MultiplyOp;
	Operator *_DivideOp;
	std::vector<Number *> _Nums;
	std::vector<Operator *> _Ops;
	std::vector<int> _FreeOpPos;

	Sprite *_Board;

	int _Target;
	int _Result;
	int _Operator;
	int _FirstNum;
	int _PropNum;
	int _SecondNum;
	int _LeftTime;
	int _TotalTime;
	int _PropIndex;
	MathStep _MathStep;
	bool _IsPropGen;
	bool _IsWinOrLose;

	Label *_LeftLabel;
	std::vector<Operator *> _ShowOps;
	Label *_LeftLabel1;
	Label *_MiddleLabel;
	Label *_RightLabel;
	Monkey * _Monkey;
	DrawNode * _DrawNode;

	ui::Button * _LButton;
	ui::Button * _RButton;
	ui::Button * _UButton;
	ui::Button * _DButton;
	
	SEL_SCHEDULE _UpdateTimer;

	Sprite *_WinBackSprite;
	Sprite *_LostBackSprite;
	ui::Button *_QuitButton;
	ui::Button *_ContinueButton;

	EventListenerKeyboard *_Linster;
};
#endif // GameScene_h__
