#include "stdafx.h"
#include "Model.h"
#include <assert.h>
#include <fstream>


Model::Model(void):
	model(NULL), colors(NULL), bbox(NULL), vbo(0), ibo(0), render_inited(false)
{
}

Model::~Model(void)
{
	if(model)
		glmDelete(model);

	if(colors)
		delete[] colors;

	if(bbox)
		delete bbox;

	unbindVBO();
	deleteVBO();

}

bool Model::LoadModel(char* obj_file, char* color_file)
{
	model = glmReadOBJ(obj_file);
	if(!model)
		return false;

	//normal
	if( ! model->normals )
	{
		glmFacetNormals(model);
		glmVertexNormals(model, 90.0);

		assert(model->numnormals == model->numvertices);
	}

	//bbox
	bbox = new BBox();
	float dim[3];
	float center[3];
	glmDimensions(model, dim);
	glmCenter(model, center);
	for(int i = 0; i < 3; i++)
	{
		bbox->min[i] = center[i] - dim[i] / 2.0;
		bbox->max[i] = center[i] + dim[i] / 2.0;
	}

	//color
	return readColor(color_file);
}

bool Model::readColor(char* color_file)
{
	std::ifstream fin;
	fin.open(color_file);
	if( ! fin.is_open() )
		return false;

	colors = new float[model->numvertices * 3];
	int i;
	for(i = 0; i < model->numvertices; i++)
	{
		fin >> colors[i * 3] >> colors[i * 3 + 1] >> colors[i * 3 + 2];
	}

	i--;
	printf("%f %f %f", colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);

	fin.close();

	return true;
}

void Model::render()
{
	if( ! render_inited )
	{
		buildVBO();
		bindVBO();

		render_inited = true;
	}

	glColor3f(0.0, 0.0, 1.0);

	glDrawElements(GL_TRIANGLES, model->numtriangles * 3, GL_UNSIGNED_INT, NULL);
}

void Model::buildVBO()
{
	//vbo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numvertices * 9, NULL, GL_STATIC_DRAW);
	GLfloat* pbuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	int i, k;
	for(i = 0; i < model->numvertices; i++)
	{
		for(k = 0; k < 3; k++)
			*(pbuffer++) = model->vertices[(i+1) * 3 + k];
		for(k = 0; k < 3; k++)
			*(pbuffer++) =  model->normals[(i+1) * 3 + k];
		for(k = 0; k < 3; k++)
			*(pbuffer++) =  colors[i * 3 + k];
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//ibo
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * model->numtriangles * 3, NULL, GL_STATIC_DRAW);
	GLuint* pbuffer2 = (GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
	for(int i = 0; i < model->numtriangles; i++)
	{
		for(int k = 0; k < 3; k++)
			*(pbuffer2++) = model->triangles[i].vindices[k] - 1;

		//assert(model->triangles[i].vindices[k] <= model->numvertices);
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::deleteVBO()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Model::bindVBO()
{
	//vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 9 * sizeof(GL_FLOAT), NULL);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 9 * sizeof(GL_FLOAT), (GLfloat*)NULL + 3);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 9* sizeof(GL_FLOAT), (GLfloat*)NULL + 6);
	//ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void Model::unbindVBO()
{
	//vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	//ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
