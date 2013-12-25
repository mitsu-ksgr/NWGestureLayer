//
//  TestScene.cpp
//  MyTest
//
//  Created by Mitsuaki.N on 2013/12/20.
//
//


#include "TestScene.h"

using namespace cocos2d;

using std::string;


namespace {

string getStrDirection( int dir )
{
    string log("");
    
    if(NWGestureLayer::isUpDir(dir))    log.append( "UP " );
    if(NWGestureLayer::isDownDir(dir))  log.append( "DOWN " );
    if(NWGestureLayer::isLeftDir(dir))  log.append( "LEFT " );
    if(NWGestureLayer::isRightDir(dir)) log.append( "RIGHT " );
    if(NWGestureLayer::isNoVerticalDir(dir))    log.append( "NoVertical " );
    if(NWGestureLayer::isNoHorizontalDir(dir))  log.append( "NoHorizontal " );
    
    return log;
}

}   // unnamed namespace


const int TestScene::mTagRotateAnimation = 1;

#pragma -mark Scene Basic Method
CCScene* TestScene::scene()
{
    CCScene *scene = CCScene::create();
    TestScene *layer = TestScene::create();
    scene->addChild(layer);
    return scene;
}

// Constructor
TestScene::TestScene() :
  mSpriteDroid( NULL )
, mBaseScale( 0.0f )
{
    CCLOG( "TestScene: constructor" );
}

TestScene::~TestScene()
{
    CCLOG( "TestScene: destructor" );
}

bool TestScene::init()
{
    // super init first.
    if( !NWGestureLayer::init() ) return false;
    this->setKeypadEnabled( true );
    
    // get window size.
    CCSize winsize = CCDirector::sharedDirector()->getWinSize();
    
    //-------------------- Create Background.
    CCLayerColor *layer_bg = CCLayerColor::create(
            ccc4( 175, 238, 238, 255),
            winsize.width, winsize.height );
    layer_bg->setPosition( CCPointZero );
    this->addChild( layer_bg );
    
    //-------------------- Create Log label.
    CCLabelTTF *label = CCLabelTTF::create( "Check log console", "", 48 );
    label->setColor( ccc3( 0, 0, 0 ) );
    label->setPosition( ccp( winsize.width*0.5f, winsize.height*0.9f ) );
    this->addChild( label, 2 );
    
    //-------------------- Create Droid Sprite.
    mSpriteDroid = CCSprite::create( "AndroidRobot.png" );
    mSpriteDroid->setPosition( ccp( winsize.width*0.5f, winsize.height*0.5f ) );
    mSpriteDroid->setColor( ccc3( 255, 255, 255 ) );
    this->addChild( mSpriteDroid );
    
    //-------------------- Create Close Button.
    CCMenuItemImage *btn_close = CCMenuItemImage::create(
        "CloseNormal.png", "CloseSelected.png",
        this, menu_selector( TestScene::menuCallbackBackTitle ) );
    btn_close->setPosition( ccp(winsize.width*0.95f, winsize.height*0.05f) );
    
    // Register button
    CCMenu *menu = CCMenu::create( btn_close, NULL );
    menu->setPosition( CCPointZero );
    this->addChild( menu, 1 );
    
    return true;
}

#pragma -mark Callback Method
void TestScene::onDown( CCPoint &touchPoint, int id )
{
    CCLOG( "onDown[%d](%6.2f, %6.2f)", id, touchPoint.x, touchPoint.y );
}
void TestScene::onTap( CCPoint &touchPoint, int id )
{
    CCLOG( "onTap[%d](%6.2f, %6.2f)", id, touchPoint.x, touchPoint.y );
}

void TestScene::onSingleTap( CCPoint &touchPoint )
{
    CCLOG( "onSingleTap(%6.2f, %6.2f)", touchPoint.x, touchPoint.y );
    ccColor3B color = this->mSpriteDroid->getColor();
    ccColor3B next = ccWHITE;
    
    if( ccc3BEqual( color, ccWHITE ) )  next = ccYELLOW;
    else if( ccc3BEqual( color, ccYELLOW  ) ) next = ccBLUE;
    else if( ccc3BEqual( color, ccBLUE    ) ) next = ccGREEN;
    else if( ccc3BEqual( color, ccGREEN   ) ) next = ccRED;
    else if( ccc3BEqual( color, ccRED     ) ) next = ccMAGENTA;
    else if( ccc3BEqual( color, ccMAGENTA ) ) next = ccORANGE;
    else if( ccc3BEqual( color, ccORANGE  ) ) next = ccGRAY;
    else next = ccWHITE;
    
    this->mSpriteDroid->setColor( next );
    this->mSpriteDroid->setPosition( ccp( touchPoint.x, touchPoint.y ) );
}
void TestScene::onDoubleTap( CCPoint &touchPoint )
{
    CCLOG( "onDoubleTap(%6.2f, %6.2f)", touchPoint.x, touchPoint.y );
    CCSize winsize = CCDirector::sharedDirector()->getWinSize();
    this->mSpriteDroid->setPosition( ccp( winsize.width*0.5f, winsize.height*0.5f ) );
    this->mSpriteDroid->setScale( 1.0f );
    this->mSpriteDroid->setRotation( 0.0f );
}

#pragma -mark Swipe Action
void TestScene::onScroll( CCPoint &touchPoint, int id )
{
    CCLOG( "onScroll[%d](%6.2f, %6.2f)", id, touchPoint.x, touchPoint.y );
    this->mSpriteDroid->setPosition( ccp( touchPoint.x, touchPoint.y ) );
}
void TestScene::onFlick( CCPoint &touchPoint, int id, int direction )
{
    string str_dir = getStrDirection( direction );
    CCLOG( "onFlick[%d](%6.2f, %6.2f) Direction: %s", id, touchPoint.x, touchPoint.y, str_dir.c_str() );
    
    //*/
    float distance = this->getTotalDistance( id );
    float angle = touchPoint.getAngle();
    
    float x = distance * cosf( angle );
    float y = distance * sinf( angle );
    if( this->isLeftDir( direction ) ) x = -x;
    if( this->isDownDir( direction ) ) y = -y;

    this->mSpriteDroid->setPosition( touchPoint );
    CCMoveBy *moveby = CCMoveBy::create( 0.5f, CCPoint( x, y ) );
    CCEaseOut *ease  = CCEaseOut::create( moveby, 3 );
    
    this->mSpriteDroid->runAction( ease );
    //*/
}
void TestScene::onSwipe( CCPoint &touchPoint, int id, int direction )
{
    string str_dir = getStrDirection( direction );
    CCLOG( "onSwipe[%d](%6.2f, %6.2f) Direction: %s", id, touchPoint.x, touchPoint.y, str_dir.c_str() );
}

#pragma -mark Hold & Dragged
void TestScene::onHold( CCPoint &touchPoint, int id )
{
    CCLOG( "onHold[%d](%6.2f, %6.2f)", id, touchPoint.x, touchPoint.y );
    
    if( this->mSpriteDroid->getActionByTag( TestScene::mTagRotateAnimation ) == NULL ) {
        CCRotateBy *rotation = CCRotateBy::create(1.0f, 360.0f );
        CCRepeatForever *repeat = CCRepeatForever::create( rotation );
        repeat->setTag( TestScene::mTagRotateAnimation );
        this->mSpriteDroid->runAction( repeat );
    }
}
void TestScene::onDrag( CCPoint &touchPoint, int id )
{
    CCLOG( "onDrag[%d](%6.2f, %6.2f)", id, touchPoint.x, touchPoint.y );
    this->mSpriteDroid->setPosition( touchPoint );
}
void TestScene::onDragEnded( CCPoint &touchPoint, int id )
{
    CCLOG( "onDragEnded[%d](%6.2f, %6.2f)", id, touchPoint.x, touchPoint.y );
    
    CCAction *act = this->mSpriteDroid->getActionByTag( TestScene::mTagRotateAnimation );
    if( act ) {
        this->mSpriteDroid->stopAction( act );
    }
}

#pragma -mark Pinch Action
void TestScene::onPinchIn( float magnification, int id1, int id2 )
{
    CCLOG( "onPinchIn[%d>-<%d] Magnification: %.3f", id1, id2, magnification );
}
void TestScene::onPinchOut( float magnification, int id1, int id2 )
{
    CCLOG( "onPinchOut[%d>-<%d] Magnification: %.3f", id1, id2, magnification );
}
void TestScene::onPinchAction( float magnification, int id1, int id2 )
{
    CCLOG( "onPinchAction[%d>-<%d] Magnification: %.3f", id1, id2, magnification );
    if( this->mIsPinchAction ) {
        this->mSpriteDroid->setScale( this->mBaseScale * magnification );
    } else {
        this->mIsPinchAction = true;
        this->mBaseScale = this->mSpriteDroid->getScale();
    }
}
void TestScene::onPinchEnded( float magnification, int id1, int id2 )
{
    this->mIsPinchAction = false;
    CCLOG( "onPinchEnded[%d>-<%d] Magnification: %.3f", id1, id2, magnification );
}

#pragma -mark Menu Selector.
// Menu Selector: goto Title.
void TestScene::menuCallbackBackTitle( CCObject *pSender )
{
    CCLOG( "Pressed Finish Button" );
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void TestScene::keyBackClicked()
{
    this->menuCallbackBackTitle( NULL );
}





