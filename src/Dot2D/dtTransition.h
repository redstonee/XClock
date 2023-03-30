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

#ifndef __DT_TRANSITION_H__
#define __DT_TRANSITION_H__

#include "dtScene.h"
#include "base/dtMacros.h"
#include "renderer/dtRenderer.h"
#include "dtNode.h"

NS_DT_BEGIN

/**
 * @addtogroup _2d
 * @{
 */

//static creation function macro
//c/c++ don't support object creation of using class name
//so, all classes need creation method.

class ActionInterval;
class Node;

/** @class TransitionEaseScene
 * @brief TransitionEaseScene can ease the actions of the scene protocol.
@since v0.8.2
@js NA
*/
class  TransitionEaseScene// : public Ref
{
public:
    /** Constructor.
     */
    virtual ~TransitionEaseScene() {}

    /** Returns the Ease action that will be performed on a linear action.
    @since v0.8.2
     *
     * @param action A given interval action.
     * @return The Ease action that will be performed on a linear action.
     */
    virtual ActionInterval * easeActionWithAction(ActionInterval * action) = 0;
};

/** @class TransitionScene
 * @brief Base class for Transition scenes.
*/
class  TransitionScene : public Scene
{
public:
    /** Orientation Type used by some transitions.
     */
    enum class Orientation
    {
        /// An horizontal orientation where the Left is nearer
        LEFT_OVER = 0,
        /// An horizontal orientation where the Right is nearer
        RIGHT_OVER = 1,
        /// A vertical orientation where the Up is nearer
        UP_OVER = 0,
        /// A vertical orientation where the Bottom is nearer
        DOWN_OVER = 1,
    };
    
    /** Creates a base transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionScene object.
     */
    static TransitionScene * create(float t, Scene *scene);

    /** Called after the transition finishes.
     */
    void finish();

    /** Used by some transitions to hide the outer scene.
     */
    void hideOutShowIn();

    Scene* getInScene() const{ return _inScene; }
    float getDuration() const { return _duration; }
    //
    // Overrides
    //
    virtual void draw(Renderer *renderer, const Transform &transform) override;
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void cleanup() override;
    
protected:
    TransitionScene();
    virtual ~TransitionScene();

    /** initializes a transition with duration and incoming scene */
    bool initWithDuration(float t,Scene* scene);
    
protected:
    virtual void sceneOrder();
    void setNewScene(float dt);

    Scene *_inScene;
    Scene *_outScene;
    float _duration;
    bool _isInSceneOnTop;
    bool _isSendCleanupToScene;
};

/** @class TransitionSceneOriented
 * @brief A Transition that supports orientation like.
 * Possible orientation: LeftOver, RightOver, UpOver, DownOver
 */
class  TransitionSceneOriented : public TransitionScene
{
public:
    /** Creates a transition with duration, incoming scene and orientation.
     * 
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @param orientation A given orientation: LeftOver, RightOver, UpOver, DownOver.
     * @return A autoreleased TransitionSceneOriented object.
     */
    static TransitionSceneOriented * create(float t,Scene* scene, Orientation orientation);
    
protected:
    TransitionSceneOriented();
    virtual ~TransitionSceneOriented();

    /** initializes a transition with duration and incoming scene */
    bool initWithDuration(float t,Scene* scene,Orientation orientation);

protected:
    Orientation _orientation;
};


/** @class TransitionMoveInL
 * @brief TransitionMoveInL:
Move in from to the left the incoming scene.
*/
class  TransitionMoveInL : public TransitionScene, public TransitionEaseScene
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionMoveInL object.
     */
    static TransitionMoveInL* create(float t, Scene* scene);

    /** Returns the action that will be performed. 
     * 
     * @return The action that will be performed.
     */
    virtual ActionInterval* action();

    virtual ActionInterval* easeActionWithAction(ActionInterval * action) override;

    //
    // Overrides
    //
    virtual void onEnter() override;

protected:
    TransitionMoveInL();
    virtual ~TransitionMoveInL();

protected:
    /** initializes the scenes */
    virtual void initScenes();

};

/** @class TransitionMoveInR
 * @brief TransitionMoveInR:
Move in from to the right the incoming scene.
*/
class  TransitionMoveInR : public TransitionMoveInL
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionMoveInR object.
     */
    static TransitionMoveInR* create(float t, Scene* scene);

protected:
    TransitionMoveInR();
    virtual ~TransitionMoveInR();

protected:
    virtual void initScenes();

};

/** @class TransitionMoveInT
 * @brief TransitionMoveInT:
Move in from to the top the incoming scene.
*/
class  TransitionMoveInT : public TransitionMoveInL 
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionMoveInT object.
     */
    static TransitionMoveInT* create(float t, Scene* scene);

protected:
    TransitionMoveInT();
    virtual ~TransitionMoveInT();

protected:
    virtual void initScenes();

};

/** @class TransitionMoveInB
 * @brief TransitionMoveInB:
Move in from to the bottom the incoming scene.
*/
class  TransitionMoveInB : public TransitionMoveInL
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionMoveInB object.
     */
    static TransitionMoveInB* create(float t, Scene* scene);

protected:
    TransitionMoveInB();
    virtual ~TransitionMoveInB();

protected:
    virtual void initScenes();

};

/** @class TransitionSlideInL
 * @brief TransitionSlideInL:
Slide in the incoming scene from the left border.
*/
class  TransitionSlideInL : public TransitionScene, public TransitionEaseScene
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionSlideInL object.
     */
    static TransitionSlideInL* create(float t, Scene* scene);

    virtual ActionInterval* easeActionWithAction(ActionInterval * action) override;

    /** Returns the action that will be performed by the incoming and outgoing scene.
     *
     * @return The action that will be performed by the incoming and outgoing scene.
     */
    virtual ActionInterval* action();

    //
    // Overrides
    //
    virtual void onEnter() override;

protected:
    TransitionSlideInL();
    virtual ~TransitionSlideInL();

protected:
    /** initializes the scenes */
    virtual void initScenes();

    virtual void sceneOrder() override;

};

/** @class TransitionSlideInR
 *@brief TransitionSlideInR:
Slide in the incoming scene from the right border.
*/
class  TransitionSlideInR : public TransitionSlideInL 
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionSlideInR object.
     */
    static TransitionSlideInR* create(float t, Scene* scene);

    /** Returns the action that will be performed by the incoming and outgoing scene. */
    virtual ActionInterval* action() override;

protected:
    TransitionSlideInR();
    virtual ~TransitionSlideInR();

protected:
    /** initializes the scenes */
    virtual void initScenes() override;

    virtual void sceneOrder() override;

};

/** @class TransitionSlideInB
 * @brief TransitionSlideInB:
Slide in the incoming scene from the bottom border.
*/
class  TransitionSlideInB : public TransitionSlideInL
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionSlideInB object.
     */
    static TransitionSlideInB* create(float t, Scene* scene);

    /** returns the action that will be performed by the incoming and outgoing scene */
    virtual ActionInterval* action() override;

protected:
    TransitionSlideInB();
    virtual ~TransitionSlideInB();

protected:
    /** initializes the scenes */
    virtual void initScenes() override;

    virtual void sceneOrder() override;

};

/** @class TransitionSlideInT
 * @brief TransitionSlideInT:
Slide in the incoming scene from the top border.
*/
class  TransitionSlideInT : public TransitionSlideInL
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionSlideInT object.
     */
    static TransitionSlideInT* create(float t, Scene* scene);

    /** returns the action that will be performed by the incoming and outgoing scene */
    virtual ActionInterval* action() override;

protected:
    TransitionSlideInT();
    virtual ~TransitionSlideInT();

protected:
    /** initializes the scenes */
    virtual void initScenes() override;

    virtual void sceneOrder() override;

};



/** @class TransitionFade
 * @brief TransitionFade:
Fade out the outgoing scene and then fade in the incoming scene.'''
*/
class  TransitionFade : public TransitionScene
{
public:
    /** Creates the transition with a duration and with an RGB color
     * Example: FadeTransition::create(2, scene, Color3B(255,0,0); // red color
     *
     * @param duration Duration time, in seconds.
     * @param scene A given scene.
     * @param color A given transition color.
     * @return A autoreleased TransitionFade object.
     */
    static TransitionFade* create(float duration, Scene* scene, const DTRGB& color);
    /** Creates the transition with a duration.
     *
     * @param duration Duration time, in seconds.
     * @param scene A given scene.
     * @return A autoreleased TransitionFade object.
     */
    static TransitionFade* create(float duration, Scene* scene);

    /**
     * @lua NA
     */
    virtual void onEnter() override;
    /**
     * @lua NA
     */
    virtual void onExit() override;
    
protected:
    TransitionFade();
    virtual ~TransitionFade();

    /** initializes the transition with a duration and with an RGB color */
    bool initWithDuration(float t, Scene*scene, const DTRGB& color);
    bool initWithDuration(float t, Scene* scene);

protected:
    DTRGB _color;

};



// end of _2d group
/// @}

NS_DT_END

#endif // __DT_TRANSITION_H__

