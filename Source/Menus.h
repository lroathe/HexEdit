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
 * Copyright (C) Copyright � 1996-2000.
 * All Rights Reserved.
 * 
 * Contributor(s):
 *		Nick Shanks (NS)
 */

#include "HexEdit.h"

#ifndef _HexEdit_Menus_
#define _HexEdit_Menus_

OSStatus InitMenubar( void );
OSStatus SmartEnableMenuItem( MenuRef menu, short item, short ok );
OSStatus AdjustMenus( void );
short GetColorMenuResID( short menuItem );
OSStatus HandleMenu( long mSelect );

#if TARGET_API_MAC_CARBON
void PostPrintingErrors( OSStatus status );	// SEL: 1.7 - carbon printing
#endif

#endif