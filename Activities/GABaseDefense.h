#ifndef _RTEGABASEDEFENSE_
#define _RTEGABASEDEFENSE_

//////////////////////////////////////////////////////////////////////////////////////////
// File:            GABaseDefense.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Header file for the ActivityMan class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "GameActivity.h"

namespace RTE
{

class Actor;


//////////////////////////////////////////////////////////////////////////////////////////
// Class:           GABaseDefense
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Base class for all GABaseDefense:s, including game modes and editors.
// Parent(s):       GameActivity.
// Class history:   8/12/2007 GABaseDefense created.

class GABaseDefense:
    public GameActivity
{

//////////////////////////////////////////////////////////////////////////////////////////
// Public member variable, method and friend function declarations

public:


// Concrete allocation and cloning definitions
ENTITYALLOCATION(GABaseDefense)


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GABaseDefense
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GABaseDefense object in system
//                  memory. Create() should be called before using the object.
// Arguments:       None.

    GABaseDefense() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      ~GABaseDefense
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to clean up a GABaseDefense object before deletion
//                  from system memory.
// Arguments:       None.

    virtual ~GABaseDefense() { Destroy(true); }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the GABaseDefense object ready for use.
// Arguments:       None.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Create();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a GABaseDefense to be identical to another, by deep copy.
// Arguments:       A reference to the GABaseDefense to deep copy.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Create(const GABaseDefense &reference);


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
// Description:     Resets the entire GABaseDefense, including its inherited members, to their
//                  default settings or values.
// Arguments:       None.
// Return value:    None.

    virtual void Reset() { Clear(); Activity::Reset(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this GABaseDefense to an output stream for
//                  later recreation with Create(Reader &reader);
// Arguments:       A Writer that the GABaseDefense will save itself with.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Save(Writer &writer) const;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the GABaseDefense object.
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
// Method:          SetBotSpawnInterval
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the interval in which new AI bots will spawn if there is an AI team
// Arguments:       The new bot spawn internval, in ms.
// Return value:    None.

    void SetBotSpawnInterval(long spawnInterval = 10000) { m_SpawnInterval = spawnInterval; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Start
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Officially starts the game accroding to parameters previously set.
// Arguments:       None.
// Return value:    An error return value signaling success or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Start();

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Pause
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Pauses and unpauses the game.
// Arguments:       Whether to pause the game or not.
// Return value:    None.

    virtual void Pause(bool pause = true);
*/
/*
//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  End
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Forces the current game's end.
// Arguments:       None.
// Return value:    None.

    virtual void End();
*/
/*
//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  UpdateEditing
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     This is a special update step for when any player is still editing the
//                  scene.
// Arguments:       None.
// Return value:    None.

    virtual void UpdateEditing();
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this ActivityMan. Supposed to be done every frame
//                  before drawing.
// Arguments:       None.
// Return value:    None.

    virtual void Update();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  DrawGUI
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the currently active GUI of a screen to a BITMAP of choice.
// Arguments:       A pointer to a screen-sized BITMAP to draw on.
//                  The absolute position of the target bitmap's upper left corner in the scene.
//                  Which screen's GUI to draw onto the bitmap.
// Return value:    None.

    virtual void DrawGUI(BITMAP *pTargetBitmap, const Vector &targetPos = Vector(), int which = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this ActivityMan's current graphical representation to a
//                  BITMAP of choice. This includes all game-related graphics.
// Arguments:       A pointer to a BITMAP to draw on.
//                  The absolute position of the target bitmap's upper left corner in the scene.
// Return value:    None.

    virtual void Draw(BITMAP *pTargetBitmap, const Vector &targetPos = Vector());


//////////////////////////////////////////////////////////////////////////////////////////
// Protected member variable and method declarations

protected:

    // Member variables
    static Entity::ClassInfo m_sClass;
    // Timer for meauring the attacker waves
    Timer m_SpawnTimer;
    // Currently how long between attacker spawns
    long m_SpawnInterval;
    // How long between attacker spawns for the EASIEST difficulty (will be higher)
    long m_SpawnIntervalEasiest;
    // How long between attacker spawns for the HARDEST difficulty (will be lower)
    long m_SpawnIntervalHardest;
    // List of all possible attackers. New attacker will be chosen randomly from this each spawn and cloned
    // MO's ARE owned by this!
    std::vector<Actor *> m_AttackerSpawns;



//////////////////////////////////////////////////////////////////////////////////////////
// Private member variable and method declarations

private:


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this Activity, effectively
//                  resetting the members of this abstraction level only.
// Arguments:       None.
// Return value:    None.

    void Clear();

};

} // namespace RTE

#endif // File