#pragma once
#include <atlopengl.h>

class CGLDrawHelper
{
private:
	static void DrawTube_INT(int faces, BOOL smooth, BOOL caps_p, BOOL wire);
	static DWORD m_font_base;

public:
	static void InitFonts(DWORD font_base);
	static void FreeFonts();

	static void DrawSphere(int stacks, int slices, BOOL wire);

	static void DrawTube(GLfloat x1, GLfloat y1, GLfloat z1,
		GLfloat x2, GLfloat y2, GLfloat z2,
			GLfloat diameter, GLfloat cap_size,
				int faces, BOOL smooth, BOOL caps_p, BOOL wire);

	static void CGLDrawHelper::DrawLabel(
		GLfloat x, GLfloat y, GLfloat z, GLfloat size, LPCTSTR label);
	
	static void DrawString(
		int window_width, int window_height, GLfloat x, GLfloat y,
			LPTSTR string, int text_height);

private:
	CGLDrawHelper(void);
};