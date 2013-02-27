#pragma once

#include "stdafx.h"
#include "Atom.h"
#include <vector>
#include <map>

class CMolecule
{
public:
	CMolecule(void);
	void CreateAtom(GLfloat x, GLfloat y, GLfloat z, TCHAR *atomName, int number);
	void Draw();

	void PutLink(int from, int to);
	void EnableLinks(bool enable);
	void EnableWire(bool enable);
	void EnableLabels(bool enable);
	void SetFontList(GLuint list);	// for labeling atoms
	void SetDescription(LPTSTR desc);
	int GetAtomsCount();
	const CString& GetFormula();
	const CString& GetDescription();
	GLfloat GetMaxDimension();
	void CalculateBoundingBox();
  void GetTranslations(GLfloat &x, GLfloat &y, GLfloat &z);
	void CalculateOutValues(GLfloat factor);
	void InitExplosion(GLfloat dist_factor = 2.0f);
	void InitImplosion(GLfloat dist_factor = 2.0f);
	void GenerateFormula();
	bool DoImpExplode(GLfloat delta);
	typedef enum DrawingMode { dmNormal, dmExplode, dmImplode };

	virtual ~CMolecule(void);

protected:
	GLuint m_font_base;
	CString m_Formula;
	DrawingMode m_DrawMode;
	bool m_bDrawLinks;
	bool m_bDrawLabels;
	bool m_bWireMode;
	static GLfloat LINKCOLOR[4];
	CString m_Description;
	GLfloat TRANSLATIONS[3];
	GLfloat m_Width, m_Height, m_Depth;
	GLfloat m_Scale, m_ElementScale;
	void RescaleAtoms();
	GLuint m_dl;
	bool m_bFromDL;

	void DrawLinks();
	void DrawAtoms();
	void DrawLabels();

	typedef CAtom* AtomPtr;
	std::map<int, AtomPtr> m_Atoms;

	typedef struct _ATOMLINK
	{
		int from;		// source atom's number
		int to;			// destination's number
		int strength; // how many links 
	} ATOMLINK;
	
	typedef ATOMLINK* ATOMLINKPTR;
	std::vector<ATOMLINKPTR> m_AtomLinks;
};
