#include "StdAfx.h"
#include "GLDrawHelper.h"
#include "Molecule.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <map>
#include <algorithm>

#define MAX(a, b) a > b ? a : b;

//#define USE_GLU

void
DrawSphereM(GLfloat x, GLfloat y, GLfloat z, GLfloat diameter, bool wireframe, GLfloat scale)
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

#define FOREACH_ATOM(x) \
	for(auto _it = m_Atoms.begin(); _it != m_Atoms.end(); ++_it) \
	{ \
		int _num = _it->first; \
		AtomPtr x; \
		x = _it->second;

#define END_FA }

GLfloat CMolecule::LINKCOLOR[4] = { 0.3f, 0.3f, 0.2f, 1.0f };

CMolecule::CMolecule(void) 
	: m_Description(_T("UNKNOWN")),
	m_bDrawLabels(false), m_bWireMode(FALSE), m_DrawMode(dmNormal),
	m_dl(0), m_bFromDL(false), m_font_base(0), m_selected_atom(-1),
	m_bListsReady(false)
{
}

CMolecule::~CMolecule(void)
{
	FOREACH_ATOM(atom)
		delete atom;
		_it->second = NULL;
	END_FA

	m_Atoms.clear();

	for(auto i = m_AtomLinks.begin();i != m_AtomLinks.end(); ++i)
		delete (*i);

	m_AtomLinks.clear();

	if (m_bFromDL)
		glDeleteLists(m_dl, 1);
	if (m_bListsReady)
	{
		glDeleteLists(m_sphere_list, 1);
		glDeleteLists(m_cylinder_list, 1);
	}
}

void
CMolecule::SetDescription(LPTSTR desc)
{
	m_Description = desc;
}

const CString&
CMolecule::GetDescription()
{
	return m_Description;
}

const CString& CMolecule::GetFormula()
{
	return m_Formula;
}

int
CMolecule::GetAtomsCount()
{
	return m_Atoms.size();
}

GLfloat
CMolecule::GetMaxDimension()
{
	return m_maxSize + m_maxAtomSize/2.0f;
}

void
CMolecule::EnableLinks(bool enable)
{
	m_bDrawLinks = enable;
	m_bFromDL = false;	// need to create new list
}

void
CMolecule::EnableWire(bool enable)
{
	m_bWireMode = enable;
	m_bFromDL = false;	// need to create new list
}

void CMolecule::EnableLabels(bool enable)
{
	m_bDrawLabels = enable;
	m_bFromDL = false;	// need to create new list
}

void CMolecule::SetFontList(GLuint list)
{
	m_font_base = list;
}

void CMolecule::SetSelected(int num)
{
	if (m_selected_atom != num)
		m_bFromDL = false;

	m_selected_atom = num;
}

void
CMolecule::InitExplosion(GLfloat dist_factor)
{
	//CalculateOutValues(dist_factor);
	FOREACH_ATOM(atom)
		atom->SetNormalAsCurrent();
		atom->SetMoveStatus(TRUE);
		GLfloat speed = (GLfloat)(rand()%10+5); // /10;
		atom->SetSpeed(speed);
	END_FA
	m_DrawMode = dmExplode;
}

void
CMolecule::InitImplosion(GLfloat dist_factor)
{
	CalculateOutValues(dist_factor);	// this should be done only once
	FOREACH_ATOM(atom)
		atom->SetOutAsCurrent();
		atom->SetMoveStatus(TRUE);
		GLfloat speed = (GLfloat)(rand()%5+2); // /10;
		atom->SetSpeed(speed);
	END_FA
	m_DrawMode = dmImplode;
}

void
CMolecule::PutLink(int from, int to)
{
	// we need to check for duplicates and update multi links
	bool already_defined = FALSE;

	for(auto i = m_AtomLinks.begin();i != m_AtomLinks.end(); ++i)
	{
		ATOMLINKPTR link = *i;
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
		alink->strength = 1;	// one link, normal strength
													// if there will be more links between those two atoms,
													// strenght will be increased
		m_AtomLinks.push_back(alink);
	}
	//}
}

void
CMolecule::RescaleAtoms()
{
	GLfloat max_atom, min_atom;
	CAtom::SizeLimits(max_atom, min_atom);

	m_maxAtomSize = max_atom;

	FOREACH_ATOM(atom)
		GLfloat bot = 0.4f;
    GLfloat top = 0.6f;
    //GLfloat min = 1.17f;
    //GLfloat max = 1.80f;
    GLfloat ratio = (atom->GetSize() - min_atom) / (max_atom - min_atom);
    atom->SetScaledSize(bot + (ratio * (top - bot)));
	END_FA
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
	END_FA
}

/// Large molecules are not always initally visible beacuse
/// of their size. This function calculates minimal bounding
/// box, which would be a base for rescaling and translating.
/// Molecule is centered in the middle of the box.
void
CMolecule::CalculateBoundingBox()
{
	int count = m_Atoms.size();

	if(count == 0)
	{
		for(int i=0;i<3;i++)
			TRANSLATIONS[i] = 0.0f;
		m_Depth = 0.0f;
		m_Width = 0.0f;
		m_Height = 0.0f;
		return;
	}

	RescaleAtoms();

	GLfloat x1, y1, z1, x2, y2, z2;
	AtomPtr firstAtom = m_Atoms.begin()->second;
	x1 = x2 = firstAtom->GetX();
	y1 = y2 = firstAtom->GetY();
	z1 = z2 = firstAtom->GetZ();

  // find min point
  FOREACH_ATOM(atom)
    if(atom->GetX() < x1) x1 = atom->GetX();
		if(atom->GetY() < y1) y1 = atom->GetY();
		if(atom->GetZ() < z1) z1 = atom->GetZ();
  END_FA

  if (x1 < 0)
  {
    // move all atoms, so all the x's will be positive
    FOREACH_ATOM(atom)
      GLfloat x, y, z;
      atom->GetCoords(x, y, z);
      x += (-x1);
      atom->SetCoords(x, y, z);
    END_FA
  }

  if (y1 < 0)
  {
    // move all atoms, so all the y's will be positive
    FOREACH_ATOM(atom)
      GLfloat x, y, z;
      atom->GetCoords(x, y, z);
      y += (-y1);
      atom->SetCoords(x, y, z);
    END_FA
	}

  if (z1 < 0)
  {
    // move all atoms, so all the z's will be positive
    FOREACH_ATOM(atom)
      GLfloat x, y, z;
      atom->GetCoords(x, y, z);
      z += (-z1);
      atom->SetCoords(x, y, z);
    END_FA
  }

  firstAtom->GetCoords(x1, y1, z1);
	x2 = x1, y2 = y1, z2 = z1;
  
	FOREACH_ATOM(atom)
		if(atom->GetX() < x1) x1 = atom->GetX();
		if(atom->GetY() < y1) y1 = atom->GetY();
		if(atom->GetZ() < z1) z1 = atom->GetZ();
		
		if(atom->GetX() > x2) x2 = atom->GetX();
		if(atom->GetY() > y2) y2 = atom->GetY();
		if(atom->GetZ() > z2) z2 = atom->GetZ();
	END_FA

	m_Width = fabs(x2-x1);
	m_Height = fabs(y2-y1);
	m_Depth = fabs(z2-z1);

	GLfloat size = m_Width > m_Height ? m_Width : m_Height;
	size = size > m_Depth ? size : m_Depth;
	m_maxSize = size;	

	// XXX not sure what's going on here - translations were relative
	// to the molecule, changed that to global, works better
	// (to avoid 'dipping' into ground in reflection mode)
	// center around the origin (all axes positive, so we need -)
	TRANSLATIONS[0] = -(x1+m_Width/2);	// X translation
	TRANSLATIONS[1] = -(y1+m_Height/2);	// Y translation
	TRANSLATIONS[2] = -(z1+m_Depth/2);	// Z translation

	// TODO: these settings should be available throught setup dialog
	const static int scale_after_that = 10;
	const static int max_atoms = 500;
	
	if(count >= max_atoms)
	{
		m_ElementScale = 0.3f; // exception
		return;
	}

	if(count > scale_after_that)
	{
		// max scale is 1.0f, min scale is 0.2f
		GLfloat ratio = (GLfloat)(count-scale_after_that)/(max_atoms-scale_after_that);
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
	m_Atoms[number] = a;
}

bool
CMolecule::DoImpExplode(GLfloat delta)
{
	bool all_atoms_ready = TRUE;
	FOREACH_ATOM(atom)
		if(atom->GetMoveStatus())
		{
			GLfloat speed = atom->GetSpeed() * delta;
			//GLfloat speed = atom->GetSpeed()*(m_ExplosionStep);
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
				if(atom->NearToOut(speed))
				{
					atom->SetOutAsCurrent();
					atom->SetMoveStatus(FALSE);
          atom->SetSkip(TRUE);  // don't draw
				}
				else
				{
					all_atoms_ready = FALSE;
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
	END_FA
	if (all_atoms_ready && m_DrawMode == dmImplode)
		m_DrawMode = dmNormal; // dla explode nie ustawiam, zeby nie pokazywal linkow przy zoomout
	return all_atoms_ready;
}

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

template <typename T>
struct less_comparer
{
	//typedef std::pair<T1, T2> PairType;
	bool operator() (T const &a, T const &b)
	{
		// true if and ony if a < b
		// put C atoms first, then H, then the rest in alphabetical order
		if (!_tcscmp(a.c_str(), b.c_str())) return false; // identical keys, must be false to pass operator< test
		if (!_tcscmp(a.c_str(),_T("C"))) return true;
		if (!_tcscmp(b.c_str(), _T("C"))) return false;
		if (!_tcscmp(a.c_str(), _T("H"))) return true;
		if (!_tcscmp(b.c_str(), _T("H"))) return false;
		return _tcscmp(a.c_str(), b.c_str()) < 0;
	}
};

void CMolecule::GenerateFormula()
{
	using namespace std;

	map<tstring, int, less_comparer<tstring>> counts;
	FOREACH_ATOM(atom)
		counts[atom->GetName()]++;
	END_FA
	
	if (counts.size() > 20) // too many unique atoms
		return;

	for(auto it=counts.begin();it!=counts.end();++it)
	{
		CString f;
		f.Format(_T("%s(%d)"), it->first.c_str(), it->second);
		m_Formula += f;
	}
}

void CMolecule::Prepare()
{
	ASSERT_CONTEXT

	GenerateFormula();
	CalculateBoundingBox();
	MakeLists();
}

void CMolecule::MakeLists()
{
	// sphere for atoms
	m_sphere_list = glGenLists(1);
	glNewList(m_sphere_list, GL_COMPILE);
#ifdef USE_GLU
	GLUquadric *q = gluNewQuadric();
	gluQuadricDrawStyle(q, m_bWireMode ? GLU_LINE : GLU_FILL);
	gluSphere(q, 1.0f, (int)(12*m_ElementScale), (int)(24*m_ElementScale));
	gluDeleteQuadric(q);
#else
	CGLDrawHelper::DrawSphere((int)(12*m_ElementScale),(int)(24*m_ElementScale), m_bWireMode);
#endif
	glEndList();

	// cylinder for bonds
	m_cylinder_list = glGenLists(1);
	glNewList(m_cylinder_list, GL_COMPILE);
#ifdef USE_GLU
	GLUquadric *q2 = gluNewQuadric();
	gluQuadricDrawStyle(q2, m_bWireMode ? GLU_LINE : GLU_FILL);
	gluCylinder(q2, 1.0f, 1.0f, 1.0f, 12*m_ElementScale, 24*m_ElementScale); 
	gluDeleteQuadric(q2);
#else
	CGLDrawHelper::DrawTubeRaw(30, true, false, m_bWireMode);
#endif
	glEndList();
	m_bListsReady = true;
}

void CMolecule::DrawLabels()
{
	FOREACH_ATOM(atom)
		if (atom->GetSkip())
      continue;
		GLfloat color[4];
		atom->GetColor(color);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		glColor3fv(color);
		GLfloat size = m_bDrawLinks ? atom->GetScaledSize() : atom->GetSize();
		GLfloat x, y, z;
		atom->GetCurrentCoords(x, y, z);
		CGLDrawHelper::DrawLabel(m_font_base, x, y, z, 2.0f* size, 
			_num == m_selected_atom ?  atom->GetFullName() : atom->GetName());
	END_FA
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
		// draw selected atom as white
		if (_num == m_selected_atom)
			color[0] = color[1] = color[2] = 1.0f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		GLfloat size = m_bDrawLinks ? atom->GetScaledSize() : atom->GetSize();
		GLfloat x, y, z;
		atom->GetCurrentCoords(x, y, z);
		glLoadName(_num);
		glPushMatrix();
			glTranslatef(x, y, z);
			glScalef(size, size, size);
			glCallList(m_sphere_list);
		glPopMatrix();
		//DrawSphereM(x, y, z, size, m_bWireMode, m_ElementScale);
	END_FA
}

void
CMolecule::DrawLinks()
{
	// set default link color
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LINKCOLOR);

	for (auto i = std::begin(m_AtomLinks);i != std::end(m_AtomLinks); ++i)
	{
		ATOMLINKPTR link = *i;
		
		// find atoms involved in link
		CAtom *fromAtom = NULL, *toAtom = NULL;
		auto p = m_Atoms.find(link->from);
		if (p != m_Atoms.end())
			fromAtom = p->second;
		p = m_Atoms.find(link->to);
		if (p != m_Atoms.end())
			toAtom = p->second;

		if(toAtom && fromAtom)
		{
			// ok, we have them both, draw the link
			// XXX precalculate link values (diameter, length, offsets)

			GLfloat x1 = fromAtom->GetX(), y1 = fromAtom->GetY(), z1 = fromAtom->GetZ(),
				x2 = toAtom->GetX(), y2 = toAtom->GetY(), z2 = toAtom->GetZ();
			GLfloat X,Y,Z;
			GLfloat length, diameter = link->strength*0.1f;

			if (diameter <= 0) diameter = 0.1f;

			X = (x2 - x1);
			Y = (y2 - y1);
			Z = (z2 - z1);

			if (X == 0 && Y == 0 && Z == 0)
				return;

			length = sqrt (X*X + Y*Y + Z*Z);

			glPushMatrix();
#ifdef USE_GLU
				// z-aligned
				glTranslatef(x1, y1, z1);
				glRotatef(atan2(X, Z) * (180/M_PI), 0, 1, 0);
				glRotatef(-atan2(Y, sqrt(X*X+Z*Z)) * (180/M_PI), 1, 0, 0);
				glScalef (diameter, diameter, length);
#else
				// y-aligned
				glTranslatef(x1, y1, z1);
				glRotatef (-atan2 (X, Y)               * (GLfloat)(180.0f / M_PI), 0, 0, 1);
				glRotatef ( atan2 (Z, sqrt(X*X + Y*Y)) * (GLfloat)(180.0f / M_PI), 1, 0, 0);
				glScalef (diameter, length, diameter);
#endif
				glCallList(m_cylinder_list);
			glPopMatrix();
			/*DrawTubeM(fromAtom->GetX(), fromAtom->GetY(), fromAtom->GetZ(),
				toAtom->GetX(), toAtom->GetY(), toAtom->GetZ(), link->strength*0.1f, m_bWireMode, m_ElementScale);*/
		}
	}
}

void
CMolecule::Draw()
{
	if (m_Atoms.size() == 0)
		return;
	
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
			m_bFromDL = true;
			glCallList(m_dl);
		}
		else
			glCallList(m_dl);
		if (m_bDrawLabels) DrawLabels();
	}
	else // explosion or implosion
	{
		if(m_bFromDL)
		{
			glDeleteLists(m_dl, 1);
			m_bFromDL = false;
		}
		DrawAtoms();
		if (m_bDrawLabels) DrawLabels();
	}
}

