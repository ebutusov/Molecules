#include "StdAfx.h"
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <atlmisc.h>

#include "GLDrawHelper.h"

CGLDrawHelper::CGLDrawHelper(void)
{
}

typedef struct { GLfloat x, y, z; } XYZ;

void
CGLDrawHelper::DrawSphere(int stacks, int slices, BOOL wire)
{
	int i,j;
  double theta1, theta2, theta3;
  XYZ e, p;
  XYZ la = { 0, 0, 0 }, lb = { 0, 0, 0 };
  XYZ c = {0, 0, 0};  /* center */
  double r = 1.0;     /* radius */
  int stacks2 = stacks * 2;

  if (r < 0)
    r = -r;
  if (slices < 0)
    slices = -slices;

  if (slices < 4 || stacks < 2 || r <= 0)
    {
      glBegin (GL_POINTS);
      glVertex3f (c.x, c.y, c.z);
      glEnd();
      return;
    }

  glFrontFace(GL_CW);

  for (j = 0; j < stacks; j++)
    {
      theta1 = j       * (M_PI+M_PI) / stacks2 - M_PI_2;
      theta2 = (j + 1) * (M_PI+M_PI) / stacks2 - M_PI_2;

      glBegin (wire ? GL_LINE_LOOP : GL_TRIANGLE_STRIP);
      for (i = 0; i <= slices; i++)
        {
          theta3 = i * (M_PI+M_PI) / slices;

          if (wire && i != 0)
            {
              glVertex3f (lb.x, lb.y, lb.z);
              glVertex3f (la.x, la.y, la.z);
            }

          e.x = cos (theta2) * cos(theta3);
          e.y = sin (theta2);
          e.z = cos (theta2) * sin(theta3);
          p.x = c.x + r * e.x;
          p.y = c.y + r * e.y;
          p.z = c.z + r * e.z;

          glNormal3f (e.x, e.y, e.z);
          glTexCoord2f (i       / (double)slices,
                        2*(j+1) / (double)stacks2);
          glVertex3f (p.x, p.y, p.z);
          if (wire) la = p;

          e.x = cos(theta1) * cos(theta3);
          e.y = sin(theta1);
          e.z = cos(theta1) * sin(theta3);
          p.x = c.x + r * e.x;
          p.y = c.y + r * e.y;
          p.z = c.z + r * e.z;

          glNormal3f (e.x, e.y, e.z);
          glTexCoord2f (i   / (double)slices,
                        2*j / (double)stacks2);
          glVertex3f (p.x, p.y, p.z);
          if (wire) lb = p;
        }
      glEnd();
    }
}

void
CGLDrawHelper::DrawTube(GLfloat x1, GLfloat y1, GLfloat z1,
      GLfloat x2, GLfloat y2, GLfloat z2,
      GLfloat diameter, GLfloat cap_size,
      int faces, BOOL smooth, BOOL caps_p, BOOL wire)
{
	 GLfloat length, X, Y, Z;

  if (diameter <= 0) abort();

  X = (x2 - x1);
  Y = (y2 - y1);
  Z = (z2 - z1);

  if (X == 0 && Y == 0 && Z == 0)
    return;

  length = sqrt (X*X + Y*Y + Z*Z);

  glPushMatrix();

  glTranslatef(x1, y1, z1);
  glRotatef (-atan2 (X, Y)               * (180 / M_PI), 0, 0, 1);
  glRotatef ( atan2 (Z, sqrt(X*X + Y*Y)) * (180 / M_PI), 1, 0, 0);
  glScalef (diameter, length, diameter);

  /* extend the endpoints of the tube by the cap size in both directions */
  if (cap_size != 0)
  {
    GLfloat c = cap_size/length;
    glTranslatef (0, -c, 0);
    glScalef (1, 1+c+c, 1);
   }

	CGLDrawHelper::DrawTube_INT(faces, smooth, caps_p, wire);
  glPopMatrix(); 
	
} 

void
CGLDrawHelper::DrawTube_INT(int faces, BOOL smooth, BOOL caps_p, BOOL wire)
{
  int i;
  GLfloat step = M_PI * 2 / faces;
  GLfloat s2 = step/2;
  GLfloat th;
  GLfloat x, y, x0=0, y0=0;
  int z = 0;

  /* side walls
   */
  glFrontFace(GL_CCW);
  glBegin (wire ? GL_LINES : (smooth ? GL_QUAD_STRIP : GL_QUADS));

  th = 0;
  x = 1;
  y = 0;

  if (!smooth)
    {
      x0 = cos (s2);
      y0 = sin (s2);
    }

  if (smooth) faces++;

  for (i = 0; i < faces; i++)
    {
      if (smooth)
        glNormal3f(x, 0, y);
      else
        glNormal3f(x0, 0, y0);

      glVertex3f(x, 0, y);
      glVertex3f(x, 1, y);

      th += step;
      x  = cos (th);
      y  = sin (th);

      if (!smooth)
        {
          x0 = cos (th + s2);
          y0 = sin (th + s2);

          glVertex3f(x, 1, y);
          glVertex3f(x, 0, y);
        }
    }
  glEnd();

  /* End caps
   */
  if (caps_p)
    for (z = 0; z <= 1; z++)
      {
        glFrontFace(z == 0 ? GL_CCW : GL_CW);
        glNormal3f(0, (z == 0 ? -1 : 1), 0);
        glBegin(wire ? GL_LINE_LOOP : GL_TRIANGLE_FAN);
        if (! wire) glVertex3f(0, z, 0);
        for (i = 0, th = 0; i <= faces; i++)
          {
            GLfloat x = cos (th);
            GLfloat y = sin (th);
            glVertex3f(x, z, y);
            th += step;
          }
        glEnd();
      }
}
 
void
CGLDrawHelper::DrawString(GLuint font_dlist, int window_width, int window_height, GLfloat x, GLfloat y, LPTSTR string,
													int text_height)
{
	int copy_len = _tcslen(string);
	// nvoglnt seems to be buggy in glCallLists
	// looks like _sometimes_ it uses 4 bytes instead of one when GL_UNSIGNED_BYTE is passed
	// this allocates 4 times more memory than needed
	LPTSTR copy = new TCHAR[(copy_len+1)*4];
	LPTSTR copyptr = copy;
	_tcsncpy_s(copy, copy_len+1, string, copy_len);
	TCHAR delims[] = "\r\n|";
	int y_pos = y;
	TCHAR *context;

  glPushAttrib (GL_TRANSFORM_BIT |  /* for matrix contents */
                GL_ENABLE_BIT);     /* for various glDisable calls */
  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_TEXTURE_2D);
  {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
      glLoadIdentity();
			//glListBase(font_dlist - 32);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      {
        glLoadIdentity();
        gluOrtho2D (0, window_width, 0, window_height);
				LPTSTR sp = _tcstok_s(copy, delims, &context);
				int strings = 0;
				while(sp != NULL)
				{
					glRasterPos2f(x, y_pos);
					int len = _tcslen(sp);
					glPushAttrib(GL_LIST_BIT);
					glListBase(font_dlist);
					glCallLists(len, GL_UNSIGNED_BYTE, sp);
					glPopAttrib();
					/*TCHAR *minuses = new TCHAR[len+1];
					TCHAR *m = minuses;
					for(int i=0;i<len;i++)
						*m++ = '-';
					*m = '\0';
					y_pos -= text_height;
					glRasterPos2f(x, y_pos);
					glPushAttrib(GL_LIST_BIT);
					glListBase(font_dlist);
					glCallLists(len, GL_UNSIGNED_BYTE, minuses);
					glPopAttrib();
					delete [] minuses;*/
					y_pos -= text_height;
					sp = _tcstok_s(NULL, delims, &context);
				}
      }
      glPopMatrix();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  }
  glPopAttrib();
  glMatrixMode(GL_MODELVIEW);
	delete [] copyptr;
}

CGLDrawHelper::~CGLDrawHelper(void)
{
}
