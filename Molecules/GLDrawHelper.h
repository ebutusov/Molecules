#pragma once

class CGLDrawHelper
{
private:
	static void DrawTube_INT(int faces, bool smooth, bool caps_p, bool wire);
public:
	static void DrawSphere(int stacks, int slices, bool wire);

	static void DrawTube(GLfloat x1, GLfloat y1, GLfloat z1,
		GLfloat x2, GLfloat y2, GLfloat z2,
			GLfloat diameter, GLfloat cap_size,
				int faces, bool smooth, bool caps_p, bool wire);

	static void CGLDrawHelper::DrawLabel(GLuint font_base,
		GLfloat x, GLfloat y, GLfloat z, GLfloat size, LPCTSTR label);
	
	static void DrawString(GLuint font_base,
		int window_width, int window_height, GLfloat x, GLfloat y,
			LPCTSTR string, int text_height);

private:
	CGLDrawHelper(void);
};