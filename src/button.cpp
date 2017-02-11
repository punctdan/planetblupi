// Button.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include "gettext.h"
#include "def.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "button.h"
#include "misc.h"
#include "event.h"


/////////////////////////////////////////////////////////////////////////////

// Constructeur.

CButton::CButton()
{
	m_type            = 0;
	m_bEnable         = true;
	m_bHide           = false;
	m_state           = 0;
	m_mouseState      = 0;
	m_nbMenu          = 0;
	m_nbToolTips      = 0;
	m_selMenu         = 0;
	m_bMouseDown      = false;
	m_bMinimizeRedraw = false;
	m_bRedraw         = false;
}

// Destructeur.

CButton::~CButton()
{
}


// Cr�e un nouveau bouton.

bool CButton::Create(CPixmap *pPixmap, CSound *pSound,
					 POINT pos, int type, bool bMinimizeRedraw,
					 int *pMenu, int nbMenu,
					 const char **pToolTips,
					 int region, unsigned int message)
{
	POINT		iconDim;
	int			i, icon;

	static int ttypes[] =
	{
		DIMBUTTONX,DIMBUTTONY,		// button00.bmp
	};

	if ( type < 0 || type > 0 )  return false;

	iconDim.x  = ttypes[type*2+0];
	iconDim.y  = ttypes[type*2+1];

	m_pPixmap         = pPixmap;
	m_pSound          = pSound;
	m_type            = type;
	m_bMinimizeRedraw = bMinimizeRedraw;
	m_bEnable         = true;
	m_bHide           = false;
	m_message         = message;
	m_pos             = pos;
	m_dim             = iconDim;
	m_nbMenu          = nbMenu;
	m_selMenu         = 0;
	m_state           = 0;
	m_mouseState      = 0;
	m_bMouseDown      = false;
	m_bRedraw         = true;

	m_nbToolTips = 0;
	while (pToolTips[m_nbToolTips])
		++m_nbToolTips;

	for ( i=0 ; i<nbMenu ; i++ )
	{
		icon = pMenu[i];

		if ( region == 1 )  // palmiers ?
		{
			if ( icon ==  0 )  icon = 90;  // sol normal
			if ( icon ==  1 )  icon = 91;  // sol inflammable
			if ( icon ==  2 )  icon = 92;  // sol inculte
			if ( icon ==  7 )  icon =  9;  // plante
			if ( icon ==  8 )  icon = 10;  // arbre
		}

		if ( region == 2 )  // hiver ?
		{
			if ( icon ==  0 )  icon = 96;  // sol normal
			if ( icon ==  1 )  icon = 97;  // sol inflammable
			if ( icon ==  2 )  icon = 98;  // sol inculte
			if ( icon ==  8 )  icon = 99;  // arbre
		}

		if ( region == 3 )  // sapin ?
		{
			if ( icon ==  0 )  icon = 102;  // sol normal
			if ( icon ==  1 )  icon = 103;  // sol inflammable
			if ( icon ==  2 )  icon = 104;  // sol inculte
			if ( icon ==  8 )  icon = 105;  // arbre
		}

		m_iconMenu[i] = icon;
	}

	m_toolTips = pToolTips;

	return true;
}

// Dessine un bouton dans son �tat.

void CButton::Draw()
{
	int			i;
	POINT		pos;
	RECT		rect;

	if ( m_bMinimizeRedraw && !m_bRedraw )  return;
	m_bRedraw = false;

	if ( m_bHide )  // bouton cach� ?
	{
		rect.left   = m_pos.x;
		rect.right  = m_pos.x+m_dim.x;
		rect.top    = m_pos.y;
		rect.bottom = m_pos.y+m_dim.y;
		m_pPixmap->DrawPart(-1, CHBACK, m_pos, rect, 1);  // dessine le fond
		return;
	}

	if ( m_bEnable )  // bouton actif ?
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, m_mouseState, m_pos);
	}
	else
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, 4, m_pos);
	}

	if ( m_nbMenu == 0 )  return;

	pos = m_pos;
	if ( m_nbMenu > 0 )
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type,
							m_iconMenu[m_selMenu]+6, pos);
	}

	if ( m_nbMenu == 1 || !m_bEnable || !m_bMouseDown )  return;

	pos = m_pos;
	pos.x += m_dim.x+2;
	for ( i=0 ; i<m_nbMenu ; i++ )
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, i==m_selMenu?1:0, pos);
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, m_iconMenu[i]+6, pos);
		pos.x += m_dim.x-1;
	}
}

void CButton::Redraw()
{
	m_bRedraw = true;
}

int CButton::GetState()
{
	return m_state;
}

void CButton::SetState(int state)
{
	if ( m_state      != state ||
		 m_mouseState != state )
	{
		m_bRedraw = true;
	}

	m_state      = state;
	m_mouseState = state;
}

int CButton::GetMenu()
{
	return m_selMenu;
}

void CButton::SetMenu(int menu)
{
	if ( m_selMenu != menu )
	{
		m_bRedraw = true;
	}

	m_selMenu = menu;
}


bool CButton::GetEnable()
{
	return m_bEnable;
}

void CButton::SetEnable(bool bEnable)
{
	if ( m_bEnable != bEnable )
	{
		m_bRedraw = true;
	}

	m_bEnable = bEnable;
}


bool CButton::GetHide()
{
	return m_bHide;
}

void CButton::SetHide(bool bHide)
{
	if ( m_bHide != bHide )
	{
		m_bRedraw = true;
	}

	m_bHide = bHide;
}


// Traitement d'un �v�nement.

bool CButton::TreatEvent(const SDL_Event &event)
{
	POINT		pos;

	if ( m_bHide || !m_bEnable )  return false;

	//pos = ConvLongToPos(lParam);

    switch (event.type)
    {
	case SDL_MOUSEBUTTONDOWN:
		if (   event.button.button != SDL_BUTTON_LEFT
			&& event.button.button != SDL_BUTTON_RIGHT)
			break;

		pos.x = event.button.x;
		pos.y = event.button.y;
			if ( MouseDown(pos) )  return true;
			break;

	case SDL_MOUSEMOTION:
		pos.x = event.motion.x;
		pos.y = event.motion.y;
			if ( MouseMove(pos) )  return true;
			break;

	case SDL_MOUSEBUTTONUP:
		if (   event.button.button != SDL_BUTTON_LEFT
			&& event.button.button != SDL_BUTTON_RIGHT)
			break;

		pos.x = event.button.x;
		pos.y = event.button.y;
			if ( MouseUp(pos) )  return false;  // (*)
			break;
	}

	return false;
}

// (*) Tous les boutons doivent recevoir l'�v�nement BUTTONUP !


// Indique si la souris est sur ce bouton.

bool CButton::MouseOnButton(POINT pos)
{
	return Detect(pos);
}


// Retourne le tooltips pour un bouton, en fonction
// de la position de la souris.

const char *CButton::GetToolTips(POINT pos)
{
	int		width = m_dim.x;
	int		rank;

	if (m_bHide || !m_bEnable)
		return nullptr;

	if ( m_nbMenu > 1 && m_bMouseDown )  // sous-menu d�roul� ?
	{
		width += 2+(m_dim.x-1)*m_nbMenu;
	}

	if (   pos.x < m_pos.x
		|| pos.x > m_pos.x+width
		|| pos.y < m_pos.y
		|| pos.y > m_pos.y+m_dim.y)
		return nullptr;

	rank = (pos.x-(m_pos.x+2+1))/(m_dim.x-1);
	if ( rank < 0 )  rank = 0;
	if (rank > m_nbToolTips)
		return nullptr;

	if ( m_nbMenu > 1 )
	{
		if ( m_bMouseDown && rank > 0 )
		{
			rank --;
		}
		else
		{
			rank = m_selMenu;
		}
	}

	return gettext (m_toolTips[rank]);
}


// D�tecte si la souris est dans le bouton.

bool CButton::Detect(POINT pos)
{
	int		width = m_dim.x;

	if ( m_bHide || !m_bEnable )  return false;

	if ( m_nbMenu > 1 && m_bMouseDown )  // sous-menu d�roul� ?
	{
		width += 2+(m_dim.x-1)*m_nbMenu;
	}

	if ( pos.x < m_pos.x         ||
		 pos.x > m_pos.x+width   ||
		 pos.y < m_pos.y         ||
		 pos.y > m_pos.y+m_dim.y )  return false;

	return true;
}

// Bouton de la souris press�.

bool CButton::MouseDown(POINT pos)
{
	if ( !Detect(pos) )  return false;

	m_mouseState = 1;
	m_bMouseDown = true;
	m_bRedraw    = true;
	CEvent::PushUserEvent (WM_UPDATE);

	m_pSound->PlayImage(SOUND_CLICK, pos);
	return true;
}

// Souris d�plac�s.

bool CButton::MouseMove(POINT pos)
{
	bool	bDetect;
	int		iState, iMenu;

	iState = m_mouseState;
	iMenu  = m_selMenu;

	bDetect = Detect(pos);

	if ( m_bMouseDown )
	{
		if ( bDetect )  m_mouseState = 1;  // press�
		else            m_mouseState = m_state;
	}
	else
	{
		if ( bDetect )  m_mouseState = m_state+2;  // survoll�
		else            m_mouseState = m_state;
	}

	if ( m_nbMenu > 1 &&
		 m_bMouseDown &&
		 pos.x > m_pos.x+m_dim.x+2 )  // dans sous-menu ?
	{
		m_selMenu = (pos.x-(m_pos.x+m_dim.x+2))/(m_dim.x-1);
		if ( m_selMenu >= m_nbMenu )
		{
			m_selMenu = m_nbMenu-1;
		}
	}

	if ( iState != m_mouseState ||
		 iMenu  != m_selMenu    )
	{
		m_bRedraw = true;
		CEvent::PushUserEvent (WM_UPDATE);
	}

	return m_bMouseDown;
}

// Bouton de la souris rel�ch�.

bool CButton::MouseUp(POINT pos)
{
	bool	bDetect;

	bDetect = Detect(pos);

	m_mouseState = m_state;
	m_bMouseDown = false;
	m_bRedraw    = true;

	if ( !bDetect )  return false;

	if ( m_message != -1 )
	{
		CEvent::PushUserEvent (m_message);
	}

	return true;
}

