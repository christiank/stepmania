#include "stdafx.h"
/*
-----------------------------------------------------------------------------
 Class: ScreenCaution

 Desc: Screen that displays while resources are being loaded.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "ScreenCaution.h"
#include "GameConstantsAndTypes.h"
#include "PrefsManager.h"
#include "AnnouncerManager.h"
#include "GameState.h"
#include "RageMusic.h"


#define NEXT_SCREEN				THEME->GetMetric("ScreenCaution","NextScreen")


const ScreenMessage SM_GoToPrevState	= ScreenMessage(SM_User-6);
const ScreenMessage SM_DoneOpening		= ScreenMessage(SM_User-7);
const ScreenMessage SM_StartClosing		= ScreenMessage(SM_User-8);
const ScreenMessage SM_GoToNextScreen	= ScreenMessage(SM_User-9);


ScreenCaution::ScreenCaution()
{
	GAMESTATE->m_bPlayersCanJoin = true;

	m_sprCaution.Load( THEME->GetPathTo("Graphics","caution") );
	m_sprCaution.StretchTo( CRect(SCREEN_LEFT,SCREEN_TOP,SCREEN_RIGHT,SCREEN_BOTTOM) );
	this->AddSubActor( &m_sprCaution );
	
	m_Wipe.OpenWipingRight( SM_DoneOpening );
	this->AddSubActor( &m_Wipe );

	m_FadeWipe.SetOpened();
	this->AddSubActor( &m_FadeWipe );

	this->SendScreenMessage( SM_StartClosing, 3 );

	MUSIC->LoadAndPlayIfNotAlready( THEME->GetPathTo("Sounds","caution music") );
}


void ScreenCaution::Input( const DeviceInput& DeviceI, const InputEventType type, const GameInput &GameI, const MenuInput &MenuI, const StyleInput &StyleI )
{
	if( m_Wipe.IsClosing() )
		return;

	Screen::Input( DeviceI, type, GameI, MenuI, StyleI );
}


void ScreenCaution::HandleScreenMessage( const ScreenMessage SM )
{
	switch( SM )
	{
	case SM_StartClosing:
		if( !m_Wipe.IsClosing() )
			m_Wipe.CloseWipingRight( SM_GoToNextScreen );
		break;
	case SM_DoneOpening:
		SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_CAUTION) );
		break;
	case SM_GoToPrevState:
		SCREENMAN->SetNewScreen( "ScreenTitleMenu" );
		break;
	case SM_GoToNextScreen:
		SCREENMAN->SetNewScreen( NEXT_SCREEN );
		break;
	}
}

void ScreenCaution::MenuStart( const PlayerNumber p )
{
	if( p != PLAYER_INVALID  &&  !GAMESTATE->m_bSideIsJoined[p] )
	{
		GAMESTATE->m_bSideIsJoined[p] = true;
		SOUND->PlayOnceStreamed( THEME->GetPathTo("Sounds","menu start") );
		SCREENMAN->RefreshCreditsMessages();
		return;	// don't fall though
	}

	if( !m_Wipe.IsOpening()  &&  !m_Wipe.IsClosing() )
		m_Wipe.CloseWipingRight( SM_GoToNextScreen );
}

void ScreenCaution::MenuBack( const PlayerNumber p )
{
	if(m_FadeWipe.IsClosing())
		return;
	this->ClearMessageQueue();
	m_FadeWipe.CloseWipingLeft( SM_GoToPrevState );
	SOUND->PlayOnceStreamed( THEME->GetPathTo("Sounds","menu back") );
}

