#include "Shape.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include <math.h>

using namespace std;

static bool DEBUG_LOAD_OBJS = true;
static bool DEBUG_DRAW_LIGHTS = true;

static Shape shape;
static double spin_angle = 0.001;
static int shape_key = 1;

static bool red = false;
static bool fullscreen = false;

static bool left_clicked = false;
static bool right_clicked = false;
static int x_mouse;
static int y_mouse;

GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
GLfloat mat_diffuse[] = { 0.1, 0.5, 0.8, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat no_shininess[] = { 0.0 };
GLfloat low_shininess[] = { 5.0 };
GLfloat high_shininess[] = { 128 };
GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};

GLfloat d_position[] = {0, 1, 0, 0};
GLfloat p_position[] = {0, -5, 0, 1};
GLfloat s_position[] = {-10, 0, 0, 1};
GLfloat s_direction[] = {1, 0, 0};
static float theta = 0;

static bool shader_toggle = false;
static bool toggle1 = false;
static bool toggle2 = false;
static bool toggle3 = false;
static bool freeze = false;

int Window::width  = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here

//----------------------------------------------------------------------------
// Callback method called when system is idle.
void Window::idleCallback(void)
{
	//shape.spin(spin_angle);
  displayCallback();
}

//----------------------------------------------------------------------------
// Callback method called when window is resized.
void Window::reshapeCallback(int w, int h)
{
	if (!fullscreen) {
		width = w;
		height = h;
	}
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-10.0, 10.0, -10.0, 10.0, 10, 1000.0); // set perspective projection viewing frustum
  glTranslatef(0, 0, -20);
}

Matrix4& Shape::getModelMatrix() {
	return shape.model;
}

Matrix4& Shape::getModelViewMatrix() {
	return shape.modelview;
}

void Shape::updateModelViewMatrix() {
	Matrix4 cam_inv = Matrix4(shape.getCameraMatrix());
	cam_inv.inverse();
	shape.getModelViewMatrix() = cam_inv.multiply(shape.getModelMatrix());
}

//----------------------------------------------------------------------------
// Callback method called when window readraw is necessary or
// when glutPostRedisplay() was called.
void Window::displayCallback(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color and depth buffers
  glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(shape.getModelViewMatrix().getGLMatrix());
	Material cube = Material(GL_FRONT_AND_BACK);
	Material dragon = Material(GL_FRONT);
	Material bunny = Material(GL_FRONT_AND_BACK);
	Material sandal = Material(GL_FRONT_AND_BACK);

	switch (shape_key) {
		case 1: // cube
			cube.setAmbient(no_mat);
			cube.setDiffuse(mat_diffuse);
			cube.setSpecular(mat_specular);
			cube.setShininess(high_shininess);
			cube.setEmission(no_mat);
			
			drawCube();
			break;

		case 2: // dragon
			dragon.setAmbient(no_mat);
			dragon.setDiffuse(mat_diffuse);
			dragon.setSpecular(mat_specular);
			dragon.setShininess(high_shininess);
			dragon.setEmission(no_mat);

			drawShape(dragon_nVerts, dragon_vertices, dragon_normals);
			break;
		case 3: // bunny
			bunny.setAmbient(no_mat);
			bunny.setDiffuse(no_mat);
			bunny.setSpecular(mat_specular);
			bunny.setShininess(high_shininess);
			bunny.setEmission(no_mat);

			drawShape(bunny_nVerts, bunny_vertices, bunny_normals);
			break;
		case 4: // sandle
			sandal.setAmbient(no_mat);
			sandal.setDiffuse(no_mat);
			sandal.setSpecular(mat_specular);
			sandal.setShininess(high_shininess);
			sandal.setEmission(no_mat);

			drawShape(sandal_nVerts, sandal_vertices, sandal_normals);
			break;
		case 8: // house scene1
			shape.drawHouse();
			break;
		case 9: // house scene2
			shape.drawHouse();
			break;
	}

	if (toggle1) {
			glPushMatrix();
				glRotatef(theta, 1, 0, 0);
				if (!freeze)	
					shape.directional.setPosition(d_position);
				if (DEBUG_DRAW_LIGHTS) drawDirectionalLight();
			glPopMatrix();
	}

	if (toggle2) {
		glPushMatrix();
			glRotatef(theta, 0, 0, 1);
			if (!freeze)
				shape.point.setPosition(p_position);
			if (DEBUG_DRAW_LIGHTS) drawPointLight();
		glPopMatrix();
	}

	if (toggle3) {
		glPushMatrix();
			glRotatef(theta, 0, 1, 0);
			if (!freeze) {
				shape.spot.setPosition(s_position);
				shape.spot.setSpotDirection(s_direction);
			}
			if (DEBUG_DRAW_LIGHTS) drawSpotLight();
		glPopMatrix();
	}

	if (!freeze)
		theta+=0.1;
	if (theta > 360 || theta < 0) theta = 0;

	/* TODO: sphere for testing
	glPushMatrix();
		glColor3f(1,1,1);
		glTranslatef(-5,0,0);
		glutSolidSphere(1,10,10);
	glPopMatrix();
	*/

  glFlush();  
  glutSwapBuffers();
}

void Window::drawDirectionalLight() {
		glBegin(GL_LINES);
			glColor3f(10, 10, 10);
			glVertex3f(0, 10, 0);
			glVertex3f(0 - 5*d_position[0], 10 - 5*d_position[1], 0 - 5*d_position[2]);
		glEnd();
}

void Window::drawPointLight() {
		glColor3f(10, 10, 10);
		glTranslated(p_position[0], p_position[1], p_position[2]);
		glScaled(0.5, 0.5, 0.5);
		glutSolidSphere(1, 10, 10);
}

void Window::drawSpotLight() {		
		glBegin(GL_LINES);
			glColor3f(10, 10, 10);
			glVertex3f(s_position[0], s_position[1], s_position[2]);
			glVertex3f(s_position[0] + 3*s_direction[0], s_position[1] + 3*s_direction[1], s_position[2] + 3*s_direction[2]);
		glEnd();
}

void Window::drawCube() {
		glBegin(GL_QUADS);
			// Draw front face:
			glColor3f(1, 0, 0);
			glNormal3f(0.0, 0.0, 1.0);   
			glVertex3f(-1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0,  1.0);
			glVertex3f( 1.0, -1.0,  1.0);
			glVertex3f(-1.0, -1.0,  1.0);
    
			// Draw left side:
			glColor3f(0, 1, 0);
			glNormal3f(-1.0, 0.0, 0.0);
			glVertex3f(-1.0,  1.0,  1.0);
			glVertex3f(-1.0,  1.0, -1.0);
			glVertex3f(-1.0, -1.0, -1.0);
			glVertex3f(-1.0, -1.0,  1.0);
    
			// Draw right side:
			glColor3f(0, 0, 1);
			glNormal3f(1.0, 0.0, 0.0);
			glVertex3f( 1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0, -1.0);
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f( 1.0, -1.0,  1.0);
  
			// Draw back face:
			glColor3f(1, 0, 1);
			glNormal3f(0.0, 0.0, -1.0);
			glVertex3f(-1.0,  1.0, -1.0);
			glVertex3f( 1.0,  1.0, -1.0);
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f(-1.0, -1.0, -1.0);
  
			// Draw top side:
			glColor3f(1, 1, 0);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(-1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0, -1.0);
			glVertex3f(-1.0,  1.0, -1.0);
  
			// Draw bottom side:
			glColor3f(0, 1, 1);
			glNormal3f(0.0, -1.0, 0.0);
			glVertex3f(-1.0, -1.0, -1.0);
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f( 1.0, -1.0,  1.0);
			glVertex3f(-1.0, -1.0,  1.0);
		glEnd();
}

void Shape::loadData()
{
  // put code to load data model here
	ObjReader::readObj("dragon_smooth.obj", dragon_nVerts, &dragon_vertices, &dragon_normals, &dragon_texcoords, dragon_nIndices, &dragon_indices);
	ObjReader::readObj("bunny_n.obj", bunny_nVerts, &bunny_vertices, &bunny_normals, &bunny_texcoords, bunny_nIndices, &bunny_indices);
	ObjReader::readObj("sandal.obj", sandal_nVerts, &sandal_vertices, &sandal_normals, &sandal_texcoords, sandal_nIndices, &sandal_indices);
}

void Shape::calculateStuff(int nVerts, float *vertices) {
	float max_arr[3] = {-1000, -1000, -1000};
	float min_arr[3] = {1000, 1000, 1000};

	for (int i = 0; i < nVerts/3; i++) {
		for (int v = 0; v < 3; v++) {
			if (vertices[9*i+3*v] < min_arr[0]) {
				min_arr[0] = vertices[9*i+3*v];
			}
			if (vertices[9*i+3*v] > max_arr[0]) {
				max_arr[0] = vertices[9*i+3*v];
			}
			if (vertices[(9*i)+(3*v)+1] < min_arr[1]) {
				min_arr[1] = vertices[(9*i)+(3*v)+1];
			}
			if (vertices[(9*i)+(3*v)+1] > max_arr[1]) {
				max_arr[1] = vertices[(9*i)+(3*v)+1];
			}
			if (vertices[(9*i)+(3*v)+2] < min_arr[2]) {
				min_arr[2] = vertices[(9*i)+(3*v)+2];
			}
			if (vertices[(9*i)+(3*v)+2] > max_arr[2]) {
				max_arr[2] = vertices[(9*i)+(3*v)+2];
			}

		}
	}

	shape.x = 0.5*(max_arr[0] + min_arr[0]);
	shape.y = 0.5*(max_arr[1] + min_arr[1]);
	shape.z = 0.5*(max_arr[2] + min_arr[2]);

	shape.translation.identity();
	shape.translation.m[0][3] = -shape.x;
	shape.translation.m[1][3] = -shape.y;
	shape.translation.m[2][3] = -shape.z;

	cout << "minimum values: " << min_arr[0] << ", " << min_arr[1] << ", " << min_arr[2] << "\n";
  cout << "maximum values: " << max_arr[0] << ", " << max_arr[1] << ", " << max_arr[2] << "\n\n";

	cout << "center: (" << shape.x << ", " << shape.y << ", " << shape.z << ")\n\n";
	
	float x_diff = max_arr[0]-min_arr[0];
	float y_diff = max_arr[1]-min_arr[1];
	float z_diff = max_arr[2]-min_arr[2];

	float max1 = max(x_diff, y_diff);
	max1 = max(max1, z_diff);

	scaling_x = 28/max1;
	scaling_y = 28/max1;
	scaling_z = 28/max1;

	shape.scale.identity();
	shape.scale.m[0][0] = scaling_x;
	shape.scale.m[1][1] = scaling_y;
	shape.scale.m[2][2] = scaling_z;

	cout << "scaling factor : " << scaling_x << ", " << scaling_y <<  ", " << scaling_z <<"\n\n";

}

void Window::drawShape(int nVerts, float *vertices, float *normals) {
	glBegin(GL_TRIANGLES);
	for (int i=0; i<nVerts/3; i++) {
		if (red == true) {
			// red
			glColor3f(0.5,0.5,0.5);
			red = false;
		}
		else {
			// blue
			glColor3f(0.5,0.5,0.5);
			red = true;
		}
		for (int v=0; v<3; v++) {
			glNormal3f(normals[9*i+3*v], normals[(9*i)+(3*v)+1], normals[(9*i)+(3*v)+2]);
			glVertex3f(vertices[9*i+3*v], vertices[(9*i)+(3*v)+1], vertices[(9*i)+(3*v)+2]);
		}
	}
	glEnd();
}

Shape::Shape() {
	shape.getModelMatrix().identity();

	shape.getModelMatrix() = shape.getModelMatrix().multiply(shape.setScaleMatrix(8.0));

	shape.getCameraMatrix().identity();
	shape.updateModelViewMatrix();

	cout << "initialized model matrix:\n";
	shape.getModelMatrix().print();

	cout << "initialized camera matrix:\n";
	shape.getCameraMatrix().print();

	cout << "initialized modelview matrix:\n";
	shape.getModelViewMatrix().print();
}

Matrix4& Shape::getCameraMatrix() {
	return shape.camera;
}

int main(int argc, char *argv[])
{
	float specular[]  = {1.0, 1.0, 1.0, 1.0};
  float shininess[] = {100.0};

  glutInit(&argc, argv);      	      	      // initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   // open an OpenGL context with double buffering, RGB colors, and depth buffering
  glutInitWindowSize(Window::width, Window::height);      // set initial window size
  glutCreateWindow("OpenGL Cube for CSE167");    	      // open window and set window title
  glDisable(GL_LIGHTING);
  
  
  glEnable(GL_DEPTH_TEST);            	      // enable depth buffering
  glClear(GL_DEPTH_BUFFER_BIT);       	      // clear depth buffer
  glClearColor(0.0, 0.0, 0.0, 0.0);   	      // set clear color to black
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // set polygon drawing mode to fill front and back of each polygon
  glDisable(GL_CULL_FACE);     // disable backface culling to render both sides of polygons
  glDisable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);             	      // set shading to smooth
  glMatrixMode(GL_PROJECTION); 
  
  // Generate material properties:
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  //Generate light source:
  glEnable(GL_LIGHTING);
  
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };

	GLfloat light_diffuse_d[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_specular_d[] = { 1.0, 0.0, 0.0, 1.0 };

	GLfloat light_diffuse_p[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat light_specular_p[] = { 0.0, 1.0, 0.0, 1.0 };

	GLfloat light_diffuse_s[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat light_specular_s[] = { 0.0, 0.0, 1.0, 1.0 };

	shape.directional = Light(0);
	shape.directional.setPosition(d_position);
	shape.directional.setAmbient(light_ambient);
	shape.directional.setDiffuse(light_diffuse_d);
	shape.directional.setSpecular(light_specular_d);

	shape.point = Light(1);
	shape.point.setPosition(p_position);
	shape.point.setAmbient(light_ambient);
	shape.point.setDiffuse(light_diffuse_p);
	shape.point.setSpecular(light_specular_p);

	shape.spot = Light(2);
	shape.spot.setPosition(s_position);
	shape.spot.setAmbient(light_ambient);
	shape.spot.setDiffuse(light_diffuse_s);
	shape.spot.setSpecular(light_specular_s);
	shape.spot.setSpotCutoff(5);
	shape.spot.setSpotDirection(s_direction);

	if (!toggle1)
		shape.directional.disable();
	if (!toggle2)
		shape.point.disable();
	if (!toggle3)
		shape.spot.disable();
  
  // Install callback functions:
  glutDisplayFunc(Window::displayCallback);
  glutReshapeFunc(Window::reshapeCallback);
  glutIdleFunc(Window::idleCallback);

	// to avoid cube turning white on scaling down
  glEnable(GL_NORMALIZE);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	// Process keyboard input
  glutKeyboardFunc(Window::processNormalKeys);
  glutSpecialFunc(Window::processSpecialKeys);
	glutMouseFunc(Window::processMouseClick);
	glutMotionFunc(Window::processMouseMove);
	
	// load obj files
	if (DEBUG_LOAD_OBJS)
		shape.loadData();

  glutMainLoop();
  return 0;
}

double Shape::getAngle() {
	return angle;
}

void Shape::setAngle(double a) {
	angle = a;
}

Matrix4 Shape::setScaleMatrix(float factor) {
	Matrix4 scale = Matrix4();
	for (int i = 0; i < 3; i++)
	{
		scale.set(i, i, factor);
	}
	return scale;
}

void Shape::drawHouse() {
	int tris = 0;
	glBegin(GL_QUADS);
	for (int i=0; i<7; i++) {
		glColor3f(house_colors[12*i], house_colors[12*i+1], house_colors[12*i+2]);
		glNormal3f(house_indices[i*6], house_indices[i*6+1], house_indices[i*6+2]);
		for (int v=0; v<4; v++) {
			glVertex3f(house_vertices[12*i+3*v], house_vertices[(12*i)+(3*v)+1], house_vertices[(12*i)+(3*v)+2]);
		}
		glNormal3f(house_indices[i*6+3], house_indices[i*6+4], house_indices[i*6+5]);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	for (int i=7; i<8; i++) {
		glColor3f(house_colors[12*i], house_colors[12*i+1], house_colors[12*i+2]);
		glNormal3f(house_indices[i*6], house_indices[i*6+1], house_indices[i*6+2]);
		for (int v=0; v<3; v++) {
			glVertex3f(house_vertices[12*i+3*v], house_vertices[(12*i)+(3*v)+1], house_vertices[(12*i)+(3*v)+2]);
		}
		tris++;
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int i=8; i<10; i++) {
		glColor3f(house_colors[12*i-3*tris], house_colors[12*i+1-3*tris], house_colors[12*i+2-3*tris]);
		glNormal3f(house_indices[i*6-3*tris], house_indices[i*6+1-3*tris], house_indices[i*6+2-3*tris]);
		for (int v=0; v<4; v++) {
			glVertex3f(house_vertices[12*i+3*v-3*tris], house_vertices[(12*i)+(3*v)+1-3*tris], house_vertices[(12*i)+(3*v)+2-3*tris]);
		}
		glNormal3f(house_indices[i*6+3-3*tris], house_indices[i*6+4-3*tris], house_indices[i*6+5-3*tris]);
	}
	glEnd();
	
	glBegin(GL_TRIANGLES);
	for (int i=10; i<11; i++) {
		glColor3f(house_colors[12*i-3*tris], house_colors[12*i+1-3*tris], house_colors[12*i+2-3*tris]);
		glNormal3f(house_indices[i*6]-3*tris, house_indices[i*6+1]-3*tris, house_indices[i*6+2]-3*tris);
		for (int v=0; v<3; v++) {
			glVertex3f(house_vertices[12*i+3*v-3*tris], house_vertices[(12*i)+(3*v)+1-3*tris], house_vertices[(12*i)+(3*v)+2-3*tris]);
		}
		tris++;
	}
	glEnd();
}


void Shape::spin(double deg)
{
  if (shape.angle > 360.0 || shape.angle < -360.0) shape.angle = 0.0;
	shape.getModelViewMatrix().rotateY(deg);
}


void Window::processNormalKeys(unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 'c':
			// reverse the direction of the spin
			spin_angle = -spin_angle;
			cout << "reverse spin\n";
			break;
		case 'x':
			// move cube left by a small amount
			shape.getModelViewMatrix().translate(-1, 0, 0);
			cout << "move left\n";
			break;
		case 'X':
			// move cube right by a small amount
			shape.getModelViewMatrix().translate(1, 0, 0);
			cout << "move right\n";
			break;
		case 'y':
			// move cube down by a small amount
			shape.getModelViewMatrix().translate(0, -1, 0);
			cout << "move down\n";
			break;
		case 'Y':
			// move cube up by a small amount
			shape.getModelViewMatrix().translate(0, 1, 0);
			cout << "move up\n";
			break;
		case 'z':
			// move cube into of the screen by a small amount
			shape.getModelViewMatrix().translate(0, 0, -1);
			cout << "move in\n";
			break;
		case 'Z':
			// move cube out of the screen by a small amount
			shape.getModelViewMatrix().translate(0, 0, 1);
			cout << "move out\n";
			break;
		case 'r':
			// reset position and size to its initial position
			shape.getModelMatrix().identity();
			if (shape_key == 8)
				shape.getCameraMatrix() = Matrix4::createCameraMatrix(Vector3(0, 10, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
			else if (shape_key == 9)
				shape.getCameraMatrix() = Matrix4::createCameraMatrix(Vector3(-15, 5, 10), Vector3(-5, 0, 0), Vector3(0, 1, 0.5));
			else
				shape.getCameraMatrix().identity();
			shape.setAngle(0);
			shape.updateModelViewMatrix();
			break;
		case 'a':
			// rotate cube about the OpenGL window's z axis by a small number of degrees counterclockwise
			// The z axis crosses the screen in its center.
			if (shape.angle > 360.0 || shape.angle < -360.0) shape.angle = 0.0;
			shape.getModelViewMatrix().rotateWindowZ(-100*spin_angle);
			cout << "rotate CW window z-axis\n";
			break;
		case 'A':
			// rotate cube about the OpenGL window's z axis by a small number of degrees clockwise
			// The z axis crosses the screen in its center.
			if (shape.angle > 360.0 || shape.angle < -360.0) shape.angle = 0.0;
			shape.getModelViewMatrix().rotateWindowZ(100*spin_angle);
			cout << "rotate CCW window z-axis\n";
			break;
		case 's':
			// scale cube down (about its center, not the center of the screen)
			shape.getModelViewMatrix().scale(0.95, 0.95, 0.95);
			cout << "scale down\n";
			break;
		case 'S':
			// scale cube up (about its center, not the center of the screen)
			shape.getModelViewMatrix().scale(1.05, 1.05, 1.05);
			cout << "scale up\n";
			break;
		case 'f': //toggle screenmode
			if(!fullscreen) {
        glutFullScreen();
        fullscreen = true;
			} 
			else {
        glutReshapeWindow(Window::width, Window::height);
        glutPositionWindow(100, 100);
        fullscreen = false;
			}
	    break;
		case '1':
				toggle1 = !toggle1;
				toggle1 ? shape.directional.enable() : shape.directional.disable();
				toggle1 ? cout << "directional ON\n" : cout << "directional OFF\n";
			break;
		case '2':
				toggle2 = !toggle2;
				toggle2 ? shape.point.enable() : shape.point.disable();
				toggle2 ? cout << "point ON\n" : cout << "point OFF\n";
			break;
		case '3':
				toggle3 = !toggle3;
				toggle3 ? shape.spot.enable() : shape.spot.disable();
				toggle3 ? cout << "spot ON\n" : cout << "spot OFF\n";
			break;
		case '5':
			shader_toggle = !shader_toggle;
			if (shader_toggle)
				Shader shader = Shader("diffuse_shading.vert", "diffuse_shading.frag", true);
			else
				Shader shader = Shader("diffuse_shading.vert", "diffuse_shading.frag", false);
			break;
		case 'm':
			freeze = !freeze;
			freeze ? cout << "freeze!\n" : cout << "...unfreeze\n";
			break;
	}
}

void Window::processSpecialKeys(int key, int x, int y)
{
	shape.getModelMatrix().identity();
	shape.getCameraMatrix().identity();
	switch (key) 
	{	
		case GLUT_KEY_F1:
			// cube
			shape_key = 1;
			shape.getModelMatrix().identity();
			shape.getModelMatrix() = shape.getModelMatrix().multiply(shape.setScaleMatrix(8.0));
			shape.getCameraMatrix().identity();
			break;
		case GLUT_KEY_F2:
			// dragon
			shape_key = 2;
			shape.getModelMatrix().identity();
			shape.getModelMatrix().translate(0, -5, 0);
			shape.getModelMatrix() = shape.getModelMatrix().multiply(shape.setScaleMatrix(15.0));
			shape.getCameraMatrix().identity();
			break;
		case GLUT_KEY_F3:
			// bunny
			shape_key = 3;
			shape.getModelMatrix().identity();
			shape.getModelMatrix().translate(0, 4, 0);
			shape.getModelMatrix() = shape.getModelMatrix().multiply(shape.setScaleMatrix(12.0));
			shape.getCameraMatrix().identity();
			break;
		case GLUT_KEY_F4:
			// sandal
			shape_key = 4;
			shape.getModelMatrix().identity();
			shape.getModelMatrix().translate(1, 0, 0);
			shape.getModelMatrix() = shape.getModelMatrix().multiply(shape.setScaleMatrix(4.0));
			shape.getCameraMatrix().identity();
			break;
		case GLUT_KEY_F8:
			// house view1
			shape_key = 8;
			shape.getCameraMatrix() = Matrix4::createCameraMatrix(Vector3(0, 10, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
			break;
		case GLUT_KEY_F9:
			// show house view2
			shape_key = 9;
			shape.getCameraMatrix() = Matrix4::createCameraMatrix(Vector3(-15, 5, 10), Vector3(-5, 0, 0), Vector3(0, 1, 0.5));
			break;
	}
	shape.updateModelViewMatrix();
}

void Window::processMouseClick(int button, int state, int x, int y) {
		switch (button) {
			case GLUT_LEFT_BUTTON:
				if (state == GLUT_DOWN) {
					left_clicked = true;
					x_mouse = x;
					y_mouse = y;
				}
				else
					left_clicked = false;
				break;

			case GLUT_RIGHT_BUTTON:
				if (state == GLUT_DOWN) {
					right_clicked = true;
					x_mouse = x;
					y_mouse = y;
				}
				else
					right_clicked = false;
				break;
		}
}

void Window::processMouseMove(int x, int y) {
	  if (!freeze) {
			if (left_clicked) { // rotate modelview
				if (x != x_mouse || y != y_mouse) {
					shape.getModelViewMatrix().trackballRotation(Window::width, Window::height, x_mouse, y_mouse, x, y);
					x_mouse = x;
					y_mouse = y;
				}
			}
			else if (right_clicked) { // zoom modelview
				if (y < y_mouse) {
					shape.getModelViewMatrix().scale(1.05,1.05,1.05);
					y_mouse = y;
				}
				else if (y > y_mouse) {
					shape.getModelViewMatrix().scale(0.95, 0.95, 0.95);
					y_mouse = y;
				}
			}
		}
		else {
			if (left_clicked) { // rotate light
				if (x != x_mouse || y != y_mouse) {
					if (y < y_mouse)
						theta -= 100*acos((x*x_mouse + y*y_mouse)/(sqrtf(x*x + y*y) * sqrtf(x_mouse*x_mouse + y_mouse*y_mouse)));
					else if (y > y_mouse)
						theta += 100*acos((x*x_mouse + y*y_mouse)/(sqrtf(x*x + y*y) * sqrtf(x_mouse*x_mouse + y_mouse*y_mouse)));

					if (theta > 360) theta = 0;
					if (theta < 0) theta = 360;

					x_mouse = x;
					y_mouse = y;
				}
			}
			else if (right_clicked) { // zoom light
				float y_diff = ((float)(y - y_mouse))/height;

				float y_scale = 1+y_diff;
				cout << y_scale << "\n";
				
				d_position[1]*=y_scale;
				d_position[2]*=y_scale;
				shape.directional.setPosition(d_position);

				p_position[0]*=y_scale;
				p_position[1]*=y_scale;
				shape.point.setPosition(p_position);

				s_position[0]*=y_scale;
				s_position[2]*=y_scale;
				shape.spot.setPosition(s_position);

				y_mouse = y;
			}
		}
}