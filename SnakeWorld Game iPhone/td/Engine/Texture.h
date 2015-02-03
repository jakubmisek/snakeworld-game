#pragma once
#include "global.h"
#include <string>
#include <list>
#include <vector>

typedef enum TextureNumberEnum
{
	tex_empty,
	tex_empty2,
//	tex_empty3,
//	tex_empty4,
//	tex_mobs_28x28,
//	tex_mobs_32x32,
	tex_lifebar,
	tex_mob_overlays,
	tex_incoming_bg,
	tex_freeze,
	tex_tiles_20x20,
	tex_overlays_20x20,
	tex_towers_32x32,
	tex_misc_64x64,
	tex_game_interface,
	tex_landscape,
	tex_challenge,
	tex_title1,
	tex_title2,
	tex_title3,
	tex_title4,
	tex_stats,
	tex_victory,
	tex_defeat,
//	tex_transition_burn,
	tex_typewriter,
	tex_rewards,
	tex_reward_back,
	tex_oldman,
	tex_level_select_trim,
	tex_radius,
	tex_logo,
	tex_star,
	tex_back,
	tex_play,

	tex_scroll,
	tex_help1,
	tex_help2,
	tex_help3,
	tex_help4,
	tex_help5,
	tex_help6,
	tex_help7,

//	tex_sans_8,
//	tex_sans_8o,
	tex_sans_8s,
//	tex_sans_10,
	tex_sans_12o,
	tex_sans_typewriter,
//	tex_sans_16,
//	tex_sans_24,
//	tex_sans_24o,
	tex_sans_24s,
	tex_serif_10,
	tex_sans_numbers,

	tex_mobs,
	tex_deaths = tex_mobs + 100
} TextureNumber;

class Texture
{
	friend class TextureHandler;
	friend class Animation;
	friend class TextSprite;
public:
	bool m_bTrimMode;
	uint m_width;
	uint m_height;
	GLuint spriteTexture;
	uint grid_x;
	uint grid_y;

protected:
	Texture(void);
	virtual ~Texture(void);

	Texture(std::string png_filename);
	Texture(uint width, uint height);	// empty texture
	Texture(const std::string png_filename, uint gridx, uint gridy);//, bool space_mode);

	byte* image;
	std::string filename;

	void LoadTexture(std::string png_filename);

	//bool m_bSpaceMode;
};

struct TextureEntryStruct
{
	TextureNumber no;
	Texture* tex;
	int count;
	uint ticks_when_closed;
};

class TextureHandler
{
public:
	static Texture* OpenTexture(TextureNumber texno, uint width, uint height);	// empty texture
	static Texture* OpenTexture(TextureNumber texno) { return TextureHandler::OpenTexture(texno, 0, 0); } // defined texture

	static void FlushTextures(bool immediate = false);

	static void CloseTexture(TextureNumber texno);
	static void CloseTexture(Texture* tex);

	static void CloseAllTextures();
protected:
	static TextureEntryStruct* FindTexture(TextureNumber texno);
#ifdef _DEBUG
	static void UpdateMemorySize();
#endif

};


