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
	//1-9 ��������
	//11-���Ӽ�ʱʱ�� 12-���ټ�ʱʱ��
	//13-Ŀ�����ı� 14-�Ѿ��Ե����ֱ�һ�� 15-�Ѿ��Ե��������һ�� 16-��������һ������
	int _Num;
	bool _Avai;
	Sprite *sp;
};

#endif // __NUMBER_H__
