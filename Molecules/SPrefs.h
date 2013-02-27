#pragma once
#include "Registry.h"

#define SETTINGS_KEY "Software\\MatrixInc\\Molecules\\Settings"

typedef struct __PREFS
{

	GLfloat fMaxSpeed;	// x-axis acceleration: from 0.0f to 5.0f
	GLfloat fMinSpeed;	// y-axis acceleration: from 0.0f to 5.0f
	DWORD dFrameDelay;	// delay between frames: from 0 to 30 ms
	DWORD dDisplayTime;	// molecule presentation time: from 10000 to 60000 ms
	BOOL bShowDesc;	// show or hide molecule name: true or false
	BOOL bShowFPS;	// show framerate: true or false
	BOOL bWire;	// show molecule as wireframe: true or false
	BOOL bShowLinks;	// show links between atoms: true or false
	BOOL bAnimateBuild;	// animated building process	
	DWORD dRunTime;	// time after which screensaver will quit: from 0 (never) to 60
	BOOL bTeleType;	// animated molecule description (teletype style)
  BOOL bReflection;	// draws molecule reflected in the water
	BOOL bShowLabels;	// draws labes with atom names


  void Load()
  {
    CRegistry reg;
    reg.SetRootKey(HKEY_CURRENT_USER);
    reg.SetKey(SETTINGS_KEY, TRUE);
    fMaxSpeed = (GLfloat)reg.ReadFloat("MaxSpeed", 1.5f);
    dDisplayTime = reg.ReadDword("DisplayTime", 30000);
    dFrameDelay = reg.ReadDword("FrameDelay", 25);
    bShowDesc = reg.ReadBool("ShowDesc", TRUE);
    bShowFPS = reg.ReadBool("ShowFPS", FALSE);
    bWire = reg.ReadBool("Wire", FALSE);
    bShowLinks = reg.ReadBool("ShowLinks", TRUE);
    bAnimateBuild = reg.ReadBool("AnimateBuild", TRUE);
    dRunTime = reg.ReadDword("RunTime", 0);
    bTeleType = reg.ReadBool("TeleType", TRUE);
    bReflection = reg.ReadBool("Reflection", TRUE);
    bShowLabels = reg.ReadBool("ShowLabels", FALSE);

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
      reg.WriteDword("DisplayTime", dDisplayTime);
      reg.WriteDword("FrameDelay", dFrameDelay);
      reg.WriteBool("ShowFPS", bShowFPS);
      reg.WriteBool("ShowDesc", bShowDesc);
      reg.WriteBool("Wire", bWire);
      reg.WriteBool("ShowLinks", bShowLinks);
      reg.WriteBool("AnimateBuild", bAnimateBuild);
      reg.WriteDword("RunTime", dRunTime);
      reg.WriteBool("TeleType", bTeleType);
      reg.WriteBool("Reflection", bReflection);
      reg.WriteBool("ShowLabels", bShowLabels);
      return TRUE;
    }
    else return FALSE;
  }

} SPreferences;
