#include "stdafx.h"

char* obj_file;
char* color_file;

Model*	model = NULL;

//camera:
View*		view = NULL;

// mouse control:
int lastMousePos[2];
bool mouse_left_down = false;
bool mouse_right_down = false;
bool mouse_middle_down = false;

float modelRotate[2] = {0.0, 0.0};	//model rotation degree around axis x & y
float modelTranslate[3] = {0.0, 0.0, 0.0};

//fps counter
LARGE_INTEGER freq1, last_time, current_time;
int frame_count;

//save current view to file
void saveView()
{
	std::string file_name("config.ini");
	std::fstream outputfile(file_name,std::ios::out);
	if(!outputfile)
	{
		std::cout<<"Error: create config.ini failed!\n";
		return;
	}
	//model rotation
	outputfile<<modelRotate[0]<<" "<<modelRotate[1]<<"\n";
	//model translation
	outputfile<<modelTranslate[0]<<" "<<modelTranslate[1]<<" "<<modelTranslate[2]<<"\n";
    //camera position
    outputfile<<view->CamPos[0]<<" "<<view->CamPos[1]<<" "<<view->CamPos[2]<<"\n";	
	outputfile.close();
	std::cout<<"Saved view to config.ini.\n";
}

//load view from file
void loadView()
{
	std::string file_name("config.ini");
	std::fstream inputfile(file_name,std::ios::in);
	if(!inputfile)
	{
		std::cout<<"Error: open config.ini failed!\n";
		return;
	}
	//model rotation
	inputfile>>modelRotate[0]>>modelRotate[1];
	//model translation
	inputfile>>modelTranslate[0]>>modelTranslate[1]>>modelTranslate[2];
	//camera position
	inputfile>>view->CamPos[0]>>view->CamPos[1]>>view->CamPos[2];
	inputfile.close();
	std::cout<<"Loaded view from config.ini.\n";
}

void init()
{
	glewInit();

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	//light
	float light_ambient[4] = {0.4, 0.4, 0.4, 1.0};
	float light_diffuse[4] = {0.6, 0.6, 0.6, 1.0};
	float light_specular[4] = {1.0, 1.0, 1.0, 1.0};
	float light_pos[4] = {1.0, 1.0, 1.0, 0.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	//material
	float mat_specular[4] = {1.0, 1.0, 1.0, 1.0};
	float mat_shininess = 16.0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	//model
	model = new Model();
	model->LoadModel(obj_file, color_file);

	//camera
	float model_size[3];
	for(int i = 0; i < 3; i++)
		model_size[i] = model->getBBox()->max[i] -model->getBBox()->min[i] ;
	float model_radix =sqrt(model_size[0] * model_size[0] + model_size[1] * model_size[1] + model_size[2] * model_size[2]);

	view = new View();
	view->CamPos[0] = 0.0;
	view->CamPos[1] = 0.0;
	view->CamPos[2] = model_radix*1.2;

	//antialias
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    //glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
}

void reshape(int w, int h)
{
	glutReshapeWindow(w, h);

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLdouble)w / h, 0.001, 100.0);
	glMatrixMode(GL_MODELVIEW);

	view->width = w;
	view->height = h;

}

void onMouseClick(int button, int state, int x, int y)
{
	if(button == GLUT_RIGHT_BUTTON)
	{	
		if(state == GLUT_DOWN)
		{
			mouse_right_down = true;
			lastMousePos[0] = x;
			lastMousePos[1] = y;
		}	
		else if(state == GLUT_UP)
		{
			mouse_right_down = false;
		}
	}

	if(button == GLUT_LEFT_BUTTON)
	{	
		if(state == GLUT_DOWN)
		{
			mouse_left_down = true;
			lastMousePos[0] = x;
			lastMousePos[1] = y;
		}	
		else if(state == GLUT_UP)
		{
			mouse_left_down = false;
		}
	}

	if(button == GLUT_MIDDLE_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			mouse_middle_down = true;
			lastMousePos[0] = x;
			lastMousePos[1] = y;
		}
		else if(state == GLUT_UP)
		{
			mouse_middle_down = false;
		}
	}
}

void onMouseMove(int x, int y)
{
	if(mouse_right_down)
	{
		modelRotate[1] += x - lastMousePos[0]; 
		modelRotate[0] += y - lastMousePos[1];

		lastMousePos[0] = x;
		lastMousePos[1] = y;
	}

	if(mouse_left_down)
	{
		view->CamPos[0] -= (x - lastMousePos[0]) * view->CamPos[2] * 0.001;
		view->CamPos[1] += (y - lastMousePos[1]) * view->CamPos[2] * 0.001;

		lastMousePos[0] = x;
		lastMousePos[1] = y;
	}

	if(mouse_middle_down)
	{
		double scale = 0.1;
		double delta = y - lastMousePos[1];
		view->CamPos[2] += delta * scale;
		lastMousePos[0] = x;
		lastMousePos[1] = y;
	}
}

void display()
{
	frame_count++;
	QueryPerformanceCounter(&current_time);
	if((current_time.QuadPart - last_time.QuadPart) / freq1.QuadPart >= 1)
	{
		printf("%d fps\n", frame_count);
		frame_count = 0;
		last_time = current_time;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(view->CamPos[0], view->CamPos[1], view->CamPos[2] ,
		view->CamPos[0] , view->CamPos[1] , 0.0,
		0.0, 1.0, 0.0);

	glRotatef(modelRotate[0], 1.0, 0.0, 0.0);
	glRotatef(modelRotate[1], 0.0, 1.0, 0.0);

	glTranslatef(modelTranslate[0], modelTranslate[1], modelTranslate[2]);


	model->render();


	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 'l'://loadview
	  loadView();
	  break;
  case 's'://saveview
	  saveView();
	  break;
  }
}

void idle()
{
	glutPostRedisplay();
}

void main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("need 2 input arguments:  obj_file color_file\n");
	}
	obj_file = argv[1];
	color_file = argv[2];

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(350, 250);
	glutInitWindowSize(512, 512);
	glutCreateWindow("");

	init();

	//fps counter:
	QueryPerformanceFrequency(&freq1);
	QueryPerformanceCounter(&last_time);
	frame_count = 0;

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);  
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseMove);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
}