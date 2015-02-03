#include "stdafx.h"
#include "global.h"

void SetFullScreen()
{
#ifdef _xWINDOWS
	glViewport(0, 0, 480, 320);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 479, 0, 319, BACKZ, FRONTZ);
	glRotatef(90, 0, 0, 1);
	glTranslatef(0, -480, 0);
	glMatrixMode(GL_MODELVIEW);
#else
	glViewport(0, 0, 320, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 320, 0, 480, BACKZ, FRONTZ);
	glMatrixMode(GL_MODELVIEW);
#endif

}

