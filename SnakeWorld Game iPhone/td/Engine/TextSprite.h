#pragma once

#include "Sprite.h"
#include <string>

struct LetterFrame
{
	GLfloat textureVert[8];
	GLfloat polyVert[8];
};
typedef enum FontSizeTypeEnum
{
//	FontSize_sans_8o,
	FontSize_sans_8s,
	FontSize_sans_12o,
//	FontSize_sans_16,
//	FontSize_sans_24,
//	FontSize_sans_24o,
	FontSize_sans_24s,
//	FontSize_sans_32s,
	FontSize_serif_10,
	FontSize_sans_numbers,
	FontSize_sans_typewriter
} FontSizeType;

class TextHandler;

class TextSprite : public Sprite
{
public:
	TextSprite(TextureNumber tex, FontSizeType fs);
	virtual ~TextSprite();
	void SetString(const std::string &text);
	void SetText(const char* text)
	{
		std::string str = std::string(text); SetString(str); 
	}
	std::string TextString() { return std::string(m_text); }
	std::string GetText() { return std::string(m_text); }
	void SetTextFormatted(char* format, int number);
	void SetTextI(int intval);
	void SetTextF(float fval, int num_of_decimals = 2);
	void CalcLetters();
	void SetWidth(float w) { SetSize(w, IdealHeight()); }
	float IdealHeight();
	float IdealWidth();
	void SetIdealHeight() { SetSize(size.getW(), IdealHeight()); }

	virtual void DrawVertexes();
	virtual void CalcSize() {CalcLetters();}	//override

	virtual bool HitTest(int x, int y, int grow_x = 0, int grow_y = 0);
protected:
	std::string m_text;

	GLshort tex_x, tex_y, tex_w, tex_h;
	LetterFrame* letterFrames;

	float TextWidth();
	float TextHeight();
	float LetterWidth(byte b);
	float LetterHeight(byte b);

	float TextureX1(byte b);
	float TextureY1(byte b);
	float TextureX2(byte b);
	float TextureY2(byte b);
	float TextureW(byte b);
	float TextureH(byte b);

	float LetterSpacing();

	byte Range(byte b);
	FontSizeType m_auto_fs;
};

class TextHandler
{
public:
	TextHandler();
	virtual ~TextHandler();
public:
	static TextSprite* CreateText(FontSizeType fs);
	void CloseText(FontSizeType fs);
	static float* Points(FontSizeType fs);
	static float IdealHeight(FontSizeType fs);
	static int ApproachNumber(int cur, int target);
	static std::string FormatNumber(const char* format, int num);
	static std::string FormatFloat(const char* format, float num);

protected:
	TextSprite* CreateTextProtected(FontSizeType fs);
};