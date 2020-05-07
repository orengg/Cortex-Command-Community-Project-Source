#ifndef _RTEACDROPSHIP_
#define _RTEACDROPSHIP_

//////////////////////////////////////////////////////////////////////////////////////////
// File:            ACDropShip.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Header file for the ACDropShip class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "ACraft.h"

namespace RTE
{

class Attachable;


//////////////////////////////////////////////////////////////////////////////////////////
// Class:           ACDropShip
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     A hovering craft, with two engines on each attached on each end which
//                  tilt independently of the body to achieve steering.
// Parent(s):       ACraft.
// Class history:   12/13/2006 ACDropShip created.

class ACDropShip:
    public ACraft
{


//////////////////////////////////////////////////////////////////////////////////////////
// Public member variable, method and friend function declarations

public:


// Concrete allocation and cloning definitions
ENTITYALLOCATION(ACDropShip)


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     ACDropShip
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a ACDropShip object in system
//                  memory. Create() should be called before using the object.
// Arguments:       None.

    ACDropShip() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      ~ACDropShip
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to clean up a ACDropShip object before deletion
//                  from system memory.
// Arguments:       None.

    virtual ~ACDropShip() { Destroy(true); }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the ACDropShip object ready for use.
// Arguments:       None.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Create();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a ACDropShip to be identical to another, by deep copy.
// Arguments:       A reference to the ACDropShip to deep copy.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    int Create(const ACDropShip &reference);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a Reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the Reader's position is untouched.
// Arguments:       The name of the property to be read.
//                  A Reader lined up to the value of the property to be read.
// Return value:    An error return value signaling whether the property was successfully
//                  read or not. 0 means it was read successfully, and any nonzero indicates
//                  that a property of that name could not be found in this or base classes.

    virtual int ReadProperty(std::string propName, Reader &reader);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Reset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Resets the entire ACDropShip, including its inherited members, to their
//                  default settings or values.
// Arguments:       None.
// Return value:    None.

    virtual void Reset() { Clear(); ACraft::Reset(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this ACDropShip to an output stream for
//                  later recreation with Create(Reader &reader);
// Arguments:       A Writer that the ACDropShip will save itself with.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Save(Writer &writer) const;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the SceneLayer object.
// Arguments:       Whether to only destroy the members defined in this derived class, or
//                  to destroy all inherited members also.
// Return value:    None.

    virtual void Destroy(bool notInherited = false);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetClass
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the ClassInfo instance of this Entity.
// Arguments:       None.
// Return value:    A reference to the ClassInfo of this' class.

    virtual const Entity::ClassInfo & GetClass() const { return m_sClass; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:   GetClassName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the class name of this Entity.
// Arguments:       None.
// Return value:    A string with the friendly-formatted type name of this object.

    virtual const std::string & GetClassName() const { return m_sClass.GetName(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMass
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the mass value of this ACDropShip, including the mass of its
//                  currently attached body parts and inventory.
// Arguments:       None.
// Return value:    A float describing the mass value in Kilograms (kg).

    virtual float GetMass() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetAltitude
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the altitide of this' pos (or appropriate low point) over the
//                  terrain, in pixels.
// Arguments:       The max altitude you care to check for. 0 Means check the whole scene's height.
//                  The accuracy within which measurement is acceptable. Higher number
//                  here means less calculation.
// Return value:    The rough altitude over the terrain, in pixels.

    virtual float GetAltitude(int max = 0, int accuracy = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  DetectObstacle
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks for obstacles in the travel direction.
// Arguments:       How far ahead of travel direction to check for obstacles.
// Return value:    Which MOID was detected as obstacle. g_NoMOID means nothing was detected.

    virtual MOID DetectObstacle(float distance);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the MOID of this MovableObject for this frame.
// Arguments:       A moid specifying the MOID that this MovableObject is
//                  assigned for this frame.
// Return value:    None.

    virtual void SetID(const MOID newID);

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnBounce
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Defines what should happen when this MovableObject hits and then
//                  bounces off of something. This is called by the owned Atom/AtomGroup
//                  of this MovableObject during travel.
// Arguments:       The position where the bounce-hit occurred.
// Return value:    Wheter the MovableObject should immediately halt any travel going on
//                  after this bounce.

    virtual bool OnBounce(const Vector &pos);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnSink
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Defines what should happen when this MovableObject hits and then
//                  sink into something. This is called by the owned Atom/AtomGroup
//                  of this MovableObject during travel.
// Arguments:       The position where the sink-hit occurred.
// Return value:    Wheter the MovableObject should immediately halt any travel going on
//                  after this sinkage.

    virtual bool OnSink(const Vector &pos);
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AutoStabilizing
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Tells whether this has the means and will try to right itself, or if
//                  that's up to the Controller to do.
// Arguments:       None.
// Return value:    Wheter this will try to auto stabilize.

    virtual bool AutoStabilizing() { return true; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GibThis
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gibs this, effectively destroying it and creating multiple gibs or
//                  pieces in its place.
// Arguments:       The impulse (kg * m/s) of the impact causing the gibbing to happen.
//					The internal blast impulse which will push the gibs away from the center.
//                  A pointer to an MO which the gibs shuold not be colliding with!
// Return value:    None.

    virtual void GibThis(Vector impactImpulse = Vector(), float internalBlast = 10, MovableObject *pIgnoreMO = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  IsOnScenePoint
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this' current graphical representation overlaps
//                  a point in absolute scene coordinates.
// Arguments:       The point in absolute scene coordinates.
// Return value:    Whether this' graphical rep overlaps the scene point.

    virtual bool IsOnScenePoint(Vector &scenePoint) const;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  UpdateAI
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates this' AI state. Supposed to be done every frame that this has
//                  a CAI controller controlling it.
// Arguments:       None.
// Return value:    None.

    virtual void UpdateAI();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates this MovableObject. Supposed to be done every frame.
// Arguments:       Nosssssssne.
// Return value:    None.

    virtual void Update();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this ACDropShip's current graphical representation to a
//                  BITMAP of choice.
// Arguments:       A pointer to a BITMAP to draw on.
//                  The absolute position of the target bitmap's upper left corner in the Scene.
//                  In which mode to draw in. See the DrawMode enumeration for the modes.
//                  Whether to not draw any extra 'ghost' items of this MovableObject,
//                  indicator arrows or hovering HUD text and so on.
// Return value:    None.

    virtual void Draw(BITMAP *pTargetBitmap,
                      const Vector &targetPos = Vector(),
                      DrawMode mode = g_DrawColor,
                      bool onlyPhysical = false) const;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:	GetMaxPassengers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     The recomended, not absolute, maximum number of actors that fit in the
//                  invetory. Used by the activity AI.
// Arguments:       None.
// Return value:    An integer with the recomended number of actors that fit in the craft.
//                  Default is four.

    virtual int GetMaxPassengers() const { return m_MaxPassengers > -1 ? m_MaxPassengers : 4; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:	GetRThruster
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the right side engine.
// Arguments:       None.
// Return value:    An AEmitter pointer.

    AEmitter * GetRThruster() const { return m_pRThruster; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:	GetLThruster
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the left side engine.
// Arguments:       None.
// Return value:    An AEmitter pointer.

    AEmitter * GetLThruster() const { return m_pLThruster; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:	GetURThruster
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the right side secondary thruster.
// Arguments:       None.
// Return value:    An AEmitter pointer.

    AEmitter * GetURThruster() const { return m_pURThruster; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:	GetULThruster
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the left side secondary thruster.
// Arguments:       None.
// Return value:    An AEmitter pointer.

    AEmitter * GetULThruster() const { return m_pULThruster; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:	GetLHatch
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the left side hatch.
// Arguments:       None.
// Return value:    An Attachable pointer.

	Attachable * GetLHatch() const { return m_pLHatch; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:	GetRHatch
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the right side hatch.
// Arguments:       None.
// Return value:    An Attachable pointer.

	Attachable * GetRHatch() const { return m_pRHatch; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ResetEmissionTimers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reset the timers of all emissions so they will start/stop at the 
//                  correct relative offsets from now.
// Arguments:       None.
// Return value:    None.

    virtual void ResetEmissionTimers();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetTotalWoundCount
//////////////////////////////////////////////////////////////////////////////////////////
// Description:		Returns total wound count of this actor and all vital attachables.
// Arguments:       None.
// Return value:    Returns total number of wounds of this actor.

	virtual int GetTotalWoundCount() const; 

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetTotalWoundLimit
//////////////////////////////////////////////////////////////////////////////////////////
// Description:		Returns total wound limit of this actor and all vital attachables.
// Arguments:       None.
// Return value:    Returns total wound limit of this actor.

	virtual int GetTotalWoundLimit() const; 


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          RemoveAnyRandomWounds
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes a specified amount of wounds from the actor and all standard attachables.
// Arguments:       Amount of wounds to remove.
// Return value:    Damage taken from removed wounds.

	virtual int RemoveAnyRandomWounds(int amount);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMOIDs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Puts all MOIDs associated with this MO and all it's descendants into MOIDs vector
// Arguments:       Vector to store MOIDs
// Return value:    None.

	virtual void GetMOIDs(std::vector<MOID> &MOIDs) const;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMaxEngineAngle
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Get max engine rotation angle in degrees.
// Arguments:       None.
// Return value:    Max engine angle in degrees.

	virtual float GetMaxEngineAngle() const { return m_MaxEngineAngle; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetMaxEngineAngle
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets max engine rotation angle in degrees.
// Arguments:       Max engine angle in degrees.
// Return value:    None.

	virtual void SetMaxEngineAngle(float newAngle) { m_MaxEngineAngle = newAngle; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetLateralControlSpeed
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the abstract rate of LateralControl change. Default is 6
// Arguments:       None.
// Return value:    Current lateral control speed value.

	float GetLateralControlSpeed() const { return m_LateralControlSpeed; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetLateralControlSpeed
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the abstract rate of LateralControl change. Default is 6
// Arguments:       New lateral control speed value.
// Return value:    None.

	void SetLateralControlSpeed(float newSpeed) { m_LateralControl = newSpeed; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetMaxEngineAngle
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets lateral control value -1.0 to 1.0 control of sideways movement. 0 means try to stand still in X.
// Arguments:       None.
// Return value:    Current lateral control value.

	float GetLateralControl() const { return m_LateralControl; }


//////////////////////////////////////////////////////////////////////////////////////////
// Protected member variable and method declarations

protected:

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  UpdateChildMOIDs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes this MO register itself and all its attached children in the
//                  MOID register and get ID:s for itself and its children for this frame.
// Arguments:       The MOID index to register itself and its children in.
//                  The MOID of the root MO of this MO, ie the highest parent of this MO.
//                  0 means that this MO is the root, ie it is owned by MovableMan.
//                  Whether this MO should make a new MOID to use for itself, or to use
//                  the same as the last one in the index (presumably its parent),
// Return value:    None.

    virtual void UpdateChildMOIDs(std::vector<MovableObject *> &MOIDIndex,
                                 MOID rootMOID = g_NoMOID,
                                 bool makeNewMOID = true);


    // Member variables
    static Entity::ClassInfo m_sClass;
    // Body AtomGroups.
    AtomGroup *m_pBodyAG;
    // Thruster emitters.
    AEmitter *m_pRThruster;
    AEmitter *m_pLThruster;
    AEmitter *m_pURThruster;
    AEmitter *m_pULThruster;

    // Hatch doors
    Attachable *m_pRHatch;
    Attachable *m_pLHatch;
    // How much the hatch doors rotate to open
    Matrix m_HatchSwingRange;
    // From 0 to 1.0, the state of hatch door openness
    float m_HatchOpeness;

    // -1.0 to 1.0 control of sideways movement. 0 means try to stand still in X.
    float m_LateralControl;
	// Abstract speed at which Lateralcontroll is changed
	float m_LateralControlSpeed;

    // Automatically stabilize the craft with the upper thrusters? Defaults to yes.
    int m_AutoStabilize;
    // The craft explodes if it has been on its side for more than this many MS (default 4000). Disable by setting to -1.
    float m_ScuttleIfFlippedTime;

	// Maximum engine rotation in degrees
	float m_MaxEngineAngle;

//////////////////////////////////////////////////////////////////////////////////////////
// Private member variable and method declarations

private:

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this ACDropShip, effectively
//                  resetting the members of this abstraction level only.
// Arguments:       None.
// Return value:    None.

    void Clear();


    // Disallow the use of some implicit methods.
    ACDropShip(const ACDropShip &reference);
    ACDropShip & operator=(const ACDropShip &rhs);

};

} // namespace RTE

#endif // File