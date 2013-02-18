#include "stdafx.h"
#include"bmp.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
//										BMP TEXTURE LOADER
/////////////////////////////////////////////////////////////////////////////////////////////////
//void BMP_Texture(UINT textureArray[], LPSTR strFileName, int ID)
//{
//	if(!strFileName)   return;
//	
//	LoadDIBitmap();
//	//AUX_RGBImageRec *pBitMap = auxDIBImageLoad(strFileName);
//	
//	if(pBitMap == NULL)	exit(0);
//
//	glGenTextures(1, &textureArray[ID]);
//	glBindTexture(GL_TEXTURE_2D, textureArray[ID]);
//	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitMap->sizeX, pBitMap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitMap->data);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
//
//	if (pBitMap)										
//	{
//		if (pBitMap->data)								
//		{
//			free(pBitMap->data);						
//		}
//		free(pBitMap);									
//	}
//}

BOOL LoadTexture(LPTSTR file, UINT textureArray[], int ID)
{
	HBITMAP hBMP;
	BITMAP BMP;
	glGenTextures(1, &textureArray[ID]);
	hBMP = (HBITMAP)LoadImage(::GetModuleHandle(NULL), file, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if(!hBMP)
		return FALSE;
	GetObject(hBMP, sizeof(BMP), &BMP);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);                                                                // Pixel Storage Mode (Word Alignment / 4 Bytes)

  // Typical Texture Generation Using Data From The Bitmap
  
	glBindTexture(GL_TEXTURE_2D, textureArray[ID]);                                                                // Bind To The Texture ID
  
  glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
  //glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, BMP.bmBits);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BMP.bmWidth, BMP.bmHeight, GL_RGB, GL_UNSIGNED_BYTE, BMP.bmBits);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        // Linear Min Filter
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        // Linear Mag Filter
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

	DeleteObject(hBMP);
	return TRUE;
}


BOOL LoadTextureToList(LPTSTR file, GLuint *list)
{
	HBITMAP hBMP;
	BITMAP BMP;
	GLuint textures[1];
	glGenTextures(1, &textures[0]);
	hBMP = (HBITMAP)LoadImage(::GetModuleHandle(NULL), file, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if(!hBMP)
		return FALSE;
	GetObject(hBMP, sizeof(BMP), &BMP);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);                                                                // Pixel Storage Mode (Word Alignment / 4 Bytes)

  // Typical Texture Generation Using Data From The Bitmap
  
	glNewList(*list = glGenLists(1), GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, textures[0]);                                                                // Bind To The Texture ID
		glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        // Linear Min Filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        // Linear Mag Filter
	glEndList();
	DeleteObject(hBMP);
	return TRUE;
}