#ifndef Operator_h__
#define Operator_h__

#include "cocos2d.h"

USING_NS_CC;

class Operator : public Sprite
{
public:
	CREATE_FUNC(Operator);

	bool init();

	void setType(int Type);
	int getType();
private:
	int _Type;	// 0:+ 1:- 2:* 3:-
	Sprite *_Sprite;
};
#endif // Operator_h__
