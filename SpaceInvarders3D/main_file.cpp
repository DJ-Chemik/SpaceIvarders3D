#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"

using namespace std;

//klasa do rysowania pojedynczego wielokata
class Polygon {
    public:
        std::vector<float> vertices;
        std::vector<float> texCoords;
        std::vector<float> normals;
        int vertexCount;
        Polygon(std::vector<float> vertices, std::vector<float> texCoords, std::vector<float> normals) {
            this -> vertices = vertices;
            this -> texCoords = texCoords;
            this -> normals = normals;
            this -> vertexCount = vertices.size()/4;
        }
        void draw(ShaderProgram *sp);
};

//klasa do wczytania i rysowania modelu
class Model {
    public:
        std::vector< glm::vec3 > temp_vertices; //wspolrzedne kolejnych wierzchoĹ‚kĂłw w pliku
        std::vector< glm::vec2 > temp_text_coords; //kolejne wspolrzedne teksturowania z pliku
        std::vector< glm::vec3 > temp_normals; // kolejne wektory normalne z pliku

        //dane do rysowania wszystkich trojkatow modelu
        std::vector<float> trianglesVertices;
        std::vector<float> trianglesTexCoords;
        std::vector<float> trianglesNormals;

        //dane do rysowania wszystkich czworokatow modelu
        std::vector<float> quadsVertices;
        std::vector<float> quadsTexCoords;
        std::vector<float> quadsNormals;

        //dane do rysowania pozostalych wielokatow
        std::vector<Polygon> polygons;

        void loadOBJ(const char* path);
        void draw(ShaderProgram *sp);
    private:
        void loadGroup(FILE* file);
};

class Laser {
    public:
        float moveZ;
        float bulletSpeed = 5.0f;
        glm::mat4 shipModelMatrix;
        void draw(ShaderProgram *sp);
        void wystrzelenie();
};

void Laser::draw(ShaderProgram *sp) {
    glm::mat4 Ml = glm::translate(shipModelMatrix, glm::vec3(0.0f, 0.0f, moveZ));
    Ml = glm::scale(Ml, glm::vec3(0.1f, 0.1f, 2.5f));

    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(Ml));

    glEnableVertexAttribArray(sp->a("vertex"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,myCubeVertices); //WskaĹĽ tablicÄ™ z danymi dla atrybutu vertex

    glEnableVertexAttribArray(sp->a("normal"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,myCubeVertexNormals); //WskaĹĽ tablicÄ™ z danymi dla atrybutu normal

    glEnableVertexAttribArray(sp->a("texCoord0"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
    glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,myCubeTexCoords); //WskaĹĽ tablicÄ™ z danymi dla atrybutu texCoord0

    glDrawArrays(GL_TRIANGLES,0,myCubeVertexCount); //Narysuj obiekt

    glDisableVertexAttribArray(sp->a("vertex"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glDisableVertexAttribArray(sp->a("normal"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glDisableVertexAttribArray(sp->a("texCoord0"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
}

void Laser::wystrzelenie(){
    moveZ-=bulletSpeed;

}

class Enemy{
    public:
        static Model model;
        float moveZ;
        float shipSpeed = 0.05f;
        float shipDirectionMax=0.005f;
        glm::mat4 enemyModelMatrix;
        void draw(ShaderProgram *sp);


        void coming();
        const static float directionChange=0.005f;
        int direction = (rand()%3)+0; //0 - X , 1-Y, 2-XY
        int sign = (rand()%4)+0; //0,2-plus, 1,3-minus




};

void Enemy::draw(ShaderProgram *sp){
    glm::mat4 Menemy = glm::translate(enemyModelMatrix, glm::vec3(0.0f, 0.0f, moveZ));
    Menemy = glm::rotate(Menemy,-PI*0.15f,glm::vec3(1.0f,0.0f,0.0f));
    Menemy = glm::rotate(Menemy,-PI/2,glm::vec3(0.0f,1.0f,0.0f));
    Menemy = glm::rotate(Menemy,-PI/2,glm::vec3(1.0f,0.0f,0.0f));
    Menemy = glm::scale(Menemy, glm::vec3(0.0005f, 0.0005f, 0.0005f));

    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(Menemy));

    model.draw(sp);
}

void Enemy::coming(){

    if(direction==0)
    {
        if(sign==0 || sign==2)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(directionChange, 0.0f, 0.0f));
        }
        else if(sign==1 || sign==3)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(-directionChange, 0.0f, 0.0f));
        }
    }
    else if(direction==1)
    {
        if(sign==0 || sign==2)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(0.0f, directionChange, 0.0f));
        }
        else if(sign==1 || sign==3)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(0.0f, -directionChange, 0.0f));
        }
    }
    else if(direction==2)
    {
        if(sign==0)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(directionChange, directionChange, 0.0f));
        }
        else if(sign==1)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(directionChange, -directionChange, 0.0f));
        }
        else if(sign==2)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(-directionChange, directionChange, 0.0f));
        }
        else if(sign==3)
        {
            enemyModelMatrix = glm::translate(enemyModelMatrix, glm::vec3(-directionChange, -directionChange, 0.0f));
        }
    }



    moveZ-=shipSpeed;
}

float speed_x=0; //obroty kostki w poziomie
float speed_y=0; //obroty kostki w pionie
float speed_moveX=0;
float speed_moveY=0;


float aspectRatio=1;

ShaderProgram *sp;

Model spaceship;
Model Enemy::model;

//Uchwyty na tekstury
GLuint tex0;
GLuint tex1;
GLuint tex2;

std::vector<Laser> lasers;
std::vector<Enemy> enemies;
glm::mat4 currentShipMatrix;


void addLaser() {
    Laser laser;
    laser.moveZ = -2.0f;
    laser.shipModelMatrix = currentShipMatrix;
    lasers.push_back(laser);
}

//proces dodawania wrogiego statku
void addEnemy(){
    Enemy enemy;
    enemy.moveZ = 10.0f;
    glm::mat4 Mx=glm::mat4(1.0f);
    enemy.enemyModelMatrix = Mx;
    enemies.push_back(enemy);
}

//procedura która losowo dodaje wrogów
void addFoes(){
    int liczba = (rand() % 120)+0;
    if (liczba==1)
    {
        addEnemy();
    }

}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
        if (key==GLFW_KEY_D) speed_moveX=-5;
        if (key==GLFW_KEY_A) speed_moveX=5;
        if (key==GLFW_KEY_W) speed_moveY=5;
        if (key==GLFW_KEY_S) speed_moveY=-5;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
        if (key==GLFW_KEY_D) speed_moveX=0;
        if (key==GLFW_KEY_A) speed_moveX=0;
        if (key==GLFW_KEY_W) speed_moveY=0;
        if (key==GLFW_KEY_S) speed_moveY=0;
        if (key==GLFW_KEY_SPACE) addLaser();
    }
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}



//Funkcja wczytująca teksturę
GLuint readTexture(const char* filename) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0);

  //Wczytanie do pamięci komputera
  std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
  unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
  //Wczytaj obrazek
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import do pamięci karty graficznej
  glGenTextures(1,&tex); //Zainicjuj jeden uchwyt
  glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
  //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("vertex.glsl",NULL,"fragment.glsl");

    tex0=readTexture("statek.png");
    tex1=readTexture("laser.png");
    tex2=readTexture("statek2.png");

    spaceship.loadOBJ("prometheus.obj");
    Enemy::model.loadOBJ("ufo.obj");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);
    glDeleteTextures(1,&tex2);
    delete sp;
}

void drawObject(ShaderProgram *sp, GLenum mode, std::vector<float> vertices, std::vector<float> normals, std::vector<float> texCoords) {
    glEnableVertexAttribArray(sp->a("vertex"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,&(vertices[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu vertex

    glEnableVertexAttribArray(sp->a("normal"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,&(normals[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu normal

    glEnableVertexAttribArray(sp->a("texCoord0"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
    glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,&(texCoords[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu texCoord0

    glDrawArrays(mode,0,vertices.size()/4); //Narysuj obiekt

    glDisableVertexAttribArray(sp->a("vertex"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glDisableVertexAttribArray(sp->a("normal"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glDisableVertexAttribArray(sp->a("texCoord0"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
}

void Polygon::draw(ShaderProgram *sp) {
    drawObject(sp, GL_POLYGON, vertices, normals, texCoords);
}

void Model::draw(ShaderProgram *sp) {
    for(int i=0; i<polygons.size(); i++) {
        polygons[i].draw(sp);
    }

    drawObject(sp, GL_TRIANGLES, trianglesVertices, trianglesNormals, trianglesTexCoords);
    drawObject(sp, GL_QUADS, quadsVertices, quadsNormals, quadsTexCoords);
}

void Model::loadGroup(FILE * file) {
    while(1) {
        char lineHeader[128];
        char line[1024];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;
        if (strcmp(lineHeader, "v") == 0){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0){
            glm::vec2 vt;
            fscanf(file, "%f %f\n", &vt.x, &vt.y );
            temp_text_coords.push_back(vt);
        } else if (strcmp(lineHeader, "vn") == 0){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        } else if (strcmp(lineHeader, "f") == 0){
            std::vector<float> vertices;
            std::vector<float> texCoords;
            std::vector<float> normals;
            while(1) {
                int vertexIndex, texIndex, normalIndex;
                res = fscanf(file, "%d/%d/%d", &vertexIndex, &texIndex, &normalIndex);
                if(res != 3)
                    break;

                vertices.push_back(temp_vertices[vertexIndex-1].x);
                vertices.push_back(temp_vertices[vertexIndex-1].y);
                vertices.push_back(temp_vertices[vertexIndex-1].z);
                vertices.push_back(1.0f);
                texCoords.push_back(temp_text_coords[texIndex-1].x);
                texCoords.push_back(temp_text_coords[texIndex-1].y);
                normals.push_back(temp_normals[normalIndex-1].x);
                normals.push_back(temp_normals[normalIndex-1].y);
                normals.push_back(temp_normals[normalIndex-1].z);
                normals.push_back(0.0f);
            }
            if(vertices.size() == 12) {
                trianglesVertices.insert(trianglesVertices.end(), vertices.begin(), vertices.end());
                trianglesNormals.insert(trianglesNormals.end(), normals.begin(), normals.end());
                trianglesTexCoords.insert(trianglesTexCoords.end(), texCoords.begin(), texCoords.end());
            }
            else if(vertices.size() == 16) {
                quadsVertices.insert(quadsVertices.end(), vertices.begin(), vertices.end());
                quadsNormals.insert(quadsNormals.end(), normals.begin(), normals.end());
                quadsTexCoords.insert(quadsTexCoords.end(), texCoords.begin(), texCoords.end());
            }
            else {
                Polygon polygon(vertices, texCoords, normals);
                polygons.push_back(polygon);
            }
        } else if (strcmp(lineHeader, "g") == 0 || strcmp(lineHeader, "o") == 0){
            fscanf(file, "%s\n", lineHeader);
            break;
        }
    }
}

void Model::loadOBJ(const char* path) {
    FILE * file = fopen(path, "r");
    if(file == NULL){
        printf("Impossible to open the file!\n");
        exit(EXIT_FAILURE);
    }
    while(!feof(file)) {
        loadGroup(file);
    }
    fclose(file);
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y, float moveX, float moveY) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
         glm::vec3(0, 0, -5),
         glm::vec3(0,0,0),
         glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku

    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania


    glm::mat4 M=glm::mat4(1.0f);
    M=glm::translate(M,glm::vec3(moveX, 0.0f,0.0f));
    M=glm::translate(M,glm::vec3(0.0f,moveY,0.0f));
	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Wylicz macierz modelu
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz modelu
	M=glm::scale(M, glm::vec3(0.15f, 0.15f, 0.15f));
	currentShipMatrix = M;

    sp->use();//Aktywacja programu cieniującego
    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));
    glUniform4f(sp->u("lp"),0,0,-6,1); //Współrzędne źródła światła

    glUniform1i(sp->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex0);

    spaceship.draw(sp);

    glUniform1i(sp->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex1);

    for(int i=0; i<lasers.size(); i++) {
        lasers[i].wystrzelenie();
        lasers[i].draw(sp);
    }

    glUniform1i(sp->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex2);

     for(int i=0; i<enemies.size(); i++) {
        enemies[i].coming();
        enemies[i].draw(sp);
    }

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

void petlaGlowna(GLFWwindow* window)
{
    //Główna pętla
	float angle_x=PI; //Aktualny kąt obrotu obiektu
	float angle_y=0; //Aktualny kąt obrotu obiektu
    float moveX=0;
    float moveY=0;
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
        addFoes(); //dodaje wrogów w losowym czasie
        angle_x+=speed_x*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        angle_y+=speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        moveX+=speed_moveX*glfwGetTime();
        moveY+=speed_moveY*glfwGetTime();
        if(moveX > 2) moveX = 2;
        if(moveX < -2) moveX = -2;
        if(moveY > 2) moveY = 2;
        if(moveY < -2) moveY = -2;
        glfwSetTime(0); //Zeruj timer
		drawScene(window,angle_x,angle_y, moveX, moveY); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

}


int main(void)
{
    srand( time( NULL ) );

	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1920, 1080, "Space Invarders 3D", NULL, NULL);  //Utwórz okno 1920x1080 o tytule "Space Invarders 3D" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	petlaGlowna(window);

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
