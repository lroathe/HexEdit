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
 * Portions created by Lane Roathe are
 * Copyright (C) Copyright � 1996-2000.
 * All Rights Reserved.
 * 
 * Contributor(s):
 *		Nick Shanks
 */

#include "EditScrollbar.h"
#include "EditRoutines.h"

extern globals g;

ControlActionUPP trackActionUPP;		// LR: init'd in Main!

/*** SETUP SCROLL BARS ***/
void SetupScrollBars( EditWindowPtr dWin )
{
	Rect	sRect, r;

	GetWindowPortBounds( dWin->oWin.theWin, &r );

	sRect.left = r.right - ( SBarSize - 1 );
	sRect.top = r.top + HeaderHeight;	// NS: move to below header
	sRect.right = r.right + 1;
	sRect.bottom = r.bottom	- GrowIconSize;
 	dWin->vScrollBar = NewControl( dWin->oWin.theWin, &sRect, "\p", true, 0, 0, sRect.bottom - sRect.top, scrollBarProc, 1L );
	AdjustScrollBars( dWin->oWin.theWin, 1 );
}

// Adjust scroll bars when they need to be redrawn for some reason.
// resizeFlag is an optimization to avoid extra work when you aren't resizing.

/*** ADJUST SCROLL BARS ***/
void AdjustScrollBars( WindowRef theWin, short resizeFlag )
{
	short			w, h;
	GrafPtr			savePort;
	long			limit;
	Rect			winRect;
	EditWindowPtr	dWin = (EditWindowPtr) GetWRefCon( theWin );

	GetWindowPortBounds( theWin, &winRect );

	GetPort( &savePort );
	
	if( resizeFlag )
	{
		// Adjust Lines Per Page
		// LR: allow one more line ( clipped at bottom of theWin instead of always a full line in theWin )
		dWin->linesPerPage = ( ( ( winRect.bottom - SBarSize ) - ( HeaderHeight + 1 ) - TopMargin - BotMargin ) / LineHeight );

		// Move sliders to new position
		// LR: per Shanks, control below theWin's header
		MoveControl( dWin->vScrollBar, winRect.right - ( SBarSize - 1 ), winRect.top + ( HeaderHeight - 1 ) );	

		// Change their sizes to fit new theWin dimensions
		w = SBarSize;
		h = winRect.bottom - winRect.top - ( GrowIconSize - 1 ) - ( HeaderHeight - 1 );

		SizeControl( dWin->vScrollBar, w, h );

		// Reset their maximum values
		SetControlMaximum( dWin->vScrollBar, h );
	}

	// Reposition pashorting if you have resized or scrolled past the legal
	// bounds	Note: this call will usually be followed by an update
	limit = ( ( dWin->fileSize+15 ) & 0xFFFFFFF0 ) - ( dWin->linesPerPage << 4 );
	if( dWin->editOffset > limit )
		dWin->editOffset = limit;
	if( dWin->editOffset < 0 )
		dWin->editOffset = 0;

	// Set the value of the sliders accordingly
	// 
// LR: why?	h = theWin->portRect.bottom - theWin->portRect.top - ( GrowIconSize - 1 ) - ( HeaderHeight - 1 );
	if( limit > 0 )
	{
		h = winRect.bottom - winRect.top - ( GrowIconSize - 1 ) - ( HeaderHeight -1 );

		SetControlMaximum( dWin->vScrollBar, h );
		if( dWin->editOffset < 64000L )
			SetControlValue( dWin->vScrollBar, (short) ( ( dWin->editOffset*h )/limit ) );
		else
			SetControlValue( dWin->vScrollBar, (short) ( dWin->editOffset / ( limit/h ) ) );
	}
	else
	{
		SetControlMaximum( dWin->vScrollBar, 0 );
		SetControlValue( dWin->vScrollBar, 0 );
	}
		
	SetPort( savePort );
}

// Callback routine to handle arrows and page up, page down
EditWindowPtr	gDWin;

/*** MY SCROLL ACTION ***/
pascal void MyScrollAction( ControlHandle theControl, short thePart )
{
	#pragma unused( theControl )	// LR

	long		curPos, newPos;
	short		pageWidth;
	Rect		myRect;

	WindowRef	gp = gDWin->oWin.theWin;

	curPos = gDWin->editOffset;
	newPos = curPos;

	GetWindowPortBounds( gp, &myRect );

	myRect.right -= SBarSize-1;
	myRect.bottom -= SBarSize-1;

	pageWidth = ( gDWin->linesPerPage-1 ) * SBarSize;

	switch( thePart )
	{
		case kControlUpButtonPart:		newPos = curPos - SBarSize;		break;	// LR: -- UH compliant
		case kControlDownButtonPart:	newPos = curPos + SBarSize;		break;
		case kControlPageUpPart:		newPos = curPos - pageWidth;	break;
		case kControlPageDownPart:		newPos = curPos + pageWidth;	break;
	}

	if( newPos != curPos )
		HEditScrollToPosition( gDWin, newPos );
}

// Intercept Handler for scroll bars
// Returns true if user clicked on scroll bar

/*** HANDLE SCROLL BAR CLICK ***/
Boolean MyHandleControlClick( WindowRef window, Point mouseLoc )
{
	short 			controlPart;
	ControlRef		control;
	short			vPos;
	Rect			winRect;
	EditWindowPtr	dWin = (EditWindowPtr) GetWRefCon( window );
	ControlActionUPP scrollAction;

	// NS: v1.6.6, new scrolling code to enable live scrolling on post-Appearance systems
	controlPart = FindControl( mouseLoc, window, &control );
	if( control == nil ) return false;
	
	// Identify theWin for callback procedure
	gDWin = dWin;

	// scroll the window	-- use old bits for now
/*	if( controlPart == kControlIndicatorPart && !g.useAppearance )	// in thumb (129)
	{
		TrackControl( control, mouseLoc, nil );
		::DrawBody( window );
	}
	else										// in arrows or page up/down
	{
		scrollAction = NewControlActionProc( FileScrollAction );
		TrackControl( control, mouseLoc, scrollAction );
		DisposeRoutineDescriptor( scrollAction );
	}
*/

	// Use default behavior for thumb, program will crash if you don't!!
	if( controlPart == kControlIndicatorPart && !g.useAppearance )	scrollAction = 0L;
	else															scrollAction = trackActionUPP;	// LR: Universal Headers requirement fix

	// Perform scrollbar tracking
	controlPart = TrackControl( control, mouseLoc, scrollAction );
	if( !controlPart ) return false;
	else if( controlPart == kControlIndicatorPart )
	{
		long	newPos, h, limit;
		vPos = GetControlValue( dWin->vScrollBar );

		GetWindowPortBounds( window, &winRect );
		h = winRect.bottom - winRect.top - ( GrowIconSize - 1 ) - ( HeaderHeight - 1 );

		limit = ( ( dWin->fileSize+( SBarSize-1 ) ) & 0xFFFFFFF0 ) - ( dWin->linesPerPage << 4 );
		if( vPos == h )
			newPos = limit;	// LR: v1.6.5 LR already computed! ( ( dWin->fileSize+( SBarSize-1 ) ) & 0xFFFFFFF0 ) - ( dWin->linesPerPage << 4 );
		else if( limit < 64000L )		// JAB 12/10 Prevent Overflow in Calcuation
			newPos = ( vPos*limit )/h;
		else
			newPos = vPos*( limit/h );

		newPos -= newPos & 0x0F;

		HEditScrollToPosition( dWin, newPos );
	}

	return true;
}

/*** SCROLL TO SELECTION ***/
void ScrollToSelection( EditWindowPtr dWin, long pos, Boolean forceUpdate, Boolean centerFlag )
{
	long	curAddr;
	curAddr = dWin->editOffset;
	if( pos >= curAddr && pos < curAddr+( dWin->linesPerPage << 4 ) )
	{
		if( forceUpdate )
		{
			DrawPage( dWin );
		}
		UpdateOnscreen( dWin->oWin.theWin );
		AdjustScrollBars( dWin->oWin.theWin, false );
		return;
	}
	if( centerFlag )
	{
		curAddr = pos - ( pos % 16 );
		curAddr -= 16 * ( dWin->linesPerPage /2 - 1 );
		// No need to adjust for limits, will be done by scroll routine
	}
	else
	{

		if( pos < curAddr )
		{
			// Scroll Up
			curAddr = pos;
			curAddr -= ( curAddr % 16 );
		}
		else
		{
			// Scroll Down
			curAddr = pos - ( dWin->linesPerPage -1 ) * 16;
			curAddr -= ( curAddr % 16 );
		}
	}
	HEditScrollToPosition( dWin, curAddr );
}

/*** HEDIT SCROLL TO POSITION ***/
void HEditScrollToPosition( EditWindowPtr dWin, long newPos )
{
	long	limit;

	SetPortWindowPort( dWin->oWin.theWin );

	// Constrain scrolling position to legal limits
	limit = ( ( dWin->fileSize+( SBarSize-1 ) ) & 0xFFFFFFF0 ) - ( dWin->linesPerPage << 4 );
	if( newPos > limit )
		newPos = limit;
	if( newPos < 0 )
		newPos = 0;

	// LR: v1.6.5 reduce auto-scroll flicker (Max Horn)
	if( newPos != dWin->editOffset )
	{
		dWin->editOffset = newPos;
		
		// Adjust Scrollbars
		AdjustScrollBars( dWin->oWin.theWin, false );

		// 12/10/93 - Optimize Drawing
		SetCurrentChunk( dWin, dWin->editOffset );

		DrawPage( dWin );
		UpdateOnscreen( dWin->oWin.theWin );
	}
}

/*** AUTO SCROLL ***/
void AutoScroll( EditWindowPtr dWin, Point pos )
{
	short offset;
	if( pos.v < ( HeaderHeight + 1 ) + TopMargin )
		offset = -16;
	else if( pos.v >= ( HeaderHeight + 1 ) + TopMargin + dWin->linesPerPage * LineHeight )
		offset = 16;
	else
		return;

	HEditScrollToPosition( dWin, dWin->editOffset+offset );
}