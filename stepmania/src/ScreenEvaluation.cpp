#include "stdafx.h"
/*
-----------------------------------------------------------------------------
 Class: ScreenEvaluation

 Desc: See header.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "ScreenEvaluation.h"
#include "ScreenSelectMusic.h"
#include "SongManager.h"
#include "ScreenManager.h"
#include "GameManager.h"
#include "RageUtil.h"
#include "GameConstantsAndTypes.h"
#include "Notes.h"
#include "PrefsManager.h"
#include "RageLog.h"
#include "AnnouncerManager.h"
#include "ScreenMusicScroll.h"
#include "GameState.h"
#include "ScreenSelectCourse.h"


#define BANNER_X				THEME->GetMetricF("Evaluation","BannerX")
#define BANNER_Y				THEME->GetMetricF("Evaluation","BannerY")
#define STAGE_X					THEME->GetMetricF("Evaluation","StageX")
#define STAGE_Y					THEME->GetMetricF("Evaluation","StageY")
#define GRADE_P1_X				THEME->GetMetricF("Evaluation","GradeP1X")
#define GRADE_P2_X				THEME->GetMetricF("Evaluation","GradeP2X")
#define GRADE_Y					THEME->GetMetricF("Evaluation","GradeY")
#define JUDGE_LABELS_X			THEME->GetMetricF("Evaluation","JudgeLabelsX")
#define JUDGE_NUMBERS_P1_X		THEME->GetMetricF("Evaluation","JudgeNumbersP1X")
#define JUDGE_NUMBERS_P2_X		THEME->GetMetricF("Evaluation","JudgeNumbersP2X")
#define JUDGE_START_Y			THEME->GetMetricF("Evaluation","JudgeStartY")
#define JUDGE_SPACING_Y			THEME->GetMetricF("Evaluation","JudgeSpacingY")
#define SCORE_LABELS_X			THEME->GetMetricF("Evaluation","ScoreLabelsX")
#define SCORE_NUMBERS_P1_X		THEME->GetMetricF("Evaluation","ScoreNumbersP1X")
#define SCORE_NUMBERS_P2_X		THEME->GetMetricF("Evaluation","ScoreNumbersP2X")
#define SCORE_Y					THEME->GetMetricF("Evaluation","ScoreY")
#define BONUS_P1_X				THEME->GetMetricF("Evaluation","BonusP1X")
#define BONUS_P2_X				THEME->GetMetricF("Evaluation","BonusP2X")
#define BONUS_Y					THEME->GetMetricF("Evaluation","BonusY")
#define NEW_RECORD_P1_X			THEME->GetMetricF("Evaluation","NewRecordP1X")
#define NEW_RECORD_P2_X			THEME->GetMetricF("Evaluation","NewRecordP2X")
#define NEW_RECORD_Y			THEME->GetMetricF("Evaluation","NewRecordY")
#define TRY_EXTRA_STAGE_X		THEME->GetMetricF("Evaluation","TryExtraStageX")
#define TRY_EXTRA_STAGE_Y		THEME->GetMetricF("Evaluation","TryExtraStageY")
#define TIMER_SECONDS			THEME->GetMetricI("Evaluation","TimerSeconds")

float GRADE_X( int p ) {
	switch( p ) {
		case PLAYER_1:	return GRADE_P1_X;
		case PLAYER_2:	return GRADE_P2_X;
		default:		ASSERT(0);	return 0;
	}
}
float JUDGE_NUMBERS_X( int p ) {
	switch( p ) {
		case PLAYER_1:	return JUDGE_NUMBERS_P1_X;
		case PLAYER_2:	return JUDGE_NUMBERS_P2_X;
		default:		ASSERT(0);	return 0;
	}
}
float SCORE_NUMBERS_X( int p ) {
	switch( p ) {
		case PLAYER_1:	return SCORE_NUMBERS_P1_X;
		case PLAYER_2:	return SCORE_NUMBERS_P2_X;
		default:		ASSERT(0);	return 0;
	}
}
float BONUS_X( int p ) {
	switch( p ) {
		case PLAYER_1:	return BONUS_P1_X;
		case PLAYER_2:	return BONUS_P2_X;
		default:		ASSERT(0);	return 0;
	}
}
float NEW_RECORD_X( int p ) {
	switch( p ) {
		case PLAYER_1:	return NEW_RECORD_P1_X;
		case PLAYER_2:	return NEW_RECORD_P2_X;
		default:		ASSERT(0);	return 0;
	}
}

const ScreenMessage SM_GoToSelectMusic		=	ScreenMessage(SM_User+1);
const ScreenMessage SM_GoToSelectCourse		=	ScreenMessage(SM_User+2);
const ScreenMessage SM_GoToFinalEvaluation	=	ScreenMessage(SM_User+3);
const ScreenMessage SM_GoToMusicScroll		=	ScreenMessage(SM_User+4);


ScreenEvaluation::ScreenEvaluation( bool bSummary )
{
	LOG->Trace( "ScreenEvaluation::ScreenEvaluation()" );
	
	int l, p;	// for counting


	///////////////////////////
	// Set m_ResultMode.  This enum will make our life easier later when we init different pieces depending on context.
	///////////////////////////
	switch( GAMESTATE->m_PlayMode )
	{
	case PLAY_MODE_ARCADE:
		m_ResultMode = bSummary ? RM_ARCADE_SUMMARY : RM_ARCADE_STAGE;
		break;
	case PLAY_MODE_ONI:
	case PLAY_MODE_ENDLESS:
		m_ResultMode = RM_ONI;
		break;
	default:
		ASSERT(0);
	}


	///////////////////////////
	// Figure out which statistics we're going to display
	///////////////////////////
	int		iPossibleDancePoints[NUM_PLAYERS];
	int		iActualDancePoints[NUM_PLAYERS];
	int		iTapNoteScores[NUM_PLAYERS][NUM_TAP_NOTE_SCORES];
	int		iHoldNoteScores[NUM_PLAYERS][NUM_HOLD_NOTE_SCORES];
	int		iMaxCombo[NUM_PLAYERS];
	float	fScore[NUM_PLAYERS];
	float	fPossibleRadarValues[NUM_PLAYERS][NUM_RADAR_CATEGORIES];
	float	fActualRadarValues[NUM_PLAYERS][NUM_RADAR_CATEGORIES];

	switch( m_ResultMode )
	{
	case RM_ARCADE_SUMMARY:
		COPY( iPossibleDancePoints,	GAMESTATE->m_iAccumPossibleDancePoints );
		COPY( iActualDancePoints,	GAMESTATE->m_iAccumActualDancePoints );
		COPY( iTapNoteScores,		GAMESTATE->m_AccumTapNoteScores );
		COPY( iHoldNoteScores,		GAMESTATE->m_AccumHoldNoteScores );
		COPY( iMaxCombo,			GAMESTATE->m_iAccumMaxCombo );
		COPY( fScore,				GAMESTATE->m_fAccumScore );
		COPY( fPossibleRadarValues,	GAMESTATE->m_fAccumRadarPossible );
		COPY( fActualRadarValues,	GAMESTATE->m_fAccumRadarActual );
		break;
	case RM_ARCADE_STAGE:
	case RM_ONI:
		COPY( iPossibleDancePoints,	GAMESTATE->m_iPossibleDancePoints );
		COPY( iActualDancePoints,	GAMESTATE->m_iActualDancePoints );
		COPY( iTapNoteScores,		GAMESTATE->m_TapNoteScores );
		COPY( iHoldNoteScores,		GAMESTATE->m_HoldNoteScores );
		COPY( iMaxCombo,			GAMESTATE->m_iMaxCombo );
		COPY( fScore,				GAMESTATE->m_fScore );
		COPY( fPossibleRadarValues,	GAMESTATE->m_fRadarPossible );
		COPY( fActualRadarValues,	GAMESTATE->m_fRadarActual );
		break;
	}


	///////////////////////////
	// Andy:
	// Fake COOL! / GOOD / OOPS for Ez2dancer using the DDR Rankings.
/*	Todo:  Accomodate this using theme metrics

	if( GAMESTATE->m_CurGame == GAME_EZ2 ) 
	{
		for( int p=0; p<NUM_PLAYERS; p++ )
		{
			iTapNoteScores[p][TNS_PERFECT] += iTapNoteScores[p][TNS_GREAT];
			iTapNoteScores[p][TNS_GREAT] = 0;
			iTapNoteScores[p][TNS_MISS] += iTapNoteScores[p][TNS_BOO];
			iTapNoteScores[p][TNS_BOO] = 0;
		}
	}
*/

	///////////////////////////
	// Init the song banners depending on m_ResultMode
	///////////////////////////
	/* EZ2 should hide these things by placing them off screen with theme metrics
	if( GAMESTATE->m_CurGame != GAME_EZ2 )
	{
	*/
		switch( m_ResultMode )
		{
		case RM_ARCADE_STAGE:
			m_BannerWithFrame[0].LoadFromSongAndNotes( GAMESTATE->m_pCurSong, GAMESTATE->m_pCurNotes );
			m_BannerWithFrame[0].SetXY( BANNER_X, BANNER_Y );
			this->AddSubActor( &m_BannerWithFrame[0] );

			m_textStage.LoadFromFont( THEME->GetPathTo("Fonts","header1") );
			m_textStage.TurnShadowOff();
			m_textStage.SetXY( STAGE_X, STAGE_Y );
			m_textStage.SetZoom( 0.5f );
			m_textStage.SetText( GAMESTATE->GetStageText() + " Stage" );
			this->AddSubActor( &m_textStage );

			break;
		case RM_ARCADE_SUMMARY:
			{
				// crop down to 3
				for( int p=0; p<NUM_PLAYERS; p++ )
				{
					if( GAMESTATE->m_apSongsPlayed.GetSize() > STAGES_TO_SHOW_IN_SUMMARY )
						GAMESTATE->m_apSongsPlayed.RemoveAt( 0, GAMESTATE->m_apSongsPlayed.GetSize() - STAGES_TO_SHOW_IN_SUMMARY );
				}

				const int iSongsToShow = GAMESTATE->m_apSongsPlayed.GetSize();
				ASSERT( iSongsToShow > 0 );

				for( int i=0; i<iSongsToShow; i++ )
				{
					m_BannerWithFrame[i].LoadFromSong( GAMESTATE->m_apSongsPlayed[i] );
					float fBannerOffset = i - (iSongsToShow-1)/2.0f;
					m_BannerWithFrame[i].SetXY( BANNER_X + fBannerOffset*32, BANNER_Y + fBannerOffset*16 );
					m_BannerWithFrame[i].SetZoom( 0.70f );
					this->AddSubActor( &m_BannerWithFrame[i] );
				}
			}
			break;
		case RM_ONI:
			m_BannerWithFrame[0].LoadFromCourse( GAMESTATE->m_pCurCourse );
			m_BannerWithFrame[0].SetXY( BANNER_X, BANNER_Y );
			this->AddSubActor( &m_BannerWithFrame[0] );
			break;
		}
	/*
	}
	*/



	//////////////////////////
	// Init graphic elements
	//////////////////////////
	m_Menu.Load(
		THEME->GetPathTo("Graphics","evaluation background"), 
		THEME->GetPathTo("Graphics",m_ResultMode==RM_ARCADE_SUMMARY?"evaluation summary top edge":"evaluation top edge"),
		"Press START to continue",
		false, true, TIMER_SECONDS 
		);
	this->AddSubActor( &m_Menu );


	for( l=0; l<NUM_JUDGE_LINES; l++ ) 
	{
		/* EZ2 should hide these things by placing them off screen with theme metrics
		if (GAMESTATE->m_CurGame != GAME_EZ2)
		{
		*/
			m_sprJudgeLabels[l].Load( THEME->GetPathTo("Graphics","evaluation judge labels") );
			m_sprJudgeLabels[l].StopAnimating();
			m_sprJudgeLabels[l].SetState( l );
			m_sprJudgeLabels[l].SetXY( JUDGE_LABELS_X, JUDGE_START_Y + l*JUDGE_SPACING_Y );
			m_sprJudgeLabels[l].SetZoom( 1.0f );
			this->AddSubActor( &m_sprJudgeLabels[l] );
		/*
		}
		*/

		for( p=0; p<NUM_PLAYERS; p++ ) 
		{
			m_textJudgeNumbers[l][p].LoadFromFont( THEME->GetPathTo("Fonts","score numbers") );
			m_textJudgeNumbers[l][p].TurnShadowOff();
			m_textJudgeNumbers[l][p].SetXY( JUDGE_NUMBERS_X(p), JUDGE_START_Y + l*JUDGE_SPACING_Y );
			m_textJudgeNumbers[l][p].SetZoom( 0.7f );
			m_textJudgeNumbers[l][p].SetDiffuseColor( PlayerToColor(p) );

/*
			TODO:  This should somehow be accomodated with a switch in the theme metric file.

			// RE-ARRANGE the scoreboard for EZ2Dancer Scoring

			if (l == 0 && GAMESTATE->m_CurGame == GAME_EZ2) // Change Position For Ez2dancer
			{
				m_textJudgeNumbers[l][p].SetXY( JUDGE_NUMBERS_X_EZ2[p], JUDGE_EZ2_COOL_Y);
			}
			else if (l == 2 && GAMESTATE->m_CurGame == GAME_EZ2)
			{
				m_textJudgeNumbers[l][p].SetXY( JUDGE_NUMBERS_X_EZ2[p], JUDGE_EZ2_COOL_Y + 55);
			}
			else if (l == 4 && GAMESTATE->m_CurGame == GAME_EZ2)
			{
				m_textJudgeNumbers[l][p].SetXY( JUDGE_NUMBERS_X_EZ2[p], JUDGE_EZ2_COOL_Y + 120);
			}
			else if ((l == 1 || l == 3 ) && GAMESTATE->m_CurGame == GAME_EZ2)
			{
				m_textJudgeNumbers[l][p].SetZoomX(0); // Hide These Ones
			}
			else if (l == 5 && GAMESTATE->m_CurGame == GAME_EZ2 ) // sneakily use this one for the max combo.
			{
				if (p == PLAYER_1 )
				{
					m_textJudgeNumbers[l][p].SetXY( JUDGE_NUMBERS_X_EZ2[p]-40, JUDGE_EZ2_COOL_Y + 180);
				}
				else
				{
					m_textJudgeNumbers[l][p].SetXY( JUDGE_NUMBERS_X_EZ2[p]+40, JUDGE_EZ2_COOL_Y + 180);
				}
			}
*/
			this->AddSubActor( &m_textJudgeNumbers[l][p] );
		}
	}

/*	if (GAMESTATE->m_CurGame != GAME_EZ2)
	{
*/
		m_sprScoreLabel.Load( THEME->GetPathTo("Graphics","evaluation score labels") );
		m_sprScoreLabel.SetState( m_ResultMode==RM_ONI ? 1 : 0 );
		m_sprScoreLabel.StopAnimating();
		m_sprScoreLabel.SetXY( SCORE_LABELS_X, SCORE_Y );
		m_sprScoreLabel.SetZoom( 1.0f );
		this->AddSubActor( &m_sprScoreLabel );
/*	}
*/

	for( p=0; p<NUM_PLAYERS; p++ ) 
	{
		if( !GAMESTATE->IsPlayerEnabled( (PlayerNumber)p ) )	// || GAMESTATE->m_CurGame == GAME_EZ2 )	// If EZ2 wants to hide this graphic, place it somewhere off screen using theme metrics
			continue;	// skip

		m_ScoreDisplay[p].SetXY( SCORE_NUMBERS_X(p), SCORE_Y );
		m_ScoreDisplay[p].SetZoomY( 0.9f );
		m_ScoreDisplay[p].SetDiffuseColor( PlayerToColor(p) );
		this->AddSubActor( &m_ScoreDisplay[p] );
	}



	//
	// Calculate grades
	//
	Grade grade[NUM_PLAYERS];
	for( p=0; p<NUM_PLAYERS; p++ )
	{
		if( !GAMESTATE->IsPlayerEnabled(p)  ||  GAMESTATE->m_bUsedAutoPlayer  ||  GAMESTATE->m_fSecondsBeforeFail[p] != -1 )
		{
			grade[p] = GRADE_E;
		}
		else
		{
//Based on the percentage of your total "Dance Points" to the maximum possible number, the following rank is assigned: 
//
//100% - AAA
//93 % - AA
//80 % - A
//65 % - B
//45 % - C
//Less - D
//Fail - E
			float fPercentDancePoints = iActualDancePoints[p] / (float)iPossibleDancePoints[p];
			fPercentDancePoints = max( fPercentDancePoints, 0 );

			if     ( fPercentDancePoints >= 1.00 )	grade[p] = GRADE_AAA;
			else if( fPercentDancePoints >= 0.93 )	grade[p] = GRADE_AA;
			else if( fPercentDancePoints >= 0.80 )	grade[p] = GRADE_A;
			else if( fPercentDancePoints >= 0.65 )	grade[p] = GRADE_B;
			else if( fPercentDancePoints >= 0.45 )	grade[p] = GRADE_C;
			else									grade[p] = GRADE_D;
		}
	}

	Grade max_grade = GRADE_NO_DATA;
	for( p=0; p<NUM_PLAYERS; p++ )
		max_grade = max( max_grade, grade[p] ); 


	//////////////////////////
	// Set Numbers 
	//////////////////////////
	for( p=0; p<NUM_PLAYERS; p++ )
	{
		if( !GAMESTATE->IsPlayerEnabled( (PlayerNumber)p ) )
			continue;	// skip

		m_textJudgeNumbers[0][p].SetText( ssprintf("%4d", iTapNoteScores[p][TNS_PERFECT]) );
		m_textJudgeNumbers[1][p].SetText( ssprintf("%4d", iTapNoteScores[p][TNS_GREAT]) );
		m_textJudgeNumbers[2][p].SetText( ssprintf("%4d", iTapNoteScores[p][TNS_GOOD]) );
		m_textJudgeNumbers[3][p].SetText( ssprintf("%4d", iTapNoteScores[p][TNS_BOO]) );
		m_textJudgeNumbers[4][p].SetText( ssprintf("%4d", iTapNoteScores[p][TNS_MISS]) );
		m_textJudgeNumbers[5][p].SetText( ssprintf("%4d", iHoldNoteScores[p][HNS_OK]) );

		if( m_ResultMode==RM_ONI )
		{
			const float fSurviveSeconds = GAMESTATE->GetPlayerSurviveTime( (PlayerNumber)p );
			int iMinsDisplay = (int)fSurviveSeconds/60;
			int iSecsDisplay = (int)fSurviveSeconds - iMinsDisplay*60;
			int iLeftoverDisplay = int( (fSurviveSeconds - iMinsDisplay*60 - iSecsDisplay) * 100 );
			m_ScoreDisplay[p].SetText( ssprintf( "%02d:%02d:%02d", iMinsDisplay, iSecsDisplay, iLeftoverDisplay ) );
		}
		else
		{
			m_ScoreDisplay[p].SetScore( fScore[p] );
		}

		/*
		TODO:  Accomodate this with a theme metric

		// SNEAKY! We take the max combo, and put it into element 5, because Ez2dancer 
		// doesn't care for OK's and plus this text element is already nicely aligned =)
		if (GAMESTATE->m_CurGame == GAME_EZ2)
		{
			m_textJudgeNumbers[5][p].SetText( ssprintf("%4d", iMaxCombo[p]) );
		}
		*/

		m_BonusInfoFrame[p].SetBonusInfo( (PlayerNumber)p, fPossibleRadarValues[p], fActualRadarValues[p], iMaxCombo[p] );
		m_StageBox[p].SetStageInfo( (PlayerNumber)p, GAMESTATE->m_iSongsBeforeFail[p] );
	}


	////////////////////////
	// update persistent statistics
	////////////////////////
	for( p=0; p<NUM_PLAYERS; p++ )
	{
		if( !GAMESTATE->IsPlayerEnabled(p) )
			continue;

		if( GAMESTATE->m_bUsedAutoPlayer )
			continue;

		switch( m_ResultMode )
		{
		case RM_ARCADE_STAGE:

			Notes* pNotes = GAMESTATE->m_pCurNotes[p];
			pNotes->m_iNumTimesPlayed++;

			if( iMaxCombo[p] > pNotes->m_iMaxCombo )
				pNotes->m_iMaxCombo = iMaxCombo[p];

			if( fScore[p] > pNotes->m_iTopScore )
			{
				pNotes->m_iTopScore = (int)fScore[p];
				m_bNewRecord[p] = true;
			}

			if( grade[p] > pNotes->m_TopGrade )
				pNotes->m_TopGrade = grade[p];
			break;
		}
	}
	


	
	m_bTryExtraStage = false;
	if( (GAMESTATE->IsFinalStage() || GAMESTATE->IsExtraStage())  &&  m_ResultMode==RM_ARCADE_STAGE )
	{
		for( p=0; p<NUM_PLAYERS; p++ )
		{
			if( !GAMESTATE->IsPlayerEnabled( (PlayerNumber)p ) )
				continue;	// skip

			if( GAMESTATE->m_pCurNotes[p]->m_DifficultyClass == CLASS_HARD  &&  grade[p] >= GRADE_AA )
				m_bTryExtraStage = true;
		}
	}
	if( PREFSMAN->m_bEventMode )
		m_bTryExtraStage = false;



	for( p=0; p<NUM_PLAYERS; p++ )
	{
		if( !GAMESTATE->IsPlayerEnabled( (PlayerNumber)p ) )
			continue;	// skip

/*		Chris:  If you don't want EZ2 to have a grade frame, then make a theme that has a 1x1 transparent graphic for the grade frame.

		if ( GAMESTATE->m_CurGame != GAME_EZ2)
		{
*/
			m_sprGradeFrame[p].Load( THEME->GetPathTo("Graphics","evaluation grade frame") );
			m_sprGradeFrame[p].StopAnimating();
			m_sprGradeFrame[p].SetState( p );
			m_sprGradeFrame[p].SetXY( GRADE_X(p), GRADE_Y );
			this->AddSubActor( &m_sprGradeFrame[p] );
/*		}
*/
		switch( m_ResultMode )
		{
		case RM_ONI:
			{
				m_textOniPercent[p].LoadFromFont( THEME->GetPathTo("Fonts","header1") );
				m_textOniPercent[p].SetXY( GRADE_X(p), GRADE_Y );
				m_textOniPercent[p].SetShadowLength( 2 );
				m_textOniPercent[p].SetZoomX( 1.3f );
				m_textOniPercent[p].SetZoomY( 2.5f );
				m_textOniPercent[p].SetEffectGlowing( 1.0f );

				float fPercentDancePoints =  iActualDancePoints[p] / (float)iPossibleDancePoints[p] + 0.0001f;	// correct for rounding errors
				fPercentDancePoints = max( 0, fPercentDancePoints );
				m_textOniPercent[p].SetText( ssprintf("%.1f%%", fPercentDancePoints*100) );
				this->AddSubActor( &m_textOniPercent[p] );

				m_StageBox[p].SetXY( BONUS_X(p), BONUS_Y );
				this->AddSubActor( &m_StageBox[p] );
			}
			break;
		case RM_ARCADE_STAGE:
		case RM_ARCADE_SUMMARY:
			/*
			Todo: Accomodate this in theme metrics

			if (GAMESTATE->m_CurGame != GAME_EZ2 )
			{
			*/
				m_Grades[p].SetXY( GRADE_X(p), GRADE_Y );
				m_Grades[p].SetZ( -2 );
				m_Grades[p].SetZoom( 1.0f );
				m_Grades[p].SetEffectGlowing( 1.0f );
				m_Grades[p].SpinAndSettleOn( grade[p] );

				m_BonusInfoFrame[p].SetXY( BONUS_X(p), BONUS_Y );
				this->AddSubActor( &m_BonusInfoFrame[p] );
			/*
			}
			else // Ez2dancer Style Grade Display.
			{
				m_Grades[p].SetXY( GRADE_X[p], GRADE_Y + 150);
				m_Grades[p].SetZ( -2 );
				if (p == PLAYER_1 )
				{
					m_Grades[p].SetXY( GRADE_X[p]-10, GRADE_Y + 150);	
				}
				m_Grades[p].SetGrade( grade[p] );
				m_Grades[p].SetRotation( D3DX_PI );
				m_Grades[p].SetZoom( 8 );
				m_Grades[p].BeginTweening( 0.6f );

				m_Grades[p].SetTweenRotationZ( 0 );
				m_Grades[p].SetTweenZoom( 2 );

			}
			*/
			this->AddSubActor( &m_Grades[p] );
			break;
		}

/*	Chris:  If EZ2 wants to hide these things, place them off screen using theme metrics
	
		if ( GAMESTATE->m_CurGame == GAME_EZ2)
			continue;
*/
		m_bNewRecord[p] = false;

		if( m_bNewRecord[p] )
		{
			m_textNewRecord[p].LoadFromFont( THEME->GetPathTo("Fonts","header1") );
			m_textNewRecord[p].SetXY( NEW_RECORD_X(p), NEW_RECORD_Y );
			m_textNewRecord[p].SetShadowLength( 2 );
			m_textNewRecord[p].SetText( "IT'S A NEW RECORD!" );
			m_textNewRecord[p].SetZoom( 0.5f );
			m_textNewRecord[p].SetEffectGlowing( 1.0f );
			this->AddSubActor( &m_textNewRecord[p] );
		}
	}
		
	
	if( m_bTryExtraStage )
	{
		m_textTryExtraStage.LoadFromFont( THEME->GetPathTo("Fonts","header1") );
		m_textTryExtraStage.SetXY( TRY_EXTRA_STAGE_X, TRY_EXTRA_STAGE_Y );
		if( GAMESTATE->IsExtraStage() )
			m_textTryExtraStage.SetText( "Try Another Extra Stage!" );
		else
			m_textTryExtraStage.SetText( "Try Extra Stage!" );
		m_textTryExtraStage.SetZoom( 1 );
		m_textTryExtraStage.SetEffectGlowing( 1.0f );
		this->AddSubActor( &m_textTryExtraStage );

		SOUND->PlayOnceStreamed( THEME->GetPathTo("Sounds","evaluation extra stage") );
	}
	else
	{
		
		switch( m_ResultMode )
		{
		case RM_ARCADE_STAGE:
			switch( max_grade )
			{
			case GRADE_E:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_E) );		break;
			case GRADE_D:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_D) );		break;
			case GRADE_C:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_C) );		break;
			case GRADE_B:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_B) );		break;
			case GRADE_A:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_A) );		break;
			case GRADE_AA:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_AA) );	break;
			case GRADE_AAA:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_AAA) );	break;
			case GRADE_NO_DATA:
			default:
				ASSERT(0);	// invalid grade
			}
			break;
		case RM_ONI:
		case RM_ARCADE_SUMMARY:
			switch( max_grade )
			{
			case GRADE_E:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_FINAL_E) );	break;
			case GRADE_D:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_FINAL_D) );	break;
			case GRADE_C:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_FINAL_C) );	break;
			case GRADE_B:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_FINAL_B) );	break;
			case GRADE_A:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_FINAL_A) );	break;
			case GRADE_AA:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_FINAL_AA) );	break;
			case GRADE_AAA:	SOUND->PlayOnceStreamedFromDir( ANNOUNCER->GetPathTo(ANNOUNCER_EVALUATION_FINAL_AAA) );	break;
			case GRADE_NO_DATA:
			default:
				ASSERT(0);	// invalid grade
			}
			break;
		default:
			ASSERT(0);
		}
	}

	m_Menu.TweenOnScreenFromBlack( SM_None );
}


void ScreenEvaluation::TweenOnScreen()
{
	int i, p;

	m_Menu.TweenOnScreenFromBlack( SM_None );

	float fOriginalX, fOriginalY;

	for( i=0; i<STAGES_TO_SHOW_IN_SUMMARY; i++ )
	{
		fOriginalY = m_BannerWithFrame[i].GetY();
		m_BannerWithFrame[i].SetY( fOriginalY + SCREEN_HEIGHT );
		m_BannerWithFrame[i].BeginTweeningQueued( 0.0f );
		m_BannerWithFrame[i].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
		m_BannerWithFrame[i].SetTweenY( fOriginalY );
	}
	
	fOriginalY = m_textStage.GetY();
	m_textStage.BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
	m_textStage.SetTweenY( fOriginalY );

	for( i=0; i<NUM_JUDGE_LINES; i++ ) 
	{
		/*	TODO:  Accomodate this using theme metrics

		if (GAMESTATE->m_CurGame != GAME_EZ2)
		{
		*/
			fOriginalY = m_sprJudgeLabels[i].GetY();
			m_sprJudgeLabels[i].SetY( fOriginalY + SCREEN_HEIGHT );
			m_sprJudgeLabels[i].BeginTweeningQueued( 0.2f + 0.1f*i );
			m_sprJudgeLabels[i].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
			m_sprJudgeLabels[i].SetTweenY( fOriginalY );
		/*
		}
		*/

		for( int p=0; p<NUM_PLAYERS; p++ ) 
		{
			fOriginalX = m_textJudgeNumbers[i][p].GetX();
			m_textJudgeNumbers[i][p].SetX( fOriginalX + SCREEN_WIDTH/2*(p==PLAYER_1 ? 1 : -1) );
			m_textJudgeNumbers[i][p].BeginTweeningQueued( 0.2f + 0.1f*i );
			m_textJudgeNumbers[i][p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
			m_textJudgeNumbers[i][p].SetTweenX( fOriginalX );
		}
	}

	/*  Chris:  If EZ2 wants to hide these things, position them off screen using theme metrics

	if (GAMESTATE->m_CurGame != GAME_EZ2)
	{
	*/
		fOriginalY = m_sprScoreLabel.GetY();
		m_sprScoreLabel.SetY( fOriginalY + SCREEN_HEIGHT );
		m_sprScoreLabel.BeginTweeningQueued( 0.8f + 0.1f*i );
		m_sprScoreLabel.BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
		m_sprScoreLabel.SetTweenY( fOriginalY );
		
		for( p=0; p<NUM_PLAYERS; p++ ) 
		{
			fOriginalX = m_ScoreDisplay[p].GetX();
			m_ScoreDisplay[p].SetX( fOriginalX + SCREEN_WIDTH/2*(p==PLAYER_1 ? 1 : -1) );
			m_ScoreDisplay[p].BeginTweeningQueued( 0.8f + 0.1f*i );
			m_ScoreDisplay[p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
			m_ScoreDisplay[p].SetTweenX( fOriginalX );
		}

		for( p=0; p<NUM_PLAYERS; p++ )
		{
			fOriginalX = m_BonusInfoFrame[p].GetX();
			m_BonusInfoFrame[p].SetX( fOriginalX + SCREEN_WIDTH/2*(p==PLAYER_1 ? 1 : -1) );
			m_BonusInfoFrame[p].BeginTweeningQueued( 0.2f + 0.1f*i );
			m_BonusInfoFrame[p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
			m_BonusInfoFrame[p].SetTweenX( fOriginalX );

			fOriginalX = m_StageBox[p].GetX();
			m_StageBox[p].SetX( fOriginalX + SCREEN_WIDTH/2*(p==PLAYER_1 ? 1 : -1) );
			m_StageBox[p].BeginTweeningQueued( 0.2f + 0.1f*i );
			m_StageBox[p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_BEGIN );
			m_StageBox[p].SetTweenX( fOriginalX );

			m_sprGradeFrame[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_sprGradeFrame[p].SetTweenZoomY( 0 );

			m_Grades[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_Grades[p].SetTweenZoomY( 0 );

			m_textOniPercent[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_textOniPercent[p].SetTweenZoomY( 0 );

			m_textNewRecord[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
			m_textNewRecord[p].SetTweenZoomY( 0 );
		}
		
		m_textTryExtraStage.BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
		m_textTryExtraStage.SetTweenZoomY( 0 );
	/*
	}
	*/
}

void ScreenEvaluation::TweenOffScreen()
{
	int i, p;

	for( i=0; i<STAGES_TO_SHOW_IN_SUMMARY; i++ )
	{
		m_BannerWithFrame[i].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
		m_BannerWithFrame[i].SetTweenZoomY( 0 );
	}

	m_textStage.BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
	m_textStage.SetTweenZoomY( 0 );

	for( i=0; i<NUM_JUDGE_LINES; i++ ) 
	{
		/*  Chris:  If EZ2 wants to hide these things, position them off screen using theme metrics
		if (GAMESTATE->m_CurGame != GAME_EZ2)
		{
		*/
			m_sprJudgeLabels[i].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
			m_sprJudgeLabels[i].SetTweenZoomY( 0 );
		/*
		}
		*/

		for( int p=0; p<NUM_PLAYERS; p++ ) 
		{
			m_textJudgeNumbers[i][p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
			m_textJudgeNumbers[i][p].SetTweenZoomY( 0 );
		}
	}
	
	/*  Chris:  If EZ2 wants to hide these things, position them off screen using theme metrics
	if (GAMESTATE->m_CurGame != GAME_EZ2)
	{
	*/
		m_sprScoreLabel.BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
		m_sprScoreLabel.SetTweenZoomY( 0 );


		for( p=0; p<NUM_PLAYERS; p++ ) 
		{
			m_ScoreDisplay[p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
			m_ScoreDisplay[p].SetTweenZoomY( 0 );
		}
	/*
	}
	*/
	for( p=0; p<NUM_PLAYERS; p++ )
	{
		m_Grades[p].SettleImmediately();

		m_BonusInfoFrame[p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
		m_BonusInfoFrame[p].SetTweenZoomY( 0 );

		m_StageBox[p].BeginTweeningQueued( MENU_ELEMENTS_TWEEN_TIME, Actor::TWEEN_BIAS_END );
		m_StageBox[p].SetTweenZoomY( 0 );

		m_sprGradeFrame[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
		m_sprGradeFrame[p].SetTweenZoomY( 0 );

		m_Grades[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
		m_Grades[p].SetTweenZoomY( 0 );

		m_textOniPercent[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
		m_textOniPercent[p].SetTweenZoomY( 0 );

		m_textNewRecord[p].BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
		m_textNewRecord[p].SetTweenZoomY( 0 );
	}

	m_textTryExtraStage.BeginTweening( MENU_ELEMENTS_TWEEN_TIME );
	m_textTryExtraStage.SetTweenZoomY( 0 );
}


void ScreenEvaluation::Update( float fDeltaTime )
{
	Screen::Update( fDeltaTime );
}

void ScreenEvaluation::DrawPrimitives()
{
	m_Menu.DrawBottomLayer();
	Screen::DrawPrimitives();
	m_Menu.DrawTopLayer();
}

void ScreenEvaluation::Input( const DeviceInput& DeviceI, const InputEventType type, const GameInput &GameI, const MenuInput &MenuI, const StyleInput &StyleI )
{
	LOG->Trace( "ScreenEvaluation::Input()" );

	if( m_Menu.IsClosing() )
		return;

	Screen::Input( DeviceI, type, GameI, MenuI, StyleI );
}

void ScreenEvaluation::HandleScreenMessage( const ScreenMessage SM )
{
	switch( SM )
	{
	case SM_MenuTimer:
		MenuStart( PLAYER_INVALID );
		break;
	case SM_GoToSelectMusic:
		SCREENMAN->SetNewScreen( new ScreenSelectMusic );
		break;
	case SM_GoToSelectCourse:
		SCREENMAN->SetNewScreen( new ScreenSelectCourse );
		break;
	case SM_GoToMusicScroll:
		SCREENMAN->SetNewScreen( new ScreenMusicScroll );
		break;
	case SM_GoToFinalEvaluation:
		SCREENMAN->SetNewScreen( new ScreenEvaluation(true) );
		break;
	}
}

void ScreenEvaluation::MenuBack( const PlayerNumber p )
{
	MenuStart( p );
}

void ScreenEvaluation::MenuStart( const PlayerNumber p )
{
	TweenOffScreen();

	if( PREFSMAN->m_bEventMode )
	{
		switch( GAMESTATE->m_PlayMode )
		{
		case PLAY_MODE_ARCADE:
			m_Menu.TweenOffScreenToMenu( SM_GoToSelectMusic );
			break;
		case PLAY_MODE_ONI:
		case PLAY_MODE_ENDLESS:
			m_Menu.TweenOffScreenToMenu( SM_GoToSelectCourse );
			break;
		default:
			ASSERT(0);
		}
	}
	else	// not event mode
	{
		if( m_bTryExtraStage )
			m_Menu.TweenOffScreenToMenu( SM_GoToSelectMusic );
		else if( m_ResultMode == RM_ARCADE_STAGE  &&  GAMESTATE->m_iCurrentStageIndex == PREFSMAN->m_iNumArcadeStages-1  )
			m_Menu.TweenOffScreenToMenu( SM_GoToFinalEvaluation );
		else if( m_ResultMode == RM_ARCADE_STAGE  &&  GAMESTATE->m_iCurrentStageIndex < PREFSMAN->m_iNumArcadeStages-1  )
			m_Menu.TweenOffScreenToMenu( SM_GoToSelectMusic );
		else
			m_Menu.TweenOffScreenToBlack( SM_GoToMusicScroll, false );
	}

	//
	// Increment the stage counter.
	//
	GAMESTATE->m_iCurrentStageIndex++;
}

