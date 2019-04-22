#include "pch.h"
#include <iostream>
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<stdlib.h>
#include<string>
#include<stack>
#include<vector>
using namespace std;

struct Grupo
{
	float transX, transY, transZ, rotX, rotY, rotZ, rotAngle, scaleX, scaleY, scaleZ;
	std::string modelos;
	int nNomes = 0, nGrupos = 0;
	std::vector<float> vertices;
};

struct Status;

struct Edge
{
	char MatchContent;
	Status* Start;
	Status* End;
};

struct Status
{
	vector<Edge*> InEdges;
	vector<Edge*> OutEdges;
	bool FinalStatus;
};


/*int main()
{
	Grupo grupo;
	grupo.vertices.push_back(1);


	Edge* ee;
	Status erha;
	erha.InEdges.push_back(ee);


	int a = 1 + 2;

}*/