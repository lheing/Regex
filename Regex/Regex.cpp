// Regex.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<stdlib.h>
#include<string>
#include<stack>
#include<set>
#include<vector>
#include<queue>
#include<map>
using namespace std;

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

set<Edge*> Edges;
set<Status*> Statuses;
set< set<Status*>> subSet;
set<Status*> Closure;
queue<set<Status*>> q;
map<set<Status*>, int> Map;
map<int, set<Status*>> MapReverse;
map<char, int> Char;
map<int, char> CharReverse;
int transition[205][205];
int statusCount = 0;
int CharCount = 0;


void printSet(set<Status*> s)
{
	set<Status*>::iterator it;
	vector<Edge*>::iterator vit;
	for (it = s.begin(); it != s.end(); it++)
	{
		cout << "(";
		for (vit = (*it)->OutEdges.begin(); vit != (*it)->OutEdges.end(); vit++)
		{
			cout << (*vit)->MatchContent << " ";
		}
		cout << ")";
	}
	cout << endl;
}

void printGraph(Edge *start, int n)
{
	Status *End = start->End;

	Edges.insert(start);
	cout << n << "   " << start->MatchContent << endl;

	if (End==NULL || End->OutEdges.empty())
		return;

	vector<Edge*>::iterator it;
	for (it = End->OutEdges.begin(); it != End->OutEdges.end(); it++)
	{

		if (Edges.find(*it) == Edges.end())
			printGraph(*it, n + 1);
	}
}


bool isCharacter(char c)
{
	if ((c >= 'A'&&c <= 'Z')
		|| (c >= 'a'&&c <= 'z')
		|| (c >= '0'&&c <= '9'))
		return true;
	return false;
}

void Find(Status* now)
{
	Closure.insert(now);

	vector<Edge*>::iterator it;
	for (it = now->OutEdges.begin(); it != now->OutEdges.end(); it++)
	{
		if ((*it)->MatchContent == '#') Find((*it)->End);
	}
	return;
}

set<Status*> closure(Status* now)
{
	Closure.clear();
	Find(now);
	return Closure;
}

set<Status*> Union(set<Status*> a, set<Status*> b)
{
	set<Status*>::iterator it;

	for (it = b.begin(); it != b.end(); it++)
	{
		a.insert(*it);
	}
	return a;
}

void FindCharStatus(set<Status*> closure1)
{
	set<Status*>::iterator cit;
	set<char> character;

	for (cit = closure1.begin(); cit != closure1.end(); cit++)
	{
		Status* start = *cit;
		vector<Edge*>::iterator it;
		

		for (it = start->OutEdges.begin(); it != start->OutEdges.end(); it++)
		{
			if (isCharacter((*it)->MatchContent))
			{
				char c = (*it)->MatchContent;

				set<Status*> qi = closure((*it)->End);
				

				if (character.find(c) == character.end())
				{
					character.insert(c);
					if (Map[qi] == NULL)
					{
						Map[qi] = ++statusCount;
						MapReverse[statusCount] = qi;
					}

					if (Char[c] == NULL)  //离散化
					{
						Char[c] = ++CharCount;
						CharReverse[CharCount] = c;
					}
				}
				else if (Char[c] != NULL && transition[Map[closure1]][Char[c]] != -1)
				{
					
					int statusCnt = transition[Map[closure1]][Char[c]];
					set<Status*> preq = MapReverse[statusCnt];

					map<set<Status*>, int>::iterator mit = Map.find(preq);
					if (mit != Map.end()) Map.erase(mit);

					map<int, set<Status*>>::iterator mrit = MapReverse.find(statusCnt);
					if (mrit != MapReverse.end()) MapReverse.erase(mrit);

					qi = Union(preq, qi);
					Map[qi] = statusCnt;
					MapReverse[statusCnt] = qi;
				}

				transition[Map[closure1]][Char[c]] = Map[qi];

			}
		}

		
	}

	set<char>::iterator scit;
	for (scit = character.begin(); scit != character.end(); scit++)
	{
		if (transition[Map[closure1]][Char[*scit]] != -1)
		{
			set<Status*> qi = MapReverse[transition[Map[closure1]][Char[*scit]]];
			if (subSet.find(qi) == subSet.end())
			{
				subSet.insert(qi);
				q.push(qi);
			}
		}

	}
}

void destroyGraph()
{

}

Edge* Construction()
{
	Edge* start = new Edge;
	map<Status*, int> subStatus;
	map<int, Status*> subStatusReverse;
	int cnt = 0;

	Status* init = new Status;
	subStatus[init] = cnt;
	subStatusReverse[cnt] = init;

	start->MatchContent = '#';
	start->Start = NULL;
	start->End = init;

	init->InEdges.push_back(start);
	init->FinalStatus = false;

	set<Status*>::iterator it;
	for (it = MapReverse[subStatus[init]].begin(); it != MapReverse[subStatus[init]].end(); it++)
	{
		if ((*it)->FinalStatus == true)
		{
			init->FinalStatus = true;
			
		}
	}


	for (int i = 0; i <= statusCount; i++)
	{
		for (int j = 0; j <= CharCount; j++)
		{
			if (transition[i][j] != -1)
			{
				if (subStatusReverse[i] == NULL)
				{
					Status* tmp = new Status;
					subStatus[tmp] = ++cnt;
					subStatusReverse[cnt] = tmp;
				}
				if (subStatusReverse[transition[i][j]] == NULL)
				{
					Status* tmp = new Status;
					subStatus[tmp] = ++cnt;
					subStatusReverse[cnt] = tmp;
				}

				Status * s = subStatusReverse[i];
				Status * e = subStatusReverse[transition[i][j]];

				//s->FinalStatus = false;
				e->FinalStatus = false;

				set<Status*>::iterator it;
				for (it = MapReverse[ subStatus[e] ].begin(); it != MapReverse[subStatus[e]].end(); it++)
				{
					if ((*it)->FinalStatus == true)
					{
						e->FinalStatus = true;
					}
				}

				Edge* conn = new Edge;
				conn->Start = s;
				conn->End = e;
				conn->MatchContent = CharReverse[j];

				s->OutEdges.push_back(conn);
				e->InEdges.push_back(conn);
			}
		}
	}

	Edges.clear();
	return start;
}


Edge* subSetConstruction(Edge *start)
{
	statusCount = 0;
	CharCount = 0;
	memset(transition, -1, sizeof(transition));

	subSet.clear();
	Map.clear();
	MapReverse.clear();
	Char.clear();
	CharReverse.clear();

	set<Status*> q0 = closure(start->End);
	q.push(q0);
	subSet.insert(q0);
	Map[q0] = statusCount;
	MapReverse[statusCount] = q0;

	while (!q.empty())
	{
		set<Status*> tmp = q.front();
		q.pop();
		FindCharStatus(tmp);
	}
	
	Edge* result = Construction();

	destroyGraph();
	return result;
}

Edge* singleCharGraph(char c)
{
	Status* Init = new Status;
	Status* Final = new Status;
	Edge* start = new Edge;
	Edge* Char = new Edge;

	Init->InEdges.push_back(start);
	Init->OutEdges.push_back(Char);
	Init->FinalStatus = false;

	Final->InEdges.push_back(Char);
	Final->OutEdges.clear();
	Final->FinalStatus = true;

	Char->MatchContent = c;
	Char->Start = Init;
	Char->End = Final;

	start->MatchContent = '#';
	start->Start = NULL;
	start->End = Init;

	return start;
}

void addEdge(Status* now, Status *Start, Status* Final)
{
	Statuses.insert(now);
	if ( now->FinalStatus == true )
	{
		Edge* s = new Edge;
		Edge* f = new Edge;
		s->MatchContent = '#';
		s->Start = now;
		s->End = Start;
		f->MatchContent = '#';
		f->Start = now;
		f->End = Final;
		now->OutEdges.push_back(s);
		now->OutEdges.push_back(f);
		now->FinalStatus = false;
		Start->InEdges.push_back(s);
		Final->InEdges.push_back(f);
		return;
	}

	vector<Edge*>::iterator  it;
	for (it = now->OutEdges.begin(); it != now->OutEdges.end(); it++)
	{
		if(Statuses.find((*it)->End) == Statuses.end())addEdge((*it)->End, Start, Final);
	}
}

Edge* Kleene(Edge* a)
{
	Statuses.clear();
	Edge *start = new Edge;
	Edge *I2F = new Edge;
	Status* Init = new Status;
	Status* Final = new Status;

	a->Start = Init;

	start->MatchContent = '#';
	start->Start = NULL;
	start->End = Init;

	I2F->MatchContent = '#';
	I2F->Start = Init;
	I2F->End = Final;

	Init->InEdges.push_back(start);
	Init->OutEdges.push_back(a);
	Init->OutEdges.push_back(I2F);
	Init->FinalStatus = false;

	Final->InEdges.push_back(I2F);
	Final->FinalStatus = true;

	addEdge(a->End, a->End, Final);

	return start;
}

void connect(Status* now, Edge* b)
{
	if (now->FinalStatus == true)
	{
		now->OutEdges.push_back(b);
		b->Start = now;
		now->FinalStatus = false;
		return;
	}
	
	vector<Edge*>::iterator it;
	for (it = now->OutEdges.begin(); it != now->OutEdges.end(); it++)
	{
		connect((*it)->End, b);
	}
}

Status* choose(Status* now, Edge* b)
{
	Statuses.insert(now);
	if (now->FinalStatus == true)
	{
		now->FinalStatus = false;
		now->OutEdges.push_back(b);
		b->Start = now;
		return now;
	}

	vector<Edge*>::iterator it;
	for (it = now->OutEdges.begin(); it != now->OutEdges.end(); it++)
	{
		if(Statuses.find((*it)->End) == Statuses.end())
			choose( (*it)->End, b );
	}
}

Edge* AND(Edge* a, Edge* b)
{
	connect(a->End, b);
	return a;
}

Edge* OR(Edge* a, Edge* b)
{
	Edge* start = new Edge;
	Edge* ToFinalA = new Edge;
	Edge* ToFinalB = new Edge;
	Status* Init = new Status;
	Status* Final = new Status;

	Init->InEdges.push_back(start);
	Init->OutEdges.push_back(a);
	Init->OutEdges.push_back(b);
	Init->FinalStatus = false;

	a->Start = Init;
	b->Start = Init;

	Final->InEdges.push_back(ToFinalA);
	Final->InEdges.push_back(ToFinalB);
	Final->FinalStatus = true;

	start->Start = NULL;
	start->End = Init;
	start->MatchContent = '#';

	ToFinalA->End = Final;
	ToFinalA->MatchContent = '#';

	ToFinalB->End = Final;
	ToFinalB->MatchContent = '#';

	Statuses.clear();
	choose(a->End, ToFinalA);
	Statuses.clear();
	choose(b->End, ToFinalB);

	return start;
}

Edge* Thompson(string Regex)
{
	stack<Edge*> Character;
	stack<char> Operator;

	size_t len = Regex.size();

	Edge* NFA = new Edge;

	for (size_t i = 0; i < len; i++)
	{
		if (isCharacter(Regex[i]))
		{
			Edge* Char = singleCharGraph(Regex[i]);

			if (i > 0)
			{
				if(isCharacter(Regex[i - 1]) || Regex[i - 1] == ')')
					Operator.push('+');
			}
			Character.push(Char);
		}
		else if (Regex[i] == '*')
		{
			Edge* tmp = Character.top();
			Edge* klee = Kleene(tmp);
			Character.pop();
			Character.push(klee);
		}
		else if (Regex[i] == '|')
		{
				while (!Operator.empty() && Operator.top() == '+')
				{
					Edge* a = Character.top();
					Character.pop();
					Edge* b = Character.top();
					Character.pop();
					Operator.pop();
					Edge* con = AND(b, a);
					Character.push(con);
				}
			
			Operator.push('|');
		}
		else if (Regex[i] == '(')
		{
			if (i > 0)
			{
				if (isCharacter(Regex[i - 1]) || Regex[i - 1] == ')')
					Operator.push('+');
			}
			Operator.push('(');
		}
		else if (Regex[i] == ')')
		{
			while (Operator.top() != '(')
			{
				if (Operator.top() == '+')
				{
					Edge* a = Character.top();
					Character.pop();
					Edge* b = Character.top();
					Character.pop();
					Edge* con = AND(b, a);
					Character.push(con);
				}
				else if (Operator.top() == '|')
				{
					Edge* a = Character.top();
					Character.pop();
					Edge* b = Character.top();
					Character.pop();
					Edge* orz = OR(b, a);
					Character.push(orz);
				}
				Operator.pop();
			}
			Operator.pop();
		}
	}

		/*while (!Character.empty())
		{
			vector<Edge*>::iterator it;
			for (it = Character.top()->End->OutEdges.begin(); it != Character.top()->End->OutEdges.end(); it++)
				cout << (*it)->MatchContent << endl;
			Character.pop();
		}*/

	while (!Operator.empty())
	{
		if (Operator.top() == '+')
		{
			Edge* a = Character.top();
			Character.pop();
			Edge* b = Character.top();
			Character.pop();
			Edge* con = AND(b, a);
			Character.push(con);
		}
		else if (Operator.top() == '|')
		{
			Edge* a = Character.top();
			Character.pop();
			Edge* b = Character.top();
			Character.pop();
			Edge* orz = OR(b, a);
			Character.push(orz);
		}
		Operator.pop();
	}

	if (Character.size() == 1)
	{
		NFA = Character.top();
		Character.pop();
	}
	return NFA;
}


void Rev(Status* now, Edge* newStart)
{
	if (Statuses.find(now) != Statuses.end())
	{
		return;
	}
	Statuses.insert(now);
	vector<Edge*>::iterator it;

	for (it = now->OutEdges.begin(); it != now->OutEdges.end(); it++)
	{
		if (Edges.find(*it) == Edges.end())
		{
			Edges.insert((*it));
			Rev( (*it)->End, newStart);
			swap((*it)->End, (*it)->Start);
		}
	}
	if (now->InEdges.empty() && now->OutEdges.empty())
	{
		int a;
	}
	else if (now->InEdges.empty() && !now->OutEdges.empty())
	{
		now->InEdges = now->OutEdges;
		now->OutEdges.clear();
	}
	else if (!now->InEdges.empty() && now->OutEdges.empty())
	{
		now->OutEdges = now->InEdges;
		now->InEdges.clear();
	}
	else
	{
		//swap(now->InEdges, now->OutEdges);
		vector<Edge*> tmp;
		tmp = now->InEdges;
		now->InEdges = now->OutEdges;
		now->OutEdges = tmp;
	}
	
	if (now->FinalStatus == true)
	{
		newStart->Start = NULL;
		now->InEdges.push_back(newStart);
		newStart->End = now;
		now->FinalStatus = false;
	}
	return;
}

void addFinalEdge(Status* now, Status* init)
{
	Statuses.insert(now);
	if (now->FinalStatus == true)
	{
		Edge* e = new Edge;
		e->MatchContent = '#';
		e->Start = now;
		e->End = init;
		init->InEdges.push_back(e);
		now->FinalStatus = false;
		now->OutEdges.push_back(e);
	}

	vector<Edge*>::iterator it;

	for (it = now->OutEdges.begin(); it != now->OutEdges.end(); it++)
	{
		if (Statuses.find((*it)->End) == Statuses.end())
		{
			addFinalEdge((*it)->End, init);
		}
	}
}

Edge* reverse(Edge* start)
{
	Status* init = start->End;
	

	Status* newInit = new Status;
	newInit->FinalStatus = true;
	newInit->OutEdges.clear();
	Statuses.clear();
	Statuses.insert(newInit);
	addFinalEdge(init, newInit);

		
	Edge* newStart = new Edge;
	newStart->MatchContent = '#';
	init->InEdges.clear();

	Edges.clear();
	Statuses.clear();
	Rev(init,newStart);

	init->FinalStatus = true;

	free(start);

	return newStart;
}

map<Status*, int> StatusNum;
int Ch[255][255];
int inputChar[255];
set<Status*> Number;
int inputCharCnt;
int StatusNumCnt;
bool Final[255];
;
void handleDFA(Edge* s)
{
	Edges.insert(s);
	
	Status* now = s->End;

	if (Number.find(now) == Number.end())
	{
		StatusNum[now] = StatusNumCnt;
		cout << StatusNumCnt << endl;
		StatusNumCnt++;
	}

	if (now->FinalStatus == true)
	{
		Final[StatusNum[now]] = true;
	}

		Number.insert(now);
		if (s->MatchContent != '#')
		{
			char c = s->MatchContent;
			if (inputChar[c] == -1)
			{
				inputChar[c] = ++inputCharCnt;
			}
			Ch[StatusNum[s->Start]][ inputChar[c] ] = StatusNum[s->End];
			//cout << StatusNum[s->Start] << ", " << inputChar[c] << ", " << StatusNum[s->End];
		}

	vector<Edge*>::iterator it;

	for (it = now->OutEdges.begin(); it != now->OutEdges.end(); it++)
	{
		if (Edges.find(*it) == Edges.end())
		{
			handleDFA(*it);
		}
	}
}

string Match(string Str)
{
	int next = 0;
	string result = "";
	for (int i = 0; i < Str.size(); i++)
	{
		char c = (char)Str[i];
		/*if (Final[next] == true)
		{
			return result;
		}*/
		if ( Ch[next][inputChar[c] ]== -1 )
		{
			printf("无法匹配！！！");
			return "";
		}
		result = result + Str[i];

		next = Ch[next][ inputChar[c] ];
	}
	return result;
}

int main()
{
	string mainString;
	string Regex;
	cout << "请输入字符串" << endl;
	cin >> mainString;
	cout << "请输入正则表达式" << endl;
	cin >> Regex;
	Edge* NFA = Thompson(Regex);
	Edge* Reverse = reverse(NFA);

	

	Edge* DFA = subSetConstruction(Reverse);
	Reverse = reverse(DFA);
	DFA = subSetConstruction(Reverse);

	//Edges.clear();
	//printGraph(DFA, 1);
	
	Edges.clear();
	inputCharCnt = 0;
	StatusNumCnt = 0;
	memset(Ch, -1, sizeof(Ch));
	memset(inputChar, -1, sizeof(inputChar));
	handleDFA(DFA);

	/*for (int i = 0; i < 5; i++)
	{
		cout << Ch[i][1] << " " << Ch[i][2] << " " << Ch[i][3] << endl;
	}*/
	
	string result = Match(mainString);

	cout << result << endl;

	
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
