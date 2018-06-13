#include "Operator.h"

bool Operator::init()
{
	if (!Sprite::init())
	{
		return false;
	}

	return true;
}

void Operator::setType(int Type)
{
	_Type = Type;
	std::string filename = "";
	switch (_Type)
	{
	case 0:
		filename = "plus.png";
		break;
	case 1:
		filename = "minus.png";
		break;
	case 2:
		filename = "multiply.png";
		break;
	case 3:
		filename = "divide.png";
		break;
	default:
		break;
	}
	_Sprite = Sprite::create(filename);
	addChild(_Sprite);
}

int Operator::getType()
{
	return _Type;
}
