#ifndef _TEXTS_H_
#define _TEXTS_H_

#include "xml.h"
#include "string.h"

enum	ETexts
{
	tMainMenu = 0,
	t43,t1610,t169,
	tLowest,
	tLow,
	tMedium,
	tHigh,
	tLowestSmooth,
	tLowSmooth,
	tMediumSmooth,
	tHighSmooth,
	tSettings,
	tOff,
	tYes,
	tNo,
	tQuitGame,
	tNewGame,
	tStartGame,
	tTexture,
	tName,
	tEmail,
	tPassword,
	tDescription,
	tBack,
	tAudio,
	tGraphics,
	tGrassDetail,
	tFullscreen,
	tFullscreenAspectRatio,
	tConnecting,
	tConnectTo,
	tReconnect,
	tWait,
	tCancel,
	tLength,
	tYouKilled,
	tSelfkill,
	tKilledBy,
	tBestScore,
	tChat,
	tChatFlowWarning,
	tChatVisibility,
	
	tUseAuthorization,

	tTextsCount
};

class	CGameTexts
{
public:
	CGameTexts()
	{
		pLoadedTexts = 0;
	}

	CString	texts[tTextsCount];

	bool LoadTexts( unsigned int language );

	CString&	operator[] (ETexts e){ return pLoadedTexts->texts[e]; }

protected:
	static CGameTexts	*pLoadedTexts;
};

static CGameTexts	g_gameTexts;



#endif//_TEXTS_H_