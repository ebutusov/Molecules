#include "StdAfx.h"
#include "GLDrawHelper.h"
#include "Molecule.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define MAX(a, b) a > b ? a : b;

//#define USE_GLU

void
DrawSphereM(GLfloat x, GLfloat y, GLfloat z, GLfloat diameter, BOOL wireframe, GLfloat scale)
{
	glPushMatrix();
	glTranslatef(x, y, z);
#ifdef USE_GLU
	GLUquadric *q = gluNewQuadric();
	gluQuadricDrawStyle(q, wireframe ? GLU_LINE : GLU_FILL);
	gluSphere(q, diameter, (int)(12*scale), (int)(24*scale));
	gluDeleteQuadric(q);
#else
	glScalef(diameter, diameter, diameter);
	// 12, 24
	CGLDrawHelper::DrawSphere((int)(12*scale),(int)(24*scale), wireframe);
#endif
	glPopMatrix();
}

void
DrawTubeM(GLfloat x1, GLfloat y1, GLfloat z1, 
				 GLfloat x2, GLfloat y2, GLfloat z2,
				 GLfloat diameter, bool wire, GLfloat scale)
{
#ifdef USE_GLU
	GLfloat X = (x2 - x1);
  GLfloat Y = (y2 - y1);
  GLfloat Z = (z2 - z1);

  if (X == 0 && Y == 0 && Z == 0)
    return;

  GLfloat length = sqrt (X*X + Y*Y + Z*Z);
  glPushMatrix();
	glTranslatef(x1, y1, z1);
	glRotatef(atan2(X, Z) * (180/M_PI), 0, 1, 0);
	glRotatef(-atan2(Y, sqrt(X*X+Z*Z)) * (180/M_PI), 1, 0, 0);
	GLUquadric *q = gluNewQuadric();
	gluQuadricDrawStyle(q, wire ? GLU_LINE : GLU_FILL);
	gluCylinder(q, diameter*scale, diameter*scale, length, 12*scale, 24*scale); 
	gluDeleteQuadric(q);
	glPopMatrix();
#else
	glPushMatrix();
	CGLDrawHelper::DrawTube(x1, y1, z1, x2, y2, z2, diameter*scale, 0.0f, 30, TRUE, FALSE, wire);
	glPopMatrix();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
// MOLECULE IMPLEMENTATION
////////////////////////////////////////////////////////////////////////////////////////


#define FOREACH_ATOM(x) {\
	POSITION pos = m_Atoms.GetStartPosition(); \
	AtomPtr x; \
	while(pos != NULL) \
	{ \
  x = m_Atoms.GetNextValue(pos);

#define END_FOREACH_ATOM() } }


GLfloat CMolecule::LINKCOLOR[4] = { 0.3f, 0.3f, 0.2f, 1.0f };

CMolecule::CMolecule(void)
{
	m_bDrawLinks = FALSE;
	m_bWireMode = FALSE;
	m_Description = NULL;
	m_DrawMode = dmNormal;
	m_dl = 0;
	m_bFromDL = FALSE;
}

CMolecule::~CMolecule(void)
{
	POSITION pos = m_Atoms.GetStartPosition();
	int num;
	AtomPtr atom;
	while(pos != NULL)
	{
		num = m_Atoms.GetKeyAt(pos);
		atom = m_Atoms.GetNextValue(pos);
		delete atom;
	}
	m_Atoms.RemoveAll();
	
	for(unsigned int i=0;i<m_AtomLinks.GetCount();i++)
	{
		ATOMLINKPTR link = m_AtomLinks.GetAt(i);
		delete link;
	}
	m_AtomLinks.RemoveAll();
	if(m_Description)
		delete [] m_Description;
}

void
CMolecule::SetDescription(LPTSTR desc)
{
	m_Description = new TCHAR[_tcslen(desc)+1];
	_tcsncpy_s(m_Description, _tcslen(desc)+1, desc, _tcslen(desc));
}

LPTSTR
CMolecule::GetDescription()
{
	if(m_Description)
		return m_Description;
	else return "UNKNOWN";
}

int
CMolecule::GetAtomsCount()
{
	return m_Atoms.GetCount();
}

GLfloat
CMolecule::GetMaxDimension()
{
	return m_Scale;
}

void
CMolecule::EnableLinks(BOOL enable)
{
	m_bDrawLinks = enable;
	if(enable)
		RescaleAtoms();
}

void
CMolecule::EnableWire(BOOL enable)
{
	m_bWireMode = enable;
}

void
CMolecule::InitExplosion(GLfloat step, GLfloat factor)
{
	this->m_ExplosionStep = step;
	CalculateOutValues(factor);
	FOREACH_ATOM(atom)
		atom->SetNormalAsCurrent();
		atom->SetMoveStatus(TRUE);
	END_FOREACH_ATOM()
	m_DrawMode = dmExplode;
}

void
CMolecule::InitImplosion(GLfloat step, GLfloat factor)
{
	CalculateOutValues(factor);	// this should be done only once
	this->m_ExplosionStep = step;
	FOREACH_ATOM(atom)
		atom->SetOutAsCurrent();
		atom->SetMoveStatus(TRUE);
	END_FOREACH_ATOM()
	m_DrawMode = dmImplode;
}

void
CMolecule::PutLink(int from, int to)
{
	if(m_AtomLinks.GetCount() == 0)
	{
		ATOMLINKPTR alink = new ATOMLINK();
		alink->from = from;
		alink->to = to;
		alink->strength = 1;	// one link, normal strength
													// if there will be more links between those two atoms,
													// strenght will be increased
		m_AtomLinks.Add(alink);
	}
	else
	{
		// we need to check for duplicates before inserting this link
		BOOL already_defined = FALSE;
		for(int i=0; i<m_AtomLinks.GetCount(); i++)
		{
			ATOMLINKPTR link = m_AtomLinks.GetAt(i);
			if((link->from == from && link->to == to))
			{
				link->strength++;
				already_defined = TRUE;
				break; // multi link between two atoms
			}
			if(link->to == from && link->from == to)
				already_defined = TRUE;	// reverse link already exists, don't add
		}
		if(!already_defined)
		{
			ATOMLINKPTR alink = new ATOMLINK();
			alink->from = from;
			alink->to = to;
			alink->strength = 1;
			m_AtomLinks.Add(alink);
		}
	}
}

void
CMolecule::RescaleAtoms()
{
	FOREACH_ATOM(atom)
		//num = m_Atoms.GetKeyAt(pos);
		GLfloat bot = 0.4f;
    GLfloat top = 0.6f;
    GLfloat min = 1.17f;
    GLfloat max = 1.80f;
    GLfloat ratio = (atom->GetSize() - min) / (max - min);
    atom->SetScaledSize(bot + (ratio * (top - bot)));
	END_FOREACH_ATOM()
}

void
CMolecule::CalculateOutValues(GLfloat dist_factor)
{
	FOREACH_ATOM(atom)
		GLfloat x,y,z;
		atom->GetCoords(x, y, z);
		GLfloat max = MAX(m_Width, (m_Height, m_Depth));
		int xdir, ydir, zdir;
		xdir = rand()%2 == 0 ? 1 : -1;
		ydir = rand()%2 == 0 ? 1 : -1;
		zdir = rand()%2 == 0 ? 1 : -1;
		x += xdir*dist_factor*(rand()%(int)(max+1));
		y += ydir*dist_factor*(rand()%(int)(max+1));
		z += zdir*dist_factor*(rand()%(int)(max+1));
		atom->SetOutCoords(x, y, z);
		GLfloat speed = (GLfloat)(rand()%6+3)/10;
		atom->SetSpeed(speed);
	END_FOREACH_ATOM()
}

/// Large molecules are not always initally visible beacuse
/// of their size. This function calculates minimal bounding
/// box, which would be a base for rescaling and translating.
/// Molecule is centered in the middle of the box.
void
CMolecule::CalculateBoundingBox()
{
	if(m_Atoms.GetCount() == 0)
	{
		for(int i=0;i<3;i++)
			TRANSLATIONS[i] = 0.0f;
		m_Depth = 0.0f;
		m_Width = 0.0f;
		m_Height = 0.0f;
		return;
	}

	GLfloat x1, y1, z1, x2, y2, z2;
	POSITION p = m_Atoms.GetStartPosition();
	AtomPtr firstAtom = m_Atoms.GetValueAt(p);
	x1 = x2 = firstAtom->GetX();
	y1 = y2 = firstAtom->GetY();
	z1 = z2 = firstAtom->GetZ();

  
  // find min point
  FOREACH_ATOM(atom)
    if(atom->GetX() < x1) x1 = atom->GetX();
		if(atom->GetY() < y1) y1 = atom->GetY();
		if(atom->GetZ() < z1) z1 = atom->GetZ();
  END_FOREACH_ATOM()

  if (x1 < 0)
  {
    // move all atoms, so all the x's will be positive
    FOREACH_ATOM(atom)
      GLfloat x, y, z;
      atom->GetCoords(x, y, z);
      x += (-x1);
      atom->SetCoords(x, y, z);
    END_FOREACH_ATOM()
  }

  if (y1 < 0)
  {
    // move all atoms, so all the y's will be positive
    FOREACH_ATOM(atom)
      GLfloat x, y, z;
      atom->GetCoords(x, y, z);
      y += (-y1);
      atom->SetCoords(x, y, z);
    END_FOREACH_ATOM()
  }

  if (z1 < 0)
  {
    // move all atoms, so all the z's will be positive
    FOREACH_ATOM(atom)
      GLfloat x, y, z;
      atom->GetCoords(x, y, z);
      z += (-z1);
      atom->SetCoords(x, y, z);
    END_FOREACH_ATOM()
  }

  firstAtom->GetCoords(x1, y1, z1);
  
	FOREACH_ATOM(atom)
		if(atom->GetX() < x1) x1 = atom->GetX();
		if(atom->GetY() < y1) y1 = atom->GetY();
		if(atom->GetZ() < z1) z1 = atom->GetZ();
		
		if(atom->GetX() > x2) x2 = atom->GetX();
		if(atom->GetY() > y2) y2 = atom->GetY();
		if(atom->GetZ() > z2) z2 = atom->GetZ();
	END_FOREACH_ATOM()

	m_Width = fabs(x2-x1);
	m_Height = fabs(y2-y1);
	m_Depth = fabs(z2-z1);

	GLfloat size = m_Width > m_Height ? m_Width : m_Height;
	size = size > m_Depth ? size : m_Depth;
	m_Scale = size;	

	TRANSLATIONS[0] = -(x1+m_Width/2);	// X translation
	TRANSLATIONS[1] = -(y1+m_Height/2);	// Y translation
	TRANSLATIONS[2] = -(z1+m_Depth/2);	// Z translation

	// TODO: these settings should be available throught setup dialog
	const static int scale_after_that = 10;
	const static int max_atoms = 500;
	
	int cnt = m_Atoms.GetCount();
	if(cnt >= max_atoms)
	{
		m_ElementScale = 0.3f; // exception
		return;
	}

	if(cnt > scale_after_that)
	{
		// max scale is 1.0f, min scale is 0.2f
		GLfloat ratio = (GLfloat)(cnt-scale_after_that)/(max_atoms-scale_after_that);
		m_ElementScale = 1.0f - (ratio * (1.0f-0.3f));
	}
	else
		m_ElementScale = 1.0f;
}

void
CMolecule::GetTranslations(GLfloat &x, GLfloat &y, GLfloat &z)
{
  x = TRANSLATIONS[0];
  y = TRANSLATIONS[1];
  z = TRANSLATIONS[2];
}

void
CMolecule::CreateAtom(GLfloat x, GLfloat y, GLfloat z, TCHAR *atomName, int number)
{
	CAtom *a = new CAtom(atomName);
	a->SetCoords(x, y, z);
	m_Atoms.SetAt(number, a);
}

BOOL
CMolecule::DoImpExplode()
{
	BOOL all_atoms_ready = TRUE;
	FOREACH_ATOM(atom)
		if(atom->GetMoveStatus())
		{
			GLfloat speed = atom->GetSpeed()*(m_ExplosionStep);
			GLfloat size = this->m_bDrawLinks ? atom->GetScaledSize()/2 : atom->GetSize()/2;
			all_atoms_ready = FALSE;
			if(m_DrawMode == dmImplode)
			{
				if(atom->NearToNormal(speed))
				{
					atom->SetNormalAsCurrent();
					atom->SetMoveStatus(FALSE);
				}
				else
				{
					// normalnie trzeba policzyc wektor przesuniecia od punktu 0
					// i zmieniac wspolrzedne atomu przeciwnie do znakow jego wspolrzednych
					GLfloat x,y,z;
					atom->GetCurrentCoords(x, y, z);
					if(fabsf(atom->GetX() - x) > speed)
						x += atom->GetX() < x ? -speed : speed;
					else
						x = atom->GetX();
					if(fabs(atom->GetY() - y) > speed)
						y += atom->GetY() < y ? -speed : speed;
					else
						y = atom->GetY();
					if(fabs(atom->GetZ() - z) > speed)
						z += atom->GetZ() < z ? -speed : speed;
					else
						z = atom->GetZ();
					atom->SetCurrentCoords(x, y, z);
				}
			}
			else if(m_DrawMode == dmExplode)
			{
				// dla explozji nie ma sensu tego sprawdzac
				if(atom->NearToOut(this->m_ExplosionStep))
				{
					atom->SetOutAsCurrent();
					atom->SetMoveStatus(FALSE);
          atom->SetSkip(TRUE);  // don't draw
				}
				else
				{
					GLfloat x,y,z;
					atom->GetCurrentCoords(x, y, z);
					GLfloat xo, yo, zo;
					atom->GetOutCoords(xo, yo, zo);
					if(fabsf(xo - x) > speed)
						x += xo < x ? -speed : speed;
					else
						x = xo;
					if(fabs(yo - y) > speed)
						y += yo < y ? -speed: speed;
					else
						y = yo;
					if(fabs(zo - z) > speed)
						z += zo < z ? -speed: speed;
					else
						z = zo;
					atom->SetCurrentCoords(x, y, z);
				}
			}
		}
	END_FOREACH_ATOM()
	if(all_atoms_ready && m_DrawMode == dmImplode)
		m_DrawMode = dmNormal; // dla explode nie ustawiam, zeby nie pokazywal linkow przy zoomout
	return all_atoms_ready;
}

void
CMolecule::DrawAtoms()
{
	FOREACH_ATOM(atom)
    if (atom->GetSkip())
      continue;
		GLfloat color[4];
		atom->GetColor(color);
		color[3] = 1.0f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		GLfloat size = m_bDrawLinks ? atom->GetScaledSize() : atom->GetSize();
		GLfloat x, y, z;
		atom->GetCurrentCoords(x, y, z);
		DrawSphereM(x, y, z, size, m_bWireMode, m_ElementScale);
	END_FOREACH_ATOM()
}

void
CMolecule::DrawLinks()
{
	// set default link color
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LINKCOLOR);
	for(int i=0;i<m_AtomLinks.GetCount(); i++)
	{
		ATOMLINKPTR link = m_AtomLinks.GetAt(i);
		CAtom *fromAtom;
		m_Atoms.Lookup(link->from, fromAtom);
		CAtom *toAtom;
		m_Atoms.Lookup(link->to, toAtom);
		if(toAtom && fromAtom)
		{
			DrawTubeM(fromAtom->GetX(), fromAtom->GetY(), fromAtom->GetZ(),
				toAtom->GetX(), toAtom->GetY(), toAtom->GetZ(), link->strength*0.1f, m_bWireMode, m_ElementScale);
		}
	}
}

void
CMolecule::Draw()
{
	if(m_Atoms.GetCount() == 0)
		return;

	// apply scaling and translations, if needed
	//glTranslatef(TRANSLATIONS[0], TRANSLATIONS[1], TRANSLATIONS[2]);
	
	//DrawAtoms();
	//if(m_DrawMode == dmNormal && m_bDrawLinks)
	//	DrawLinks();
	if(m_DrawMode == dmNormal)
	{
		if(!m_bFromDL)
		{
			// all goes to the display list
			m_dl = glGenLists(1);
			glNewList(m_dl, GL_COMPILE);
			DrawAtoms();
			if(m_bDrawLinks)
				DrawLinks();
			glEndList();
			m_bFromDL = TRUE;
			glCallList(m_dl);
		}
		else
			glCallList(m_dl);
	}
	else
	{
		if(m_bFromDL)
		{
			glDeleteLists(m_dl, 1);
			m_bFromDL = FALSE;
		}
		DrawAtoms();	
	}
}

