#pragma once
//#include "Archive.h" // not used, settings are stored in registry
#include "Registry.h"

//////////////////////////////////////////////////
// Preferences data storage
//////////////////////////////////////////////////

// #define SETTINGS_FILE "settings.bin"
#define SETTINGS_KEY "Software\\MatrixInc\\Molecules\\Settings"

typedef struct __PREFS
{

	GLfloat fMaxSpeed;	// x-axis acceleration: from 0.0f to 5.0f
	GLfloat fMinSpeed;	// y-axis acceleration: from 0.0f to 5.0f
	DWORD dFrameDelay;	// delay between frames: from 0 to 30 ms
	DWORD dShowTime;	// molecule presentation time: from 10000 to 60000 ms
	BOOL bShowDesc;		// show or hide molecule name: true or false
	BOOL bShowFPS;			// show framerate: true or false
	BOOL bWire;			// show molecule as wireframe: true or false
	BOOL bShowLinks;		// show links between atoms: true or false
	BOOL bAnimateBuild;	// animated building process	
	DWORD dRunTime;			// time after which screensaver will quit: from 0 (never) to 60
	BOOL bTeleType;			// animated molecule description (teletype style)
  BOOL bReflection;   // draws molecule reflected in the water


  void Load()
  {
    CRegistry reg;
    reg.SetRootKey(HKEY_CURRENT_USER);
    reg.SetKey(SETTINGS_KEY, TRUE);
    fMaxSpeed = (GLfloat)reg.ReadFloat("MaxSpeed", 3.0f);
    dShowTime = reg.ReadDword("ShowTime", 30000);
    dFrameDelay = reg.ReadDword("FrameDelay", 30);
    bShowDesc = reg.ReadBool("ShowDesc", TRUE);
    bShowFPS = reg.ReadBool("ShowFPS", FALSE);
    bWire = reg.ReadBool("Wire", FALSE);
    bShowLinks = reg.ReadBool("ShowLinks", TRUE);
    bAnimateBuild = reg.ReadBool("AnimateBuild", TRUE);
    dRunTime = reg.ReadDword("RunTime", 0);
    bTeleType = reg.ReadBool("TeleType", TRUE);
    bReflection = reg.ReadBool("Reflection", TRUE);

    // save settings, in case some of them did not exist
    Save();
  }

  BOOL Save()
  {
    CRegistry reg;
    reg.SetRootKey(HKEY_CURRENT_USER);
    if (reg.SetKey(SETTINGS_KEY, TRUE))
    {
      reg.WriteFloat("MaxSpeed", fMaxSpeed);
      reg.WriteDword("ShowTime", dShowTime);
      reg.WriteDword("FrameDelay", dFrameDelay);
      reg.WriteBool("ShowFPS", bShowFPS);
      reg.WriteBool("ShowDesc", bShowDesc);
      reg.WriteBool("Wire", bWire);
      reg.WriteBool("ShowLinks", bShowLinks);
      reg.WriteBool("AnimateBuild", bAnimateBuild);
      reg.WriteDword("RunTime", dRunTime);
      reg.WriteBool("TeleType", bTeleType);
      reg.WriteBool("Reflection", bReflection);
      return TRUE;
    }
    else return FALSE;
  }

	//void Serialize(CXArchive &ar)
	//{
	//	const int arch_version = 5; // v5 - 07.04.2008 - reflection
	//	if(ar.IsStoring())
	//	{
	//		ar << arch_version;
	//		ar << (float)fXspeed;
	//		ar << (float)fYspeed;
	//		ar << (float)fZspeed;
	//		ar << dShowTime;
	//		ar << dMoveDelay;
	//		ar << ShowDesc;
	//		ar << ShowFPS;
	//		ar << IsWire;
	//		ar << ShowLinks;
	//		ar << AnimateBuild;	// v2
	//		ar << dRunTime;
	//		ar << bTeleType;
 //     ar << bReflection;
	//	}
	//	else
	//	{
	//		int version = 0;
	//		ar >> version;
	//		ar >> fXspeed;
	//		ar >> fYspeed;
	//		ar >> fZspeed;
	//		ar >> dShowTime;
	//		ar >> dMoveDelay;
	//		ar >> ShowDesc;
	//		ar >> ShowFPS;
	//		ar >> IsWire;
	//		ar >> ShowLinks;
	//		if(version >= 2)
	//			ar >> AnimateBuild;
	//		else
	//			AnimateBuild = TRUE;
	//		if(version >= 3)
	//			ar >> dRunTime;
	//		else
	//			dRunTime = 0;
	//		if(version >= 4)
	//			ar >> bTeleType;
	//		else
	//			bTeleType = TRUE;
 //     if (version >= 5)
 //       ar >> bReflection;
 //     else
 //       bReflection = TRUE;
	//	}
	//}

} SPreferences;
