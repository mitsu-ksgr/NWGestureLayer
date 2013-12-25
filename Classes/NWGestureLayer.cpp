//
//  NWGestureLayer.cpp
//  NoviceWorks
//
//  Created by Mitsuaki.N on 2013/11/29.
//
//

// std & platform
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <sys/time.h>

// cocos2dx
#include "cocos2d.h"

// myclass
#include "NWGestureLayer.hpp"


using namespace cocos2d;
using std::string;
using std::vector;


namespace {

#pragma -mark Support Functions

double getTimeOfDay() {
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 0.000001;
}


#pragma -mark TouchInfo Class
class TouchInfo : public CCObject {
public:
    int     id;
    double  startTime;      // sec
    bool    hasMoved;
    bool    hasHold;
    bool    hasEnded;
    vector<CCPoint> touchHistory;

    static TouchInfo* create( CCTouch *touch ) {
        TouchInfo *ti = new TouchInfo();
        if( !ti ) {
            CC_SAFE_DELETE( ti );
            return NULL;
        }
        ti->autorelease();
        
        // set info
        if( touch ) {
            ti->id = touch->getID();
            ti->insertHistory( touch );
        } else {
            ti->id = -1;
        }
        ti->startTime = getTimeOfDay();
        ti->hasMoved = false;
        ti->hasHold  = false;
        ti->hasEnded = false;

        return ti;
    }
    
    void insertHistory( CCTouch *touch ) {
        CCPoint p = touch->getLocation();
        touchHistory.push_back( p );
    }
    
    float getTotalDistance() {
        float total_distance = 0.0f;
        int size = this->touchHistory.size();
        for(int i = 1; i < size; ++i ) {
            CCPoint pre = this->touchHistory[i-1];
            CCPoint cur = this->touchHistory[i];
            total_distance += pre.getDistance( cur );
        }
        return total_distance;
    }
    
    int getDirection( float correction_val ) {
        CCPoint start = this->touchHistory[0];
        CCPoint end   = this->touchHistory.back();
        
        float dx = end.x - start.x;
        float dy = end.y - start.y;
        
        // Error correction
        if( dx != 0.0f ) {
            if( dx > 0.0f ) dx = dx < correction_val ? 0.0f : dx;
            else dx = dx > -correction_val ? 0.0f : dx;
        }
        
        // setup distance flag.
        int dist = 0;
        if( dx != 0.0f ) dist |= dx < 0.0f ? NWGestureLayer::LEFT : NWGestureLayer::RIGHT;
        if( dy != 0.0f ) dist |= dy < 0.0f ? NWGestureLayer::DOWN : NWGestureLayer::UP;

        return dist;
    }
};

void safeInsertObjectToCCArray( CCArray *ary, CCObject *obj, int index )
{
    int cnt = ary->count();
    if( index < cnt ) {
        ary->replaceObjectAtIndex( index, obj );
        return;
    }
    
    // insert dummy objects.
    for(int i = cnt; i < index; ++i ) {
        ary->addObject( TouchInfo::create( NULL ) );
    }
    ary->addObject( obj );
}


} // unnamed namespace


#pragma -mark Class Basic Method.
NWGestureLayer::NWGestureLayer() :
// Private Attribute
  mFirstTapId( -1 )
, mFirstTapTime( 0 )
, mFirstTapPoint( CCPointZero )
, mBaseDistanceOfPinch( 0.0f )
, mPreviousDistanceOfPinch( 0.0f )

// Common
, mDistanceThresholdForMoved( 0.0f )    // default value set in constructor.

// Config: Option
, mIsMultitapSupported( true )
, mIsPinchActionSupported( true )
 
// Config: SingleTap & DoubleTap
, mTimeThresholdForDoubleTap( 0.25 )

// Config: Hold & Drag
, mDetectionAccuracyOfHold( 0.1f )
, mTimeThresholdForHold( 1.0 )

// Config: Flick
, mTimeThresholdForFlick( 0.25 )
{
    CCLOG( "NWGestureLayer: constructor" );

    // get window value
    CCSize win_size = CCDirector::sharedDirector()->getWinSize();
    float win_diagonal = sqrtf(
            win_size.width * win_size.width +
            win_size.height * win_size.height );

    // base value for determine move or not.
    this->mDistanceThresholdForMoved = win_diagonal / 10.0f;
    
    // pinch
    mTouchIdForPinch[0] = -1;
    mTouchIdForPinch[1] = -1;
}

NWGestureLayer::~NWGestureLayer()
{
    CCLOG( "NWGestureLayer: destructor" );
    mTouchInfos.removeAllObjects();

    //CC_SAFE_RELEASE_NULL( &mTouchInfos );
}

bool NWGestureLayer::init()
{
    CCLOG( "NWGestureLayer: init" );
    if( !CCLayer::init() ) return false;
    
    this->setTouchEnabled( true );
    this->setTouchMode( kCCTouchesAllAtOnce );
    
    // schedule hold handler.
    this->schedule(
        schedule_selector( NWGestureLayer::scheduleHoldHandler ),
        this->mDetectionAccuracyOfHold );
    
    return true;
}

#pragma -mark Getter
vector<CCPoint>* NWGestureLayer::getTouchHistory( int id )
{
    int size = this->mTouchInfos.count();
    if( size <= id ) {
        return NULL;
    }
    
    TouchInfo *info = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex( id ) );
    return &info->touchHistory;
}
float NWGestureLayer::getTotalDistance( int id )
{
    int size = this->mTouchInfos.count();
    if( size <= id ) return 0.0f;
    
    TouchInfo *info = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex( id ) );
    return info->getTotalDistance();
}


#pragma -mark Cocos2dx Touch Event
void NWGestureLayer::ccTouchesBegan( CCSet *pTouches, CCEvent *pEvent )
{
    CCTouch *touch_id0 = NULL;

    for( CCSetIterator it = pTouches->begin(); it != pTouches->end(); ++it ) {
        CCTouch *touch = static_cast<CCTouch*>(*it);
        int id = touch->getID();
        
        // for single tap.
        if( id == 0 ) touch_id0 = touch;
        if( !this->mIsMultitapSupported && id ) continue;

        TouchInfo *ti = TouchInfo::create( touch );
        safeInsertObjectToCCArray( &this->mTouchInfos, ti, ti->id );
        
        // callback
        CCPoint touch_point = touch->getLocation();
        this->onDown( touch_point, ti->id );
        
        // pinch
        if( this->mIsPinchActionSupported ) this->pinchActionHandler( touch );
    }
    
    // callback
    if( !this->mIsMultitapSupported && touch_id0 ) {
        this->onTouchBegan( touch_id0, pEvent );
    } else {
        this->onTouchesBegan( pTouches, pEvent );
    }
}

void NWGestureLayer::ccTouchesMoved( CCSet *pTouches, CCEvent *pEvent )
{
    CCTouch *touch_id0 = NULL;
    
    for( CCSetIterator it = pTouches->begin(); it != pTouches->end(); ++it ) {
        CCTouch *touch = static_cast<CCTouch*>(*it);
        int id = touch->getID();
        
        // for single tap.
        if( id == 0 ) touch_id0 = touch;
        if( !this->mIsMultitapSupported && id ) continue;
        
        TouchInfo *info = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex(id) );
        
        // check move
        if( !info->hasMoved ) {
            float distance = info->touchHistory[0].getDistance( touch->getLocation() );
            if( distance > this->mDistanceThresholdForMoved ) {
                info->hasMoved = true;
            }
        }
        
        // insert history.
        info->insertHistory( touch );
        
        // pinch action.
        if( this->mIsPinchActionSupported && this->pinchActionHandler( touch ) ) {
            // pass.
        
        // moved! callback
        } else if( info->hasMoved ) {
            CCPoint touch_point = touch->getLocation();
            if( info->hasHold ) this->onDrag( touch_point, id );
            else                this->onScroll( touch_point, id );
        }
    }
    
    // callback
    if( !this->mIsMultitapSupported && touch_id0 ) {
        this->onTouchMoved( touch_id0, pEvent );
    } else {
        this->onTouchesMoved( pTouches, pEvent );
    }
}

void NWGestureLayer::ccTouchesEnded( CCSet *pTouches, CCEvent *pEvent )
{
    CCTouch *touch_id0 = NULL;
    
    for(CCSetIterator it = pTouches->begin(); it != pTouches->end(); ++it ) {
        CCTouch *touch = static_cast<CCTouch*>(*it);
        int id = touch->getID();
        
        // for single tap.
        if( id == 0 ) touch_id0 = touch;
        if( !this->mIsMultitapSupported && id ) continue;
        
        TouchInfo *info = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex(id) );
        info->insertHistory( touch );
        info->hasEnded = true;
        
        // callback
        CCPoint touch_point = touch->getLocation();
        // end of drag.
        if( info->hasHold ) {
            this->onDragEnded( touch_point, id );
         
        // Pinch Action.
        } else if( this->mIsPinchActionSupported &&
                   this->pinchActionHandler( touch, true ) ) {
            // pass.
            
        // end of scroll
        } else if( info->hasMoved ) {
            // check time
            double scroll_time = getTimeOfDay() - info->startTime;
            int dir_flags = info->getDirection( this->getDistanceThresholdForMoved() );
            
            // is Flick!
            if( scroll_time < this->mTimeThresholdForFlick ) {
                this->onFlick( touch_point, id, dir_flags );
                
            // is Swipe
            } else {
                this->onSwipe( touch_point, id, dir_flags );
            }
            
        // end of Tap.
        } else if( !info->hasMoved ) {
            this->onTap( touch_point, id );
            this->tapEventManager( touch );
        }
    }
    
    // callback
    if( !this->mIsMultitapSupported && touch_id0 ) {
        this->onTouchEnded( touch_id0, pEvent );
    } else {
        this->onTouchesEnded( pTouches, pEvent );
    }
}

void NWGestureLayer::ccTouchesCancelled( CCSet *pTouches, CCEvent *pEvent )
{
    CCTouch *touch_id0 = NULL;
    
    for(CCSetIterator it = pTouches->begin(); it != pTouches->end(); ++it ) {
        CCTouch *touch = static_cast<CCTouch*>(*it);
        int id = touch->getID();
        
        // for single tap.
        if( id == 0 ) touch_id0 = touch;
        if( !this->mIsMultitapSupported && id ) continue;
        
        TouchInfo *info = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex(id) );
        info->insertHistory( touch );
        info->hasEnded = true;
        
        CCPoint touch_point = touch->getLocation();
        this->onCancelled( touch_point, id );

        // pinch
        if( this->mIsPinchActionSupported ) this->pinchActionHandler( touch, true );
    }
    
    // callback
    if( !this->mIsMultitapSupported && touch_id0 ) {
        this->onTouchCancelled( touch_id0, pEvent );
    } else {
        this->onTouchesCancelled( pTouches, pEvent );
    }
}


#pragma -mark SingeTap or DoubleTap
// this func will used in schedule.
void NWGestureLayer::scheduleSingleTapHandler()
{
    if( this->mFirstTapId < 0 ) return;
    TouchInfo *info = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex( this->mFirstTapId ) );
    if( info ) {
        onSingleTap( info->touchHistory.back() );
    }
    
    this->mFirstTapId = -1;
    this->mFirstTapTime = 0;
    this->mFirstTapPoint = CCPointZero;
}

void NWGestureLayer::tapEventManager( CCTouch *touch )
{
    // stop singletap handler.
    this->unschedule( schedule_selector( NWGestureLayer::scheduleSingleTapHandler ) );

    // Check Double Tap
    do {
        // check ID.
        if( touch->getID() != this->mFirstTapId ) break;
        
        // check tap interval
        double interval = getTimeOfDay() - this->mFirstTapTime;
        if( interval > this->mTimeThresholdForDoubleTap ) break;
        
        // check tap distance.
        float distance = this->mFirstTapPoint.getDistance( touch->getLocation() );
        if( distance > this->mDistanceThresholdForMoved ) break;
        
        // DoubleTap!
        CCPoint tap_point = touch->getLocation();
        onDoubleTap( tap_point );
        
        // finish.
        this->mFirstTapId = -1;
        this->mFirstTapTime = 0;
        this->mFirstTapPoint = CCPointZero;
        return;
        
    } while(0);
    
    // Reset! new tap.
    this->mFirstTapId = touch->getID();
    this->mFirstTapTime = getTimeOfDay();
    this->mFirstTapPoint = touch->getLocation();
    
    // start a schedule for the SingleTap callback.
    this->scheduleOnce(
        schedule_selector( NWGestureLayer::scheduleSingleTapHandler ),
        this->mTimeThresholdForDoubleTap );
}

#pragma -mark Hold Action
void NWGestureLayer::scheduleHoldHandler()
{
    double now = getTimeOfDay();

    CCObject *element;
    CCARRAY_FOREACH( &this->mTouchInfos, element ) {
        TouchInfo *ti = static_cast<TouchInfo*>( element );
        if( ti->id == -1 || ti->hasMoved || ti->hasEnded || ti->hasHold ) {
            continue;
        }
        // is pinch ---> continue;
        if( (this->mTouchIdForPinch[0] != -1 && this->mTouchIdForPinch[1] != -1) &&
            (ti->id == this->mTouchIdForPinch[0] || ti->id == this->mTouchIdForPinch[1]) ) {
                continue;
        }

        double elapsed_time = now - ti->startTime;
        if( elapsed_time > this->mTimeThresholdForHold ) {
            ti->hasHold = true;
            CCPoint hold_point = ti->touchHistory.back();
            this->onHold( hold_point, ti->id );
        }
    }
}

#pragma -mark Pinch Action
float NWGestureLayer::getDistanceBetweenTwoTouch( int id1, int id2 )
{
    TouchInfo *t1, *t2;
    t1 = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex( id1 ) );
    t2 = static_cast<TouchInfo*>( this->mTouchInfos.objectAtIndex( id2 ) );
    
    CCPoint t1p = t1->touchHistory.back();
    CCPoint t2p = t2->touchHistory.back();
    
    return t1p.getDistance( t2p );
}

// return is pinch action.
bool NWGestureLayer::pinchActionHandler( CCTouch *touch, bool is_end )
{
    do {
        int id = touch->getID();
        
        // finish.
        if( is_end ) {
            int id1 = this->mTouchIdForPinch[0];
            int id2 = this->mTouchIdForPinch[1];
            int end_id = id == id1 ? id1 : id == id2 ? id2 : -1;
            if( end_id != -1 ) {
                // callback
                if( id1 != -1 && id2 != -1 ) {
                    float distance = fabsf( this->getDistanceBetweenTwoTouch( id1, id2 ) );
                    float magnification = 1.0f;
                    if( this->mBaseDistanceOfPinch != 0.0f ) {
                        magnification = distance / this->mBaseDistanceOfPinch;
                    }
                    this->onPinchEnded( magnification, id1, id2 );
                }

                this->mTouchIdForPinch[ end_id ] = -1;
                this->mBaseDistanceOfPinch = 0.0f;
                this->mPreviousDistanceOfPinch = 0.0f;
            }
            break;
        }
        
        // setting finger id
        bool is_new_register = false;
        if( this->mTouchIdForPinch[0] != id &&
            this->mTouchIdForPinch[1] != id ) {
            // register new touch id.
            if( this->mTouchIdForPinch[0] == -1 )      this->mTouchIdForPinch[0] = id;
            else if( this->mTouchIdForPinch[1] == -1 ) this->mTouchIdForPinch[1] = id;
            else break; // pass.
            
            is_new_register = true;
        }
        
        // check two finger.
        if( this->mTouchIdForPinch[0] == -1 || this->mTouchIdForPinch[1] == -1 ) {
            break;
        }
        
        // alias
        int id1 = this->mTouchIdForPinch[0];
        int id2 = this->mTouchIdForPinch[1];
        
        // if there is a new registration, calculate base distance.
        if( is_new_register ) {
            this->mPreviousDistanceOfPinch = this->mBaseDistanceOfPinch = fabsf(
                this->getDistanceBetweenTwoTouch( id1, id2 )
            );
        }
        
        // get magnification
        float distance = fabsf( this->getDistanceBetweenTwoTouch( id1, id2 ) );
        float magnification = 1.0f;
        if( this->mBaseDistanceOfPinch != 0.0f ) {
            magnification = distance / this->mBaseDistanceOfPinch;
        }
        
        // callback
        this->onPinchAction( magnification, id1, id2 );
        if( distance < this->mPreviousDistanceOfPinch ) {
            this->onPinchIn( magnification, id1, id2 );
        } else {
            this->onPinchOut( magnification, id1, id2 );
        }
        
        this->mPreviousDistanceOfPinch = distance;
        return true;
    } while(0);
    return false;
}


