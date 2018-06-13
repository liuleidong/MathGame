#ifndef HomeScene_h__
#define HomeScene_h__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;

USING_NS_CC;

class HomeScene : public Layer
{
public:
	static Scene *createScene();

	virtual bool init();

	CREATE_FUNC(HomeScene);

	void startButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
	void settingButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
	void aboutButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
};
#endif // HomeScene_h__
