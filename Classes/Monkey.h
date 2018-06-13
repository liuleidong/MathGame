#ifndef __MONKEY_H__
#define __MONKEY_H__

#include "cocos2d.h"
using namespace cocos2d;
class Monkey : public Sprite
{
public:
	enum class Direction {
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	enum class Status {
		LAND,
		TREE,
		HOLE
	};

public:
	CREATE_FUNC(Monkey);
	bool init();

	void reset();

	void idle(int type);//0-½��  1-����  2-��
	void jump();

	void runLeft(float dt);
	void runRight(float dt);
	void moveLeft(float dt);
	void moveRight(float dt);

	void climbUp(float dt);
	void climbDown(float dt);

	void update(float dt);//1-��  2-��
	void runAnimation();
	void climbAnimation();
	void cryAnimation();
	void stopAnimationByStatus(Direction d);

	//�жϺ������������λ�ã��Ƿ�ת������״̬
	int judgeMonkeyPosWithTree();

	void doUpPressed();
	void doUpReleased();
	void doDownPressed();
	void doDownReleased();
	void doLeftPressed();
	void doLeftReleased();
	void doRightPressed();
	void doRightReleased();

	void setLandIdx();
	void holeCallBack(float dt);

	void playEffect(const char* pszFilePath);
	void stopEffect();
public:
	bool isJumping;
	bool isLeft;
	bool isRunL;
	bool isRunR;
	bool isClimbU;
	bool isClimbD;
	bool isDropDown;
	bool isUpLand;	//��ʶ�Ƿ����ϲ���½����
	int moveStep;	//0-δ�ƶ� 1-��ʼ�ƶ� 2-�����ƶ�
	Status _Status;

	Sprite * sp;
	Action * run;
	Action * climb;
	Action * jmp;
	Action * cry;

	float vy;
	float g;
	float oy;

	std::vector<Vec2> _TreePos;
	int _CurTreeIdx;
	int _CurLandIdx;
	int _DelayCount;

	int _RunSoundID;
	int _DiSoundID;
	int _DangSoundID;
	bool _IsSoundOn;

	std::vector<Rect> _LandPos;
	std::vector<Rect> _HolePos;
};

#endif // __MONKEY_H__