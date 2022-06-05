#include <stdio.h>
#include <windows.h>
#include <gl/glut.h>
#include <math.h>

#define NUMBER_OF_POINT		100								// ���尡���� �ִ� ���ؽ� ����
#define WindowWidth			512
#define WindowHeight		512

float vertex[NUMBER_OF_POINT][2];								// ���ؽ� ��ǥ�� �����ϴ� �迭

int iDrawMode;												// ������Ƽ�� ���� (line, line-strip, triangle, quad �� ����) 
int iNumPoints;												// ������� �Էµ� ���ؽ��� ����
int index;

int iTransMode;
float trans_x,trans_y=0;


float glRed = 1.0f, glGreen = 1.0f, glBlue = 1.0f;

/* -------------------------------------------------

@ Display Callback Functions

   -------------------------------------------------*/
void ScanBitmapFile(FILE *fp,unsigned char *arr, int width, int height)
{										
	BITMAPFILEHEADER bfh;										// ��Ʈ�� ���� ó���� ���� ��� ����ü
	BITMAPINFOHEADER bih;										// ��Ʈ�� ���� ��� ����ü

	bih.biClrUsed  = 0;											// ������ ��
	bih.biBitCount = 24;										// 1�ȼ��� ���� ��Ʈ��
	bih.biWidth = width;										// ��
	bih.biHeight = height;										// ����
	bih.biXPelsPerMeter = 0;									// ���� �ػ�
	bih.biYPelsPerMeter = 0;									// ���� �ػ�
	bih.biPlanes = 1;											// ��Ʈ �÷��� ��(�׻� 1)
	bih.biCompression = BI_RGB;									// ���� ����
	bih.biSize = sizeof(BITMAPINFOHEADER);						// ����ü�� ũ��
	bih.biSizeImage = 3 * bih.biWidth * bih.biHeight;			// �̹��� �ȼ��� ũ��
	bih.biClrImportant = 0;										// �߿��� ���� �ε���


	bfh.bfType = 0x4d42;										// 16bit �̹��� Ÿ��
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfSize = sizeof(bfh) + sizeof(bih) + bih.biSizeImage;	// ������ü�� ũ��
	bfh.bfOffBits = sizeof(bfh) + sizeof(bih);					// �ȼ� ������ġ

	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);				// ���� ��� ����
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);				// ���� ��� ����
	fwrite(arr, 1, bih.biSizeImage, fp);									
}



void display()
{
	GLint index;
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(glRed, glGreen, glBlue);
	glPointSize(4);
	
	glBegin(GL_POINTS);
		for(index = 0; index < iNumPoints; index++)
		{
			glVertex2f(vertex[index][0], vertex[index][1]);
		}
	glEnd();

	glBegin(iDrawMode);
		for(index = 0; index < iNumPoints; index++)
		{
			glVertex2f(vertex[index][0], vertex[index][1]);
		}
	glEnd();
	glFlush();
}

/* -------------------------------------------------

@ Reshape Callback Functions

   -------------------------------------------------*/
void reshape(int _w, int _h)
{
	glViewport(0, 0, _w, _h);	
	
	// used perspective projection
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluOrtho2D(0.0, 500.0, 0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* -------------------------------------------------

@ Mouse Callback Functions

   -------------------------------------------------*/
void mouse(int _button, int _state, int _x, int _y)
{
	printf("Mouse Is Pressed! (%d, %d, %d, %d)\n",_button, _state, _x, _y);
	printf("%d \n",vertex[0][0]);
	if(_button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN)
	{
		vertex[iNumPoints][0] = _x;
		vertex[iNumPoints][1] = 512 - _y;
		iNumPoints++;
		glutPostRedisplay();
	}
}

/* -------------------------------------------------

@ Keyboard Callback Functions

   -------------------------------------------------*/
void keyboard(unsigned char _key, int _x, int _y)
{
	switch(_key)
	{
		// exit
		case 27:		//Esc
		case 'q':
		case 'Q':
			exit(0);
		break;
		
		// model ����
			
		case '1':
			iDrawMode = GL_POINTS;
			printf("You selected DrawMode 1 : POINT. \n");
			break;
		case '2':
			iDrawMode = GL_LINES;
			printf("You selected DrawMode 2 : LINE. \n");
			break;	
		case '3':
			iDrawMode = GL_LINE_STRIP;
			printf("You selected DrawMode 3 : LINE_STRIP. \n");
			break;
		case '4':
			iDrawMode = GL_LINE_LOOP;
			printf("You selected DrawMode 4 : LINE_LOOP. \n");
			break;
		case '5':
			iDrawMode = GL_POLYGON;
			printf("You selected DrawMode 5 : POLYGON. \n");
			break;
		case '6':
			iDrawMode = GL_TRIANGLES;
			printf("You selected DrawMode 6 : TRIANGLE. \n");
			break;
		case '7':
			iDrawMode = GL_TRIANGLE_STRIP;
			printf("You selected DrawMode 7 : TRIANGLE_STRIP. \n");
			break;
		case '8':
			iDrawMode = GL_TRIANGLE_FAN;
			printf("You selected DrawMode 8 : TRIANGLE_FAN. \n");
			break;
		case '9':
			iDrawMode = GL_QUADS;
			printf("You selected DrawMode 9 : QUAD. \n");
			break;
		case '0':
			iDrawMode = GL_QUAD_STRIP;
			printf("You selected DrawMode 0 : QUAD_STRIP. \n");
			break;

	// color ����

		case 'R':
		case 'r':
			glRed = 1.0f;
			glGreen = 0.0f;
			glBlue = 0.0f;
			printf("You selected Red Color.\n");
			break;

		case 'G':
		case 'g':
			glRed = 0.0f;
			glGreen = 1.0f;
			glBlue = 0.0f;
			printf("You selected Green Color.\n");
			break;

		case 'B':
		case 'b':
			glRed = 0.0f;
			glGreen = 0.0f;
			glBlue = 1.0f;
			printf("You selected Blue Color.\n");
			break;


		
	// �̵�, Ȯ��, ȸ��
		case 'T':
		case 't':
			printf("Trans mode\n");
			iTransMode=20;
			break;

		case'S':
		case's':
			printf("Scale mode\n");
			iTransMode=21;
			break;
		case'O':
		case'o':
			printf("rotate mode\n");
			iTransMode=22;
			break;

	// ȭ�� Ŭ����
		case 'C':
		case 'c':
			iNumPoints=0;
			break;	

	// ��Ʈ�� ����

		case'P':
		case'p':
			FILE *fp;
			fp=fopen("bit.bmp","wb");

			for(int i=0;i<iNumPoints;i++){
			}


			ScanBitmapFile(fp, (unsigned char *)vertex , WindowWidth, WindowHeight);
			fclose(fp);
			break;

		default:
			printf("You pressed %c\n", _key);
			break;
	}
	glutPostRedisplay();
}

void specialKey(int _key, int _x, int _y){
	float vert;
	switch(_key){
		case GLUT_KEY_LEFT :
			if(iTransMode==20){
				trans_x = -0.5;
				trans_y = 0;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x=0.5;
				trans_y=1;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}
			else if(iTransMode==22){
				for(int i=0;i<iNumPoints; i++){
					vert=vertex[i][0];
					vertex[i][0]= vertex[i][0]*cos(0.1)- (vertex[i][1]*sin(0.1)) ;
					vertex[i][1]= vert*sin(0.1)+vertex[i][1]*cos(0.1);
				}
			}
			glutPostRedisplay();
			break;

		case GLUT_KEY_RIGHT:
			if(iTransMode==20){
				trans_x = 0.5;
				trans_y = 0;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x= 1.5;
				trans_y= 1;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}

			else if(iTransMode==22){
				for(int i=0;i<iNumPoints; i++){
					vert=vertex[i][0];
					vertex[i][0]= vertex[i][0]*cos(0.1)- (vertex[i][1]* -sin(0.1));
					vertex[i][1]= vert* -sin(0.1)+vertex[i][1]*cos(0.1);
				}
			}
			glutPostRedisplay();
			break;

		case GLUT_KEY_UP:
			if(iTransMode==20){
				trans_x = 0;
				trans_y = 0.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x= 1;
				trans_y= 1.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}
			glutPostRedisplay();
			break;

		case GLUT_KEY_DOWN:
			if(iTransMode==20){
				trans_x = 0;
				trans_y = -0.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]+trans_x;
					vertex[i][1]= vertex[i][1]+trans_y;
				}
			}
			else if(iTransMode==21){
				trans_x= 1;
				trans_y= 0.5;
				for(int i=0;i<iNumPoints; i++){
					vertex[i][0]= vertex[i][0]*trans_x;
					vertex[i][1]= vertex[i][1]*trans_y;
				}
			}
			glutPostRedisplay();
			break;
	}	
}

/* -------------------------------------------------

@ Main Functions

   -------------------------------------------------*/

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);// used double buffers, RGBA and depth buffer
	glutInitWindowSize( WindowWidth, WindowHeight );				// default window size
	glutInitWindowPosition(512,512);			// default window position

	glutCreateWindow("Computer Graphics assignment_2 : Event Handler");
	
	// set callback functions
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutDisplayFunc(display);

	glutMainLoop();
	return 0;
}