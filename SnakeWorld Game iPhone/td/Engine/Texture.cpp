#include "stdafx.h"
#include "global.h"
#include "Texture.h"
#include "MainObj.h"

#ifdef _WINDOWS
#include "..\Windows\lodepng.h"
#else
#import "LoadTexture.h"
#endif

#ifdef _WINDOWS
#define GL_CLAMP_TO_EDGE                         0x812F
#endif

extern MainObj* _mainObj;

Texture::Texture(void)
{
//	image = NULL;
	m_bTrimMode = false;
}
Texture::~Texture(void)
{
	glDeleteTextures(1, &spriteTexture);
//	if (image != NULL) free(image);
}

Texture::Texture(uint w, uint h)
{
	m_width = w;
	m_height = h;
	grid_x = m_width;
	grid_y = m_height;
	image = new byte[m_width*m_height*4];//(byte*)(malloc(m_width * m_height * 4));
	memset(image, 0, m_width*m_height*4);
	for (uint i = 0; i < m_width * m_height * 4;i++)
	{
		image[i]= i%255;
	}
	glGenTextures(1, &spriteTexture);
	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//	GLenum ge = glGetError();
	delete image;
	image = NULL;
//	m_bSpaceMode = false;
	m_bTrimMode = false;
}

Texture::Texture(const std::string png_filename, uint gridx, uint gridy)//, bool space_mode)
{
	LoadTexture(png_filename);
	grid_x = gridx;
	grid_y = gridy;
//	m_bSpaceMode = space_mode;
}

Texture::Texture(std::string png_filename)
{
	LoadTexture(png_filename);
	grid_x = m_width;
	grid_y = m_height;
//	m_bSpaceMode = false;
}

void Texture::LoadTexture(std::string png_filename)
{
	filename = png_filename;
	std::string actual = filename;
#ifdef _WINDOWS
	actual = "..\\images\\Production\\";
	actual.append(filename);
	filename = actual;
#endif

	glGenTextures(1, &spriteTexture);
	glBindTexture(GL_TEXTURE_2D, spriteTexture);

#ifdef _WINDOWS
	byte* buffer;
	size_t buffersize, imagesize;
	LodePNG_Decoder decoder;

	LodePNG_loadFile(&buffer, &buffersize, filename.data()); /*load the image file with given filename*/
	LodePNG_Decoder_init(&decoder);
	LodePNG_decode(&decoder, &image, &imagesize, buffer, buffersize); /*decode the png*/

	if (decoder.infoPng.width == 0)	// file not loaded
	{
		Trace("FileNotFound!\n");
		image = (byte*)malloc(8*8*4);
		m_width = 8;
		m_height = 8;
		for (int i = 0; i < 8; i++)
		{
			image[i*8*4 + i*4 + 0] = 0x88;
			image[i*8*4 + (7-i)*4 + 0] = 0x88;
			image[i*8*4 + i*4 + 1] = 0x88;
			image[i*8*4 + (7-i)*4 + 1] = 0x88;
			image[i*8*4 + i*4 + 2] = 0x88;
			image[i*8*4 + (7-i)*4 + 2] = 0x88;
			image[i*8*4 + i*4 + 3] = 0x88;
			image[i*8*4 + (7-i)*4 + 3] = 0x88;
		}
		for (int i = 0; i < 64; i++)
		{
			image[i*4] = 0xFF;
			image[i*4+3] = 0xFF;
		}
	}
	else
	{
		m_width = decoder.infoPng.width;
		m_height = decoder.infoPng.height;
	}

	free(buffer);
	LodePNG_Decoder_cleanup(&decoder);

	//for (unsigned int i = 0; i < imagesize ; i++)
	//{
	//	image[i]=0xaa;
	//}

	// flip vertically
	for (uint yy = 0; yy < m_height/2; yy++)
	{
		byte* s;
		byte* d;
		s = image + (m_width * yy * 4);
		d = image + (m_height-1) * m_width * 4 - (m_width * yy * 4);
		for (uint xx = 0; xx < m_width; xx++)
		{
			byte c;
			c = *s; *s = *d; *d = c; s+=1; d+=1;
			c = *s; *s = *d; *d = c; s+=1; d+=1;
			c = *s; *s = *d; *d = c; s+=1; d+=1;
			c = *s; *s = *d; *d = c; s+=1; d+=1;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	GLenum ge = glGetError();

	free(image);

	//image = NULL;

#else
	unsigned int size = 0;
	unsigned char* buffer = NULL;
	if (::LoadTexture(&buffer, &size, &m_width, &m_height, filename.data()))
	{	
		// flip vertically
		for (uint yy = 0; yy < m_height/2; yy++)
		{
			byte* s;
			byte* d;
			s = buffer + (m_width * yy * 4);
			d = buffer + (m_height-1) * m_width * 4 - (m_width * yy * 4);
			for (uint xx = 0; xx < m_width; xx++)
			{
				byte c;
				c = *s; *s = *d; *d = c; s+=1; d+=1;
				c = *s; *s = *d; *d = c; s+=1; d+=1;
				c = *s; *s = *d; *d = c; s+=1; d+=1;
				c = *s; *s = *d; *d = c; s+=1; d+=1;
			}
		}
		// Specify a 2D texture image, provideing the a pointer to the image data in memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		// Release the image data
		free(buffer);
	}
#endif
	// Set the texture parameters to use a minifying filter and a linear filer (weighted average)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

}

std::list<TextureEntryStruct*> shared_texs;
Texture* TextureHandler::OpenTexture( TextureNumber texno, uint width, uint height )
{
	TextureEntryStruct* te = TextureHandler::FindTexture(texno);
	if (te == NULL)
	{
		te = new TextureEntryStruct();
		te->no = texno;
		te->count = 1;
		if (texno == tex_empty) te->tex = new Texture(width, height);
		if (texno == tex_empty2) te->tex = new Texture(width, height);
//		if (texno == tex_empty3) te->tex = new Texture(width, height);
//		if (texno == tex_empty4) te->tex = new Texture(width, height);
		//if (texno == tex_mobs_20x20) te->tex = new Texture("C_mobs_20x20.png", 20, 20);
//		if (texno == tex_mobs_28x28) te->tex = new Texture("C_mobs_28x28.png", 28, 28);
//		if (texno == tex_mobs_32x32) te->tex = new Texture("C_mobs_32x32.png", 32, 32);
		if (texno == tex_lifebar) te->tex = new Texture("lifebar.png", 32, 8);
		if (texno == tex_mob_overlays) te->tex = new Texture("mob_overlays.png", 16, 16);

		if (texno == tex_incoming_bg) te->tex = new Texture("incoming_background.png", 28, 28);
		if (texno == tex_freeze) te->tex = new Texture("ice_freeze.png", 32, 32);
		if (texno == tex_tiles_20x20) te->tex = new Texture("C_tiles_20x20.png", 20, 20);
		if (texno == tex_overlays_20x20) te->tex = new Texture("C_overlays_20x20.png", 20, 20);
		if (texno == tex_towers_32x32) te->tex = new Texture("C_towers_32x32.png", 32, 32);
		if (texno == tex_misc_64x64) te->tex = new Texture("C_misc_64x64.png", 64, 64);
		
		if (texno == tex_game_interface) te->tex = new Texture("game_interface.png", 50, 40);

		if (texno == tex_landscape) te->tex = new Texture("select_landscape.png", 1024, 256);
		if (texno == tex_challenge) te->tex = new Texture("challenge.png", 1024, 256);
		if (texno == tex_level_select_trim) te->tex = new Texture("level_select_trim.png", 480, 64);

		if (texno == tex_title1) te->tex = new Texture("title1.png", 480, 320);
		if (texno == tex_title2) te->tex = new Texture("title2.png", 480, 320);
		if (texno == tex_title3) te->tex = new Texture("title3.png", 480, 320);
		if (texno == tex_title4) te->tex = new Texture("title4.png", 480, 320);
		if (texno == tex_stats) te->tex = new Texture("stats.png", 480, 320);

//		if (texno == tex_transition_burn) te->tex = new Texture("transition_burn.png", 64, 64);

		if (texno == tex_typewriter) te->tex = new Texture("typewriter_48x48.png", 48, 48);
		if (texno == tex_rewards) te->tex = new Texture("rewards_64x64.png", 64, 64);
		if (texno == tex_reward_back) te->tex = new Texture("rewards_back.png", 480, 320);
		if (texno == tex_oldman) te->tex = new Texture("oldman_32x32.png", 32, 32);

		if (texno == tex_radius) te->tex = new Texture("g_radius.png", 64, 64);

		if (texno == tex_logo) te->tex = new Texture("logo.png", 512, 128);
		if (texno == tex_star) te->tex = new Texture("star.png", 16, 16);
		if (texno == tex_back) te->tex = new Texture("back.png", 96, 48);
		if (texno == tex_play) te->tex = new Texture("play.png", 96, 48);
		
		if (texno == tex_victory) te->tex = new Texture("victory.png", 480, 320);
		if (texno == tex_defeat) te->tex = new Texture("defeat.png", 480, 320);

		if (texno == tex_scroll) te->tex = new Texture("scroll.png", 480, 320);
		if (texno == tex_help1) te->tex = new Texture("help1.png", 256, 256);
		if (texno == tex_help2) te->tex = new Texture("help2.png", 256, 256);
		if (texno == tex_help3) te->tex = new Texture("help3.png", 256, 256);
		if (texno == tex_help4) te->tex = new Texture("help4.png", 256, 256);
		if (texno == tex_help5) te->tex = new Texture("help5.png", 256, 256);
		if (texno == tex_help6) te->tex = new Texture("help6.png", 256, 256);
		if (texno == tex_help7) te->tex = new Texture("help7.png", 256, 256);


//		if (texno == tex_sans_8) te->tex = new Texture("sans_8.png", 1, 1);
//		if (texno == tex_sans_8o) te->tex = new Texture("sans_8o.png", 1, 1);
		if (texno == tex_sans_8s) te->tex = new Texture("sans_8s.png", 1, 1);
//		if (texno == tex_sans_10) te->tex = new Texture("sans_10.png", 1, 1);
		if (texno == tex_sans_12o) te->tex = new Texture("sans_12o.png", 1, 1);
		if (texno == tex_sans_typewriter) te->tex = new Texture("sans_typewriter.png", 1, 1);
//		if (texno == tex_sans_16) te->tex = new Texture("sans_16.png", 1, 1);
//		if (texno == tex_sans_24) te->tex = new Texture("sans_24.png", 1, 1);
//		if (texno == tex_sans_24o) te->tex = new Texture("sans_24o.png", 1, 1);
		if (texno == tex_sans_24s) te->tex = new Texture("sans_24s.png", 1, 1);
//		if (texno == tex_sans_32s) te->tex = new Texture("sans_32s.png", 1, 1);
		if (texno == tex_serif_10) te->tex = new Texture("serif_10.png", 1, 1);
		if (texno == tex_sans_numbers) te->tex = new Texture("sans_numbers.png", 1, 1);
		if (texno >= tex_mobs && texno < tex_deaths)
		{
			std::string fname = Format("mob_%d.png", texno - tex_mobs);
			te->tex = new Texture(fname.data(), 32, 32);
		}
		if (texno >= tex_deaths)
		{
			std::string fname = Format("death_%d.png", texno - tex_deaths);
			te->tex = new Texture(fname.data(), 32, 32);
		}

		if (te->tex == NULL) te->tex = new Texture(8,8);
		shared_texs.push_back(te);
		Trace("Opening Texture %s(%d) %d bytes\n", te->tex->filename.data(), texno, te->tex->m_height*te->tex->m_width*4);
		return te->tex;
	}
	else
	{
		te->count++;
		return te->tex;
	}
	FlushTextures();
}

void TextureHandler::CloseTexture( TextureNumber texno )
{
	TextureEntryStruct* te = TextureHandler::FindTexture(texno);
	if (te != NULL)
	{
		te->count--;
		if (te->count <= 0)
		{
			te->ticks_when_closed = last_main_timer.last_timer;
		}
	}
	FlushTextures();
}

void TextureHandler::CloseTexture( Texture* tex )
{
	std::list<TextureEntryStruct*>::iterator i;
	for (i = shared_texs.begin(); i != shared_texs.end(); i++)
	{
		TextureEntryStruct* te = (*i);
		if (te->tex == tex)
		{
			CloseTexture(te->no);
			return;	// list might change
		}
	}
	FlushTextures();
}
TextureEntryStruct* TextureHandler::FindTexture( TextureNumber texno )
{
	std::list<TextureEntryStruct*>::iterator i;
	for (i = shared_texs.begin(); i != shared_texs.end(); i++)
	{
		TextureEntryStruct* te = (*i);
		if (te->no == texno) return te;
	}
	return NULL;
}

void TextureHandler::FlushTextures(bool immediate)
{
	std::list<TextureEntryStruct*>::iterator i;
	bool repeat = false;
	do
	{
		repeat = false;
		for (i = shared_texs.begin(); i != shared_texs.end(); i++)
		{
			TextureEntryStruct* te = (*i);
			if (te->count <= 0)
			{
				if ((te->ticks_when_closed > last_main_timer.last_timer + 5000) || immediate)
				{
					Trace("DELETING TEXTURE %s(%d) %d bytes\n", te->tex->filename.data(), te->no, te->tex->m_height*te->tex->m_width*4);
					delete te->tex;
					shared_texs.remove(te);
					delete te;
					repeat = true;
					goto goto_sucks_but_sometimes_you_gotta_do_it;
				}
			}
		}
goto_sucks_but_sometimes_you_gotta_do_it:
		int dummy = 0;		dummy++;
	} while (repeat);
#ifdef _DEBUG
	UpdateMemorySize();
#endif
}

void TextureHandler::CloseAllTextures()
{
	std::list<TextureEntryStruct*>::iterator i;
	for (i = shared_texs.begin(); i != shared_texs.end(); i++)
	{
		TextureEntryStruct* te = (*i);
		te->count = 0;
	}
	FlushTextures(true);
}
#ifdef _DEBUG
void TextureHandler::UpdateMemorySize()
{
	int s = 0;
	std::list<TextureEntryStruct*>::iterator i;
	for (i = shared_texs.begin(); i != shared_texs.end(); i++)
	{
		TextureEntryStruct* te = (*i);
		s += (te->tex->m_height * te->tex->m_width * 4);
	}	
	_mainObj->SetTextSize(s);
}
#endif
