#ifndef __NUMBER_H__
#define __NUMBER_H__

#include "cocos2d.h"
using namespace cocos2d;
class Number : public Sprite
{
public:
	enum class PropType {
		COUNTER_ADD = 11,
		COUNTER_SUB,
		RESULT_CHANGE,
		NUMBER_CHANGE,
		OPERATOR_CHANGE,
		NUMBER_REINIT,
		OPERATOR_POS,
		MONKEY_POS
	};
public:
	CREATE_FUNC(Number);
	bool init();
	void update(float dt);
	void flickAnimation();
	void setNumber(int num);
	int getNumber() { return _Num; }
	bool getAvai();
	void setAvai(bool isAvai);
private:
	//1-9 正常数字
	//11-增加计时时间 12-减少计时时间
	//13-目标结果改变 14-已经吃的数字变一下 15-已经吃的运算符变一下 16-重新生成一遍数字
	int _Num;
	bool _Avai;
	Sprite *sp;
};

#endif // __NUMBER_H__
