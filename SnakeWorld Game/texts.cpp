#include "texts.h"
#include "xml.h"

CGameTexts*	CGameTexts::pLoadedTexts = 0;

bool CGameTexts::LoadTexts( unsigned int language )
{
	pLoadedTexts = this;

	//
	// text keys
	//
	texts[tMainMenu] = L"mainmenu";
	texts[t43] = L"4:3";
	texts[t1610] = L"16:10";
	texts[t169] = L"16:9";
	texts[tLowest] = L"lowest";
	texts[tLow] = L"low";
	texts[tMedium] = L"medium";
	texts[tHigh] = L"high";
	texts[tLowestSmooth] = L"lowestsmooth";
	texts[tLowSmooth] = L"lowsmooth";
	texts[tMediumSmooth] = L"mediumsmooth";
	texts[tHighSmooth] = L"highsmooth";
	texts[tSettings] = L"settings";
	texts[tOff] = L"off";
	texts[tYes] = L"yes";
	texts[tNo] = L"no";
	texts[tQuitGame] = L"quitgame";
	texts[tNewGame] = L"newgame";
	texts[tStartGame] = L"startgame";
	texts[tTexture] = L"texture";
	texts[tName] = L"name";
	texts[tDescription] = L"description";
	texts[tBack] = L"back";
	texts[tAudio] = L"audio";
	texts[tGraphics] = L"graphics";
	texts[tGrassDetail] = L"grassdetail";
	texts[tFullscreen] = L"fullscreen";
	texts[tFullscreenAspectRatio] = L"fullscreenaspectratio";
	texts[tConnecting] = L"connecting";
	texts[tConnectTo] = L"connectto";
	texts[tReconnect] = L"reconnect";
	texts[tWait] = L"wait";
	texts[tCancel] = L"cancel";
	texts[tLength] = L"length";
	texts[tYouKilled] = L"youkilled";
	texts[tSelfkill] = L"selfkill";
	texts[tKilledBy] = L"killedby";
	texts[tBestScore] = L"bestscore";
	texts[tChat] = L"chat";
	texts[tChatFlowWarning] = L"chatflow";
	texts[tChatVisibility] = L"chatvisibility";

	//
	// load texts
	//

	XmlDocument	doc;

	wchar_t fname[256];
	swprintf_s( fname, L"languages\\%u.xml", language );

	if ( !doc.LoadFromFile( fname ) )
	{
		swprintf_s( fname, L"languages\\default.xml" );

		if ( !doc.LoadFromFile( fname ) )
			return false;
	}

	// copy strings from xml
	XmlNodeList&nodes = doc.DocumentElement()->ChildNodes;

	for ( XmlNodeList::iterator it = nodes.GetIterator(); !it.EndOfList(); ++it )
	{
		for (int i = 0; i < (int)tTextsCount; ++i)
			if ( it->Attributes[0] && it->Attributes[0]->Value == texts[i] )
			{
				texts[i] = it->InnerText;
				break;
			}
	}


	// done
	return true;
}