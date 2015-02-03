#include "stdafx.h"
#include "Animation.h"

//Animation::Animation()
//{
//	frameno = 0;
//	ticks_into_frame = 0;
//	last_rendered_ticks = -1;
//	m_RepeatType = AnimRepeat_Loop;
//	AnimEnded = false;
//	m_bBackwards = false;
//	m_bPaused = false;
//	m_appearDelay = 0;
//}

Animation::Animation( Texture* tex, td_timer* use_timer )
{
	m_timer = use_timer;
	texture = tex;
	m_bPaused = false;
	m_appearDelay = 0;

	frameno = 0;
	ticks_into_frame = 0;
	last_rendered_ticks = -1;
	m_RepeatType = AnimRepeat_Loop;
	AnimEnded = false;
	m_bBackwards = false;
}
Animation::~Animation()
{
	std::vector<AnimationFrame*>::iterator c_it;
	for (size_t i = 0; i < GetFrames()->size(); i++)
	{
		AnimationFrame* f = GetFrames()->at(i);
		delete f;
	}
	TextureHandler::CloseTexture(texture);
	GetFrames()->clear();
}

void Animation::CalculateFrame(AnimationFrame* f, uint grid_pos)
{
	uint grid_cols = (texture->m_width / texture->grid_x);
	//if (texture->m_bSpaceMode)
	//{
	//	grid_cols = (texture->m_width / (texture->grid_x + 1));
	//}
	uint grid_y = grid_pos / grid_cols;
	uint grid_x = grid_pos - (grid_y * grid_cols);

	//if (texture->m_bSpaceMode)
	//{
	//	f->x = (float)((texture->grid_x + 1) * grid_x);
	//	f->y = (float)((texture->grid_y + 1) * grid_y);
	//	f->w = (float)(texture->grid_x);
	//	f->h = (float)(texture->grid_y);
	//}
	//else
	{
		f->x = (float)(texture->grid_x * grid_x);
		f->y = (float)(texture->grid_y * grid_y);
		f->w = (float)(texture->grid_x);
		f->h = (float)(texture->grid_y);
	}
}
void Animation::CalculateFrame(AnimationFrame* f, uint grid_x, uint grid_y)
{
	//if (texture->m_bSpaceMode)
	//{
	//	f->x = (float)((texture->grid_x + 1) * grid_x);
	//	f->y = (float)((texture->grid_y + 1) * grid_y);
	//	f->w = (float)(texture->grid_x);
	//	f->h = (float)(texture->grid_y);
	//}
	//else
	{
		f->x = (float)(texture->grid_x * grid_x);
		f->y = (float)(texture->grid_y * grid_y);
		f->w = (float)(texture->grid_x);
		f->h = (float)(texture->grid_y);
	}
}

void Animation::AddFrame()
{
	AnimationFrame* f = new AnimationFrame();
	f->x = 0;
	f->y = 0;
	f->w = (float)texture->m_width;
	f->h = (float)texture->m_height;
	f->delay = MasterDelay;
	GetFrames()->push_back(f);
	CalcTexcoords();
}

//void Animation::AddFrames(uint frames, uint delay)
//{
//	for(uint i = 0; i < frames; i++)
//	{
//		AnimationFrame* f = new AnimationFrame();
//		f->x = (float)((texture->m_width / frames) * i);
//		f->y = 0;
//		f->w = (float)texture->m_width / frames;
//		f->h = (float)texture->m_height;
//		f->delay = delay == -1 ? MasterDelay : delay;
//		GetFrames()->push_back(f);
//	}
//	CalcTexcoords();
//}

void Animation::AddFrame(float x, float y, float w, float h, uint delay)
{
	AnimationFrame* f = new AnimationFrame();
	f->x = x;
	f->y = y;
	f->w = w;
	f->h = h;
	f->delay = delay == -1 ? MasterDelay : delay;
	GetFrames()->push_back(f);
	CalcTexcoords();
}
void Animation::AddFrame(uint grid_pos, uint delay)
{
	AnimationFrame* f = new AnimationFrame();
	f->delay = delay == -1 ? MasterDelay : delay;
	CalculateFrame(f, grid_pos);
	GetFrames()->push_back(f);
	CalcTexcoords();
}
void Animation::SetFrame( uint frame_no, uint grid_pos, uint delay /*= -1*/ )
{
	if (GetFrames()->size() == 0)		AddFrame();
	if (frame_no >= (uint)GetFrames()->size())		frame_no = (uint)GetFrames()->size() - 1;
	CalculateFrame(GetFrames()->at(frame_no), grid_pos);
	AnimationFrame* f = GetFrames()->at(frame_no);
	f->delay = delay == -1 ? MasterDelay : delay;
	CalcTexcoords();
}

void Animation::SetFrame( uint frame_no, float x, float y, float w, float h, uint delay /*= -1*/ )
{
	if (GetFrames()->size() == 0)		AddFrame();
	if (frame_no >= (uint)GetFrames()->size())		frame_no = (uint)GetFrames()->size() - 1;
	AnimationFrame* f = GetFrames()->at(frame_no);
	f->x = x;
	f->y = y;
	f->w = w;
	f->h = h;
	f->delay = delay == -1 ? MasterDelay : delay;
	CalcTexcoords();
}

void Animation::SetFrameDelay( uint frame_no, uint delay /*= -1*/ )
{
	if (GetFrames()->size() == 0)		AddFrame();
	if (frame_no >= (uint)GetFrames()->size())		frame_no = (uint)GetFrames()->size() - 1;
	AnimationFrame* f = GetFrames()->at(frame_no);
	f->delay = delay == -1 ? MasterDelay : delay;
}
void Animation::AddFrame(uint grid_x, uint grid_y, uint delay)
{
	AnimationFrame* f = new AnimationFrame();
	f->delay = delay == -1 ? MasterDelay : delay;
	CalculateFrame(f, grid_x, grid_y);
	GetFrames()->push_back(f);
	CalcTexcoords();
}

void Animation::Render()
{
	if (last_rendered_ticks != last_timer())
	{
		if (m_appearDelay > 0)
		{
			m_appearDelay -= elapsed_ticks();
			last_rendered_ticks = last_timer();
			return;
		}
		if (GetFrames()->size() > 1)
		{
			if (m_bPaused)
			{
				last_rendered_ticks += elapsed_ticks();
			}
			else
			{
				AnimationFrame* f = GetFrames()->at(frameno);
				ticks_into_frame += elapsed_ticks();
				if (ticks_into_frame >= f->delay)
				{
					ticks_into_frame -= f->delay;
					if (GetFrames()->size() > 0)
					{
						switch (m_RepeatType)
						{
						case AnimRepeat_None:
							{
								if (frameno >= GetFrames()->size() - 1)
									AnimEnded = true;
								else
									frameno++;
							}
							break;
						case AnimRepeat_Loop:
							{
								if (frameno >= GetFrames()->size() - 1)
								{
									frameno = 0;
									AnimEnded = true;
								}
								else
									frameno++;
							}
							break;
						case AnimRepeat_Bounce:
							{
								if (!m_bBackwards)
								{
									if (frameno >= GetFrames()->size() - 1)
									{
										frameno--;
										m_bBackwards = true;
									}
									else
										frameno++;
								}
								else
								{
									if (frameno <=0)
									{
										frameno++;
										m_bBackwards = false;
									}
									else
									{
										if (frameno == 1) AnimEnded = true;
										frameno--;
									}
								}
							}
							break;
						}
						CalcTexcoords();
					}
					else
						AnimEnded = true;
				}
			}
		}
	}
	last_rendered_ticks = last_timer();
	glBindTexture(GL_TEXTURE_2D, texture->spriteTexture);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Animation::CalcTexcoords()
{
	AnimationFrame* f = GetFrames()->at(frameno);

	if (texture->m_bTrimMode && 1==0)
	{
		spriteTexcoords[0] = (((GLfloat)f->x) + .5f) / (GLfloat)(texture->m_width);// + (.50f / texture->m_width);
		spriteTexcoords[1] = (((GLfloat)f->y) + .5f) / (GLfloat)(texture->m_height);// + (.50f / texture->m_width);

		spriteTexcoords[2] = (((GLfloat)(f->x + f->w) - .5f ) / (GLfloat)(texture->m_width));// + (.50f / texture->m_width);
		spriteTexcoords[3] = spriteTexcoords[1];

		spriteTexcoords[4] = spriteTexcoords[0];
		spriteTexcoords[5] = (((GLfloat)(f->y + f->h) - .5f ) / (GLfloat)(texture->m_height));// + (.50f / texture->m_width);

		spriteTexcoords[6] = spriteTexcoords[2];
		spriteTexcoords[7] = spriteTexcoords[5];
	}
	else
	{
		spriteTexcoords[0] = (((GLfloat)f->x) ) / (GLfloat)(texture->m_width);// + (.50f / texture->m_width);
		spriteTexcoords[1] = (((GLfloat)f->y) ) / (GLfloat)(texture->m_height);// + (.50f / texture->m_width);

		spriteTexcoords[2] = (((GLfloat)(f->x + f->w) ) / (GLfloat)(texture->m_width));
		spriteTexcoords[3] = spriteTexcoords[1];

		spriteTexcoords[4] = spriteTexcoords[0];
		spriteTexcoords[5] = (((GLfloat)(f->y + f->h) ) / (GLfloat)(texture->m_height));

		spriteTexcoords[6] = spriteTexcoords[2];
		spriteTexcoords[7] = spriteTexcoords[5];
	}

	//spriteTexcoords[0] = (((GLfloat)f->x) ) / (GLfloat)(texture->m_width);// + (.50f / texture->m_width);
	//spriteTexcoords[1] = (((GLfloat)f->y) ) / (GLfloat)(texture->m_height);// + (.50f / texture->m_width);

	//spriteTexcoords[2] = (((GLfloat)(f->x + f->w - 1)  ) / (GLfloat)(texture->m_width));// + (.50f / texture->m_width);
	//spriteTexcoords[3] = spriteTexcoords[1];

	//spriteTexcoords[4] = spriteTexcoords[0];
	//spriteTexcoords[5] = (((GLfloat)(f->y + f->h - 1)  ) / (GLfloat)(texture->m_height));// + (.50f / texture->m_width);

	//spriteTexcoords[6] = spriteTexcoords[2];
	//spriteTexcoords[7] = spriteTexcoords[5];

	//	spriteTexcoords[0] = .1;
	//	spriteTexcoords[1] = 0;
	//	spriteTexcoords[2] = 0;
	//	spriteTexcoords[3] = 0;
	//	spriteTexcoords[4] = .1;
	//	spriteTexcoords[5] = -.1;
	//	spriteTexcoords[6] = 0;
	//	spriteTexcoords[7] = -.1;
}

uint Animation::TotalAnimationTime()
{
	uint ret = m_appearDelay;
	for (size_t i = 0; i < GetFrames()->size(); i++)
	{
		AnimationFrame* f = GetFrames()->at(i);
		ret += f->delay;
	}
	return ret;
}

void Animation::SetFrameCount( uint count )
{
	while (GetFrames()->size() < count)
	{
		AddFrame(0, 0, 0);
		GetFrames()->at(GetFrames()->size() - 1)->x = GetFrames()->at(0)->x;
		GetFrames()->at(GetFrames()->size() - 1)->y = GetFrames()->at(0)->y;
		GetFrames()->at(GetFrames()->size() - 1)->w = GetFrames()->at(0)->w;
		GetFrames()->at(GetFrames()->size() - 1)->h = GetFrames()->at(0)->h;
		GetFrames()->at(GetFrames()->size() - 1)->delay = GetFrames()->at(0)->delay;
	}
}

void Animation::FlipHorizontal()
{
	for (uint i = 0; i < GetFrames()->size(); i++)
	{
		float x = GetFrames()->at(i)->x;
//		float y = GetFrames()->at(i)->y;
		float w = GetFrames()->at(i)->w;
//		float h = GetFrames()->at(i)->h;
		GetFrames()->at(i)->x = x + w - 1;
		//GetFrames()->at[i]->y = y + h - 1;
		GetFrames()->at(i)->w = 0 - w;
		//GetFrames()->at[i]->h = 0 - h;
	}
	CalcTexcoords();
}

