/****************************************************************************
Copyright (c) 2009-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "dtTransition.h"
#include "dtActionInterval.h"
#include "dtActionInstant.h"
#include "dtActionEase.h"
#include "dtLayer.h"
#include "dtDirector.h"
#include "base/dtEventDispatcher.h"

NS_DT_BEGIN

const unsigned int kSceneFade = 0xFADEFADE;

TransitionScene::TransitionScene()
: _inScene(nullptr)
, _outScene(nullptr)
, _duration(0.0f)
, _isInSceneOnTop(false)
, _isSendCleanupToScene(false)
{
}

TransitionScene::~TransitionScene()
{
    DT_SAFE_RELEASE(_inScene);
    DT_SAFE_RELEASE(_outScene);
}

TransitionScene * TransitionScene::create(float t, Scene *scene)
{
    TransitionScene * pScene = new (std::nothrow) TransitionScene();
    if(pScene && pScene->initWithDuration(t,scene))
    {
        pScene->autorelease();
        return pScene;
    }
    DT_SAFE_DELETE(pScene);
    return nullptr;
}

bool TransitionScene::initWithDuration(float t, Scene *scene)
{
    if(scene == nullptr)
    {
        return false;
    }

    if (Scene::init())
    {
        _duration = t;

        // retain
        _inScene = scene;
        _inScene->retain();
        _outScene = Director::getInstance()->getRunningScene();
        if (_outScene == nullptr)
        {
            _outScene = Scene::create();
            // just change its state is running that can run actions later
            // issue: https://github.com/cocos2d/cocos2d-x/issues/17442
            _outScene->onEnter();
        }
        _outScene->retain();
        
        sceneOrder();

        return true;
    }
    else
    {
        return false;
    }
}

void TransitionScene::sceneOrder()
{
    _isInSceneOnTop = true;
}

void TransitionScene::draw(Renderer *renderer, const Transform &transform)
{
    Scene::draw(renderer, transform);

    if( _isInSceneOnTop ) {
        _outScene->visit(renderer, transform);
        _inScene->visit(renderer, transform);
    } else {
        _inScene->visit(renderer, transform);
        _outScene->visit(renderer, transform);
    }
}

void TransitionScene::finish()
{
    // clean up
    _inScene->setVisible(true);
    _inScene->setPosition(0,0);

    _outScene->setVisible(false);
    _outScene->setPosition(0,0);

    //[self schedule:@selector(setNewScene:) interval:0];
    this->schedule(DT_SCHEDULE_SELECTOR(TransitionScene::setNewScene), 0);
}

void TransitionScene::setNewScene(float /*dt*/)
{    
    this->unschedule(DT_SCHEDULE_SELECTOR(TransitionScene::setNewScene));
    
    // Before replacing, save the "send cleanup to scene"
    Director *director = Director::getInstance();
    _isSendCleanupToScene = director->isSendCleanupToScene();
    
    director->replaceScene(_inScene);
    
    // issue #267
    _outScene->setVisible(true);
}

void TransitionScene::hideOutShowIn()
{
    _inScene->setVisible(true);
    _outScene->setVisible(false);
}


// custom onEnter
void TransitionScene::onEnter()
{
    
    Scene::onEnter();
    
    // disable events while transitions
    _eventDispatcher->setEnabled(false);
    
    // outScene should not receive the onEnter callback
    // only the onExitTransitionDidStart
    _outScene->onExitTransitionDidStart();
    
    _inScene->onEnter();
}

// custom onExit
void TransitionScene::onExit()
{
    
    Scene::onExit();
    
    // enable events while transitions
    _eventDispatcher->setEnabled(true);
    _outScene->onExit();

    // _inScene should not receive the onEnter callback
    // only the onEnterTransitionDidFinish
    _inScene->onEnterTransitionDidFinish();

}

// custom cleanup
void TransitionScene::cleanup()
{
    
    Scene::cleanup();

    if( _isSendCleanupToScene )
        _outScene->cleanup();
}

//
// Oriented Transition
//

TransitionSceneOriented::TransitionSceneOriented()
{
}

TransitionSceneOriented::~TransitionSceneOriented()
{
}

TransitionSceneOriented * TransitionSceneOriented::create(float t, Scene *scene, Orientation orientation)
{
    TransitionSceneOriented * newScene = new (std::nothrow) TransitionSceneOriented();
    newScene->initWithDuration(t,scene,orientation);
    newScene->autorelease();
    return newScene;
}

bool TransitionSceneOriented::initWithDuration(float t, Scene *scene, Orientation orientation)
{
    if ( TransitionScene::initWithDuration(t, scene) )
    {
        _orientation = orientation;
    }
    return true;
}


//
// MoveInL
//
TransitionMoveInL::TransitionMoveInL()
{
}

TransitionMoveInL::~TransitionMoveInL()
{
}

TransitionMoveInL* TransitionMoveInL::create(float t, Scene* scene)
{
    TransitionMoveInL* newScene = new (std::nothrow) TransitionMoveInL();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInL::onEnter()
{
    TransitionScene::onEnter();
    this->initScenes();

    ActionInterval *a = this->action();

    _inScene->runAction
    (
        Sequence::create
        (
            this->easeActionWithAction(a),
            CallFunc::create(DT_CALLBACK_0(TransitionScene::finish,this)),
            nullptr
        )
    );
}
 
ActionInterval* TransitionMoveInL::action()
{
    return MoveTo::create(_duration, Vec2(0.0f,0.0f));
}

ActionInterval* TransitionMoveInL::easeActionWithAction(ActionInterval* action)
{
    return EaseOut::create(action, 2.0f);
//    return [EaseElasticOut actionWithAction:action period:0.4f];
}

void TransitionMoveInL::initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(-s.width,0);
}

//
// MoveInR
//
TransitionMoveInR::TransitionMoveInR()
{
}
TransitionMoveInR::~TransitionMoveInR()
{
}

TransitionMoveInR* TransitionMoveInR::create(float t, Scene* scene)
{
    TransitionMoveInR* newScene = new (std::nothrow) TransitionMoveInR();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInR::initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(s.width,0);
}

//
// MoveInT
//
TransitionMoveInT::TransitionMoveInT()
{
}
TransitionMoveInT::~TransitionMoveInT()
{
}

TransitionMoveInT* TransitionMoveInT::create(float t, Scene* scene)
{
    TransitionMoveInT* newScene = new (std::nothrow) TransitionMoveInT();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInT::initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(0,s.height);
}

//
// MoveInB
//
TransitionMoveInB::TransitionMoveInB()
{
}
TransitionMoveInB::~TransitionMoveInB()
{
}

TransitionMoveInB* TransitionMoveInB::create(float t, Scene* scene)
{
    TransitionMoveInB* newScene = new (std::nothrow) TransitionMoveInB();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInB::initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(0,-s.height);
}


//
// SlideInL
//

// The adjust factor is needed to prevent issue #442
// One solution is to use DONT_RENDER_IN_SUBPIXELS images, but NO
// The other issue is that in some transitions (and I don't know why)
// the order should be reversed (In in top of Out or vice-versa).
#define ADJUST_FACTOR 0.5f
TransitionSlideInL::TransitionSlideInL()
{
}

TransitionSlideInL::~TransitionSlideInL()
{
}

void TransitionSlideInL::onEnter()
{
    TransitionScene::onEnter();
    this->initScenes();

    ActionInterval *in = this->action();
    ActionInterval *out = this->action();

    ActionInterval* inAction = easeActionWithAction(in);
    ActionInterval* outAction = Sequence::create
    (
       easeActionWithAction(out),
       CallFunc::create(DT_CALLBACK_0(TransitionScene::finish,this)),
       nullptr
    );
    _inScene->runAction(inAction);
    _outScene->runAction(outAction);
}

void TransitionSlideInL::sceneOrder()
{
    _isInSceneOnTop = false;
}

void TransitionSlideInL:: initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(-(s.width-ADJUST_FACTOR),0.0f);
}

ActionInterval* TransitionSlideInL::action()
{
    Size s = Director::getInstance()->getCanvasSize();
    return MoveBy::create(_duration, Vec2(s.width-ADJUST_FACTOR,0.0f));
}

ActionInterval* TransitionSlideInL::easeActionWithAction(ActionInterval* action)
{
    return EaseOut::create(action, 2.0f);
}

TransitionSlideInL* TransitionSlideInL::create(float t, Scene* scene)
{
    TransitionSlideInL* newScene = new (std::nothrow) TransitionSlideInL();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

//
// SlideInR
//
TransitionSlideInR::TransitionSlideInR()
{
}
TransitionSlideInR::~TransitionSlideInR()
{
}

TransitionSlideInR* TransitionSlideInR::create(float t, Scene* scene)
{
    TransitionSlideInR* newScene = new (std::nothrow) TransitionSlideInR();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionSlideInR::sceneOrder()
{
    _isInSceneOnTop = true;
}

void TransitionSlideInR::initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(s.width-ADJUST_FACTOR,0);
}


ActionInterval* TransitionSlideInR:: action()
{
    Size s = Director::getInstance()->getCanvasSize();
    return MoveBy::create(_duration, Vec2(-(s.width-ADJUST_FACTOR),0.0f));
}


//
// SlideInT
//
TransitionSlideInT::TransitionSlideInT()
{
}
TransitionSlideInT::~TransitionSlideInT()
{
}

TransitionSlideInT* TransitionSlideInT::create(float t, Scene* scene)
{
    TransitionSlideInT* newScene = new (std::nothrow) TransitionSlideInT();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionSlideInT::sceneOrder()
{
    _isInSceneOnTop = false;
}

void TransitionSlideInT::initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(0,s.height-ADJUST_FACTOR);
}


ActionInterval* TransitionSlideInT::action()
{
    Size s = Director::getInstance()->getCanvasSize();
    return MoveBy::create(_duration, Vec2(0.0f,-(s.height-ADJUST_FACTOR)));
}

//
// SlideInB
//
TransitionSlideInB::TransitionSlideInB()
{
}
TransitionSlideInB::~TransitionSlideInB()
{
}

TransitionSlideInB* TransitionSlideInB::create(float t, Scene* scene)
{
    TransitionSlideInB* newScene = new (std::nothrow) TransitionSlideInB();
    if(newScene && newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    DT_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionSlideInB::sceneOrder()
{
    _isInSceneOnTop = true;
}

void TransitionSlideInB:: initScenes()
{
    Size s = Director::getInstance()->getCanvasSize();
    _inScene->setPosition(0,-(s.height-ADJUST_FACTOR));
}


ActionInterval* TransitionSlideInB:: action()
{
    Size s = Director::getInstance()->getCanvasSize();
    return MoveBy::create(_duration, Vec2(0.0f,s.height-ADJUST_FACTOR));
}


//
// Fade Transition
//
TransitionFade::TransitionFade()
{
}
TransitionFade::~TransitionFade()
{
}

TransitionFade * TransitionFade::create(float duration, Scene *scene, const DTRGB& color)
{
    TransitionFade * transition = new (std::nothrow) TransitionFade();
    transition->initWithDuration(duration, scene, color);
    transition->autorelease();
    return transition;
}

TransitionFade* TransitionFade::create(float duration,Scene* scene)
{
    return TransitionFade::create(duration, scene, DTRGB(0,0,0));
}

bool TransitionFade::initWithDuration(float duration, Scene *scene, const DTRGB& color)
{
    if (TransitionScene::initWithDuration(duration, scene))
    {
        _color.r = color.r;
        _color.g = color.g;
        _color.b = color.b;
    }
    return true;
}

bool TransitionFade::initWithDuration(float t, Scene *scene)
{
    this->initWithDuration(t, scene, DTRGB(0,0,0));
    return true;
}

void TransitionFade :: onEnter()
{
    TransitionScene::onEnter();

    LayerColor* l = LayerColor::create(_color);
    _inScene->setVisible(false);

    addChild(l, 2, kSceneFade);
    Node* f = getChildByTag(kSceneFade);

    auto a = Sequence::create
        (
            FadeIn::create(_duration/2),
            CallFunc::create(DT_CALLBACK_0(TransitionScene::hideOutShowIn,this)),
            FadeOut::create(_duration/2),
            CallFunc::create(DT_CALLBACK_0(TransitionScene::finish,this)),

         nullptr
        );
    f->runAction(a);
}

void TransitionFade::onExit()
{
    TransitionScene::onExit();
    this->removeChildByTag(kSceneFade, false);
}



NS_DT_END
