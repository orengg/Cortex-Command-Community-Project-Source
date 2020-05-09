/*          ______   ______   ______  ______  ______  __  __       ______   ______   __    __   __    __   ______   __   __   _____
           /\  ___\ /\  __ \ /\  == \/\__  _\/\  ___\/\_\_\_\     /\  ___\ /\  __ \ /\ "-./  \ /\ "-./  \ /\  __ \ /\ "-.\ \ /\  __-.
           \ \ \____\ \ \/\ \\ \  __<\/_/\ \/\ \  __\\/_/\_\/_    \ \ \____\ \ \/\ \\ \ \-./\ \\ \ \-./\ \\ \  __ \\ \ \-.  \\ \ \/\ \
            \ \_____\\ \_____\\ \_\ \_\ \ \_\ \ \_____\/\_\/\_\    \ \_____\\ \_____\\ \_\ \ \_\\ \_\ \ \_\\ \_\ \_\\ \_\\"\_\\ \____-
  	         \/_____/ \/_____/ \/_/ /_/  \/_/  \/_____/\/_/\/_/     \/_____/ \/_____/ \/_/  \/_/ \/_/  \/_/ \/_/\/_/ \/_/ \/_/ \/____/
   ______   ______   __    __   __    __   __  __   __   __   __   ______  __  __       ______  ______   ______      __   ______   ______   ______
  /\  ___\ /\  __ \ /\ "-./  \ /\ "-./  \ /\ \/\ \ /\ "-.\ \ /\ \ /\__  _\/\ \_\ \     /\  == \/\  == \ /\  __ \    /\ \ /\  ___\ /\  ___\ /\__  _\
  \ \ \____\ \ \/\ \\ \ \-./\ \\ \ \-./\ \\ \ \_\ \\ \ \-.  \\ \ \\/_/\ \/\ \____ \    \ \  _-/\ \  __< \ \ \/\ \  _\_\ \\ \  __\ \ \ \____\/_/\ \/
   \ \_____\\ \_____\\ \_\ \ \_\\ \_\ \ \_\\ \_____\\ \_\\"\_\\ \_\  \ \_\ \/\_____\    \ \_\   \ \_\ \_\\ \_____\/\_____\\ \_____\\ \_____\  \ \_\
    \/_____/ \/_____/ \/_/  \/_/ \/_/  \/_/ \/_____/ \/_/ \/_/ \/_/   \/_/  \/_____/     \/_/    \/_/ /_/ \/_____/\/_____/ \/_____/ \/_____/   \/_/

/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\*/

/// <summary>
/// Main driver implementation of the Retro Terrain Engine.
/// Data Realms, LLC - http://www.datarealms.com
/// Cortex Command Center - https://discord.gg/SdNnKJN
/// Cortex Command Community Project - https://github.com/cortex-command-community
/// </summary>

// Without this nested includes somewhere deep inside Allegro will summon winsock.h and it will conflict with winsock2.h from RakNet.
#define WIN32_LEAN_AND_MEAN

#include "System.h"

#include "RTEManagers.h"
#include "MetaMan.h"
#include "ConsoleMan.h"

#include "GUI/GUI.h"
#include "GUI/AllegroBitmap.h"
#include "LoadingGUI.h"
#include "MainMenuGUI.h"
#include "ScenarioGUI.h"
#include "MetagameGUI.h"

#include "DataModule.h"
#include "SceneLayer.h"
#include "MOSParticle.h"
#include "MOSRotating.h"
#include "Controller.h"

#include "MultiplayerServerLobby.h"
#include "NetworkServer.h"
#include "NetworkClient.h"
#include "Network.h"

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Global variables.
/// </summary>

enum TITLESEQUENCE {
    START = 0,
    // DRL Logo
    LOGOFADEIN,
    LOGODISPLAY,
    LOGOFADEOUT,
	FMODLOGOFADEIN,
	FMODLOGODISPLAY,
	FMODLOGOFADEOUT,
    // Game notice
    NOTICEFADEIN,
    NOTICEDISPLAY,
    NOTICEFADEOUT,
    // Intro
    FADEIN,
    SPACEPAUSE1,
    SHOWSLIDE1,
    SHOWSLIDE2,
    SHOWSLIDE3,
    SHOWSLIDE4,
    SHOWSLIDE5,
    SHOWSLIDE6,
    SHOWSLIDE7,
    SHOWSLIDE8,
    PRETITLE,
    TITLEAPPEAR,
    PLANETSCROLL,
    PREMENU,
    MENUAPPEAR,
    // Main menu is active and operational
    MENUACTIVE,
    // Scenario mode views and transitions
    MAINTOSCENARIO,
    // Back from a scenario game to the scenario selection menu
    SCENARIOFADEIN,
    SCENARIOMENU,
    // Campaign mode views and transitions
    MAINTOCAMPAIGN,
    // Back from a battle to the campaign view
    CAMPAIGNFADEIN,
    CAMPAIGNPLAY,
    // Going back to the main menu view from a planet-centered view
    PLANETTOMAIN,
    FADESCROLLOUT,
    FADEOUT,
    END
};

// Intro slides
enum SLIDES {
    SLIDEPAST = 0,
    SLIDENOW,
    SLIDEVR,
    SLIDETRAVEL,
    SLIDEALIENS,
    SLIDETRADE,
    SLIDEPEACE,
    SLIDEFRONTIER,
    SLIDECOUNT
};

volatile bool g_Quit = false;
bool g_ResetRTE = false; //!< Signals to reset the entire RTE next iteration.
bool g_LaunchIntoEditor = false; //!< Flag for launching directly into editor activity.
const char *g_EditorToLaunch = ""; //!< String with editor activity name to launch.
bool g_InActivity = false;
bool g_ResetActivity = false;
bool g_ResumeActivity = false;
bool g_ReturnToMainMenu = false;
int g_IntroState = START;
int g_TeamCount = 2;
int g_PlayerCount = 3;
int g_DifficultySetting = 4;
int g_StationOffsetX;
int g_StationOffsetY;

MainMenuGUI *g_pMainMenuGUI = 0;
ScenarioGUI *g_pScenarioGUI = 0;
Controller *g_pMainMenuController = 0;

enum StarSize {
    StarSmall = 0,
    StarLarge,
    StarHuge,
};

struct Star {
    // Bitmap representation
    BITMAP *m_pBitmap;
    // Center location on screen
    Vector m_Pos;
    // Bitmap offset
//    int m_Offset;
    // Scrolling ratio
    float m_ScrollRatio;
    // Normalized intensity 0-1.0
    float m_Intensity;
    // Type
    StarSize m_Size;

    Star() { m_pBitmap = 0; m_Pos.Reset(); m_ScrollRatio = 1.0; m_Intensity = 1.0; m_Size = StarSmall; }
    Star(BITMAP *pBitmap, Vector &pos, float scrollRatio, float intensity) { m_pBitmap = pBitmap; m_Pos = pos; m_ScrollRatio = scrollRatio; m_Intensity = intensity; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// This handles when the quit or exit button is pressed on the window.
/// </summary>
void QuitHandler(void) { g_Quit = true; }
END_OF_FUNCTION(QuitHandler)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Load and initialize the Main Menu.
/// </summary>
/// <returns></returns>
bool InitMainMenu() {
    g_FrameMan.LoadPalette("Base.rte/palette.bmp");

    // Create the main menu interface
	g_pMainMenuGUI = new MainMenuGUI();
    g_pMainMenuController = new Controller(Controller::CIM_PLAYER, 0);
    g_pMainMenuController->SetTeam(0);
    g_pMainMenuGUI->Create(g_pMainMenuController);
    // As well as the Scenario setup menu interface
	g_pScenarioGUI = new ScenarioGUI();
    g_pScenarioGUI->Create(g_pMainMenuController);
    // And the Metagame GUI too
    g_MetaMan.GetGUI()->Create(g_pMainMenuController);

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Reset the current activity.
/// </summary>
/// <returns></returns>
bool ResetActivity()
{
    g_ResetActivity = false;

    // Clear and reset out things
    g_FrameMan.ClearBackBuffer8();
    g_FrameMan.FlipFrameBuffers();
    g_AudioMan.StopAll();

    // Quit if we should
	if (g_Quit) {
		return false;
	}

	// TODO: Deal with GUI resetting here!$@#")
    // Clear out all MO's
    g_MovableMan.PurgeAllMOs();
    // Have to reset TimerMan before creating anything else because all timers are reset against it
    g_TimerMan.ResetTime();

    g_FrameMan.LoadPalette("Base.rte/palette.bmp");
    g_FrameMan.FlipFrameBuffers();

    // Reset TimerMan again after loading so there's no residual delay
    g_TimerMan.ResetTime();
    // Enable time averaging since it helps with animation jerkiness
    g_TimerMan.EnableAveraging(true);
    // Unpause
    g_TimerMan.PauseSim(false);

    int error = g_ActivityMan.RestartActivity();
	if (error >= 0) {
		g_InActivity = true;
	} else {
        // Something went wrong when restarting, so drop out to scenario menu and open the console to show the error messages
		g_InActivity = false;
		g_ActivityMan.PauseActivity();
		g_ConsoleMan.SetEnabled(true);
		g_IntroState = MAINTOSCENARIO;
		return false;
	}
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Start the simulation back up after being paused.
/// </summary>
void ResumeActivity() {
	if (g_ActivityMan.GetActivity()->GetActivityState() != Activity::NOTSTARTED) {
		g_Quit = false;
		g_InActivity = true;
		g_ResumeActivity = false;

		g_FrameMan.ClearBackBuffer8();
		g_FrameMan.FlipFrameBuffers();
		g_FrameMan.LoadPalette("Base.rte/palette.bmp");

		g_PerformanceMan.ResetFrameTimer();
        // Enable time averaging since it helps with animation jerkiness
		g_TimerMan.EnableAveraging(true);
		g_TimerMan.PauseSim(false);
		g_ActivityMan.PauseActivity(false);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Launch multiplayer lobby activity.
/// </summary>
void EnterMultiplayerLobby() {
	g_SceneMan.SetSceneToLoad("Multiplayer Scene");
	MultiplayerServerLobby *pMultiplayerServerLobby = new MultiplayerServerLobby;
	pMultiplayerServerLobby->Create();

	pMultiplayerServerLobby->ClearPlayers(true);
	pMultiplayerServerLobby->AddPlayer(0, true, 0, 0);
	pMultiplayerServerLobby->AddPlayer(1, true, 0, 1);
	pMultiplayerServerLobby->AddPlayer(2, true, 0, 2);
	pMultiplayerServerLobby->AddPlayer(3, true, 0, 3);

	//g_FrameMan.ResetSplitScreens(true, true);
	g_ActivityMan.SetStartActivity(pMultiplayerServerLobby);
	g_ResetActivity = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Launch editor activity specified in command-line argument.
/// </summary>
void EnterEditorActivity(const char *editorToEnter) {
	if (std::strcmp(editorToEnter, "ActorEditor") == 0) { 
		g_pMainMenuGUI->StartActorEditor(); 
	} else if (std::strcmp(editorToEnter, "GibEditor") == 0) {
		g_pMainMenuGUI->StartGibEditor();
	} else if (std::strcmp(editorToEnter, "SceneEditor") == 0) {
		g_pMainMenuGUI->StartSceneEditor();
	} else if (std::strcmp(editorToEnter, "AreaEditor") == 0) {
		g_pMainMenuGUI->StartAreaEditor();
	} else if (std::strcmp(editorToEnter, "AssemblyEditor") == 0) {
		g_pMainMenuGUI->StartAssemblyEditor();
	} else {
		g_LaunchIntoEditor = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! Class that contains variables and methods for the function PlayIntroTitle()
class IntroTitleParams {
public:
    bool m_keyPressed;
    bool m_sectionSwitch;
    int m_startYOffset;
    int m_titleAppearYOffset;
    Timer m_sectionTimer;
    Timer m_songTimer;
    double m_elapsed;
    double m_duration;
    double m_sectionProgress;
    double m_scrollProgress;
    double m_scrollStart;
    double m_scrollDuration;
    Vector m_scrollOffset;
    SceneLayer* m_pBackdrop;
    int m_starCount;
    float m_backdropScrollRatio;
    StarSize m_size;
    Star* m_aStars;
    Vector m_planetPos;
    MOSParticle* m_pMoon;
    MOSParticle* m_pPlanet;
    long long m_lastShake;
    Vector m_shakeOffset;
    float m_orbitRotation;
    Vector m_capsuleOffset;
    float m_orbitRadius;
    MOSRotating* m_pPioneerCapsule;
    Vector m_stationOffset;
    MOSRotating* m_pStation;
    long long m_lastPuff;
    bool m_puffActive;
    int m_puffCount;
    MOSParticle* m_pFirePuffLarge;
    MOSParticle* m_pFirePuffMedium;
    long long m_lastPuffFrame;
    int m_puffFrame;
    int m_preMenuYOffset;
    MOSParticle* m_pTitleGlow;
    MOSParticle* m_pTitle;

    /* the original order of commands in the playintrotitle loop is:
    initial things
    scrolling logic
    DRL logo drawing
    FMOD logo drawing
    notice drawing
    scene drawing
    game logo drawing
    menu drawing
    slides drawing
    intro sequence logic
    additional user input and skipping handling
    final things*/

    //! Empty Constructor
    IntroTitleParams() {}
    
    //! Method of things to do at the start of each while loop.
    void StartOfWhile() {
        m_keyPressed = g_UInputMan.AnyStartPress();
        // Reset the key press states
        g_UInputMan.Update();
        g_TimerMan.Update();
        g_TimerMan.UpdateSim();
        g_ConsoleMan.Update();

        g_AudioMan.Update();

        if (m_sectionSwitch) { m_sectionTimer.Reset(); }
        m_elapsed = m_sectionTimer.GetElapsedRealTimeS();
        // Calculate the normalized ITP.m_sectionProgress scalar
        m_sectionProgress = m_duration <= 0 ? 0 : (m_elapsed / m_duration);
        // Clamp the ITP.m_sectionProgress scalar
        m_sectionProgress = min(m_sectionProgress, 0.9999);

        if (g_NetworkServer.IsServerModeEnabled()) { g_NetworkServer.Update(); }
    }

    //! Scrolling logic for these states: (g_IntroState >= FADEIN && g_IntroState <= PRETITLE)
    void FadeinToPretitle() {
        m_scrollProgress = (m_songTimer.GetElapsedRealTimeS() - m_scrollStart) / m_scrollDuration;
        m_scrollOffset.m_Y = LERP(0, 1.0, m_startYOffset, m_titleAppearYOffset, m_scrollProgress);
    }

    //! Scrolling logic for these states: (g_IntroState >= TITLEAPPEAR && g_IntroState <= PLANETSCROLL)
    void TitleappearToPlanetscroll() {
        m_scrollProgress = (m_songTimer.GetElapsedRealTimeS() - m_scrollStart) / m_scrollDuration;
        m_scrollOffset.m_Y = EaseOut(m_titleAppearYOffset, m_preMenuYOffset, m_scrollProgress);
    }

    //! Game logo drawing for these states: ((g_IntroState >= TITLEAPPEAR && g_IntroState < SCENARIOFADEIN) || g_IntroState == MAINTOCAMPAIGN)
    void TitleGlow() {
        m_pTitleGlow->SetPos(m_pTitle->GetPos());
        m_pTitle->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawAlpha);
        // Screen blend the title glow on top, with some flickering in its intensity
        int blendAmount = 220 + 35 * NormalRand();
        set_screen_blender(blendAmount, blendAmount, blendAmount, blendAmount);
        m_pTitleGlow->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawTrans);
    }

    //! Scene drawing for states after and including FADEIN.
    void FromFadeinOnward() {
        g_FrameMan.ClearBackBuffer32();
        Box backdropBox;
        m_pBackdrop->Draw(g_FrameMan.GetBackBuffer32(), backdropBox, m_scrollOffset * m_backdropScrollRatio);
        Vector starDrawPos;
        for (int star = 0; star < m_starCount; ++star) {
            m_size = m_aStars[star].m_Size;
            int intensity = 185 * m_aStars[star].m_Intensity + (m_size == StarSmall ? 35 : 70) * PosRand();
            set_screen_blender(intensity, intensity, intensity, intensity);
            starDrawPos.SetXY(m_aStars[star].m_Pos.m_X, m_aStars[star].m_Pos.m_Y - m_scrollOffset.m_Y * m_aStars[star].m_ScrollRatio);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), m_aStars[star].m_pBitmap, starDrawPos.GetFloorIntX(), starDrawPos.GetFloorIntY());
        }
        m_planetPos.SetXY(g_FrameMan.GetResX() / 2, 567 - m_scrollOffset.GetFloorIntY());
        m_pMoon->SetPos(Vector(m_planetPos.m_X + 200, 364 - m_scrollOffset.GetFloorIntY() * 0.60));
        m_pPlanet->SetPos(m_planetPos);
        m_pMoon->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawAlpha);
        m_pPlanet->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawAlpha);
        // Manually shake our m_shakeOffset to randomize some effects
        if (g_TimerMan.GetAbsoulteTime() > m_lastShake + 50000) {
            m_shakeOffset.m_X = RangeRand(-3, 3);
            m_shakeOffset.m_Y = RangeRand(-3, 3);
            m_lastShake = g_TimerMan.GetAbsoulteTime();
        }
        // Tell the menu that PP promo is off
        g_pMainMenuGUI->DisablePioneerPromoButton();
    }

    //! Scene drawing for these states: g_IntroState < MAINTOCAMPAIGN && g_IntroState >= FADEIN
    /*! Method to do things in these states: g_IntroState < MAINTOCAMPAIGN && g_IntroState >= FADEIN
    Draw pioneer promo capsule.*/
    void FadeinToScenariomenu() {
        if (m_orbitRotation < -c_PI * 1.27 && m_orbitRotation > -c_PI * 1.85) {
            // Start drawing pioneer capsule
            // Slowly decrease radius to show that the capsule is falling
            float radiusperc = 1 - ((fabs(m_orbitRotation) - (1.27 * c_PI)) / (0.35 * c_PI) / 4);
            // Slowly decrease m_size to make the capsule disappear after a while
            float sizeperc = 1 - ((fabs(m_orbitRotation) - (1.27 * c_PI)) / (0.35 * c_PI) / 1.5);
            // Rotate, place and draw capsule
            m_capsuleOffset.SetXY(m_orbitRadius * radiusperc, 0);
            m_capsuleOffset.RadRotate(m_orbitRotation);
            m_pPioneerCapsule->SetScale(sizeperc);
            m_pPioneerCapsule->SetPos(m_planetPos + m_capsuleOffset);
            m_pPioneerCapsule->SetRotAngle(m_orbitRotation);
            m_pPioneerCapsule->Draw(g_FrameMan.GetBackBuffer32());
        }
    }

    //! Scene drawing for states after and including FADEIN.
    /*! Place, rotate and draw station.
    A second method of things to do in the states after and including FADEIN.
    Divided into two functions because that's how it was originally.
    The order of commands probably matters. */
    void FromFadeinOnward2() {
        m_stationOffset.SetXY(m_orbitRadius, 0);
        m_stationOffset.RadRotate(m_orbitRotation);
        m_pStation->SetPos(m_planetPos + m_stationOffset);
        m_pStation->SetRotAngle(-c_HalfPI + m_orbitRotation);
        m_pStation->Draw(g_FrameMan.GetBackBuffer32());
    }

    //! Scene drawing for states: g_IntroState < MAINTOCAMPAIGN && g_IntroState >= FADEIN
    /*! Start explosion effects to show that there's something wrong with the station, but only if we're not in campaign.
    A second method of things to do in these states: g_IntroState < MAINTOCAMPAIGN && g_IntroState >= FADEIN
    Divided into two functions because that's how it was originally.
    The order of commands probably matters.*/
    void FadeinToScenariomenu2() {
        if (m_orbitRotation < -c_PI * 1.25 && m_orbitRotation > -c_TwoPI) {
            // Add explosions delay and count them
            if (g_TimerMan.GetAbsoulteTime() > m_lastPuff + 1000000) {
                m_lastPuff = g_TimerMan.GetAbsoulteTime();
                m_puffActive = true;
                m_puffCount++;
            }
            // If explosion was authorized
            if (m_puffActive) {
                // First explosion is big while other are smaller
                if (m_puffCount == 1) {
                    m_pFirePuffLarge->SetPos(m_planetPos + m_stationOffset);
                    if (g_TimerMan.GetAbsoulteTime() > m_lastPuffFrame + 50000) {
                        m_lastPuffFrame = g_TimerMan.GetAbsoulteTime();
                        m_puffFrame++;
                        if (m_puffFrame >= m_pFirePuffLarge->GetFrameCount()) {
                            // Manually reset frame counters and disable other explosions until it's time
                            m_puffFrame = 0;
                            m_puffActive = 0;
                        }
                        m_pFirePuffLarge->SetFrame(m_puffFrame);
                    }
                    m_pFirePuffLarge->Draw(g_FrameMan.GetBackBuffer32());
                }
                else {
                    m_pFirePuffMedium->SetPos(m_planetPos + m_stationOffset + m_shakeOffset);
                    if (g_TimerMan.GetAbsoulteTime() > m_lastPuffFrame + 50000) {
                        m_lastPuffFrame = g_TimerMan.GetAbsoulteTime();
                        m_puffFrame++;
                        if (m_puffFrame >= m_pFirePuffLarge->GetFrameCount()) {
                            // Manually reset frame counters and disable other explosions until it's time
                            m_puffFrame = 0;
                            m_puffActive = 0;
                        }
                        m_pFirePuffMedium->SetFrame(m_puffFrame);
                    }
                    m_pFirePuffMedium->Draw(g_FrameMan.GetBackBuffer32());
                }
            }
        }
    }

    //! Scene drawing for states after and including FADEIN.
    /*! A third method of things to do in the states after and including FADEIN.
    Divided into two functions because that's how it was originally.
    The order of commands probably matters.*/
    void FromFadeinOnward3() {
        m_orbitRotation -= 0.0020; // 0.0015
        // Keep the rotation angle from getting too large
        if (m_orbitRotation < -c_TwoPI) {
            m_orbitRotation += c_TwoPI;
        }
        g_StationOffsetX = m_stationOffset.m_X;
        g_StationOffsetY = m_stationOffset.m_Y;
    }

    //! Additional user input and skipping handling.
    /*! Method for things to do in these states: g_IntroState >= FADEIN && g_IntroState <= SLIDE8*/
    void FadeinToSlide8() {
        if (m_keyPressed) {
            g_IntroState = MENUAPPEAR;
            m_sectionSwitch = true;
            m_scrollOffset.m_Y = m_preMenuYOffset;
            m_orbitRotation = c_HalfPI - c_EighthPI;
            m_orbitRotation = -c_PI * 1.20;
        }
    }
};

//! Function that returns whether to continue regularly or stop.
bool KeepWhiling() {
    return (!g_Quit && !g_ResumeActivity);
}

//! Function that does things at the end of each while loop
void EndOfWhile() {
    // Draw the console in the menu
    g_ConsoleMan.Draw(g_FrameMan.GetBackBuffer32());
    // Wait for vertical sync before flipping frames
    vsync();
    g_FrameMan.FlipFrameBuffers();
}

/// <summary>
/// Load and display the into, title and menu sequence.
/// </summary>
/// <returns></returns>
bool PlayIntroTitle() {
    // Disable time averaging since it can make the music timing creep off target.
    g_TimerMan.EnableAveraging(false);

    // Untrap the mouse and keyboard
    g_UInputMan.DisableKeys(false);
    g_UInputMan.TrapMousePos(false);

    // Stop all audio
    g_AudioMan.StopAll();

    g_FrameMan.ClearBackBuffer32();
    g_FrameMan.FlipFrameBuffers();
    int resX = g_FrameMan.GetResX();
    int resY = g_FrameMan.GetResY();

    //Object that contains relevant variables and methods
    IntroTitleParams ITP;

    // The fade-in/out screens
    BITMAP* pFadeScreen = create_bitmap_ex(32, resX, resY);
    clear_to_color(pFadeScreen, 0);
    int fadePos = 0;

    // Load the Intro slides
    BITMAP** apIntroSlides = new BITMAP* [SLIDECOUNT];
    ContentFile introSlideFile("Base.rte/GUIs/Title/Intro/IntroSlideA.bmp");
    apIntroSlides[SLIDEPAST] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideB.bmp");
    apIntroSlides[SLIDENOW] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideC.bmp");
    apIntroSlides[SLIDEVR] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideD.bmp");
    apIntroSlides[SLIDETRAVEL] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideE.bmp");
    apIntroSlides[SLIDEALIENS] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideF.bmp");
    apIntroSlides[SLIDETRADE] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideG.bmp");
    apIntroSlides[SLIDEPEACE] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideH.bmp");
    apIntroSlides[SLIDEFRONTIER] = introSlideFile.LoadAndReleaseBitmap();

    ContentFile alphaFile;
    BITMAP* pAlpha = 0;

    MOSParticle* pDRLogo = new MOSParticle();
    pDRLogo->Create(ContentFile("Base.rte/GUIs/Title/Intro/DRLogo5x.bmp"));
    pDRLogo->SetWrapDoubleDrawing(false);

    MOSParticle* pFMODLogo = new MOSParticle();
    pFMODLogo->Create(ContentFile("Base.rte/GUIs/Title/Intro/FMODLogo.bmp"));
    pFMODLogo->SetWrapDoubleDrawing(false);

    ITP.m_pBackdrop = new SceneLayer();
    ITP.m_pBackdrop->Create(ContentFile("Base.rte/GUIs/Title/Nebula.bmp"), false, Vector(), false, false, Vector(0, -1.0));
    ITP.m_backdropScrollRatio = 1.0F / 3.0F;

    ITP.m_pTitle = new MOSParticle();
    ITP.m_pTitle->Create(ContentFile("Base.rte/GUIs/Title/Title.bmp"));
    ITP.m_pTitle->SetWrapDoubleDrawing(false);
    // Logo glow effect
    ITP.m_pTitleGlow = new MOSParticle();
    ITP.m_pTitleGlow->Create(ContentFile("Base.rte/GUIs/Title/TitleGlow.bmp"));
    ITP.m_pTitleGlow->SetWrapDoubleDrawing(false);
    // Add alpha
    alphaFile.SetDataPath("Base.rte/GUIs/Title/TitleAlpha.bmp");
    set_write_alpha_blender();
    draw_trans_sprite(ITP.m_pTitle->GetSpriteFrame(0), alphaFile.GetAsBitmap(), 0, 0);

    ITP.m_pPlanet = new MOSParticle();
    ITP.m_pPlanet->Create(ContentFile("Base.rte/GUIs/Title/Planet.bmp"));
    ITP.m_pPlanet->SetWrapDoubleDrawing(false);
    // Add alpha
    alphaFile.SetDataPath("Base.rte/GUIs/Title/PlanetAlpha.bmp");
    set_write_alpha_blender();
    draw_trans_sprite(ITP.m_pPlanet->GetSpriteFrame(0), alphaFile.GetAsBitmap(), 0, 0);

    ITP.m_pMoon = new MOSParticle();
    ITP.m_pMoon->Create(ContentFile("Base.rte/GUIs/Title/Moon.bmp"));
    ITP.m_pMoon->SetWrapDoubleDrawing(false);
    // Add alpha
    alphaFile.SetDataPath("Base.rte/GUIs/Title/MoonAlpha.bmp");
    set_write_alpha_blender();
    draw_trans_sprite(ITP.m_pMoon->GetSpriteFrame(0), alphaFile.GetAsBitmap(), 0, 0);

    ITP.m_pStation = new MOSRotating();
    ITP.m_pStation->Create(ContentFile("Base.rte/GUIs/Title/Station.bmp"));
    ITP.m_pStation->SetWrapDoubleDrawing(false);

    ITP.m_pPioneerCapsule = new MOSRotating();
    ITP.m_pPioneerCapsule->Create(ContentFile("Base.rte/GUIs/Title/Promo/PioneerCapsule.bmp"));
    ITP.m_pPioneerCapsule->SetWrapDoubleDrawing(false);

    MOSRotating* pPioneerScreaming = new MOSRotating();
    pPioneerScreaming->Create(ContentFile("Base.rte/GUIs/Title/Promo/PioneerScreaming.bmp"));
    pPioneerScreaming->SetWrapDoubleDrawing(false);

    MOSRotating* pPioneerPromo = new MOSRotating();
    pPioneerPromo->Create(ContentFile("Base.rte/GUIs/Title/Promo/PioneerPromo.bmp"));
    pPioneerPromo->SetWrapDoubleDrawing(false);

    ITP.m_pFirePuffLarge = dynamic_cast<MOSParticle*>(g_PresetMan.GetEntityPreset("MOSParticle", "Fire Puff Large", "Base.rte")->Clone());
    ITP.m_pFirePuffMedium = dynamic_cast<MOSParticle*>(g_PresetMan.GetEntityPreset("MOSParticle", "Fire Puff Medium", "Base.rte")->Clone());

    ITP.m_lastShake = 0;
    ITP.m_lastPuffFrame = 0;
    ITP.m_lastPuff = 0;
    ITP.m_puffActive = false;
    ITP.m_puffFrame = 0;
    ITP.m_puffCount = 0;

    ITP.m_shakeOffset = Vector(0, 0);

    // Generate stars!
    int starArea = resX * ITP.m_pBackdrop->GetBitmap()->h;
    ITP.m_starCount = starArea / 1000;
    ContentFile starSmallFile("Base.rte/GUIs/Title/Stars/StarSmall.bmp");
    ContentFile starLargeFile("Base.rte/GUIs/Title/Stars/StarLarge.bmp");
    ContentFile starHugeFile("Base.rte/GUIs/Title/Stars/StarHuge.bmp");
    int starSmallBitmapCount = 4;
    int starLargeBitmapCount = 1;
    int starHugeBitmapCount = 2;
    BITMAP** apStarSmallBitmaps = starSmallFile.GetAsAnimation(starSmallBitmapCount);
    BITMAP** apStarLargeBitmaps = starLargeFile.GetAsAnimation(starLargeBitmapCount);
    BITMAP** apStarHugeBitmaps = starHugeFile.GetAsAnimation(starHugeBitmapCount);
    ITP.m_aStars = new Star[ITP.m_starCount];

    for (int star = 0; star < ITP.m_starCount; ++star) {
        if (PosRand() < 0.95) {
            ITP.m_aStars[star].m_Size = StarSmall;
            ITP.m_aStars[star].m_pBitmap = apStarSmallBitmaps[SelectRand(0, starSmallBitmapCount - 1)];
            ITP.m_aStars[star].m_Intensity = RangeRand(0.001, 0.5);
        }
        else if (PosRand() < 0.85) {
            ITP.m_aStars[star].m_Size = StarLarge;
            ITP.m_aStars[star].m_pBitmap = apStarLargeBitmaps[SelectRand(0, starLargeBitmapCount - 1)];
            ITP.m_aStars[star].m_Intensity = RangeRand(0.6, 1.0);
        }
        else {
            ITP.m_aStars[star].m_Size = StarHuge;
            ITP.m_aStars[star].m_pBitmap = apStarHugeBitmaps[SelectRand(0, starLargeBitmapCount - 1)];
            ITP.m_aStars[star].m_Intensity = RangeRand(0.9, 1.0);
        }
        ITP.m_aStars[star].m_Pos.SetXY(resX * PosRand(), ITP.m_pBackdrop->GetBitmap()->h * PosRand());//resY * PosRand());
        ITP.m_aStars[star].m_Pos.Floor();
        // To match the nebula scroll
        ITP.m_aStars[star].m_ScrollRatio = ITP.m_backdropScrollRatio;
    }

    // Font stuff
    GUISkin* pSkin = g_pMainMenuGUI->GetGUIControlManager()->GetSkin();
    GUIFont* pFont = pSkin->GetFont("fatfont.bmp");
    AllegroBitmap backBuffer(g_FrameMan.GetBackBuffer32());
    int yTextPos = 0;
    // Timers
    Timer totalTimer;
    // Convenience for how many seconds have elapsed on each section
    ITP.m_elapsed = 0;
    // How long each section is, in s
    ITP.m_duration = 0;
    ITP.m_scrollDuration = 0;
    ITP.m_scrollStart = 0;
    double slideFadeInDuration = 0.5;
    double slideFadeOutDuration = 0.5;
    // Progress made on a section, from 0.0 to 1.0
    ITP.m_sectionProgress = 0;
    ITP.m_scrollProgress = 0;
    // When a section is supposed to end, relative to the song timer
    long sectionSongEnd = 0;

    // Scrolling data
    ITP.m_keyPressed = false;
    ITP.m_sectionSwitch = true;
    float planetRadius = 240;
    ITP.m_orbitRadius = 274;
    ITP.m_orbitRotation = c_HalfPI - c_EighthPI;
    // Set the start so that the nebula is fully scrolled up
    ITP.m_startYOffset = ITP.m_pBackdrop->GetBitmap()->h / ITP.m_backdropScrollRatio - (resY / ITP.m_backdropScrollRatio);
    ITP.m_titleAppearYOffset = 900;
    ITP.m_preMenuYOffset = 100;
    int topMenuYOffset = 0;
    // So planet is centered on the screen regardless of resolution
    int planetViewYOffset = 325 + planetRadius - (resY / 2);
    // Set Y to title offset so there's no jump when entering the main menu
    ITP.m_scrollOffset = Vector(0, ITP.m_preMenuYOffset);
    Vector slidePos;

    totalTimer.Reset();
    ITP.m_sectionTimer.Reset();

    // this block represents the first iteration of the original while loop.
    ITP.StartOfWhile();
    if (g_IntroState == START)
    {
        g_IntroState = LOGOFADEIN;
        ITP.m_sectionSwitch = true;
    }
    EndOfWhile();

    while (KeepWhiling() && g_IntroState == LOGOFADEIN) {
        ITP.StartOfWhile();

        // Draw the early build notice
        g_FrameMan.ClearBackBuffer32();
        pDRLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
        pDRLogo->Draw(g_FrameMan.GetBackBuffer32());

        if (ITP.m_sectionSwitch) {
            // Play juicy logo signature jingle/sound
            g_GUISound.SplashSound()->Play();
            // Black fade
            clear_to_color(pFadeScreen, 0);
            ITP.m_duration = 0.25;
            ITP.m_sectionSwitch = false;
        }
        fadePos = 255 - (255 * ITP.m_sectionProgress);
        set_trans_blender(fadePos, fadePos, fadePos, fadePos);
        draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = LOGODISPLAY;
            ITP.m_sectionSwitch = true;
        }
        else if (ITP.m_keyPressed) {
            g_IntroState = LOGOFADEOUT;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == LOGODISPLAY) {
        ITP.StartOfWhile();

        // Draw the early build notice
        g_FrameMan.ClearBackBuffer32();
        pDRLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
        pDRLogo->Draw(g_FrameMan.GetBackBuffer32());

        if (ITP.m_sectionSwitch) {
            ITP.m_duration = 2.0;
            ITP.m_sectionSwitch = false;
        }
        if (ITP.m_elapsed > ITP.m_duration || ITP.m_keyPressed) {
            g_IntroState = LOGOFADEOUT;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == LOGOFADEOUT) {
        ITP.StartOfWhile();

        // Draw the early build notice
        g_FrameMan.ClearBackBuffer32();
        pDRLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
        pDRLogo->Draw(g_FrameMan.GetBackBuffer32());

        if (ITP.m_sectionSwitch) {
            // Black fade
            clear_to_color(pFadeScreen, 0);
            ITP.m_duration = 0.25;
            ITP.m_sectionSwitch = false;
        }
        fadePos = 255 * ITP.m_sectionProgress;
        set_trans_blender(fadePos, fadePos, fadePos, fadePos);
        draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        if (ITP.m_elapsed >= ITP.m_duration || ITP.m_keyPressed) {
            g_IntroState = FMODLOGOFADEIN;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == FMODLOGOFADEIN) {
        ITP.StartOfWhile();

        // Draw FMOD Logo
        g_FrameMan.ClearBackBuffer32();
        pFMODLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
        pFMODLogo->Draw(g_FrameMan.GetBackBuffer32());

        if (ITP.m_sectionSwitch) {
            // Black fade
            clear_to_color(pFadeScreen, 0);
            ITP.m_duration = 0.25;
            ITP.m_sectionSwitch = false;
        }
        fadePos = 255 - (255 * ITP.m_sectionProgress);
        set_trans_blender(fadePos, fadePos, fadePos, fadePos);
        draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = FMODLOGODISPLAY;
            ITP.m_sectionSwitch = true;
        }
        else if (ITP.m_keyPressed) {
            g_IntroState = FMODLOGOFADEOUT;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == FMODLOGODISPLAY) {
        ITP.StartOfWhile();

        // Draw FMOD Logo
        g_FrameMan.ClearBackBuffer32();
        pFMODLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
        pFMODLogo->Draw(g_FrameMan.GetBackBuffer32());

        if (ITP.m_sectionSwitch) {
            ITP.m_duration = 2.0;
            ITP.m_sectionSwitch = false;
        }
        if (ITP.m_elapsed > ITP.m_duration || ITP.m_keyPressed) {
            g_IntroState = FMODLOGOFADEOUT;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == FMODLOGOFADEOUT) {
        ITP.StartOfWhile();

        // Draw FMOD Logo
        g_FrameMan.ClearBackBuffer32();
        pFMODLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
        pFMODLogo->Draw(g_FrameMan.GetBackBuffer32());

        if (ITP.m_sectionSwitch) {
            // Black fade
            clear_to_color(pFadeScreen, 0);
            ITP.m_duration = 0.25;
            ITP.m_sectionSwitch = false;
        }
        fadePos = 255 * ITP.m_sectionProgress;
        set_trans_blender(fadePos, fadePos, fadePos, fadePos);
        draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        if (ITP.m_elapsed >= ITP.m_duration || ITP.m_keyPressed) {
            g_IntroState = NOTICEFADEIN;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == NOTICEFADEIN) {
        ITP.StartOfWhile();

        // Draw the early build notice
        g_FrameMan.ClearBackBuffer32();
        yTextPos = g_FrameMan.GetResY() / 3;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("N O T E :"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 2;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("This game plays great with up to FOUR people on a BIG-SCREEN TV!"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 2;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("So invite some friends/enemies over, plug in those USB controllers, and have a blast -"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 4;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("Press ALT+ENTER to toggle FULLSCREEN mode"), GUIFont::Centre);
        // Draw the copyright notice
        yTextPos = g_FrameMan.GetResY() - pFont->GetFontHeight();
        char copyRight[512];
        sprintf_s(copyRight, sizeof(copyRight), "Cortex Command is TM and %c 2017 Data Realms, LLC", -35);
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, copyRight, GUIFont::Centre);

        if (ITP.m_sectionSwitch) {
            // Black fade
            clear_to_color(pFadeScreen, 0);
            ITP.m_duration = 0.5;
            ITP.m_sectionSwitch = false;
        }
        fadePos = 255 - (255 * ITP.m_sectionProgress);
        set_trans_blender(fadePos, fadePos, fadePos, fadePos);
        draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = NOTICEDISPLAY;
            ITP.m_sectionSwitch = true;
        }
        else if (ITP.m_keyPressed) {
            g_IntroState = NOTICEFADEOUT;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == NOTICEDISPLAY) {
        ITP.StartOfWhile();

        // Draw the early build notice
        g_FrameMan.ClearBackBuffer32();
        yTextPos = g_FrameMan.GetResY() / 3;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("N O T E :"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 2;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("This game plays great with up to FOUR people on a BIG-SCREEN TV!"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 2;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("So invite some friends/enemies over, plug in those USB controllers, and have a blast -"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 4;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("Press ALT+ENTER to toggle FULLSCREEN mode"), GUIFont::Centre);
        // Draw the copyright notice
        yTextPos = g_FrameMan.GetResY() - pFont->GetFontHeight();
        char copyRight[512];
        sprintf_s(copyRight, sizeof(copyRight), "Cortex Command is TM and %c 2017 Data Realms, LLC", -35);
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, copyRight, GUIFont::Centre);

        if (ITP.m_sectionSwitch) {
            ITP.m_duration = 7.0;
            ITP.m_sectionSwitch = false;
        }
        if (ITP.m_elapsed > ITP.m_duration || ITP.m_keyPressed) {
            g_IntroState = NOTICEFADEOUT;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == NOTICEFADEOUT) {
        ITP.StartOfWhile();

        // Draw the early build notice
        g_FrameMan.ClearBackBuffer32();
        yTextPos = g_FrameMan.GetResY() / 3;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("N O T E :"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 2;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("This game plays great with up to FOUR people on a BIG-SCREEN TV!"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 2;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("So invite some friends/enemies over, plug in those USB controllers, and have a blast -"), GUIFont::Centre);
        yTextPos += pFont->GetFontHeight() * 4;
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("Press ALT+ENTER to toggle FULLSCREEN mode"), GUIFont::Centre);
        // Draw the copyright notice
        yTextPos = g_FrameMan.GetResY() - pFont->GetFontHeight();
        char copyRight[512];
        sprintf_s(copyRight, sizeof(copyRight), "Cortex Command is TM and %c 2017 Data Realms, LLC", -35);
        pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, copyRight, GUIFont::Centre);

        if (ITP.m_sectionSwitch) {
            // Black fade
            clear_to_color(pFadeScreen, 0);
            ITP.m_duration = 0.5;
            ITP.m_sectionSwitch = false;
        }
        fadePos = 255 * ITP.m_sectionProgress;
        set_trans_blender(fadePos, fadePos, fadePos, fadePos);
        draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        if (ITP.m_elapsed >= ITP.m_duration || ITP.m_keyPressed) {
            g_IntroState = FADEIN;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == FADEIN) {
        ITP.StartOfWhile();
        
        // this block only for FADEIN
        if (ITP.m_sectionSwitch) {
            ITP.m_songTimer.SetElapsedRealTimeS(0.05);
            ITP.m_scrollStart = ITP.m_songTimer.GetElapsedRealTimeS();
            // 66.6s This is the end of PRETITLE
            ITP.m_scrollDuration = 66.6 - ITP.m_scrollStart;
        }

        ITP.FadeinToPretitle();

        ITP.FromFadeinOnward();

        ITP.FadeinToScenariomenu();

        ITP.FromFadeinOnward2();

        ITP.FadeinToScenariomenu2();

        ITP.FromFadeinOnward3();
        
        /*! this block only for FADEIN
        Intro sequence logic */
        if (ITP.m_sectionSwitch) {
            // Start scroll at the bottom
            ITP.m_scrollOffset.m_Y = ITP.m_startYOffset;
            // Black fade
            clear_to_color(pFadeScreen, 0);
            ITP.m_duration = 1.0;
            ITP.m_sectionSwitch = false;
            // Play intro music
            g_AudioMan.PlayMusic("Base.rte/Music/Hubnester/ccintro.ogg", 0);
            g_AudioMan.SetMusicPosition(0.05);
            // Override music volume setting for the intro if it's set to anything
            if (g_AudioMan.GetMusicVolume() > 0.1) {
                g_AudioMan.SetTempMusicVolume(1.0);
            }
            //songTimer.Reset();
            ITP.m_songTimer.SetElapsedRealTimeS(0.05);
        }
        fadePos = 255 - (255 * ITP.m_sectionProgress);
        set_trans_blender(fadePos, fadePos, fadePos, fadePos);
        draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = SPACEPAUSE1;
            ITP.m_sectionSwitch = true;
        }
        
        ITP.FadeinToSlide8();

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == SPACEPAUSE1) {
        ITP.StartOfWhile();

        ITP.FadeinToPretitle();

        ITP.FromFadeinOnward();

        ITP.FadeinToScenariomenu();

        ITP.FromFadeinOnward2();

        ITP.FadeinToScenariomenu2();

        ITP.FromFadeinOnward3();

        // this block only for SPACEPAUSE1. taken from intro sequence logic.
        if (ITP.m_sectionSwitch) {
            sectionSongEnd = 3.7;
            ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
            ITP.m_sectionSwitch = false;
        }
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = SHOWSLIDE1;
            ITP.m_sectionSwitch = true;
        }

        ITP.FadeinToSlide8();

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState >= SHOWSLIDE1 && g_IntroState <= SHOWSLIDE8) {
        ITP.StartOfWhile();

        ITP.FadeinToPretitle();

        ITP.FromFadeinOnward();

        ITP.FadeinToScenariomenu();

        ITP.FromFadeinOnward2();

        ITP.FadeinToScenariomenu2();

        ITP.FromFadeinOnward3();

        /*! Slides Drawing
        This block only for SHOWSLIDE1 to SHOWSLIDE8*/
        int slide = g_IntroState - SHOWSLIDE1;
        Vector slideCenteredPos((resX / 2) - (apIntroSlides[slide]->w / 2), (resY / 2) - (apIntroSlides[slide]->h / 2));
        // Screen wide slide
        if (apIntroSlides[slide]->w <= resX) {
            slidePos.m_X = (resX / 2) - (apIntroSlides[slide]->w / 2);
        }
        // The slides wider than the screen, pan sideways
        else {
            if (ITP.m_elapsed < slideFadeInDuration) {
                slidePos.m_X = 0;
            }
            else if (ITP.m_elapsed < ITP.m_duration - slideFadeOutDuration) {
            slidePos.m_X = EaseInOut(0, resX - apIntroSlides[slide]->w, (ITP.m_elapsed - slideFadeInDuration) / (ITP.m_duration - slideFadeInDuration - slideFadeOutDuration));
        }
            else {
                slidePos.m_X = resX - apIntroSlides[slide]->w;
            }
        }
        // TEMP?
        slidePos.m_Y = slideCenteredPos.m_Y;
        // TEMP?
        if (ITP.m_elapsed < slideFadeInDuration) {
            fadePos = EaseOut(0, 255, ITP.m_elapsed / slideFadeInDuration);
            //                slidePos.m_Y = EaseOut(slideCenteredPos.m_Y - slideFadeDistance, slideCenteredPos.m_Y, ITP.m_elapsed / slideFadeInDuration);
        }
        else if (ITP.m_elapsed < ITP.m_duration - slideFadeOutDuration) {
            fadePos = 255;
            slidePos.m_Y = slideCenteredPos.m_Y;
        }
        else {
            fadePos = EaseIn(255, 0, (ITP.m_elapsed - ITP.m_duration + slideFadeOutDuration) / slideFadeOutDuration);
            //                slidePos.m_Y = EaseIn(slideCenteredPos.m_Y, slideCenteredPos.m_Y + slideFadeDistance, (ITP.m_elapsed - ITP.m_duration + slideFadeOutDuration) / slideFadeOutDuration);
        }
        if (fadePos > 0) {
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), apIntroSlides[slide], slidePos.m_X, slidePos.m_Y);
        }

        // Intro sequence logic
        if (g_IntroState == SHOWSLIDE1) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 11.4;
                slideFadeInDuration = 2.0;
                slideFadeOutDuration = 0.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (ITP.m_elapsed > 1.25) {
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "At the end of humanity's darkest century...", GUIFont::Centre);
            }
            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = SHOWSLIDE2;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE2) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 17.3;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 2.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }
            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (ITP.m_elapsed < ITP.m_duration - 1.75) {
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...a curious symbiosis between man and machine emerged.", GUIFont::Centre);
            }

            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = SHOWSLIDE3;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE3) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 25.1;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }
            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (/*ITP.m_elapsed > 0.75 && */ITP.m_sectionProgress < 0.49) {
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "This eventually enabled humans to leave their natural bodies...", GUIFont::Centre);
            }
            else if (ITP.m_sectionProgress > 0.51) {
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...and to free their minds from obsolete constraints.", GUIFont::Centre);
            }
            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = SHOWSLIDE4;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE4) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 31.3;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }
            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "With their brains sustained by artificial means, space travel also became feasible.", GUIFont::Centre);
            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = SHOWSLIDE5;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE5) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 38.0;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }
            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Other civilizations were encountered...", GUIFont::Centre);
            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = SHOWSLIDE6;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE6) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 44.1;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }
            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...and peaceful intragalactic trade soon established.", GUIFont::Centre);
            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = SHOWSLIDE7;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE7) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 51.5;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }
            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Now, the growing civilizations create a huge demand for resources...", GUIFont::Centre);
            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = SHOWSLIDE8;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE8) {
            if (ITP.m_sectionSwitch) {
                sectionSongEnd = 64.5;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
                ITP.m_sectionSwitch = false;
            }
            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (ITP.m_sectionProgress < 0.30) {
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...which can only be satisfied by the ever-expanding frontier.", GUIFont::Centre);
            }
            else if (ITP.m_sectionProgress > 0.33 && ITP.m_sectionProgress < 0.64) {
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Competition is brutal and anything goes in this galactic gold rush.", GUIFont::Centre);
            }
            else if (ITP.m_sectionProgress > 0.67) {
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "You will now join it on a venture to an untapped planet...", GUIFont::Centre);
            }

            if (ITP.m_elapsed >= ITP.m_duration) {
                g_IntroState = PRETITLE;
                ITP.m_sectionSwitch = true;
            }
        }

        ITP.FadeinToSlide8();

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == PRETITLE) {
        ITP.StartOfWhile();

        ITP.FadeinToPretitle();

        ITP.FromFadeinOnward();

        ITP.FadeinToScenariomenu();

        ITP.FromFadeinOnward2();

        ITP.FadeinToScenariomenu2();

        ITP.FromFadeinOnward3();

        // Intro sequence logic
        if (ITP.m_sectionSwitch) {
            sectionSongEnd = 66.6;
            ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
            ITP.m_sectionSwitch = false;
        }
        yTextPos = (g_FrameMan.GetResY() / 2);
        if (ITP.m_elapsed > 0.05) {
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Prepare to assume...", GUIFont::Centre);
        }
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = TITLEAPPEAR;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == TITLEAPPEAR) {
        ITP.StartOfWhile();

        // Scrolling logic
        if (ITP.m_sectionSwitch) {
            ITP.m_scrollStart = ITP.m_songTimer.GetElapsedRealTimeS();
            ITP.m_scrollDuration = 92.4 - ITP.m_scrollStart;
        }
        
        ITP.TitleappearToPlanetscroll();

        ITP.FromFadeinOnward();

        ITP.FadeinToScenariomenu();

        ITP.FromFadeinOnward2();

        ITP.FadeinToScenariomenu2();

        ITP.FromFadeinOnward3();

        // Game logo drawing
        ITP.m_pTitle->SetPos(Vector(resX / 2, (resY / 2) - 20));

        ITP.TitleGlow();

        // Intro sequence logic
        if (ITP.m_sectionSwitch) {
            // White fade
            clear_to_color(pFadeScreen, 0xFFFFFFFF);
            sectionSongEnd = 68.2;
            ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
            ITP.m_sectionSwitch = false;
        }
        fadePos = LERP(0, 0.5, 255, 0, ITP.m_sectionProgress);
        if (fadePos >= 0) {
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
        }
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = PLANETSCROLL;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == PLANETSCROLL) {
        ITP.StartOfWhile();

        ITP.TitleappearToPlanetscroll();

        ITP.FromFadeinOnward();

        ITP.FadeinToScenariomenu();

        ITP.FromFadeinOnward2();

        ITP.FadeinToScenariomenu2();

        ITP.FromFadeinOnward3();

        // Game logo drawing
        if (ITP.m_sectionProgress > 0.5) {
            ITP.m_pTitle->SetPos(Vector(resX / 2, EaseIn((resY / 2) - 20, 120, (ITP.m_sectionProgress - 0.5) / 0.5)));
        }

        ITP.TitleGlow();

        // Intro sequence logic
        if (ITP.m_sectionSwitch) {
            sectionSongEnd = 92.4;
            ITP.m_duration = sectionSongEnd - ITP.m_songTimer.GetElapsedRealTimeS();
            ITP.m_sectionSwitch = false;
        }
        if (ITP.m_elapsed >= ITP.m_duration) {
            g_IntroState = PREMENU;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState == PREMENU) {
        ITP.StartOfWhile();

        ITP.FromFadeinOnward();

        ITP.FadeinToScenariomenu();

        ITP.FromFadeinOnward2();

        ITP.FadeinToScenariomenu2();

        ITP.FromFadeinOnward3();

        ITP.TitleGlow();

        // Intro sequence logic
        if (ITP.m_sectionSwitch) {
            ITP.m_duration = 3.0;
            ITP.m_sectionSwitch = false;
            ITP.m_scrollOffset.m_Y = ITP.m_preMenuYOffset;
        }
        if (ITP.m_elapsed >= ITP.m_duration || ITP.m_keyPressed) {
            g_IntroState = MENUAPPEAR;
            ITP.m_sectionSwitch = true;
        }

        EndOfWhile();
    }

    while (KeepWhiling() && g_IntroState != END) {
        assert(g_IntroState >= FADEIN);
        ITP.StartOfWhile();
       
        // Scroll the last bit to reveal the menu appears
        if (g_IntroState == MENUAPPEAR)
        {
            ITP.m_scrollOffset.m_Y = EaseOut(ITP.m_preMenuYOffset, topMenuYOffset, ITP.m_sectionProgress);
        }
        // Scroll down to the planet screen
        else if (g_IntroState == MAINTOSCENARIO || g_IntroState == MAINTOCAMPAIGN)
        {
            ITP.m_scrollOffset.m_Y = EaseOut(topMenuYOffset, planetViewYOffset, ITP.m_sectionProgress);
        }
        // Scroll back up to the main screen from campaign
        else if (g_IntroState == PLANETTOMAIN)
        {
            ITP.m_scrollOffset.m_Y = EaseOut(planetViewYOffset, topMenuYOffset, ITP.m_sectionProgress);
        }
        
        ////////// Scene drawing

        ITP.FromFadeinOnward();
        if (g_IntroState <= SCENARIOMENU) {
            ITP.FadeinToScenariomenu();
        }
        ITP.FromFadeinOnward2();
        if (g_IntroState <= SCENARIOMENU) {
            ITP.FadeinToScenariomenu2();
        }
        ITP.FromFadeinOnward3();

        if (g_IntroState == MENUACTIVE) {
            if (g_pMainMenuGUI->AllowPioneerPromo() && ITP.m_orbitRotation < -c_PI * 1.25 && ITP.m_orbitRotation > -c_PI * 1.95) {
                // After capsule flew some time, start showing angry pioneer
                if (ITP.m_orbitRotation < -c_PI * 1.32 && ITP.m_orbitRotation > -c_PI * 1.65) {
                    Vector pioneerScreamPos = ITP.m_planetPos - Vector(320 - 130, 320 + 44);
                    // Draw line to indicate that the screaming guy is the one in the drop pod
                    drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
                    g_pScenarioGUI->SetPlanetInfo(Vector(0, 0), planetRadius);
                    g_pScenarioGUI->DrawScreenLineToSitePoint(g_FrameMan.GetBackBuffer32(), pioneerScreamPos, ITP.m_pPioneerCapsule->GetPos(), makecol(255, 255, 255), -1, -1, 40, 0.20);
                    drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
                    // Draw pioneer
                    pPioneerScreaming->SetPos(pioneerScreamPos + ITP.m_shakeOffset);
                    pPioneerScreaming->Draw(g_FrameMan.GetBackBuffer32());
                    // Enable the promo banner and tell the menu where it can be clicked
                    g_pMainMenuGUI->EnablePioneerPromoButton();
                    Box promoBox(pioneerScreamPos.m_X - 125, pioneerScreamPos.m_Y - 70, pioneerScreamPos.m_X + 125, pioneerScreamPos.m_Y + 70);
                    g_pMainMenuGUI->SetPioneerPromoBox(promoBox);
                }
                if (ITP.m_orbitRotation < -c_PI * 1.65 && ITP.m_orbitRotation > -c_PI * 1.95) {
                    Vector promoPos = ITP.m_planetPos - Vector(320 - 128, 320 + 29);
                    // Draw pioneer promo
                    pPioneerPromo->SetPos(promoPos);
                    pPioneerPromo->Draw(g_FrameMan.GetBackBuffer32());
                    // Enable the promo banner and tell the menu where it can be clicked
                    g_pMainMenuGUI->EnablePioneerPromoButton();
                    Box promoBox(promoPos.m_X - 128, promoPos.m_Y - 80, promoPos.m_X + 128, promoPos.m_Y + 80);
                    g_pMainMenuGUI->SetPioneerPromoBox(promoBox);
                }
            }
        }

        /////////////////////////////
        // Game Logo drawing

        if ((g_IntroState >= TITLEAPPEAR && g_IntroState < SCENARIOFADEIN) || g_IntroState == MAINTOCAMPAIGN)
        {
            if (g_IntroState == MENUAPPEAR)
                ITP.m_pTitle->SetPos(Vector(resX / 2, EaseOut(120, 64, ITP.m_sectionProgress)));
            else if (g_IntroState == MAINTOSCENARIO || g_IntroState == MAINTOCAMPAIGN)
                ITP.m_pTitle->SetPos(Vector(resX / 2, EaseOut(64, -150, ITP.m_sectionProgress)));
            else if (g_IntroState > MENUAPPEAR)
                ITP.m_pTitle->SetPos(Vector(resX / 2, 64));

            ITP.m_pTitleGlow->SetPos(ITP.m_pTitle->GetPos());

            ITP.m_pTitle->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawAlpha);
            // Screen blend the title glow on top, with some flickering in its intensity
            int blendAmount = 220 + 35 * NormalRand();
            set_screen_blender(blendAmount, blendAmount, blendAmount, blendAmount);
            ITP.m_pTitleGlow->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawTrans);
        }

        /////////////////////////////
        // Menu drawing

        // Main Menu updating and drawing, behind title
        if (g_IntroState >= MENUAPPEAR)
        {
            if (g_IntroState == MENUAPPEAR)
            {
                // TODO: some fancy transparency effect here
/*
                g_pMainMenuGUI->Update();
                clear_to_color(pFadeScreen, 0xFFFF00FF);
                g_pMainMenuGUI->Draw(pFadeScreen);
                fadePos = 255 * ITP.m_sectionProgress;
                set_trans_blender(fadePos, fadePos, fadePos, fadePos);
                draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
*/
            }
            else if (g_IntroState == MENUACTIVE)
            {
                g_pMainMenuGUI->Update();
                g_pMainMenuGUI->Draw(g_FrameMan.GetBackBuffer32());
            }
        }

        // Scenario setup menu update and drawing
        if (g_IntroState == SCENARIOMENU)
        {
            g_pScenarioGUI->SetPlanetInfo(ITP.m_planetPos, planetRadius);
            g_pScenarioGUI->Update();
            g_pScenarioGUI->Draw(g_FrameMan.GetBackBuffer32());
        }

        // Metagame menu update and drawing
        if (g_IntroState == CAMPAIGNPLAY)
        {
            g_MetaMan.GetGUI()->SetPlanetInfo(ITP.m_planetPos, planetRadius);
            g_MetaMan.Update();
            g_MetaMan.Draw(g_FrameMan.GetBackBuffer32());
        }

        //////////////////////////////////////////////////////////
        // Intro sequence logic

        if (g_IntroState == MENUAPPEAR)
        {
            if (ITP.m_sectionSwitch)
            {
                ITP.m_duration = 1.0;
                ITP.m_sectionSwitch = false;
                ITP.m_scrollOffset.m_Y = ITP.m_preMenuYOffset;

                // Play the main menu ambient
                g_AudioMan.PlayMusic("Base.rte/Music/Hubnester/ccmenu.ogg", -1);
            }

            if (ITP.m_elapsed >= ITP.m_duration || g_NetworkServer.IsServerModeEnabled())
            {
                g_IntroState = MENUACTIVE;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == MENUACTIVE)
        {
            if (ITP.m_sectionSwitch)
            {
                ITP.m_scrollOffset.m_Y = topMenuYOffset;
                // Fire up the menu
                g_pMainMenuGUI->SetEnabled(true);
                // Indicate that we're now in the main menu
                g_InActivity = false;

                ITP.m_sectionSwitch = false;
            }

            // Detect quitting of the program from the menu button
            g_Quit = g_Quit || g_pMainMenuGUI->QuitProgram();

            // Detect if a scenario mode has been commanded to start
            if (g_pMainMenuGUI->ScenarioStarted())
            {
                g_IntroState = MAINTOSCENARIO;
                ITP.m_sectionSwitch = true;
            }

            // Detect if a campaign mode has been commanded to start
            if (g_pMainMenuGUI->CampaignStarted())
            {
                g_IntroState = MAINTOCAMPAIGN;
                ITP.m_sectionSwitch = true;
            }

            // Detect if the current game has been commanded to resume
            if (g_pMainMenuGUI->ActivityResumed())
                g_ResumeActivity = true;

            // Detect if a game has been commanded to restart
            if (g_pMainMenuGUI->ActivityRestarted())
            {
                // Make sure the scene is going to be reset with the new parameters
                g_ResetActivity = true;

                g_IntroState = FADESCROLLOUT;
                ITP.m_sectionSwitch = true;
            }

            if (g_NetworkServer.IsServerModeEnabled())
            {
                EnterMultiplayerLobby();
                g_IntroState = FADESCROLLOUT;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == MAINTOSCENARIO)
        {
            if (ITP.m_sectionSwitch)
            {
                ITP.m_duration = 2.0;
                ITP.m_sectionSwitch = false;

                // Reset the Scenario menu
                g_pScenarioGUI->SetEnabled(true);

                // Play the scenario music with juicy start sound
                g_GUISound.SplashSound()->Play();
                g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/thisworld5.ogg", -1);
            }

            if (ITP.m_elapsed >= ITP.m_duration || g_NetworkServer.IsServerModeEnabled())// || ITP.m_keyPressed)
            {
                g_IntroState = SCENARIOMENU;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SCENARIOFADEIN)
        {
            if (ITP.m_sectionSwitch)
            {
                // Scroll to planet pos
                ITP.m_scrollOffset.m_Y = planetViewYOffset;
                // Black fade
                clear_to_color(pFadeScreen, 0);

                ITP.m_duration = 1.0;
                ITP.m_sectionSwitch = false;
            }

            fadePos = 255 - (255 * ITP.m_sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (ITP.m_elapsed >= ITP.m_duration)
            {
                g_IntroState = SCENARIOMENU;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == SCENARIOMENU)
        {
            if (ITP.m_sectionSwitch)
            {
                ITP.m_scrollOffset.m_Y = planetViewYOffset;
                ITP.m_sectionSwitch = false;
            }

            // Detect quitting of the program from the menu button
            g_Quit = g_Quit || g_pScenarioGUI->QuitProgram();

            // Detect if user wants to go back to main menu
            if (g_pScenarioGUI->BackToMain())
            {
                g_IntroState = PLANETTOMAIN;
                ITP.m_sectionSwitch = true;
            }

            // Detect if the current game has been commanded to resume
            if (g_pScenarioGUI->ActivityResumed())
                g_ResumeActivity = true;

            // Detect if a game has been commanded to restart
            if (g_pScenarioGUI->ActivityRestarted())
            {
                // Make sure the scene is going to be reset with the new parameters
                g_ResetActivity = true;

                g_IntroState = FADEOUT;
                ITP.m_sectionSwitch = true;
            }

            // In server mode once we exited to main or scenario menu we need to start Lobby activity 
            if (g_NetworkServer.IsServerModeEnabled())
            {
                EnterMultiplayerLobby();
                g_IntroState = FADEOUT;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == MAINTOCAMPAIGN)
        {
            if (ITP.m_sectionSwitch)
            {
                ITP.m_duration = 2.0;
                ITP.m_sectionSwitch = false;

                // Play the campaign music with Meta sound start
                g_GUISound.SplashSound()->Play();
                g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/thisworld5.ogg", -1);
            }

            if (ITP.m_elapsed >= ITP.m_duration)// || ITP.m_keyPressed)
            {
                g_IntroState = CAMPAIGNPLAY;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == CAMPAIGNFADEIN)
        {
            if (ITP.m_sectionSwitch)
            {
                // Scroll to campaign pos
                ITP.m_scrollOffset.m_Y = planetViewYOffset;
                // Black fade
                clear_to_color(pFadeScreen, 0);

                ITP.m_duration = 1.0;
                ITP.m_sectionSwitch = false;
            }

            fadePos = 255 - (255 * ITP.m_sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (ITP.m_elapsed >= ITP.m_duration)
            {
                g_IntroState = CAMPAIGNPLAY;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == CAMPAIGNPLAY)
        {
            if (ITP.m_sectionSwitch)
            {
                ITP.m_scrollOffset.m_Y = planetViewYOffset;
                ITP.m_sectionSwitch = false;
            }

            // Detect quitting of the program from the menu button
            g_Quit = g_Quit || g_MetaMan.GetGUI()->QuitProgram();

            // Detect if user wants to go back to main menu
            if (g_MetaMan.GetGUI()->BackToMain())
            {
                g_IntroState = PLANETTOMAIN;
                ITP.m_sectionSwitch = true;
            }

            // Detect if a game has been commanded to restart
            if (g_MetaMan.GetGUI()->ActivityRestarted())
            {
                // Make sure the scene is going to be reset with the new parameters
                g_ResetActivity = true;

                g_IntroState = FADEOUT;
                ITP.m_sectionSwitch = true;
            }
            // Detect if the current game has been commanded to resume
            if (g_MetaMan.GetGUI()->ActivityResumed())
                g_ResumeActivity = true;
        }
        else if (g_IntroState == PLANETTOMAIN)
        {
            if (ITP.m_sectionSwitch)
            {
                ITP.m_duration = 2.0;
                ITP.m_sectionSwitch = false;
            }

            if (ITP.m_elapsed >= ITP.m_duration)// || ITP.m_keyPressed)
            {
                g_IntroState = MENUACTIVE;
                ITP.m_sectionSwitch = true;
            }
        }
        else if (g_IntroState == FADESCROLLOUT)
        {
            if (ITP.m_sectionSwitch)
            {
                // Black fade
                clear_to_color(pFadeScreen, 0x00000000);
                ITP.m_duration = 1.5;
                ITP.m_sectionSwitch = false;
            }

            ITP.m_scrollOffset.m_Y = EaseIn(topMenuYOffset, 250, ITP.m_sectionProgress);

            fadePos = EaseIn(0, 255, ITP.m_sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            // Fade out the music as well
            g_AudioMan.SetTempMusicVolume(EaseIn(g_AudioMan.GetMusicVolume(), 0, ITP.m_sectionProgress));

            if (ITP.m_elapsed >= ITP.m_duration)
            {
                g_IntroState = END;
                ITP.m_sectionSwitch = true;
                g_FrameMan.ClearBackBuffer32();
            }
        }
        else if (g_IntroState == FADEOUT)
        {
            if (ITP.m_sectionSwitch)
            {
                // White fade
                clear_to_color(pFadeScreen, 0x00000000);
                ITP.m_duration = 1.5;
                ITP.m_sectionSwitch = false;
            }

            fadePos = EaseIn(0, 255, ITP.m_sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            // Fade out the music as well
            g_AudioMan.SetTempMusicVolume(EaseIn(g_AudioMan.GetMusicVolume(), 0, ITP.m_sectionProgress));

            if (ITP.m_elapsed >= ITP.m_duration)
            {
                g_IntroState = END;
                ITP.m_sectionSwitch = true;
                g_FrameMan.ClearBackBuffer32();
            }
        }

        EndOfWhile();
    }

    // Clean up heap data
    destroy_bitmap(pFadeScreen); pFadeScreen = 0;
    for (int slide = 0; slide < SLIDECOUNT; ++slide)
    {
        destroy_bitmap(apIntroSlides[slide]);
        apIntroSlides[slide] = 0;
    }
    delete[] apIntroSlides; apIntroSlides = 0;
    delete ITP.m_pBackdrop; ITP.m_pBackdrop = 0;
    delete ITP.m_pTitle; ITP.m_pTitle = 0;
    delete ITP.m_pPlanet; ITP.m_pPlanet = 0;
    delete ITP.m_pMoon; ITP.m_pMoon = 0;
    delete ITP.m_pStation; ITP.m_pStation = 0;
    delete ITP.m_pPioneerCapsule; ITP.m_pPioneerCapsule = 0;
    delete pPioneerScreaming; pPioneerScreaming = 0;
    delete ITP.m_pFirePuffLarge; ITP.m_pFirePuffLarge = 0;
    delete ITP.m_pFirePuffMedium; ITP.m_pFirePuffMedium = 0;
    delete[] ITP.m_aStars; ITP.m_aStars = 0;

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Orders to reset the entire Retro Terrain Engine system next iteration.
/// </summary>
void ResetRTE() { g_ResetRTE = true; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Indicates whether the system is about to be reset before the next loop starts.
/// </summary>
/// <returns>Whether the RTE is about to reset next iteration of the loop or not.</returns>
bool IsResettingRTE() { return g_ResetRTE; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Game simulation loop.
/// </summary>
bool RunGameLoop() {
	if (g_Quit) {
		return true;
	}
	g_PerformanceMan.ResetFrameTimer();
	g_TimerMan.EnableAveraging(true);
	g_TimerMan.PauseSim(false);

	if (g_ResetActivity) { ResetActivity(); }

	while (!g_Quit) {
		// Need to clear this out; sometimes background layers don't cover the whole back
		g_FrameMan.ClearBackBuffer8();

		// Update the real time measurement and increment
		g_TimerMan.Update();

		bool serverUpdated = false;

		// Simulation update, as many times as the fixed update step allows in the span since last frame draw
		while (g_TimerMan.TimeForSimUpdate()) {
			serverUpdated = false;
			g_PerformanceMan.NewPerformanceSample();

			// Advance the simulation time by the fixed amount
			g_TimerMan.UpdateSim();

			g_PerformanceMan.StartPerformanceMeasurement(PerformanceMan::PERF_SIM_TOTAL);

			g_UInputMan.Update();

			// It is vital that server is updated after input manager but before activity because input manager will clear received pressed and released events on next update.
			if (g_NetworkServer.IsServerModeEnabled()) {
				g_NetworkServer.Update(true);
				serverUpdated = true;
			}
			g_FrameMan.Update();
			g_AudioMan.Update();
			g_LuaMan.Update();
			g_PerformanceMan.StartPerformanceMeasurement(PerformanceMan::PERF_ACTIVITY);
			g_ActivityMan.Update();
			g_PerformanceMan.StopPerformanceMeasurement(PerformanceMan::PERF_ACTIVITY);
			g_MovableMan.Update();

			g_ActivityMan.LateUpdateGlobalScripts();

			g_ConsoleMan.Update();
			g_PerformanceMan.StopPerformanceMeasurement(PerformanceMan::PERF_SIM_TOTAL);

			if (!g_InActivity) {
				g_TimerMan.PauseSim(true);
				// If we're not in a metagame, then show main menu
				if (g_MetaMan.GameInProgress()) {
					g_IntroState = CAMPAIGNFADEIN;
				} else {
					Activity * pActivity = g_ActivityMan.GetActivity();
					// If we edited something then return to main menu instead of scenario menu player will probably switch to area/scene editor.
					if (pActivity && pActivity->GetPresetName() == "None") {
						g_IntroState = MENUAPPEAR;
					} else {
						g_IntroState = MAINTOSCENARIO;
					}
				}
				PlayIntroTitle();
			}
			// Resetting the simulation
            // Reset and quit if user quit during reset loading
			if (g_ResetActivity && !ResetActivity()) {
				break;
			}
			// Resuming the simulation
			if (g_ResumeActivity) { ResumeActivity(); }
		}

		if (g_NetworkServer.IsServerModeEnabled()) {
			// Pause sim while we're waiting for scene transmission or scene will start changing before clients receive them and those changes will be lost.
			if (!g_NetworkServer.ReadyForSimulation()) {
				g_TimerMan.PauseSim(true);
			} else {
				if (g_InActivity) { g_TimerMan.PauseSim(false); }
			}
			if (!serverUpdated) {
				g_NetworkServer.Update();
				serverUpdated = true;
			}
			if (g_SettingsMan.GetServerSimSleepWhenIdle()) {
				signed long long ticksToSleep = g_TimerMan.GetTimeToSleep();
				if (ticksToSleep > 0) {
					double secsToSleep = (double)ticksToSleep / (double)g_TimerMan.GetTicksPerSecond();
					long long milisToSleep = (long long)secsToSleep * (1000);
					std::this_thread::sleep_for(std::chrono::milliseconds(milisToSleep));
				}
			}
		}
		g_FrameMan.Draw();
		g_FrameMan.FlipFrameBuffers();
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Command-line argument handling.
/// </summary>
/// <param name="argc">Argument name.</param>
/// <param name="argv">Argument value.</param>
/// <param name="appExitVar">The appExitVar is what the program should exit with if this returns false.</param>
/// <returns>False if app should quit right after this.</returns>
bool HandleMainArgs(int argc, char *argv[], int &appExitVar) {

    // If no additional arguments passed, just continue (first argument is the program path)
    if (argc == 1) {
		return true;
	}
    // Default program return var if fail
    appExitVar = 2;

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            // Print loading screen console to cout
			if (std::strcmp(argv[i], "-cout") == 0) {
				g_System.SetLogToCLI(true);
			} else if (i + 1 < argc) {
				// Launch game in server mode
                if (std::strcmp(argv[i], "-server") == 0 && i + 1 < argc) {
                    std::string port = argv[++i];
                    g_NetworkServer.EnableServerMode();
                    g_NetworkServer.SetServerPort(port);
				// Load a single module right after the official modules
                } else if (std::strcmp(argv[i], "-module") == 0 && i + 1 < argc) {
					g_PresetMan.SetSingleModuleToLoad(argv[++i]);
				// Launch game directly into editor activity
				} else if (std::strcmp(argv[i], "-editor") == 0 && i + 1 < argc) {
					const char *editorName = argv[++i];
					if (std::strcmp(editorName, "") == 1) {
						g_EditorToLaunch = editorName;
						g_LaunchIntoEditor = true;
					}
				}
            }
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Implementation of the main function.
/// </summary>
int main(int argc, char *argv[]) {

	///////////////////////////////////////////////////////////////////
    // Initialize Allegro

/* This is obsolete; only applied when we were loading from a compressed Base.rte // might come in handy sometime later actually
    // Load the Allegro config data from the base datafile
    DATAFILE *pConfigFile = load_datafile_object("Base.rte/Base.dat", "AConfig");
    if (pConfigFile) {
        set_config_data((char *)pConfigFile->dat, pConfigFile->size);
        // The above copies the data, so this is safe to do
        unload_datafile_object(pConfigFile);
    } */

    set_config_file("Base.rte/AllegroConfig.txt");
    allegro_init();

    // Enable the exit button on the window
    LOCK_FUNCTION(QuitHandler);
    set_close_button_callback(QuitHandler);

    // Seed the random number generator
    SeedRand();

    ///////////////////////////////////////////////////////////////////
    // Instantiate all the managers

    new ConsoleMan();
    new LuaMan();
    new SettingsMan();
    new TimerMan();
	new PerformanceMan();
    new PresetMan();
    new FrameMan();
	new PostProcessMan();
	new PrimitiveMan();
    new AudioMan();
    new GUISound();
    new UInputMan();
    new ActivityMan();
    new MovableMan();
    new SceneMan();
    new MetaMan();

	new NetworkServer();
	new NetworkClient();

    ///////////////////////////////////////////////////////////////////
    // Create the essential managers

    g_LuaMan.Create();
	
	Reader settingsReader("Base.rte/Settings.ini", false, 0, true);
    g_SettingsMan.Create(settingsReader);

	g_NetworkServer.Create();
	g_NetworkClient.Create();

    int exitVar = 0;
    if (!HandleMainArgs(argc, argv, exitVar)) {
		return exitVar;
	}
    g_TimerMan.Create();
	g_PerformanceMan.Create();
    g_PresetMan.Create();
    g_FrameMan.Create();
    g_PostProcessMan.Create();
    if (g_AudioMan.Create() >= 0) {
        g_GUISound.Create();
    }
    g_UInputMan.Create();
	if (g_NetworkServer.IsServerModeEnabled()) { g_UInputMan.SetMultiplayerMode(true); }
    g_ConsoleMan.Create();
    g_ActivityMan.Create();
    g_MovableMan.Create();
    g_MetaMan.Create();

    ///////////////////////////////////////////////////////////////////
    // Main game driver

	if (g_NetworkServer.IsServerModeEnabled()) {
		g_NetworkServer.Start();
		g_FrameMan.SetMultiplayerMode(true);
		g_AudioMan.SetMultiplayerMode(true);
		g_AudioMan.SetSoundsVolume(0);
		g_AudioMan.SetMusicVolume(0);
	}

    new LoadingGUI();
	g_LoadingGUI.InitLoadingScreen();
	InitMainMenu();

	if (g_LaunchIntoEditor) { 
		// Force mouse + keyboard with default mapping so we won't need to change manually if player 1 is set to keyboard only or gamepad.
		g_UInputMan.GetControlScheme(0)->SetDevice(1);
		g_UInputMan.GetControlScheme(0)->SetPreset(1);
		// Disable intro sequence.
		g_SettingsMan.SetPlayIntro(false);
		// Start the specified editor activity.
		EnterEditorActivity(g_EditorToLaunch);
	}

    if (g_SettingsMan.PlayIntro() && !g_NetworkServer.IsServerModeEnabled()) { PlayIntroTitle(); }

	// NETWORK Create multiplayer lobby activity to start as default if server is running
	if (g_NetworkServer.IsServerModeEnabled()) { EnterMultiplayerLobby(); }

    // If we fail to start/reset the activity, then revert to the intro/menu
    if (!ResetActivity()) { PlayIntroTitle(); }
	
    RunGameLoop();

    ///////////////////////////////////////////////////////////////////
    // Clean up

	g_NetworkClient.Destroy();
	g_NetworkServer.Destroy();

    g_MetaMan.Destroy();
    g_MovableMan.Destroy();
    g_SceneMan.Destroy();
    g_ActivityMan.Destroy();
	g_GUISound.Destroy();
    g_AudioMan.Destroy();
    g_PresetMan.Destroy();
    g_UInputMan.Destroy();
	g_PerformanceMan.Destroy();
    g_FrameMan.Destroy();
    g_TimerMan.Destroy();
    g_SettingsMan.Destroy();
    g_LuaMan.Destroy();
    ContentFile::FreeAllLoaded();
    g_ConsoleMan.Destroy();

#ifdef DEBUG_BUILD
    // Dump out the info about how well memory cleanup went
    Entity::ClassInfo::DumpPoolMemoryInfo(Writer("MemCleanupInfo.txt"));
#endif
	
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) { return main(__argc, __argv); }