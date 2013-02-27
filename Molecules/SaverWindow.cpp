#include "StdAfx.h"
#include "resource.h"
#include "SaverWindow.h"
#include "TextureLoader.h"
#include <math.h>
#include <memory>

#ifdef DEBUG
#define DEBUGDISPLAY
#endif

#define TICK() ::GetTickCount()

CSaverWindow::CSaverWindow() :

	m_bHaveCoord(false),
	m_pMolecule(NULL),
	m_fZoom(-100.0f),
	m_nFpsCount(0),
	m_nFps(0),
	m_dLastMove(0),
	m_dFrameTime(0), 
	m_bShowDesc(TRUE),
	m_bScreenTooSmall(FALSE),
	m_State(opZoomIn),
	m_fFloorPos(0.0f)
{ }


HWND CSaverWindow::Create(HWND hwndParent, BOOL bExitOnTouch, BOOL preview, LPRECT lprc)
{
	m_bTouchExit = bExitOnTouch;
	m_bPreview = preview;
	RECT rc;
	DWORD dwStyle = WS_CHILD;
	DWORD dwExStyle = WS_EX_TOOLWINDOW;

	if(!hwndParent)
	{
		dwStyle = WS_POPUP;
		dwExStyle = WS_EX_TOPMOST;
		if(!lprc)
		{
			hwndParent = GetDesktopWindow();
			::GetWindowRect(hwndParent, &rc);
			// to likwiduje problem z moja karta RadeonX600, ktora
			// cos robi aplikacjom otwierajacym okna 3d wielkosci (dokladnie) ekranu
			// dziwne, ale tylko ten wygaszacz zle dziala, inne chodza normalnie
			rc.bottom += 1;
			::ScreenToClientRect(hwndParent, &rc);
#ifdef DEBUGDISPLAY
			/*int hw = (rc.right-rc.left)/2;
			int hh = (rc.bottom-rc.top)/2;
			rc.bottom /= 2;
			rc.right /= 2;
			int move_left = hw-((rc.right-rc.left)/2);
			int move_down = hh-((rc.bottom-rc.top)/2); 
			rc.left += move_left;
			rc.right += move_left;
			rc.top += move_down;
			rc.bottom += move_down;*/
			rc.left = rc.right - (rc.right - rc.left)/3;
			rc.bottom = (rc.bottom - rc.top)/3;
#endif
		}
		else
		{
			rc = *lprc;
		}
	}
	else
	{
		::GetWindowRect(hwndParent, &rc);
		::ScreenToClientRect(hwndParent, &rc);
	}
	HWND handle = CWindowImpl<CSaverWindow>::Create(hwndParent, rc, _T("Molecules"), dwStyle, dwExStyle);
	return handle;
}

LRESULT CSaverWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	LoadPreferences();
	// we can override some of these settings in the OnInit handler
	m_bShowDesc = m_Settings.bShowDesc;
	m_bShowFps = m_Settings.bShowFPS;
	bHandled = FALSE;	// pass msg to the base class (chained)
	return 0;
}

LRESULT CSaverWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  ::glDeleteLists(m_textureFloor, 1);
	CGLDrawHelper::FreeFonts();
	::PostQuitMessage(0);
	return 0;
}

LRESULT CSaverWindow::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(this->m_bTouchExit)
	{	
		SetCursor(NULL);
		return true;
	}
	return false;
}

LRESULT CSaverWindow::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(this->m_bTouchExit)
	{
		PostMessage(WM_CLOSE, 0, 0);
		bHandled = true;
	}
	else
		bHandled = false;
	return 0;
}

LRESULT CSaverWindow::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bTouchExit)
	{
		if(m_bHaveCoord && m_paramCoord != lParam)
		{
			PostMessage(WM_CLOSE, 0, 0);
			bHandled = true;
		}
		else
		{
			m_paramCoord = lParam;
			m_bHaveCoord = true;
		}
	}
	else
		bHandled = false;
	return 0;
}


//BOOL CSaverWindow::OnIdle()
//{
//	RedrawWindow();
//	return TRUE;
//}

void CSaverWindow::OnInit()
{
	/*CMessageLoop *loop = _Module.GetMessageLoop();
	loop->AddIdleHandler(this);*/
	glClearColor(0.000f, 0.000f, 0.000f, 1.0f); //Background color
	// Activate lighting and a light source
	glMatrixMode(GL_MODELVIEW);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// TODO: dynamic light calculation should be done here
	static GLfloat pos[4] = {-1.0f, 0.4f, 0.9f, 0.0f}; 
	static GLfloat amb[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  static GLfloat dif[4] = {0.5f, 0.5f, 0.5f, 1.0f};
  static GLfloat spc[4] = {0.3f, 0.3f, 0.3f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
  //glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

  ATLVERIFY(CTextureLoader::LoadTextureToList(MAKEINTRESOURCE(IDB_WATER), &m_textureFloor));

	// font display list
	HFONT newFont = CreateFont(-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			ANSI_CHARSET, 0, 0,
			ANTIALIASED_QUALITY, 0, _T("Courier New"));
	
	if(!newFont)
		MessageBox(_T("Cannot create font!"), _T("Error"), MB_OK|MB_ICONERROR);
	else
	{
		// create font list for opengl and pass it to draw helper
		CDC dc(this->GetDC());
		HFONT oldfont = dc.SelectFont(newFont);
		TEXTMETRIC tm;
		BOOL ok = GetTextMetrics(dc.m_hDC, &tm);
		if(ok)
			m_lTextHeight = tm.tmHeight;
		else
			m_lTextHeight = 0;
		RECT rc;
		GetClientRect(&rc);
		if(rc.right-rc.left<m_lTextHeight*10)
			m_bScreenTooSmall = TRUE;

		font_base = 1000; // ot tak
		wglUseFontBitmaps(dc.m_hDC, 0, 255, font_base);
		//wglUseFontBitmaps(dc.m_hDC, 32, 96, font_base);
		CGLDrawHelper::InitFonts(font_base);
		dc.SelectFont(oldfont);
	}
	srand(TICK());
	m_MManager.LoadList();
	LoadMolecule();
	return;
}

BOOL
CSaverWindow::LoadMolecule()
{
	CFile file;
	std::unique_ptr<TCHAR> dir(::_tgetcwd(NULL, 0));
	BOOL ok = FALSE;
	CMoleculeBuilder builder;
	m_bError = FALSE;
	
	// free existing molecule if present
	if(m_pMolecule)
	{
		delete m_pMolecule;
		m_pMolecule = NULL;
	}
	
	// select new molecule to load
	LPCTSTR mol = m_MManager.GetRandomMolecule();
	if (mol)
		m_pMolecule = builder.LoadFromFile(mol);
	if(m_pMolecule == NULL)
	{
		// prepare for drawing error screen
		this->m_csErrorText.Format(_T("Can't load molecule!\nDirectory: %s\n")
			_T("This directory should contain folder named 'molecules' with *.pdb files"), dir);
		m_bError = TRUE;
		m_State = opRender;
	}
	else
	{
		// prepare for drawing molecule
		m_State = opZoomIn;
		m_pMolecule->EnableLinks(m_Settings.bShowLinks);
		m_pMolecule->EnableLabels(m_Settings.bShowLabels && !m_bPreview);
		m_pMolecule->EnableWire(m_Settings.bWire);
		if(m_Settings.bAnimateBuild)
			m_pMolecule->InitImplosion(2.0f);
		if(m_Settings.bTeleType)
			m_Blender.SetString(m_pMolecule->GetDescription(), 5);
		ok = TRUE;
	}
	return ok;
}

#define TIMER_FPS 666
#define TIMER_MOVE 69

BOOL
CSaverWindow::RunSaver()
{
	if(m_Settings.dRunTime !=0)
		m_StopTime = TICK() + m_Settings.dRunTime * 60000;
	else
		m_StopTime = 0;

	m_Twister.Init(m_Settings.fMaxSpeed);
	BOOL ret = TRUE;
  if(m_pMolecule)
	{
		SetTimer(TIMER_FPS, 1000);
		//SetTimer(TIMER_MOVE, m_Settings.dMoveDelay);
		SetTimer(TIMER_MOVE, 1);
	}
	else
	{
		SetTimer(TIMER_MOVE, m_Settings.dFrameDelay);
		ret = FALSE;
	}

  if (m_Settings.bReflection)
    m_fZoom = 0.0f;
	m_dLastMove = TICK();
  RedrawWindow();
  return ret;
}

LRESULT CSaverWindow::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == TIMER_FPS)
	{
		// blanks screen after specified amount of time
		if(m_bTouchExit && m_StopTime && TICK() >= m_StopTime)
		{
			KillTimer(TIMER_FPS);
			KillTimer(TIMER_MOVE);
			m_State = opBlank;
			if(m_pMolecule)
			{
				delete m_pMolecule;
				m_pMolecule = NULL;
			}
			//m_Twister.DoFreeRotation();
			//m_dLastMove= TICK();
			RedrawWindow();
			return 0;
		}
		m_nFps = m_nFpsCount;
		m_nFpsCount = 0;
	}
	//else if(wParam == TIMER_LOAD && m_State == opRender)
	//{
	//	m_State = opZoomOut;
	//	if(m_Settings.bAnimateBuild && m_pMolecule)
	//		m_pMolecule->InitExplosion(1.0f);
	//}
	else if(wParam == TIMER_MOVE)
	{
		DWORD tick = TICK();
		DWORD delta = tick - m_dLastMove;
		m_dLastMove = tick;
		if (delta == 0) delta = 1;
		GLfloat dsec = (GLfloat)delta/1000.0f;
		
		if (m_pMolecule && m_State == opRender && tick - m_dShowTimeStart >= m_Settings.dDisplayTime)
		{
			m_State = opZoomOut;
			if(m_Settings.bAnimateBuild && m_pMolecule)
				m_pMolecule->InitExplosion(2.0f);
		}
		m_Twister.DoFreeRotation(delta);
		UpdateFloor(delta);
		
		if (TICK() - m_dLastFrameDrawn >= m_Settings.dFrameDelay)
		{
			m_dLastFrameDrawn = TICK();
			RedrawWindow();
		}

		if (!m_pMolecule) return 0;

		const GLfloat z_rate = 10.0f * dsec,
							zo_rate = 30.0f * dsec;
		GLfloat zoom_distance = (GLfloat)-10.0f-m_pMolecule->GetMaxDimension(); // xxx
		
		switch(m_State)
		{
		default:
		case opRender:
			break;
		case opRenderBegin:
			m_dShowTimeStart = tick;
			m_State = opRender;
			break;
		case opZoomIn:
			//GLfloat zoom_distance = (GLfloat)-10.0f-m_pMolecule->GetMaxDimension(); // xxx
			if(!m_Settings.bShowLinks)
				zoom_distance -= 10.0f;
      if (m_Settings.bReflection)
      {
        // when reflection is enabled, we only do a zoom correction here
        // because new molecule may have different size
        if (fabs(fabs(m_fZoom) - fabs(zoom_distance)) > 1.0f)
        {
					// was 0.2f
          if (m_fZoom < zoom_distance)
            m_fZoom += z_rate;
          else
            m_fZoom -= z_rate;
        }
				else
					m_fZoom = zoom_distance;
      }
      else
			{
        m_fZoom += z_rate;
				if(m_fZoom > zoom_distance)
					m_fZoom = zoom_distance;
      }
			if(m_Settings.bAnimateBuild )
			{
				// make sure we've reached zoom_distance before state switch
				if(m_pMolecule->DoImpExplode(dsec) && m_fZoom == zoom_distance)
					m_State = opRenderBegin;
			}
			else if(m_fZoom == zoom_distance)
				m_State = opRenderBegin;
			break;
		case opZoomOut:
			if(m_pMolecule == NULL)
				m_fZoom = -10.0f;
			else
			{
        if (m_Settings.bReflection)
        {
          // this is really a zoom-in to the center, it allows atoms to run out of
          // visible area
          BOOL ready = TRUE;
          if (m_fZoom < 0.0f)
            m_fZoom += zo_rate;
          if(m_Settings.bAnimateBuild)
						ready = m_pMolecule->DoImpExplode(dsec) && fabs(m_fZoom) < 3.0f;
          if (ready)
            LoadMolecule();
        }
        else
        {
					m_fZoom -= zo_rate;
					bool ready = FALSE;
					if(m_Settings.bAnimateBuild)
						ready = m_pMolecule->DoImpExplode(dsec);
					if(m_fZoom < -100.0f && ready)
					{
						m_fZoom = -100.0f;
						LoadMolecule();
					}
        }
			}
		}
		m_dFrameTime = TICK() - tick;		
	}
	else
		bHandled = FALSE;
	return 0;
}

void CSaverWindow::UpdateFloor(DWORD delta)
{
	m_fFloorPos += 0.01f * delta;
	if (m_fFloorPos > 360.0f)
		m_fFloorPos = (m_fFloorPos - 360.0f);
}

// TODO: promote floor to separate class
void CSaverWindow::DrawFloor()
{
  glEnable(GL_TEXTURE_2D);
  // draw floor (the mirror)
  GLfloat color[4] = { 0.3f, 0.2f, 0.5f, 0.3f };
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
  //glTranslatef(0.0f, 0.0f, 0.0f);
  GLfloat d = 100.0f;
  //glTranslatef(0.0f, 0.0f, 10.0f*sinf(TICK()));
  glRotatef(m_fFloorPos, 0.0f, 1.0f, 0.0f);
  glCallList(m_textureFloor);
  glBegin(GL_QUADS);
    glNormal3f(0,1.0f,0);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(d, 0.0f ,-d);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, 0.0f, -d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-d, 0.0f, d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(d, 0.0f, d);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void CSaverWindow::OnRender()
{
  BOOL mirror = m_Settings.bReflection;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear buffers
	 // plane clip
  glLoadIdentity(); // Load identity matrix
  if(m_pMolecule)
	{
    //glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
    GLfloat zoom = mirror ? m_fZoom*2.0f : m_fZoom;
    //glTranslatef(0.0f, 0.0f, m_fZoom);
    m_nFpsCount++;
    GLfloat dim = m_pMolecule->GetMaxDimension();
    GLfloat tx, ty, tz;
    m_pMolecule->GetTranslations(tx, ty, tz);
	  double eqr[] = {0.0f, -1.0f, 0.0f, 0.0f};
		GLfloat matrix[16];
		m_Twister.GetRotationMatrix(matrix);
    gluLookAt(-tx, -(ty-3.0f), zoom, 0, -ty, 0, 0, 1, 0);

    if (mirror)
    {
      // draw the floor to the stencil buffer
     /* glColorMask(0, 0, 0, 0);
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, 1, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      glDisable(GL_DEPTH_TEST);
      DrawFloor();
      glEnable(GL_DEPTH_TEST);
      glColorMask(1, 1, 1, 1);
      glStencilFunc(GL_EQUAL, 1, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);*/
     
     //draw reflected molecule (hide when zooming)
     
      glEnable(GL_CLIP_PLANE0);
      glClipPlane(GL_CLIP_PLANE0, eqr);
      glPushMatrix();
        glScalef(1.0f, -1.0, 1.0f); // scale y axis  
        glTranslatef(0.0f, dim/2+0.8f, 0.0f);
				glMultMatrixf(matrix);
	      glTranslatef(tx, ty, tz);
        m_pMolecule->Draw();
      glPopMatrix();
      glDisable(GL_CLIP_PLANE0);
      glDisable(GL_STENCIL_TEST);
   
      // the real floor
      glPushMatrix();
      //glLoadIdentity();
      glEnable(GL_BLEND);
      glDisable(GL_LIGHTING);
        glColor4f(0.2f, 0.4f, 1.0f, 0.7f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        DrawFloor();
      glEnable(GL_LIGHTING);
      glDisable(GL_BLEND);
      glPopMatrix();
    }

		glTranslatef(0.0f, dim/2+0.8f, 0.0f);	// rotate aroud this origin ...
		glMultMatrixf(matrix);
		//glRotatef(x, 1.0f, 0.0f, 0.0f);
		//glRotatef(y, 0.0f, 1.0f, 0.0f);	
		//glRotatef(z, 0.0f, 0.0f, 1.0f);				// with these angles ...
		glTranslatef(tx, ty, tz);							// object's center (translates to the center of molecule)
    
		m_pMolecule->Draw();

		glColor3f(0.1f, 1.0f, 0.2f);
		if(m_bShowDesc && !m_bScreenTooSmall)
		{
			RECT rect;
			this->GetClientRect(&rect);
			CString desc;
			if(!m_bShowFps)
				desc.Format(_T("%s"), m_Settings.bTeleType? m_Blender.DoBlend() : m_pMolecule->GetDescription()); 
			else
			{
				desc.Format(_T("%s\nFPS: %3d FTIME: %3d [ms]"), 
					m_Settings.bTeleType? m_Blender.DoBlend() : m_pMolecule->GetDescription(), m_nFps, m_dFrameTime);
			}
			CGLDrawHelper::DrawString(rect.right, rect.bottom, 0.0f, (GLfloat)(rect.bottom-rect.top)-m_lTextHeight,
				(LPTSTR)(LPCTSTR)desc, m_lTextHeight);
		}   
	}
	else if(m_bError)
	{
		GLfloat matrix[16];
		m_Twister.GetRotationMatrix(matrix);
		RECT rect;
		this->GetClientRect(&rect);
		glColor3f(1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, -20.0f);
		glMultMatrixf(matrix);
		static GLfloat tcs[4] = { 1.0f, 0.2f, 0.4f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tcs);
		CGLDrawHelper::DrawTube(-10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 8, TRUE, TRUE, FALSE);
		CGLDrawHelper::DrawTube(0.0f, -10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 1.0f, 1.0f, 8, TRUE, TRUE, FALSE);
		if(!m_bScreenTooSmall)
			CGLDrawHelper::DrawString(rect.right, rect.bottom,
				0.0f, (GLfloat)(rect.bottom-rect.top)-m_lTextHeight, (LPTSTR)(LPCTSTR)m_csErrorText, m_lTextHeight);
	}
	glFlush();
	return;
}

void CSaverWindow::OnResize(int cx, int cy) 
{
	GLfloat fFovy  = 45.0f; // Field-of-view
	GLfloat fZNear = 1.0f;  // Near clipping plane
	GLfloat fZFar = 200.0f;  // Far clipping plane

	// Calculate viewport aspect
	RECT rc;
	GetClientRect(&rc);

	GLfloat fAspect = (GLfloat)(rc.right-rc.left) / (GLfloat)(rc.bottom-rc.top);

	// Define viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fFovy, fAspect, fZNear, fZFar);
	glViewport(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
	glMatrixMode(GL_MODELVIEW);
}

BOOL
CSaverWindow::LoadPreferences()
{
  m_Settings.Load();
	return TRUE;
}