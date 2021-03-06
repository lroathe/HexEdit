/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is Copyright 1993 Jim Bumgardner.
 * 
 * The Initial Developer of the Original Code is Jim Bumgardner
 * Portions created by Lane Roathe (LR) are
 * Copyright (C) Copyright � 1996-2008.
 * All Rights Reserved.
 *
 * Contributor(s):
 *		Lane Roathe
 *		Nick Shanks (NS)
 *		Scott E. Lasley (SEL) 
 *		Brian Bergstrand (BB)
 */


#if TARGET_API_MAC_CARBON && __MWERKS__
	#include <PMApplication.h>
#endif

#include <stdarg.h>
#include <string.h>

#if TARGET_API_MAC_OS8
	#if PROFILE			// 6/15 Optional profiling support
		#include <Console.h>
		#include <Profile.h>
	#endif
#else
	#ifdef PROFILE
		#undef PROFILE
	#endif
	
	#ifndef __MWERKS__
		#include <Carbon/Carbon.h>
	#endif
#endif

#if !defined(__MC68K__) && defined(__MWERKS__)
	#include <InternetConfig.h>
#endif

#ifndef _HexEdit_
#define _HexEdit_

// Comatibility macros
//LR -- 1.72 :no longer requires carbon headers to compile for non-carbon targets
#if !TARGET_API_MAC_CARBON
	#if !defined(SetPortDialogPort)
		#define SetPortDialogPort SetPort
	#endif
	#if !defined(GetDialogPort)
		#define GetDialogPort( d ) d 	// BB: added (LR -- may already be defined, check!)
	#endif
	#if !defined(GetWindowPortBounds)	//LR 180 -- already defined!
		#define GetWindowPortBounds(w,rp) *rp = (w)->portRect
	#endif
	#define GetWindowBounds(w,t,rp) *rp = (*((CWindowPeek)w)->port.portPixMap)->bounds
	#define GetPortBounds(p,rp) *rp = (p)->portRect
	#define GetPortPixMap(p) p->portPixMap
	#define GetPortHiliteColor(p,c) *(c) = (*((GVarHandle)p->grafVars))->rgbHiliteColor	//LR 185
	#define InvalWindowRect(w,r) InvalRect(r)
	#define EnableMenuItem EnableItem
	#define DisableMenuItem DisableItem
	#define FrontNonFloatingWindow FrontWindow
	#define MenuRef MenuHandle
	#if !defined(GetDialogFromWindow)
		#define GetDialogFromWindow
	#endif
#endif

// make some things a bit easier to read

#define kAppCreator			FOUR_CHAR_CODE('hDmp')
#define kDefaultFileType	FOUR_CHAR_CODE('BINA')

//LR 185 -- under Carbon, alloc more RAM for faster ops
#if TARGET_API_MAC_CARBON
	#define kChunkSize		(128L * 1024L)		//LR 185 -new-
	#define kMaxFileRAM		(kChunkSize * 8)	//LR 185 32000L
#else
	#define kChunkSize		(32L * 1024L)		//LR 185 33000L
	#define kMaxFileRAM		(kChunkSize * 4)	//LR 185330000L
//	#define kSlushRAM		1000L
#endif

#define kAllocIncrement	64L

#define kSBarSize			16
#define kGrowIconSize	14

#define kHeaderHeight	16
#define kFooterHeight	16
#define kLineWidth		16

// define the character offsets into the g.buffer array (used to create draw string)
#define kDisBytesPerLine	4
#define kDisHexStart			11
#define kDisASCIIStart		73

#define kHexBytesPerLine	16
#define kHexHexStart			12
#define kHexASCIIStart		(kHexHexStart+(kBytesPerLine*3)+1)	//61

#define kMaxWindowChars		128

//	HR/LR 050328 - PPC disassembly support
#define kBytesPerLine	(dWin->bytesPerLine)
#define kStringHexPos	(dWin->hexStart)
#define kStringTextPos	(dWin->asciiStart)

//LR: 200 #define kWindowChars		77		//((kBytesPerLine * 3) + kBytesPerLine + kLineWidth + 1)		// number of characters wide the window is
#define kWindowChars		(kStringTextPos + kBytesPerLine)

//LR: 1.7 #define TopMargin		3
//LR: 1.7 #define BotMargin		0
//LR: 1.7 #define AsciiSpacing	6
//LR: 1.7 #define DescendHeight	0

#define kBodyStrLen		(kStringTextPos+kLineWidth-kStringHexPos)	// LR: 1.7 was 75 - (kStringHexPos -  2) in EditWindow.c

//LR 1.72 from resource #define kFontFace		"\pMonaco"
//#define kFontSize		9
#define kLineHeight		(g.lineHeight)	//11	//%% make flexible (and char width?)
#define kCharWidth		(g.charWidth)	//6
#define kHexWidth			(kCharWidth*3)

#define kBodyDrawPos	0	// LR: 120 - let's not have any undrawn areas, to avoid erasing!
#define kDataDrawPos	(kBodyDrawPos + ((kStringHexPos - 1) * kCharWidth))
#define kTextDrawPos	(kBodyDrawPos + ((kStringTextPos -  1) * kCharWidth))

//	HR/LR 050328 - PPC disassembly support (use divide and mod vs. shift and &)
#define LINENUM(a)	((a) / dWin->bytesPerLine)
#define COLUMN(a)		((a) % dWin->bytesPerLine)

#define CHARPOS(a)	((a) * kCharWidth)	//1.72 (((a) << 2) + ((a) << 1))	// Multiply by 6
#define HEXPOS(a)		(CHARPOS(a) * 3)	//1.72 ((a) << 4) + ((a) << 1))	// Multiply by 18

//	HR/LR 050328 - PPC disassembly support
#define kHexWindowWidth	((kWindowChars * kCharWidth) + kSBarSize)
#define kMaxWindowWidth ((kMaxWindowChars * kCharWidth) + kSBarSize)

//LR 1.7 #define MaxWindowHeight	512	// Note - these are NOT reversed!

// LR: #define PrefResType	'prf1'
// LR: #define PrefResID	128

/* NS: now uses Apple defined constants from events.h
#define TAB_KEY			0x09
#define RETURN_KEY		0x0D
#define ENTER_KEY		0x03
#define ESC_KEY			27
*/

// LR: 1.6.5 -- better ID defs, and include all references
typedef enum { alertSave = 10000, alertError, alertNoFork, alertRevert  }AlertIDs;
typedef enum { dlgSearch = 128, dlgGoto, dlgCompare, dlgComparePref, dlgAbout = 200, dlgGetFile = 1401 } DialogIDs;
typedef enum { strUndo = 128, strPrint, strHeader, strError, strColor, strPrompt, strFiles, strFont, strURLs = 200 } StringIDs;

typedef enum { errMemory = 1, errSeek, errRead, errSetFPos, errWrite, errPaste, errFindFolder,
						errCreate, errOpen, errFileInfo, errPrintRange, errSetFileInfo, errBackup,
						errRename, errDiskFull, errHexValues, errDefaultPrinter, errGenericPrinting,
						errSave, errReadOnly
					} ErrorIDs;

typedef enum { CT_Original, CT_Work, CT_Unwritten } ChunkTypes;
typedef enum { FT_Data = 1, FT_Resource } ForkType;
typedef enum { FM_Data = 1, FM_Rsrc, FM_Smart } ForkModes;
typedef enum { HD_Decimal = FT_Resource + 1, HD_Hex, HD_Footer } Headers;
typedef enum { FN_PrefsFolder = 1, FN_PrefsFile, FN_Untitled, FN_DATA, FN_RSRC } Filenames;
typedef enum { AM_Lo, AM_Hi } AsciiModes;

//	HR/LR 050328 - PPC disassembly support
typedef enum { DM_Dump, DM_Disassembly }DrawMode;

typedef enum { EM_Hex, EM_Decimal, EM_Ascii } EditMode;
typedef enum { EO_Undo = 1, EO_Redo, EO_Typing, EO_Paste, EO_Insert, EO_Overwrite, EO_Cut, EO_Clear, EO_Delete } EditOperation;
typedef enum { ES_Note, ES_Caution, ES_Stop, ES_Fatal } ErrorSeverity;
typedef enum { C_Arrow, C_Watch, C_IBeam } CursorNumbers;
typedef enum { CM_Byte = 1, CM_Word = 2, CM_Long = 4 } CompModeSize;	//LR 185 -- make enums be size!
typedef enum { CM_Different, CM_Match } CompModeType;

// preferences Dialog items
typedef enum { CP_Done=1,CP_Cancel,CP_Bytes,CP_Words,CP_Longs,CP_Different,CP_Match,
								CP_text1, CP_text2, CP_Case } CompPref;

// Color table resources MUST start @ 128 & advance by 1
#define CM_StartingResourceID 128

// window ID
#define	kSystem7Window			128		// NS 1.7.1
#define	kAppearanceWindow		129

#define kHexEditWindowTag		1000

// Mac OS versions
#define kMacOSSevenPointOne	 			0x00000710
#define kMacOSSevenPointFivePointFive	0x00000755
#define kMacOSEight							0x00000800
#define kMacOSEightPointFive				0x00000850
#define kMacOSEightPointSix				0x00000860
#define kMacOSNine							0x00000900
#define kMacOSNinePointOne					0x00000910
#define kMacOSTen								0x00001000

// CarbonLib versions
#define kCarbonLibOnePointOne				0x00000110
#define kCarbonLibOnePointThreePointOne 0x00000131

/*** COLOUR TABLE ***/
typedef struct
{
	RGBColor	header;
	RGBColor	headerLine;
	RGBColor	headerText;

	RGBColor	bar;
	RGBColor	barLine;
	RGBColor	barText;

	RGBColor	body;
	RGBColor	text;
	RGBColor	bodyDark;
	RGBColor	dividerLine;

}	HEColorTable_t, *HEColorTablePtr, **HEColorTableHandle;

/*** GLOBAL VARIABLES ***/
//LR: 1.66 -- removed undo ptrs
typedef struct
{
	// system info
	Boolean	quitFlag;
	SInt32	systemVersion;		// NS 1.7.1
#if !TARGET_API_MAC_CARBON	// LR: v1.6
	Boolean	WNEImplemented;
	Boolean	sys7Flag;
	Boolean	colorQDFlag;
#endif
	Boolean	dragAvailable;
	Boolean	translucentDrag;
	Boolean	navAvailable;
	Boolean	appearanceAvailable;
	
	// environment gPrefs
//LR 1.72 unused	Boolean		useAppleEvents;
	Boolean	useAppearance;
	Boolean	useNavServices;
	
	// application globals
	Boolean 	cursorFlag;
	Rect		cursRect;
	UInt8		forkMode;
	UInt8		highChar;
	SInt8		buffer[512];
	UInt16	maxHeight;

//	HR/LR 050328 - PPC disassembly support
	UInt16	maxWidth;
	Boolean	disassemble;		// treat content as PPC code and disassemble it

	UInt8		searchBuffer[256];
	UInt8		searchText[256];
	UInt8		replaceText[256];
	UInt8		gotoText[256];
	Boolean	searchDisabled;
	Boolean	replaceDisabled;
	Boolean	replaceAll;

	short		fontFaceID;
	short		fontSize;
	short		charWidth;
	short		lineHeight;

#ifndef __MC68K__
	ICInstance	icRef;
#endif

	// cursors
	CursHandle	watch;
	CursHandle	iBeam;
	
	// dialogs
	DialogPtr	searchDlg;
	DialogPtr	gotoDlg;

	// Offscreen drawing surface
	GWorldPtr	offscreen;

	// printing
#if TARGET_API_MAC_OS8
	THPrint		HPrint;
#endif
#if TARGET_API_MAC_CARBON	// SEL -- 1.7
	PMPrintSettings	printSettings;
	PMPageFormat		pageFormat;
#endif
}	globals;

extern globals g;

#endif