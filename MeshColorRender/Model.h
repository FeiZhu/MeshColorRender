#pragma once
#include "stdafx.h"
#include "LoadObj/glm.h"

struct BBox {
	double min[3];
	double max[3];
};

class Model
{
private:
	GLMmodel*	model;
	float*		colors;
	BBox*		bbox;

	GLuint		vbo;
	GLuint		ibo;

	bool		render_inited;

public:
	Model(void);
	~Model(void);

	bool	LoadModel(char* obj_file, char* color_file);
	BBox*	getBBox() {	return bbox;	};

	void	render();

private:
	bool	readColor(char* color_file);

	void	buildVBO();
	void	deleteVBO();
	void	bindVBO();
	void	unbindVBO();
};
