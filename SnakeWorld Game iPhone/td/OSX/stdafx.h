/*
 *  stdafx.h
 *  Snakeworld
 *
 *  OSX VERSION
 */

#pragma once

//#define uint unsigned int
#define byte unsigned char

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <list>
#import <vector>

#ifndef null
#define null 0
#endif
#ifndef NULL
#define NULL null
#endif

#include <MacTypes.h>
#include <math.h>

#define glOrtho glOrthof
#define Trace(...)
#define _ASSERT(...)

#ifndef TRUE
#define TRUE true
#endif
