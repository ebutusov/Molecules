#pragma once

class CTextureLoader
{
private:
	CTextureLoader(void);
public:
	static BOOL LoadTexture(LPTSTR file, UINT textureArray[], int ID);
	static BOOL LoadTextureToList(LPTSTR file, GLuint *list);
	~CTextureLoader(void);
};
