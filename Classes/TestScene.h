//
//  TestScene.h
//  NWGestureLayer Test Scene
//
//  Created by Mitsuaki.N on 2013/12/20.
//
//

#ifndef __TestScene__
#define __TestScene__

#include "cocos2d.h"
#include "NWGestureLayer.hpp"

using namespace cocos2d;

class TestScene : public NWGestureLayer
{
    
public:
    TestScene();
    ~TestScene();
    
    // Create Scene
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC( TestScene );
    
    // Menu Selector
    void menuCallbackBackTitle( CCObject *pSender );
    
    // Callback Method.
    virtual void onDown( CCPoint &touchPoint, int id );
    virtual void onTap( CCPoint &touchPoint, int id );
    virtual void onSingleTap( CCPoint &touchPoint );
    virtual void onDoubleTap( CCPoint &touchPoint );
    
    virtual void onHold( CCPoint &touchPoint, int id );
    virtual void onScroll( CCPoint &touchPoint, int id );
    virtual void onFlick( CCPoint &touchPoint, int id, int direction );
    virtual void onSwipe( CCPoint &touchPoint, int id, int direction );
    virtual void onDrag( CCPoint &touchPoint, int id );
    virtual void onDragEnded( CCPoint &touchPoint, int id );
    
    virtual void onPinchIn( float magnification, int id1, int id2 );
    virtual void onPinchOut( float magnification, int id1, int id2 );
    virtual void onPinchAction( float magnification, int id1, int id2 );
    virtual void onPinchEnded( float magnification, int id1, int id2 );
    
    
    virtual void keyBackClicked(void);

private:
    static const int mTagRotateAnimation;
    CCSprite    *mSpriteDroid;
    
    bool        mIsPinchAction;
    float       mBaseScale;
};


#endif /* defined(__TestScene__) */
