#ifndef _CONNECTIONDATA_H_
#define _CONNECTIONDATA_H_


#include "binaryio.h"
#include "string.h"

// init commands
#define CMD_NEWGAMECMD		L"NEWGAME"
#define CMD_GETTEXTURECMD	L"GETTEXTURE"

// commands IDs
enum ECmdIds
{
	CMD_NAME = 1,
	CMD_DESCRIPTION = 2,
	CMD_TYPE = 3,
	CMD_TEXTURE = 4,
	CMD_LANGUAGE = 5,
	CMD_SERVER = 6,
	CMD_YOURSNAKEID = 7,
	CMD_IDONTKNOW = 8,
	CMD_SCENE = 9,
	CMD_ERROR = 10,

	CMD_RECEIVING_SNAKE = 12,
	CMD_SNAKE_HEAD = 13,
	CMD_SNAKE_DIR = 14,
	CMD_SNAKE_SPEED = 15,
	CMD_SNAKE_LENGTH = 16,
	CMD_SNAKE_SEG = 17,
	CMD_SNAKE_DEAD = 18,
	CMD_SNAKE_BOUND = 19,
	CMD_SNAKE_IDONWKNOW = 20,

	CMD_NEWSNAKE = 21,

	CMD_BESTSNAKE = 22,	// [string, double]

	CMD_APPLE = 23,         // [apple id,x,y] apple position, implicates that apple exists and defines his position
    CMD_APPLEEATEN = 24,    // [apple id] apple eaten by snake that sends this command

	CMD_CHATMESSAGE = 25,	// send:[to user name,message]		|	receive:[from user name,message]

	CMD_BLOCKEND = 255	// command block end

};

// name, description, type, texture, language, server, error
// CString

// YourSnakeId, IDontKnowSnakeId, WholeSnakeId, ReceivingSnakeId, NewSnakeId
struct CmdSnakeId
{
	unsigned int iId;
};

// scene size and refresh distance
struct CmdScene
{
	double dCx, dCy;
	double dRefreshDistance;
};

// snake head position
struct CmdSnakeHead
{
	double x,y;
};

// snake direction
struct CmdSnakeDirection
{
	double dAngle;	// in radians
};


// snake speed
struct CmdSnakeSpeed
{
	double dSpeed;
};

// snake length
struct CmdSnakeLength
{
	double dLength;
};

// snake segment
/*struct CmdSnakeSegment
{
	unsigned int uSeg;
	double x,y;
};*///bad aligned

// snake dead
struct CmdSnakeDead
{
	unsigned int uKilledBy;
};

// snake bound rect
struct CmdSnakeBound
{
	double x1,y1;
	double x2,y2;
};

// apple state
struct CmdAppleState
{
	unsigned int uAppleId;

	double x,y;
};

// apple eaten
struct CmdAppleEaten
{
	unsigned int uAppleId;
};


#endif//_CONNECTIONDATA_H_
