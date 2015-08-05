#ifndef MODEL_H
#define MODEL_H

#include "Renderer.h"

class Model
{
public:
	Model();
	~Model();

	void LoadModel(const char* idName, const char* path, Renderer* render);
	FBXFile* GetModel();
	const char* GetID();

	void DrawModel();

private:
	FBXFile* m_FBX;
	const char* m_idName;
	unsigned int m_diffuse; //default diffuse texture

};

#endif