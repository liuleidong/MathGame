#ifndef SettingScene_h__
#define SettingScene_h__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;

USING_NS_CC;

class SettingScene : public Layer
{
public:
	static Scene *createScene();

	virtual bool init();

	CREATE_FUNC(SettingScene);

	void closeButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
	void selectedMusic(Ref *pSender, ui::CheckBox::EventType type);
	void selectedSound(Ref *pSender, ui::CheckBox::EventType type);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
};
#endif // SettingScene_h__
