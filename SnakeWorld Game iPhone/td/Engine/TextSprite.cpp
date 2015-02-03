#include "stdafx.h"
#include "TextSprite.h"
#include "Touches.h"
#include <math.h>
#include "MainObj.h"
using namespace std;

//extern float bitmap_sans_8_coords[];
//extern float bitmap_sans_8o_coords[];
extern float bitmap_sans_8s_coords[];
//extern float bitmap_sans_10_coords[];
extern float bitmap_sans_12o_coords[];
//extern float bitmap_sans_16_coords[];
//extern float bitmap_sans_24_coords[];
//extern float bitmap_sans_24o_coords[];
//extern float bitmap_sans_32s_coords[];
extern float bitmap_serif_10_coords[];
extern float bitmap_sans_24s_coords[];
extern float bitmap_sans_numbers_coords[];
extern float bitmap_sans_typewriter_coords[];

#ifdef _DEBUG
//extern Sprite* vertex1;
//extern Sprite* vertex2;
//extern Sprite* vertex3;
//extern Sprite* vertex4;
#endif

extern MainObj* _mainObj;

TextHandler globalTextHandler;

TextSprite::TextSprite( TextureNumber tex, FontSizeType fs ) : Sprite(tex)
{
	m_bIntegerPixelAlign = true;
	letterFrames = NULL;
	m_auto_fs = fs;
}
TextSprite::~TextSprite()
{
	delete letterFrames;
}
void TextSprite::SetTextFormatted(char* format, int number)
{
	char buf[100];
	sprintf(buf, format, number);
	SetText(buf);
}
void TextSprite::SetString( const string &text )
{
	if (m_text.length() != text.length() || letterFrames == NULL)
	{
		delete letterFrames;
		letterFrames = new LetterFrame[text.length()];
	}
	m_text = text;
	CalcLetters();
}
void TextSprite::SetTextI( int intval )
{
	char buf[30];
	sprintf(buf, "%d", intval);
	SetText(buf);
}
void TextSprite::SetTextF( float fval, int num_of_decimals )
{
	char buf[30];
	if (num_of_decimals > 9) num_of_decimals = 9;
	string format("%.");
	format += '0' + num_of_decimals;
	format += "f";
	sprintf(buf, format.data(), fval);
	SetText(buf);
}
void TextSprite::CalcLetters()
{
	GetAnim()->SetFrame(0, 0, 0, 1, 1);
	if (letterFrames == NULL) {	SetString(m_text); return; }
	string::size_type i;
	string::value_type b;
	float totwidth = TextWidth();
	float totheight = TextHeight();

	float ratio_x = size.getW() / totwidth;
	float ratio_y = size.getH() / totheight;
	float ratio = ratio_x;
	if (ratio_y < ratio_x) ratio = ratio_y;

	float realwidth = totwidth * ratio;	// adjusted width
	float realheight = totheight * ratio;	// adjusted height

	float x = 0.0f;
	float y = 0.0f;
	if (alignment & AlignE) x = 0.0f - realwidth;
	if (alignment & AlignW) x = 0.0f;
	if (alignment == AlignN || alignment == AlignS || alignment == AlignCenter) x = 0.0f - (realwidth / 2.0f);
	if (alignment & AlignN) y = 0.0f - realheight;
	if (alignment & AlignS) y = 0.0f;
	if (alignment == AlignE || alignment == AlignW || alignment == AlignCenter) y = 0.0f - (realheight / 2.0f);

	float width;
	float inter_spacing = LetterSpacing();

	for (i = 0; i < m_text.length(); i++)
	{
		LetterFrame* lf = &(letterFrames[i]);
		b = m_text[i];


		lf->textureVert[0] = TextureX1(b);
		lf->textureVert[1] = TextureY1(b);

		lf->textureVert[2] = TextureX2(b);
		lf->textureVert[3] = lf->textureVert[1];

		lf->textureVert[4] = lf->textureVert[0];
		lf->textureVert[5] = TextureY2(b);

		lf->textureVert[6] = lf->textureVert[2];
		lf->textureVert[7] = lf->textureVert[5];

	//	Trace("%f,%f %f,%f %f,%f %f,%f\n", lf->textureVert[0],lf->textureVert[1],lf->textureVert[2],lf->textureVert[3],lf->textureVert[4],lf->textureVert[5],lf->textureVert[6],lf->textureVert[7]);

		width = LetterWidth(b) * ratio;

		lf->polyVert[0] = floor(x + .5f);
		lf->polyVert[1] = floor(y + .5f);

		lf->polyVert[2] = lf->polyVert[0] + floor(width + .5f);
		lf->polyVert[3] = lf->polyVert[1];

		lf->polyVert[4] = lf->polyVert[0];
		lf->polyVert[5] = lf->polyVert[1] + floor(realheight + .5f);

		lf->polyVert[6] = lf->polyVert[2];
		lf->polyVert[7] = lf->polyVert[5];
		x += floor(width + .5f) - inter_spacing;
		//Trace("%f,%f %f,%f %f,%f %f,%f\n", lf->polyVert[0],lf->polyVert[1],lf->polyVert[2],lf->polyVert[3],lf->polyVert[4],lf->polyVert[5],lf->polyVert[6],lf->polyVert[7]);
	}
	last_w = size.getW();
	last_h = size.getH();
}

float TextSprite::TextWidth()
{
	string::size_type i;
	string::value_type b;
	float totwidth = 0.0f;
	float inter_spacing = LetterSpacing();

	for (i = 0; i < m_text.length(); i++)
	{
		b = m_text[i];
		totwidth += floor(LetterWidth(b) + .5f) - inter_spacing;
	}
	return totwidth;
}
float TextSprite::TextHeight()
{
	float minh = 0;
	string::size_type i;
	string::value_type b;
	for (i = 0; i < m_text.length(); i++)
	{
		b = m_text[i];
		if (LetterHeight(b) > minh) minh = LetterHeight(b);
	}
	return minh;
}
float TextSprite::LetterWidth( byte b )
{
	return GetAnim()->GetTexture()->m_width * TextureW(b);
}

float TextSprite::LetterHeight( byte b )
{
	return GetAnim()->GetTexture()->m_height * TextureH(b);
}

float TextSprite::TextureX1( byte b )
{
	b = Range(b);
	return TextHandler::Points(m_auto_fs)[b * 4];
}

float TextSprite::TextureY2( byte b )
{
	b = Range(b);
	return 1.0f - TextHandler::Points(m_auto_fs)[b * 4 + 1];
}

float TextSprite::TextureX2( byte b )
{
	b = Range(b);
	return TextHandler::Points(m_auto_fs)[b * 4 + 2];
}

float TextSprite::TextureY1( byte b )
{
	b = Range(b);
	return 1.0f - TextHandler::Points(m_auto_fs)[b * 4 + 3];
}

float TextSprite::TextureW( byte b )
{
	b = Range(b);
	return TextHandler::Points(m_auto_fs)[b * 4 + 2] - TextHandler::Points(m_auto_fs)[b * 4];
}

float TextSprite::TextureH( byte b )
{
	b = Range(b);
	return TextHandler::Points(m_auto_fs)[b * 4 + 3] - TextHandler::Points(m_auto_fs)[b * 4 + 1];
}
byte TextSprite::Range( byte b )
{
	if (b < 0x20) return 1;
	if (b > 0x7f) return 1;
	return b - 0x1F;
}

//GLfloat static_text[] = {0, 0, .2, 0, 0, -.2, .2, -.2};
//GLfloat static_poly[] = {0, 0, 28, 0, 0, 28, 28, 28};

void TextSprite::DrawVertexes()
{
	glColor4f(fade.r.getValue(), fade.g.getValue(), fade.b.getValue(), fade.a.getValue());
	glBindTexture(GL_TEXTURE_2D, GetAnim()->GetTexture()->spriteTexture);
	glEnable(GL_BLEND);
	string::size_type i;
	for (i = 0; i < m_text.length(); i++)
	{
		LetterFrame* lf = &(letterFrames[i]);
		glTexCoordPointer(2, GL_FLOAT, 0, lf->textureVert);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, lf->polyVert);
		glEnableClientState(GL_VERTEX_ARRAY);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


float TextSprite::IdealHeight()
{
	return TextHandler::IdealHeight(m_auto_fs);
}

bool TextSprite::HitTest( int x, int y, int grow_x, int grow_y )
{
	if (!IsVisible()) return false;
	if (fade.a.getValue() == 0) return false;
	float ang = angle.getValue();
//	for (uint i = 0; i < m_text.length(); i++)
//	{
	LetterFrame* lf = &(letterFrames[0]);
	LetterFrame* lf2 = &(letterFrames[m_text.length() - 1]);
	for ( ; ang < 0; ang+=360 ) ;
	for ( ; ang > 360; ang-=360 ) ;

	float xx1 = min(lf->polyVert[0], min(lf->polyVert[6], min(lf2->polyVert[0], lf2->polyVert[6])));
	float yy1 = min(lf->polyVert[1], min(lf->polyVert[7], min(lf2->polyVert[1], lf2->polyVert[7])));
	float xx2 = max(lf->polyVert[0], max(lf->polyVert[6], max(lf2->polyVert[0], lf2->polyVert[6])));
	float yy2 = max(lf->polyVert[1], max(lf->polyVert[7], max(lf2->polyVert[1], lf2->polyVert[7])));
	xx1 -= grow_x;
	yy1 -= grow_y;
	xx2 += grow_x;
	yy2 += grow_y;

	if (ang == 0)
	{
		if (
			(x >= m_pos.getX() + xx1) &&
			(y >= m_pos.getY() + yy1) &&
			(x <= m_pos.getX() + xx2) &&
			(y <= m_pos.getY() + yy2)
			) return true;
	}
	float a0 = (ang * pi) / 180;
	float x0 = m_pos.getX();
	float y0 = m_pos.getY();
	

	//Y.x = [cos(a) * (X.x - C.x)] - [sin(a) * (X.y - C.y)] + C.x;
	//Y.y = [sin(a) * (X.x - C.x)] + [cos(a) * (X.y - C.y)] + C.y;

	float nx1 = (cos(a0) * xx1) - (sin(a0) * yy1) + x0;
	float ny1 = (sin(a0) * xx1) + (cos(a0) * yy1) + y0;

	float nx2 = (cos(a0) * xx2) - (sin(a0) * yy1) + x0;
	float ny2 = (sin(a0) * xx2) + (cos(a0) * yy1) + y0;

	float nx3 = (cos(a0) * xx2) - (sin(a0) * yy2) + x0;
	float ny3 = (sin(a0) * xx2) + (cos(a0) * yy2) + y0;

	float nx4 = (cos(a0) * xx1) - (sin(a0) * yy2) + x0;
	float ny4 = (sin(a0) * xx1) + (cos(a0) * yy2) + y0;

	float testx = (float)x;
	float testy = (float)y;

	float vertx[4];
	vertx[0] = nx1;
	vertx[1] = nx2;
	vertx[2] = nx3;
	vertx[3] = nx4;

	float verty[4];
	verty[0] = ny1;
	verty[1] = ny2;
	verty[2] = ny3;
	verty[3] = ny4;


	int i, j, c = 0;
	for (i = 0, j = 3; i < 4; j = i++) 
	{
		if ( ((verty[i]>testy) != (verty[j]>testy)) &&
			(testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
			c = !c;
	}

	//_mainObj->ShowDebugNumber(nx1, ny1, 1);
	//_mainObj->ShowDebugNumber(nx2, ny2, 2);
	//_mainObj->ShowDebugNumber(nx3, ny3, 3);
	//_mainObj->ShowDebugNumber(nx4, ny4, 4);

	//if (m_text ==string("<< Menu"))
	//{
	//Trace("HitTest: %d,%d vs (%f,%f)-(%f,%f) from: (%f,%f)-(%f,%f) \n", x, y,
	//	nx1,ny1,
	//	//nx2,ny2,
	//	//nx3,ny3,
	//	nx4,ny4,
	//	x1,y1,
	//	//x2,y2,
	//	//x3,y3,
	//	x4,y4
	//	);

	//_mainObj->ShowDebugPoint(x0+xx1, y0+yy1);
	//_mainObj->ShowDebugPoint(x0+xx2, y0+yy1);
	//_mainObj->ShowDebugPoint(x0+xx1, y0+yy2);
	//_mainObj->ShowDebugPoint(x0+xx2, y0+yy2);
	//}


	if (c != 0) 
	{
#ifdef _DEBUG
		//vertex1->SetCoord(nx1, ny1);
		//vertex2->SetCoord(nx2, ny2);
		//vertex3->SetCoord(nx3, ny3);
		//vertex4->SetCoord(nx4, ny4);
#endif
		return true;
	}
	//if ((ang > 45.0f && ang < 125.0f)
	//	||
	//	(ang > 180+45.0f && ang < 180+125.0f))
	//{
	//	// rotated
	//	if (
	//		(x >= m_pos.getX() + lf->polyVert[0]) &&
	//		(y >= m_pos.getY() + lf->polyVert[1]) &&
	//		(x <= m_pos.getX() + lf->polyVert[6]) &&
	//		(y <= m_pos.getY() + lf->polyVert[7])
	//		) return true;
	//}
	return false;
}

float TextSprite::IdealWidth()
{
	float width = 0;
	float inter_spacing = LetterSpacing();
	for (uint i = 0; i < m_text.length(); i++)
	{
		byte b = m_text[i];
		width += LetterWidth(b) - inter_spacing;
	}
	return width;
}

TextHandler::TextHandler()
{
}

TextHandler::~TextHandler()
{

}

TextSprite* TextHandler::CreateText( FontSizeType fs )
{
	return globalTextHandler.CreateTextProtected(fs);
}

TextSprite* TextHandler::CreateTextProtected( FontSizeType fs )
{
	TextureNumber tnum = tex_sans_8s;
//	if (fs == FontSize_sans_8o) tnum = tex_sans_8o;
	if (fs == FontSize_sans_8s) tnum = tex_sans_8s;
	if (fs == FontSize_sans_12o) tnum = tex_sans_12o;
//	if (fs == FontSize_sans_16) tnum = tex_sans_16;
//	if (fs == FontSize_sans_24) tnum = tex_sans_24;
//	if (fs == FontSize_sans_24o) tnum = tex_sans_24o;
	if (fs == FontSize_sans_24s) tnum = tex_sans_24s;
//	if (fs == FontSize_sans_32s) tnum = tex_sans_32s;
	if (fs == FontSize_serif_10) tnum =  tex_serif_10;
	if (fs == FontSize_sans_numbers) tnum = tex_sans_numbers;
	if (fs == FontSize_sans_typewriter) tnum = tex_sans_typewriter;

	TextSprite* ts = new TextSprite(tnum, fs);
	return ts;
}

float* TextHandler::Points( FontSizeType fs )
{
//	if (fs == FontSize_sans_8o) return bitmap_sans_8o_coords;
	if (fs == FontSize_sans_8s) return bitmap_sans_8s_coords;
	if (fs == FontSize_sans_12o) return bitmap_sans_12o_coords;
//	if (fs == FontSize_sans_16) return bitmap_sans_16_coords;
//	if (fs == FontSize_sans_24) return bitmap_sans_24_coords;
//	if (fs == FontSize_sans_24o) return bitmap_sans_24o_coords;
//	if (fs == FontSize_sans_32s) return bitmap_sans_32s_coords;
	if (fs == FontSize_sans_24s) return bitmap_sans_24s_coords;
	if (fs == FontSize_serif_10) return bitmap_serif_10_coords;
	if (fs == FontSize_sans_numbers) return bitmap_sans_numbers_coords;
	if (fs == FontSize_sans_typewriter) return bitmap_sans_typewriter_coords;
	return NULL;
}

float TextSprite::LetterSpacing()
{
	if (m_auto_fs == FontSize_sans_8s) return 1;
	if (m_auto_fs == FontSize_sans_12o) return 1;
//	if (m_auto_fs == FontSize_sans_numbers) return 1;
//	if (fs == FontSize_sans_24s) return bitmap_sans_24s_coords;
//	if (fs == FontSize_serif_10) return bitmap_serif_10_coords;
	return 0;
}


float TextHandler::IdealHeight( FontSizeType fs )
{
	return TextHandler::Points(fs)[0] - 1.0f;
}

int TextHandler::ApproachNumber( int cur, int target )
{
	if (cur < target - 100000) return cur + 100000;
	if (cur < target - 10000) return cur + 10000;
	if (cur < target - 1000) return cur + 1000;
	if (cur < target - 100) return cur + 100;
	if (cur < target - 10) return cur + 10;
	if (cur < target - 5) return cur + 5;
	if (cur < target) return cur + 1;
	if (cur > target + 100000) return cur - 100000;
	if (cur > target + 10000) return cur - 10000;
	if (cur > target + 1000) return cur - 1000;
	if (cur > target + 100) return cur - 100;
	if (cur > target + 10) return cur - 10;
	if (cur > target + 5) return cur - 5;
	if (cur > target) return cur - 1;
	return cur;
}

std::string TextHandler::FormatNumber( const char* format, int num )
{
	char buf[30];
	sprintf(buf, format, num);
	std::string ret = string(buf);
	return ret;
}

std::string TextHandler::FormatFloat( const char* format, float num )
{
	char buf[30];
	sprintf(buf, format, num);
	std::string ret = string(buf);
	return ret;
}