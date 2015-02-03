#include "stdafx.h"

#include "Sprite.h"
#include "math.h"
#include "Touches.h"

using namespace std;

#ifdef _DEBUG
//extern Sprite* vertex1;
//extern Sprite* vertex2;
//extern Sprite* vertex3;
//extern Sprite* vertex4;
#endif

int serial = 0;
void Sprite::Init(Texture* tex, float x, float y, float width, float height)
{
	singleanim = new Animation(tex, &last_main_timer);
	GetAnim()->SetTexture(tex);

	m_serial = ++serial;
	tex_x = 0;
	tex_y = 0;
	tex_w = 0;
	tex_h = 0;
	alignment = AlignCenter;
	angle.setValue(0.0f);
	last_w = -1.0f;
	last_h = -1.0f;
	m_bVisible = true;
	m_bDeleteWhenAnimDone = false;
	m_bDeleteWhenMoveDone = false;
	m_bDeleteWhenFadeDone = false;
	m_bRemoveAndDelete = false;
	m_background = NULL;
	for (int i = 0; i < DECAL_MAX; i++) m_decal_list[i] = NULL;
	SrcBlendMode = GL_SRC_ALPHA;
	DstBlendMode = GL_ONE_MINUS_SRC_ALPHA;

	m_bIntegerPixelAlign = false;

	SetCoord(x, y);
	SetSize(width, height);
}
void Sprite::SetTimer( td_timer* use_timer )
{
	singleanim->SetTimer(use_timer);
	Timer::SetTimer(use_timer);
	GetPos()->xa.SetTimer(use_timer);
	GetPos()->ya.SetTimer(use_timer);
	GetPos()->za.SetTimer(use_timer);
	GetSize()->m_w.SetTimer(use_timer);
	GetSize()->m_h.SetTimer(use_timer);
	fade.r.SetTimer(use_timer);
	fade.g.SetTimer(use_timer);
	fade.b.SetTimer(use_timer);
	fade.a.SetTimer(use_timer);
	angle.SetTimer(use_timer);
}

Sprite::Sprite(TextureNumber tex)
{
	Texture* t = TextureHandler::OpenTexture(tex);
	Init(t, 0, 0, (float)t->grid_x, (float)t->grid_y);
}
Sprite::Sprite(TextureNumber tex, float width, float height)
{
	Texture* t = TextureHandler::OpenTexture(tex);
	Init(t, 0, 0, width, height);
}
Sprite::Sprite(TextureNumber tex, float x_pos, float y_pos, float z_pos, float width, float height)
{
	Texture* t = TextureHandler::OpenTexture(tex);
	Init(t, x_pos, y_pos, width, height);
	GetPos()->setZ(z_pos);
}

Sprite::Sprite( Texture* attach_to )
{
	Init(attach_to, 0, 0, 2, 2);
}

Sprite::~Sprite(void)
{
	delete m_background;
	delete singleanim;
	for (int i = 0; i < DECAL_MAX; i++)
		delete m_decal_list[i];
	//TextureHandler::CloseTexture(anim.GetTexture());
}

void Sprite::RenderAt( float x, float y, float z /*= -9999*/ )
{
	if (z == -9999) z = GetPos()->getZ();
	bool visstate = IsVisible();
	Visible(true);
	RenderStacked(x, y, z);
	Visible(visstate);
}
void Sprite::Render()
{
	RenderStacked(m_pos.getX(), m_pos.getY(), m_pos.getZ());
}
void Sprite::RenderStacked(float x, float y, float z)
{
	if (last_w != size.getW() || last_h != size.getH()) 	CalcSize();

	if (IsVisible())
	{
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		glLoadIdentity();

		if (m_bIntegerPixelAlign)
			glTranslatef(floor(.5f + x), floor(.5f + y), z);
		else
			glTranslatef(x, y, z);

		if (angle.getValue() != 0.0f)
		{
			glRotatef(angle.getValue(), 0.0f, 0.0f, 1.0f);
		}
		if (m_background != NULL)
		{
			if (m_background->IsVisible())
			{
				glPushMatrix();
				if (m_bIntegerPixelAlign)
					glTranslatef(floor(.5f + m_background->GetPos()->getX()), floor(.5f + m_background->GetPos()->getY()), m_background->GetPos()->getZ());
				else
					glTranslatef(m_background->GetPos()->getX(), m_background->GetPos()->getY(), m_background->GetPos()->getZ());
				float angleval2 = m_background->angle.getValue();
				if (angleval2 != 0.0f)
				{
					glRotatef(angleval2, 0.0f, 0.0f, 1.0f);
				}
				m_background->DrawVertexes();
				glPopMatrix();
			}
		}

		if (SrcBlendMode != GL_SRC_ALPHA || DstBlendMode != GL_ONE_MINUS_SRC_ALPHA )
		{
			glBlendFunc(SrcBlendMode, DstBlendMode);
		}
		DrawVertexes();
		if (SrcBlendMode != GL_SRC_ALPHA || DstBlendMode != GL_ONE_MINUS_SRC_ALPHA )
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		for (int i = 0; i < DECAL_MAX; i++)
		{
			Sprite* decal = m_decal_list[i];
			if (decal != NULL)
			{
				if (decal->IsVisible())
				{
					glPushMatrix();
					if (m_bIntegerPixelAlign)
						glTranslatef(floor(decal->GetPos()->getX()+.5f), floor(decal->GetPos()->getY()+.5f), decal->GetPos()->getZ());
					else
						glTranslatef(decal->GetPos()->getX(), decal->GetPos()->getY(), decal->GetPos()->getZ());
						
					float angleval2 = decal->angle.getValue();
					if (angleval2 != 0.0f)
					{
						glRotatef(angleval2, 0.0f, 0.0f, 1.0f);
					}
					decal->DrawVertexes();
					glPopMatrix();
				}
			}
		}
		glPopMatrix();
	}
}

void Sprite::DrawVertexes()
{
	GetAnim()->Render();
	glVertexPointer(2, GL_FLOAT, 0, spriteVertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	glColor4f(fade.r.getValue(), fade.g.getValue(), fade.b.getValue(), fade.a.getValue());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


void Sprite::CalcSize()
{
	float real_x = 0;
	float real_y = 0;
	if (alignment & AlignE) real_x = size.getW();
	if (alignment & AlignN) real_y = size.getH();
	if (alignment == AlignCenter || alignment == AlignN || alignment == AlignS) real_x = (size.getW() / 2.0f);
	if (alignment == AlignCenter || alignment == AlignE || alignment == AlignW) real_y = (size.getH() / 2.0f);

	spriteVertices[0] = 0.0f - real_x;
	spriteVertices[1] = 0.0f - real_y;

	spriteVertices[2] = (size.getW()) - real_x ;
	spriteVertices[3] = spriteVertices[1];

	spriteVertices[4] = spriteVertices[0];
	spriteVertices[5] = (size.getH()) - real_y ;

	spriteVertices[6] = spriteVertices[2];
	spriteVertices[7] = spriteVertices[5];
	last_w = size.getW();
	last_h = size.getH();
}
void Sprite::SetCachedZ(bool IsStacked)
{
	if (IsStacked)
	{
		m_cached_z = 0-(GetPos()->getY() + spriteVertices[1] + (GetPos()->getZ() * .1f)); // bigger the Y, the further back it is
	}
	else
		m_cached_z = GetPos()->getZ();
}

bool PolyHitTest( float testx, float testy, float* xarray, float* yarray, uint count )
{
	uint i, j;
	int c = 0;
	for (i = 0, j = count - 1; i < count; j = i++) 
	{
		if ( ((yarray[i] > testy) != (yarray[j] > testy)) &&
			(testx < (xarray[j]-xarray[i]) * (testy-yarray[i]) / (yarray[j]-yarray[i]) + xarray[i]) )
			c = !c;
	}
	if ( c != 0) return true;
	return false;
}

bool Sprite::HitTest(int x, int y, int grow_x, int grow_y )
{
	if (!IsVisible()) return false;
	if (fade.a.getValue() == 0) return false;
	float ang = angle.getValue();
	//	for (uint i = 0; i < m_text.length(); i++)
	//	{
	for ( ; ang < 0; ang+=360 ) ;
	for ( ; ang > 360; ang-=360 ) ;

	float xx1 = min(spriteVertices[0], spriteVertices[6]);
	float yy1 = min(spriteVertices[1], spriteVertices[7]);
	float xx2 = max(spriteVertices[0], spriteVertices[6]);
	float yy2 = max(spriteVertices[1], spriteVertices[7]);
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

	bool c = PolyHitTest(testx, testy, vertx, verty, 4);

	if ( c ) 
	{
#ifdef _DEBUG
		//vertex1->SetCoord(nx1, ny1);
		//vertex2->SetCoord(nx2, ny2);
		//vertex3->SetCoord(nx3, ny3);
		//vertex4->SetCoord(nx4, ny4);
#endif
		return true;
	}
	return false;

}

bool Sprite::SyncState( MemFile* mf )
{
	Serialize::SyncState(mf);
	if (!SyncHeader('TRPS')) return false;
	//int dummyserial = 0;
	//SyncInt(dummyserial);
	SyncBool(m_bDeleteWhenAnimDone);
	SyncBool(m_bDeleteWhenMoveDone);
	SyncBool(m_bDeleteWhenFadeDone);
	SyncBool(m_bRemoveAndDelete);
	SyncBool(m_bIntegerPixelAlign);
	SyncBool(m_bVisible);
	SyncBool(delete_texture);

	SyncUInt(SrcBlendMode);
	SyncUInt(DstBlendMode);

	SyncTarget(m_pos.xa);
	SyncTarget(m_pos.ya);
	SyncTarget(m_pos.za);
	SyncTarget(angle);
	SyncTarget(size.m_w);
	SyncTarget(size.m_h);
	SyncTarget(fade.a);
	SyncTarget(fade.r);
	SyncTarget(fade.g);
	SyncTarget(fade.b);

	SyncFloat(last_w);
	SyncFloat(last_h);
	SyncFloat(m_cached_z);

	int a = alignment;
	SyncInt(a);
	alignment = (SpriteAlignment)a;

	CalcSize();

	return TRUE;
}

bool Sprite::AddDecal( Sprite* s )
{
	for (int i = 0; i < DECAL_MAX; i++)
	{
		if (m_decal_list[i] == NULL)
		{
			m_decal_list[i] = s;
			return true;
		}
	}
	return false;
}


int Sprite::DecalCount()
{
	int ret = 0;
	for (int i = 0; i < DECAL_MAX; i++)
	{
		if (m_decal_list[i] != NULL) ret++;
	}
	return ret;
}

void Sprite::RemoveDecal( Sprite* s )
{
	for (int i = 0; i < DECAL_MAX; i++)
	{
		if (m_decal_list[i] == s)
		{
			m_decal_list[i] = NULL;
		}
	}
}

Sprite* Sprite::Decal( int pos )
{
	if (pos < 0 || pos >= DECAL_MAX) return NULL;
	return m_decal_list[pos];
}

SpriteList::SpriteList()
{
	sort_flag = false;
	YStacked = false;
};
void SpriteList::SetTimer( td_timer* use_timer )
{
	Timer::SetTimer(use_timer);
	list <Sprite*>::iterator c1_Iter;
	for ( c1_Iter = begin(); c1_Iter != end(); c1_Iter++ )
	{
		(*c1_Iter)->SetTimer(use_timer);
	}
}
SpriteList::~SpriteList()
{
	RemoveAllAndDelete();
};
void SpriteList::Add(Sprite* s)
{
	s->SetTimer(m_timer);
	//size_t t = size();
	list <Sprite*>::iterator c1_Iter;
	//if (!YStacked)
	//{
		for ( c1_Iter = begin(); c1_Iter != end(); c1_Iter++ )
		{
			if ((*c1_Iter)->m_pos.getZ() == s->m_pos.getZ() && (*c1_Iter) != s )
			{
				s->m_pos.setZ(s->m_pos.getZ() + 0.1f);
				c1_Iter = begin();
			}
		}
	//}
	push_back(s);
	sort_flag = true;
};
bool OrderByCached(Sprite* s1, Sprite* s2)
{
	float z1 = s1->GetCachedZ();
	float z2 = s2->GetCachedZ();
	if (floor(z1*1000.0f) == floor(z2*1000.0f)) 
		return s1->m_serial < s2->m_serial;
	return  z1 < z2;
}
bool OrderByCachedReverse(Sprite* s1, Sprite* s2)
{
	float z1 = s1->GetCachedZ();
	float z2 = s2->GetCachedZ();
	if (floor(z1*1000.0f) == floor(z2*1000.0f)) 
		return s1->m_serial < s2->m_serial;
	return  z1 < z2;
}

void SpriteList::Sort()
{
	list <Sprite*>::iterator c1_Iter;
	for ( c1_Iter = begin(); c1_Iter != end(); c1_Iter++ )
	{
		(*c1_Iter)->SetCachedZ(YStacked);
	}
	if (YStacked)
		sort(OrderByCachedReverse);
	else
		sort(OrderByCached);
	sort_flag = false;
};
void SpriteList::Render()
{
	if (size() == 0) return;
	list <Sprite*>::iterator c1;

	bool repeat = false;
	do 
	{
		repeat = false;
		for ( c1 = begin(); c1 != end(); c1++ )
		{
			Sprite* s = *c1;
			if (s->m_bRemoveAndDelete)
			{
				remove(s);
				delete s;
				repeat = true;
				break;
			}
			if (s->m_bDeleteWhenAnimDone)
			{
				if (s->GetAnim()->AnimEnded)
				{
					remove(s);
					delete s;
					repeat = true;
					break;
				}
			}
			if (s->m_bDeleteWhenMoveDone)
			{
				if (s->m_pos.xa.Ended() && s->m_pos.ya.Ended())
				{
					remove(s);
					delete s;
					repeat = true;
					break;
				}
			}
			else
				if (s->m_bDeleteWhenFadeDone)
				{
					if (s->fade.a.Ended() && s->fade.r.Ended() && s->fade.g.Ended() && s->fade.b.Ended())
					{
						remove(s);
						delete s;
						repeat = true;
						break;
					}
				}
		}

	} while(repeat);

	if (size() == 0) return;
	Sort();
	c1 = begin();
	float z = (*c1)->m_pos.getZ();
	for ( c1 = begin(); c1 != end(); c1++ )
	{
		Sprite* s = *c1;
		if (YStacked)
			s->RenderStacked(s->GetPos()->getX(), s->GetPos()->getY(), z);
		else
			s->Render();
		z += .1f;
	}

};

Sprite* SpriteList::GetAt(uint pos)
{
	uint at_pos = 0;
	list <Sprite*>::iterator c1_Iter;
	for ( c1_Iter = begin(); c1_Iter != end(); c1_Iter++ )
	{
		if (at_pos == pos) return *c1_Iter;
		at_pos++;
	}
	return NULL;
}

void SpriteList::RemoveAndDeleteDelayed( Sprite* s )
{
	s->m_bRemoveAndDelete = true;
}

void SpriteList::RemoveAndDeleteImmediate( Sprite* s )
{
	remove(s);
	delete s;
}

void SpriteList::RemoveAllAndDelete()
{
	list <Sprite*>::iterator c1_Iter;
	for (; size() > 0; )
	{
		c1_Iter = begin();
		Sprite* del = *c1_Iter;
		remove(del);
		delete del;
	}
}

bool SpriteList::Has( Sprite* s )
{
	list <Sprite*>::iterator c1_Iter;
	for ( c1_Iter = begin(); c1_Iter != end(); c1_Iter++ )
	{
		if (*c1_Iter == s) return true;
	}
	return false;
}

//void Sprite::AnimateAngle(float new_angle, uint time_to_get_there, TargetType easetype /* = Linear */)
//{
//	angle.Animate(angle.Value, new_angle, time_to_get_there, easetype); 
//};
//
//void Sprite::SetAngle(float new_angle, float angle_speed)
//{
//	angle.Move(new_angle, angle_speed); 
//};
//
//void Sprite::SetFade(float fade_value, float fade_speed)
//{
//	fade.Move(fade_value, fade_speed); 
//};
//void Sprite::AnimateFade(float fade_value, uint time_to_get_there, TargetType easetype )
//{
//	fade.Animate(fade.Value, fade_value, time_to_get_there, easetype); 
//};
