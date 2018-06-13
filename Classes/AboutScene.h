#ifndef AboutScene_h__
#define AboutScene_h__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;

USING_NS_CC;

class AboutScene : public Layer
{
public:
	static Scene *createScene();

	virtual bool init();

	CREATE_FUNC(AboutScene);

	void closeButtonTouchEvent(Ref *pSender, ui::Widget::TouchEventType type);
};
#endif // AboutScene_h__
