//
//  NWGestureLayer.hpp
//  NoviceWorks
//
//  Created by Mitsuaki.N on 2013/12/17.
//
//

#ifndef __NWGestureLayer__
#define __NWGestureLayer__

#include "cocos2d.h"

/**
 *  @class  NWGestureLayer
 *  @brief  Layer for detecting gestures.
 *
 *  @author  Mitsuaki.N
 *  @date    create on 2013/12/17
 *  @version 1.0.0
 */
class NWGestureLayer : public cocos2d::CCLayer
{
public:
    //////////////////////////////////////////////////////////////////////
    // Enum Type
    //////////////////////////////////////////////////////////////////////
    /**
     *  @enum   Direction
     *  @brief  Define the directions. used by flick functions.
     */
    enum Direction {
        UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8,
    };
    static bool isUpDir( int dir )          { return dir &  UP;   }
    static bool isDownDir( int dir )        { return dir & DOWN;  }
    static bool isLeftDir( int dir )        { return dir & LEFT;  }
    static bool isRightDir( int dir )       { return dir & RIGHT; }
    static bool isNoVerticalDir( int dir )    { return !(dir&( UP |DOWN ));}
    static bool isNoHorizontalDir( int dir )  { return !(dir&(LEFT|RIGHT));}


    //////////////////////////////////////////////////////////////////////
    // NWGestureLayer Methods.
    //////////////////////////////////////////////////////////////////////
    // class method
    NWGestureLayer();
    virtual ~NWGestureLayer();
    virtual bool init();

    // Override touch events.
    virtual void ccTouchesBegan( cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent );
    virtual void ccTouchesMoved( cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent );
    virtual void ccTouchesEnded( cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent );
    virtual void ccTouchesCancelled( cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent );
    
    
    //////////////////////////////////////////////////////////////////////
    // Accessor
    //////////////////////////////////////////////////////////////////////
    /**
     *  Set whether to support the Multi-tap.
     *  @warning This func may not call from except for init().
     */
    void setMulitapSupport( bool is_supported ) {
        this->mIsMultitapSupported = is_supported;
    }
    bool isMultitapSupport() {
        return this->mIsMultitapSupported;
    }
    
    /**
     *  Set whether to support the PinchAction.
     */
    void setPinchActionSupport( bool is_supported ) {
        this->mIsPinchActionSupported = is_supported;
    }
    bool isPinchActionSupport() {
        return this->mIsPinchActionSupported;
    }

    /**
     *  Set the Base distance for determine moved or not.
     */
    void setDistanceThresholdForMoved( float distance ) {
        this->mDistanceThresholdForMoved = distance;
    }
    float getDistanceThresholdForMoved() {
        return this->mDistanceThresholdForMoved;
    }
    
    /**
     *  Set time threshold for determining Single-tap or Double-tap.
     *  if there is the next tap within this time,
     *  those tap is determined to the Double-tap.
     *  @param  time    sec.
     */
    void setTimeThresholdForDoubleTap( double time ) {
        this->mTimeThresholdForDoubleTap = time;
    }
    double getTimeThresholdForDoubleTap() {
        return this->mTimeThresholdForDoubleTap;
    }
    
    /**
     *  Set detection accuracy of hold.
     *  It's the schedule interval value for monitoring the hold.
     *  @param  time    sec.
     */
    void setDetectionAccuracyOfHold( float time ) {
        this->mDetectionAccuracyOfHold = time;
    }
    float getDetectionAccuracyOfHold() {
        return this->mDetectionAccuracyOfHold;
    }
    
    /**
     *  Set time threshold for detecting the Hold.
     *  @param  time    sec.
     */
    void setTimeThresholdForHold( double time ) {
        this->mTimeThresholdForHold = time;
    }
    double getTimeThresholdForHold() {
        return this->mTimeThresholdForHold;
    }
    
    /**
     *  Set time threshold for decide whether Flick or Swipe.
     *  if touch move is ended within this time,
     *  a touch movement is determined to Flick.
     *  @param  time    sec
     *  @warning I don't understand real method of detecting a Flick! ごめんよ！
     */
    void setTimeThresholdForFlick( double time ) {
        this->mTimeThresholdForFlick = time;
    }
    double getTimeThresholdForFlick() {
        return this->mTimeThresholdForFlick;
    }
    
    
    //////////////////////////////////////////////////////////////////////
    // Get touch infomation.
    //////////////////////////////////////////////////////////////////////
    /**
     *  Get tap path.
     *  @param id   this id is passed to each callback func.
     *  @warning Don't specify except passed id from callback funcs.
     */
    std::vector<cocos2d::CCPoint>* getTouchHistory( int id = 0 );
    
    /**
     *  Get total move distance of tap.
     *  @param id   this id is passed to each callback func.
     *  @warning Don't specify except passed id from callback funcs.
     */
    float getTotalDistance( int id = 0 );
    
    /**
     *  Get direction of move from start ponit to end point.
     *  direction is a collection of Direction Flags.
     *  @param id   this id is passed to each callback func.
     *  @return Direction of between touch start to end.
     *  @warning Don't specify except passed id from callback funcs.
     */
    int getDirection( int id = 0 );
    

    //////////////////////////////////////////////////////////////////////
    // Callback
    // -- according to your necessity override those callback func.
    //////////////////////////////////////////////////////////////////////
    // touch callback: Single-tap Mode only.
    virtual void onTouchBegan( cocos2d::CCTouch *touch, cocos2d::CCEvent *event ) {}
    virtual void onTouchMoved( cocos2d::CCTouch *touch, cocos2d::CCEvent *event ) {}
    virtual void onTouchEnded( cocos2d::CCTouch *touch, cocos2d::CCEvent *event ) {}
    virtual void onTouchCancelled( cocos2d::CCTouch *touch, cocos2d::CCEvent *event ) {}

    // touch callback: Multi-tap Mode only.
    virtual void onTouchesBegan( cocos2d::CCSet *touches, cocos2d::CCEvent *event ){}
    virtual void onTouchesMoved( cocos2d::CCSet *touches, cocos2d::CCEvent *event ){}
    virtual void onTouchesEnded( cocos2d::CCSet *touches, cocos2d::CCEvent *event ){}
    virtual void onTouchesCancelled( cocos2d::CCSet *touches, cocos2d::CCEvent *event ) {}

    
    // callback for touch actions.
    virtual void onSingleTap( cocos2d::CCPoint &touchPoint ) {}
    virtual void onDoubleTap( cocos2d::CCPoint &touchPoint ) {}
    
    virtual void onDown( cocos2d::CCPoint &touchPoint, int id ) {}
    virtual void onHold( cocos2d::CCPoint &touchPoint, int id ) {}
    virtual void onTap( cocos2d::CCPoint &touchPoint, int id ) {}
    virtual void onCancelled( cocos2d::CCPoint &touchPoint, int id ) {}
    
    virtual void onScroll( cocos2d::CCPoint &touchPoint, int id ) {}
    virtual void onFlick( cocos2d::CCPoint &touchPoint, int id, int direction ) {}
    virtual void onSwipe( cocos2d::CCPoint &touchPoint, int id, int direction ) {}
    virtual void onDrag( cocos2d::CCPoint &touchPoint, int id ) {}
    virtual void onDragEnded( cocos2d::CCPoint &touchPoint, int id ) {}
    
    virtual void onPinchIn( float magnification, int id1, int id2 ) {}
    virtual void onPinchOut( float magnification, int id1, int id2 ) {}
    virtual void onPinchAction( float magnification, int id1, int id2 ) {}
    virtual void onPinchEnded( float magnification, int id1, int id2 ) {}
    
    
private:
    //////////////////////////////////////////////////////////////////////
    // Config Parameter
    //////////////////////////////////////////////////////////////////////
    // Common
    float   mDistanceThresholdForMoved;
    
    // SingleTap & DoubleTap
    double  mTimeThresholdForDoubleTap;
    
    // Hold & Drag
    float   mDetectionAccuracyOfHold;
    double  mTimeThresholdForHold;

    // Flick
    double  mTimeThresholdForFlick;


    //////////////////////////////////////////////////////////////////////
    // Private Attribute and Functions.
    //////////////////////////////////////////////////////////////////////
    // Parameter
    bool    mIsMultitapSupported;
    bool    mIsPinchActionSupported;
    cocos2d::CCArray mTouchInfos;

    // SingleTap & DoubleTap
    int     mFirstTapId;
    double  mFirstTapTime;
    cocos2d::CCPoint mFirstTapPoint;
    
    void tapEventManager( cocos2d::CCTouch *touch);
    void scheduleSingleTapHandler();
    
    // Hold & Drag
    void scheduleHoldHandler();
    
    // PinchAction
    float   mBaseDistanceOfPinch;
    float   mPreviousDistanceOfPinch;
    int     mTouchIdForPinch[2];
    
    float getDistanceBetweenTwoTouch( int id1, int id2 );
    bool pinchActionHandler( cocos2d::CCTouch *touch, bool is_end = false );
};


#endif /* defined(__NWGestureLayer__) */
