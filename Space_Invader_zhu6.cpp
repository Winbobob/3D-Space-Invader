#define GLUT_DISABLE_ATEXIT_HACK
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <time.h>
#include <cstdio>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "glm.h"
using namespace std;

//structs
struct spaceship
{
     float x;		//coordinate x
	 float y;		//coordinate y
	 float z;		//coordinate z
	 int lifeNum;	//life num of spaceship

};

struct alien
{
     float x;		//coordinate x
	 float y;		//coordinate y
	 float z;		//coordinate z
	 int  row;		//alien row number
};

struct bigalien
{
     float x;		//coordinate x
	 float y;		//coordinate y
	 float z;		//coordinate z
	 bool alive;	//alien dead or alive
};

struct block
{
     float x;		//coordinate x
	 float y;		//coordinate y
	 float z;		//coordinate z
	 bool exist;	//exist or not
};

struct spbullet
{
     float x;		//coordinate x
	 float y;		//coordinate y
	 float z;		//coordinate z
};

struct anbullet
{
     float x;		//coordinate x
	 float y;		//coordinate y
	 float z;		//coordinate z
};

struct biganbullet
{
     float x;		//coordinate x
	 float y;		//coordinate y
	 float z;		//coordinate z
};

//global variables
int levelNum = 1;
GLMmodel* sppmodel = NULL;
GLMmodel* biganpmodel = NULL;
GLMmodel* anpmodel1 = NULL;
GLMmodel* anpmodel2 = NULL;
GLMmodel* anpmodel3 = NULL;
GLMmodel* sppmodelExplode = NULL;
GLMmodel* anpmodelExplode = NULL;
GLMmodel* biganpmodelExplode = NULL;

vector<spaceship>sps;
vector<alien>aliens;
vector<bigalien>bigaliens;
vector<block>blocks;
vector<spbullet>spbullets;  //spaceship bullet
vector<anbullet>anbullets;  //alien bullet
vector<biganbullet>biganbullets;//big alien bullet

//vector iterator
vector<alien>::iterator itr1;
vector<block>::iterator itr2;
vector<spbullet>::iterator itr3;
vector<anbullet>::iterator itr4;
vector<biganbullet>::iterator itr5; //big alien bullet
vector<bigalien>::iterator itr6; //big alien   

GLfloat spz; //store the z coordinate change of spaceship
GLfloat anz = -1.0; //store the z coordinate change of aliens
GLfloat spbx = 10.0;
static int frame = 0;
static int bigantempframe = 0;
static int spexplodeframe = 0;
bool fire = false; //judge press space bar or not
int bulletNum = 10;
float timeOfCurrentFrame = 0.0;
float timeOfLastFrame = 0.0;
float diff = 0.0;
float bigexplodex = 0.0;
float bigexplodey = 0.0;
float bigexplodez = 0.0;
int score = 0; //Game score
int count = 0;  //count the time in order to make aliens move to next row until they reach its end
int speedcount = 0;  // in order to add speed of aliens
bool soundeffect = false;
string direction = "right";  //aliens move direction
string bigdirection = "left";  //bigaliens move direction
bool result = false;  // the result of first level

void redisplay_all()
{
    glutPostRedisplay();
}

bool collision(float bulletX, float bulletZ, float objX, float objZ)
{
	if((abs(bulletX - objX) < 0.8) && (abs(bulletZ - objZ) < 0.8))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void drawText(const char *text, int length, int x, int y)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 1024, 768, 0, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	

	glRasterPos2i(x, y);
	for(int i = 0; i < length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);

}

void init()
{
	//initial all objects and relate all coordinates
	//initial spaceship
	struct spaceship sp1 = {12.0, 0.0, 0.0, 3};
	sps.push_back(sp1);

	//initial blocks (from right to left)
	for(int i = 0; i < 4; i++)
	{
		struct block bk1 = {8.0, 0.0, 10.0 - 6 * i - 0, true};
		blocks.push_back(bk1);
		struct block bk2 = {8.0, 0.0, 10.0 - 6 * i - 1, true};
		blocks.push_back(bk2);
		struct block bk3 = {8.0, 0.0, 10.0 - 6 * i - 2, true};
		blocks.push_back(bk3);
	}

	for(int i = 0; i < 4; i++)
	{
		struct block bk1 = {9.0, 0.0, 10.0 - 6 * i - 0, true};
		blocks.push_back(bk1);
		struct block bk2 = {9.0, 0.0, 10.0 - 6 * i - 2, true};
		blocks.push_back(bk2);
	}

	//initial aliens (from right to left, from front to back)
	for(int i = 0; i < 5; i++) //i represents the row number 
	{
		for(int j = 0; j < 9; j++) // j represents the column number
		{
			struct alien an1 = {-3.0 - 6 * i, 0.0, 12.0 - 2.5 * j, i};
			aliens.push_back(an1);
		}
	}
	struct alien an2 = {200, 100.0, 0.0, 10}; 
	aliens.push_back(an2);
	
    //initial big alien (from right to left)
	struct bigalien span1 = {-40.0, 0.0, -16.0, true};
	bigaliens.push_back(span1);
	struct bigalien span2 = {100.0, 100.0, 0.0, true};
	bigaliens.push_back(span2);
}

bool Matrix(float anmovespeed, int levelNum)
{
 	//draw spaceship
	if (!sppmodel) 
	{
		sppmodel = glmReadOBJ("inputs/flighter.obj");
		if (!sppmodel) exit(0);
		glmUnitize(sppmodel);
		glmFacetNormals(sppmodel);
		glmVertexNormals(sppmodel, 90.0); //calculate smooth vertex normals of the model
	}		
	glPushMatrix();
	glTranslatef(sps[0].x, 0, sps[0].z);
	glRotatef(90, 0, 1,0);   
	//glScalef(2.0, 2.0, 2.0);
	glPushMatrix();
		glColor3f(0.8, 1.0, 1.0);
	glPopMatrix();
	glmDraw(sppmodel, GLM_SMOOTH | GLM_MATERIAL);
	glPopMatrix();

	//draw big alien
	itr6 = bigaliens.begin();
	if (!biganpmodel) 
	{
		biganpmodel = glmReadOBJ("inputs/Big_Space_Invader.obj");
		if (!biganpmodel) exit(0);
		glmUnitize(biganpmodel);
		glmFacetNormals(biganpmodel);
		glmVertexNormals(biganpmodel, 90.0); //calculate smooth vertex normals of the model
	}

	//bigaliens appear periodically
	if(itr6->z >10 || itr6->z < -10)
	{
		
		glPushMatrix();
		glTranslatef(itr6->x, itr6->y, itr6->z);
		glRotatef(90, 0, 1,0); 
		glScalef(2.0, 2.0, 2.0);
		glPushMatrix();
			glColor3f(0, 0, 0);
		glPopMatrix();
		glmDraw(biganpmodel, GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glTranslatef(itr6->x, itr6->y, itr6->z);
		glRotatef(90, 0, 1,0); 
		glScalef(2.0, 2.0, 2.0);
		glPushMatrix();
			glColor4ub(255, 230, 2, 1);
		glPopMatrix();
		glmDraw(biganpmodel, GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();
	}

	//two ways to traverse vector: 1.iterator; 2.vector.size
	//draw blocks(every three subblock consists of a complete block)
	itr2 = blocks.begin();
	for ( ; itr2 != blocks.end(); itr2++)
	{
		glPushMatrix();
			glTranslatef(itr2->x, itr2->y, itr2->z);
			glPushMatrix();
				glColor4ub(166, 166, 160, 1);
			glPopMatrix();
			glutSolidCube(1);
		glPopMatrix();
	}

	//draw aliens	
	if (!anpmodel1 || (frame % 30 == 0)) 
	{
		anpmodel1 = glmReadOBJ("inputs/Space_Invader11.obj");
		if (!anpmodel1) exit(0);
		glmUnitize(anpmodel1);
		glmFacetNormals(anpmodel1);
		glmVertexNormals(anpmodel1, 90.0); //calculate smooth vertex normals of the model
	}
	else if (frame % 30 == 15) 
	{
		anpmodel1 = glmReadOBJ("inputs/Space_Invader12.obj");
		if (!anpmodel1) exit(0);
		glmUnitize(anpmodel1);
		glmFacetNormals(anpmodel1);
		glmVertexNormals(anpmodel1, 90.0); //calculate smooth vertex normals of the model
	}
	if (!anpmodel2 || (frame % 30 == 0)) 
	{
		anpmodel2 = glmReadOBJ("inputs/Space_Invader21.obj");
		if (!anpmodel2) exit(0);
		glmUnitize(anpmodel2);
		glmFacetNormals(anpmodel2);
		glmVertexNormals(anpmodel2, 90.0); //calculate smooth vertex normals of the model
	}
	else if (!anpmodel2 || (frame % 30 == 15)) 
	{
		anpmodel2 = glmReadOBJ("inputs/Space_Invader22.obj");
		if (!anpmodel2) exit(0);
		glmUnitize(anpmodel2);
		glmFacetNormals(anpmodel2);
		glmVertexNormals(anpmodel2, 90.0); //calculate smooth vertex normals of the model
	}
	if (!anpmodel3 || (frame % 30 == 0)) 
	{
		anpmodel3 = glmReadOBJ("inputs/Space_Invader31.obj");
		if (!anpmodel3) exit(0);
		glmUnitize(anpmodel3);
		glmFacetNormals(anpmodel3);
		glmVertexNormals(anpmodel3, 90.0); //calculate smooth vertex normals of the model
	}
	else if (!anpmodel3 || (frame % 30 == 15)) 
	{
		anpmodel3 = glmReadOBJ("inputs/Space_Invader32.obj");
		if (!anpmodel3) exit(0);
		glmUnitize(anpmodel3);
		glmFacetNormals(anpmodel3);
		glmVertexNormals(anpmodel3, 90.0); //calculate smooth vertex normals of the model
	}
	frame++; 
 	if((int)aliens.size() > 0)
	{
		itr1 = aliens.begin();
		for ( ; itr1 != aliens.end(); itr1++)
		{
			
			glPushMatrix();
				glTranslatef(itr1->x, itr1->y, itr1->z); 
				glRotatef(90, 0, 1,0);   
				//glScalef(2.0, 2.0, 2.0);
				
				if(itr1->row == 0)
				{
					glPushMatrix();
						glColor4ub(249, 101, 90, 1);
					glPopMatrix();
					glmDraw(anpmodel1, GLM_SMOOTH | GLM_MATERIAL);
				}

				if(itr1->row == 2)
				{
					glPushMatrix();
						glColor4ub(70, 250, 188, 1);
					glPopMatrix();
					glmDraw(anpmodel1, GLM_SMOOTH | GLM_MATERIAL);
				}
				
				if(itr1->row == 4)
				{
					glPushMatrix();
						glColor4ub(250, 70, 193, 1);
					glPopMatrix();
					glmDraw(anpmodel1, GLM_SMOOTH | GLM_MATERIAL);
				}

				if(itr1->row == 1)
				{
					glPushMatrix();
						glColor4ub(248, 250, 70, 1);
					glPopMatrix();
					glmDraw(anpmodel2, GLM_SMOOTH | GLM_MATERIAL);
				}

				if(itr1->row == 3)
				{
					glPushMatrix();
						glColor4ub(154, 70, 250, 1);
					glPopMatrix();
					glmDraw(anpmodel3, GLM_SMOOTH | GLM_MATERIAL);
				}
			glPopMatrix();
			glPushMatrix();
				glColor3f(1.0 ,1.0 ,1.0);
			glPopMatrix();
		}
	}

	//draw spaceship bullets
	if(spbullets.empty() != true)
	{
		itr3 = spbullets.begin();
		for ( ; itr3 != spbullets.end(); itr3++)
		{
			int a = 0;
			glPushMatrix();
			glTranslatef(itr3->x, 0.0, itr3->z);
			glutSolidSphere(0.3, 20 ,20);
			glPopMatrix();
			//judgement collision
			itr2 = blocks.begin();
			for ( ; itr2 != blocks.end(); itr2++)
			{
				if (collision(itr3->x, itr3->z, itr2->x, itr2->z))
				{
					blocks.erase(itr2);
					spbullets.erase(itr3);
					a = 1;
					break;
				}
			}
			if(a == 1) break;
			if((int)aliens.size() > 0)
			{
				itr1 = aliens.begin();
				for ( ; itr1 != aliens.end(); itr1++)
				{
					if (collision(itr3->x, itr3->z, itr1->x, itr1->z))
					{

						//alienexplode
						anpmodelExplode = glmReadOBJ("inputs/Space_Invader_explode.obj");
						if (!anpmodelExplode) exit(0);
						glmUnitize(anpmodelExplode);
						glmFacetNormals(anpmodelExplode);
						glmVertexNormals(anpmodelExplode, 90.0); //calculate smooth vertex normals of the model
						glTranslatef(itr1->x, itr1->y, itr1->z);
						glRotatef(90, 0, 1,0); 
						glScalef(2.0, 2.0, 2.0);
						glPushMatrix();
							if(itr1->row == 0)
							{
								glColor4ub(249, 101, 90, 1);
							}

							if(itr1->row == 2)
							{
								glColor4ub(70, 250, 188, 1);
							}
							
							if(itr1->row == 4)
							{
								glColor4ub(250, 70, 193, 1);
							}

							if(itr1->row == 1)
							{
								glColor4ub(248, 250, 70, 1);
							}

							if(itr1->row == 3)
							{
								glColor4ub(154, 70, 250, 1);
							}
						glPopMatrix();
						glmDraw(anpmodelExplode, GLM_SMOOTH | GLM_MATERIAL);
						Sleep(20);
						aliens.erase(itr1);
						spbullets.erase(itr3);
						score += 20;
						if (soundeffect == true) PlaySound("inputs\\Blast.wav",NULL, SND_ASYNC | SND_FILENAME);
						a = 1;
						break;
					}
				}
			}
			if(a == 1) break;	
			if((int)bigaliens.size() > 0)
			{
				itr6 = bigaliens.begin();
				for ( ; itr6 != bigaliens.end(); itr6++)
				{
					if(collision(itr3->x, itr3->z, itr6->x, itr6->z))
					{
						bigexplodex = itr6->x;
						bigexplodey = itr6->y;
  						bigexplodez = itr6->z;

						bigaliens.erase(itr6);
						spbullets.erase(itr3);

						score += 100;
						if (soundeffect == true) PlaySound("inputs\\Blast2.wav",NULL, SND_ASYNC | SND_FILENAME);
						bigantempframe = frame;
						
						a = 1;
						break;
					}
				}
			}
			
			if(a == 1) break;	

			itr3->x -= 0.03 * 10;
		}

	}

	//bigalien explode
 	if(frame - bigantempframe < 3 && frame > 3)
	{
		biganpmodelExplode = glmReadOBJ("inputs/Big_Space_Invader_explode.obj");
		if (!biganpmodelExplode) exit(0);
		glmUnitize(biganpmodelExplode);
		glmFacetNormals(biganpmodelExplode);
		glmVertexNormals(biganpmodelExplode, 90.0); //calculate smooth vertex normals of the model

		//itr6 = bigaliens.begin();
		glTranslatef(bigexplodex-5, bigexplodey, bigexplodez);
		glRotatef(90, 0, 1,0); 
		glScalef(10.0, 10.0, 10.0);
		glPushMatrix();
			glColor4ub(255, 230, 2, 1);
		glPopMatrix();
		glmDraw(biganpmodelExplode, GLM_SMOOTH | GLM_MATERIAL);
	}


	

	//add alien bullets
 	if((int)(timeOfCurrentFrame) % 23 == 0) //random condition
	{ 
		if((int)aliens.size() > 0)
		{
			itr1 = aliens.begin();
			for(int i = 0; i < (int)(timeOfCurrentFrame) % (int)aliens.size(); i++)
			{
				itr1++;
			}
			struct anbullet anb = {itr1->x, 0.0, itr1->z};
			anbullets.push_back(anb);
		}
	}

	//draw alien bullets
	if((int)aliens.size() > 0)
	{
		itr4 = anbullets.begin();
		for ( ; itr4 != anbullets.end(); itr4++)
		{
			int a = 0;
			glPushMatrix();
			glTranslatef(itr4->x, 0.0, itr4->z);
				glPushMatrix();
					glRotatef(90, 0, 1,0);   
					glutSolidCone(0.3, 1, 20 ,20);
				glPopMatrix();
			glPopMatrix();
			//PlaySound("inputs\\ShooterFire.wav",NULL, SND_ASYNC | SND_FILENAME);

			//judgement collision
			itr2 = blocks.begin();
			for ( ; itr2 != blocks.end(); itr2++)
			{
				if (collision(itr4->x, itr4->z, itr2->x, itr2->z))
				{
					blocks.erase(itr2);
					anbullets.erase(itr4);
					a = 1;
					break;
				}
			}
			if(a == 1) break;
			if (collision(itr4->x, itr4->z, sps[0].x, sps[0].z))
			{
				anbullets.erase(itr4);
				sps[0].lifeNum--;
				if(sps[0].lifeNum <= 0) 
				{
					//spaceship explode
					if (soundeffect == true) PlaySound("inputs\\Blast.wav",NULL, SND_ASYNC | SND_FILENAME);

					sppmodelExplode = glmReadOBJ("inputs/Space_Invader_explode.obj");
					if (!sppmodelExplode) exit(0);
					glmUnitize(sppmodelExplode);
					glmFacetNormals(sppmodelExplode);
					glmVertexNormals(sppmodelExplode, 90.0); //calculate smooth vertex normals of the model
					glTranslatef(sps[0].x, sps[0].y, sps[0].z);
					glRotatef(90, 0, 1,0); 
					glScalef(2.0, 2.0, 2.0);
					glPushMatrix();
						glColor3f(0.8, 1.0, 1.0);
					glPopMatrix();
					glmDraw(sppmodelExplode, GLM_SMOOTH | GLM_MATERIAL);

					Sleep(20);
					sps[0].x = -100;
				}
				break;
			}
			itr4->x += 0.03 * 10;
			//if(itr4->x >20) anbullets.erase(itr4);
		}
	}

	
	//add big alien bullets
	if((int)(timeOfCurrentFrame) % 211 == 0) //random condition
	{
		if((int)bigaliens.size() > 1)
		{
			itr6 = bigaliens.begin();
			struct biganbullet biganb1 = {itr6->x, 0.0, itr6->z};
			biganbullets.push_back(biganb1);
		}
	}

	//draw big alien bullets
	if((int)bigaliens.size() > 1)
	{
		itr5 = biganbullets.begin();
		for ( ; itr5 != biganbullets.end(); itr5++)
		{
			int a = 0;
			glPushMatrix();
			glTranslatef(itr5->x, 0.0, itr5->z);
				glPushMatrix();
					glRotatef(90, 0, 1,0);   
					glutSolidCone(0.3, 1, 20 ,20);
				glPopMatrix();
			glPopMatrix();

			//judgement collision
			itr2 = blocks.begin();
			for ( ; itr2 != blocks.end(); itr2++)
			{
				if (collision(itr5->x, itr5->z, itr2->x, itr2->z))
				{
					blocks.erase(itr2);
					biganbullets.erase(itr5);
					a = 1;
					break;
				}
			}
			if(a == 1) break;
			if (collision(itr5->x, itr5->z, sps[0].x, sps[0].z))
			{
				biganbullets.erase(itr5);
				sps[0].lifeNum--;
				if(sps[0].lifeNum <= 0) 
				{
					//spaceship explode
					if (soundeffect == true) PlaySound("inputs\\Blast.wav",NULL, SND_ASYNC | SND_FILENAME);

					sppmodelExplode = glmReadOBJ("inputs/Space_Invader_explode.obj");
					if (!sppmodelExplode) exit(0);
					glmUnitize(sppmodelExplode);
					glmFacetNormals(sppmodelExplode);
					glmVertexNormals(sppmodelExplode, 90.0); //calculate smooth vertex normals of the model
					glTranslatef(sps[0].x, sps[0].y, sps[0].z);
					glRotatef(90, 0, 1,0); 
					glScalef(2.0, 2.0, 2.0);
					glPushMatrix();
						glColor3f(0.8, 1.0, 1.0);
					glPopMatrix();
					glmDraw(sppmodelExplode, GLM_SMOOTH | GLM_MATERIAL);

					Sleep(20);
					sps[0].x = -100;
				}
				break;
			}
			itr5->x += 0.03 * 10;
			//if(itr5->x >20) anbullets.erase(itr5);
		}
	}

	//alien move backward and forward(z coordinate from 12~-8 to 8~-12)
	float anMaxz = -20.0, anMinz = 20.0;  //initial
	itr1 = aliens.begin();
	for ( ; itr1 != aliens.end(); itr1++)
	{
		if(itr1->z > anMaxz)
		{
			anMaxz = itr1->z;
		}
		if(itr1->z < anMinz)
		{
			anMinz = itr1->z;
		}			
	}

	int anNum = (anMaxz - anMinz) / 2.5 + 1; //the max number of aliens in a row
	float anAve = (anMaxz + anMinz) / 2; //the average z coordinate of aliens in a row
	float anWidth =2.5 * (anNum - 1); //the width of aliens

	if(anWidth >= 12)  //the width of aliens is bigger than 12
	{
		if(anMaxz <= anWidth - 12) 
		{
			direction = "left";
			count++;
		}
		if(anMaxz > 12)
		{
			direction = "right";
			count++;
		}
	}
	if(2.5 * (anNum - 1) < 12) //the width of aliens is smaller than 12
	{
		if(anMaxz <= -12 + anWidth) 
		{
			direction = "left";
			count++;
		}
		if(anMaxz > 12)
		{
			direction = "right";
			count++;
		}
	}

	if(count % 2 == 0 && count > 0)
		{
			itr1 = aliens.begin();
			for ( ; itr1 != aliens.end(); itr1++)
			{
				itr1->x += 1;
				/*if(itr1->x == 12) 
				{
					spexplodeframe = frame;
				}*/
			}
			speedcount++;
			count -=2;
		}

	if(direction == "right")
	{
		itr1 = aliens.begin();
		for ( ; itr1 != aliens.end(); itr1++)
		{
			//itr1->z += 0.5 // anNum;	
			itr1->z -= 0.05 + anmovespeed * speedcount;
			
		}
		itr6 = bigaliens.begin();
		itr6->z += 0.5;
	}
	
	if(direction == "left")
	{
		itr1 = aliens.begin();
		for ( ; itr1 != aliens.end(); itr1++)
		{
			//itr1->z -= 0.5 // anNum;	
			itr1->z += 0.05 + anmovespeed * speedcount;	
			
		}
		itr6 = bigaliens.begin();
		itr6->z -= 0.5;
	}


	//show text
	std::stringstream ss;
	std::string str;
	ss<<sps[0].lifeNum;
	ss>>str;
	std::stringstream ss1;
	std::string str1;
	ss1<<score;
	ss1>>str1;
	std::stringstream ss2;
	std::string str2;
	ss2<<levelNum;
	ss2>>str2;

	string text = "Scores:" + str1 + "    Levels: " + str2 + "  Lives:" + str;
	drawText(text.data(), text.size(), 384, 100);

	itr1 = aliens.begin();
	if(itr1->x == 12 )
	{
		if(sps[0].x == 12)
		{
			sps[0].x = -100;

			//spaceship explode
			if (soundeffect == true) PlaySound("inputs\\Blast2.wav",NULL, SND_ASYNC | SND_FILENAME);

			sppmodelExplode = glmReadOBJ("inputs/Space_Invader_explode.obj");
			if (!sppmodelExplode) exit(0);
			glmUnitize(sppmodelExplode);
			glmFacetNormals(sppmodelExplode);
			glmVertexNormals(sppmodelExplode, 90.0); //calculate smooth vertex normals of the model
			glTranslatef(sps[0].x, sps[0].y, sps[0].z);
			glRotatef(90, 0, 1,0); 
			glScalef(2.0, 2.0, 2.0);
			glPushMatrix();
				glColor3f(0.8, 1.0, 1.0);
			glPopMatrix();
			glmDraw(sppmodelExplode, GLM_SMOOTH | GLM_MATERIAL);

			Sleep(20);
		}
	}

	//Game Over(show text)
	itr1 = aliens.begin();
	if (score == 1000 || itr1->x >= 12 || sps[0].lifeNum == 0)
	{   //end game when all the aliens are gone
	
		
		if (soundeffect == true) PlaySound("inputs\\Blast2.wav",NULL, SND_ASYNC | SND_FILENAME);
		glPushMatrix();
			glColor3f(1.0 ,0.0 ,1.0);
		glPopMatrix();
		string text = "Game Over!!";
		PlaySound("inputs\\end.wav",NULL, SND_ASYNC | SND_FILENAME);
		drawText(text.data(), text.size(), 450, 200);
		
		return true;
	}
}

void display(void) 
{
	//timer
	timeOfCurrentFrame = clock();
	diff += (timeOfCurrentFrame - timeOfLastFrame)/100000;
	if(diff >= 0.05) diff = 0;
    timeOfLastFrame = timeOfCurrentFrame;

	//set background color, glClearColor should be put ahead.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	
	gluLookAt(20, 8, sps[0].z, 0, 0, sps[0].z, 0, 1, 0);

	result = Matrix(0.005, 1);

	glutSwapBuffers();
}

void reshape(int w, int h) //input the width and the height of window
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0 * w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
   // glViewport(0, 0, 1024, 576);

	// Set the correct perspective.
	gluPerspective(60, ratio, 1, 70);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) 
	{
		case 27:    //Use "Esc" to quit
			exit(0);
			break;
		case 's':    //turn on or turn off the soundeffect
			if(soundeffect == true) soundeffect = false;
			else if(soundeffect == false) soundeffect = true;
			printf("%d",(int)aliens.size());

			break;
		case ' ':    //Use space bar to shoot
			struct spbullet spb = {spbx, 0.0, spz};
			spbullets.push_back(spb);
			if (soundeffect == true) PlaySound("inputs\\ShooterFire2.wav",NULL, SND_ASYNC | SND_FILENAME);
			break;
	}
	redisplay_all();
}

void specialkeyboard (int key, int x, int y)
{
	switch (key) 
	{
		case GLUT_KEY_LEFT:
			 spz += 0.2;
			 sps[0].z += 0.2;
			 break;
		case GLUT_KEY_RIGHT:
			 spz -= 0.2;
			 sps[0].z -= 0.2;
			 break;
		/*case GLUT_KEY_UP:
			 yt += 0.1f;  
			 break;
		case GLUT_KEY_DOWN:
			 yt -= 0.1f;  
			 break;*/
		default:
			break;
	}redisplay_all();
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1024, 576);
	glutCreateWindow("Space Invader zhu6");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display); //this function is quite important
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeyboard);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
	glEnable(GL_CULL_FACE);

	redisplay_all();	
	glutMainLoop();
	return 0;
}

