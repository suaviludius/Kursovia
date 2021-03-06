﻿#pragma warning(disable:4996)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#define N 1000
#define M 100
#define PROPUSK (a[str][stol][0] == '\t' || a[str][stol][0] == ' ' || a[str][stol][0] == ',')

#define COM		if (a[str][stol][0] == '"' && a[str][stol - 1][0] != '\\' && coment != 1)\
				if(coment == 2) coment = 0;\
				else coment = 2;\
				if (coment == 2) continue;\
				\
				if (a[str][stol][0] == '/' && a[str][stol + 1][0] == '*')\
				{coment = 1; stol++;}\
				if (a[str][stol][0] == '*' && a[str][stol + 1][0] == '/') coment = 0;\
				if (a[str][stol][0] == '/' && a[str][stol + 1][0] == '/') break;\
				if (coment == 1) continue;
/*
coment = 0 - без коментариев
coment = 1 - коментарий вида /*
coment = 2 - строковая константа //
*/

/*
Помарки:
1) UNDEF в данной программе действителен только для открытого применения, без занесения его как выражение в условную конструкцию.
2) В условных конструкциях препроцессора считаются только 2 слагаемых, если их больше, результат воспринимается как "false".
3) В условных конструкциях препроцессора не считаются унарные операции, инкремент и дикремент.
4) В условных конструкциях препроцессора определены ТОЛЬКО операции сложения, вычитания, умножения, деления, модуля и XOR. 
5) Отсутствует определение #if defined (аналога #ifdef) 
6) Отсутствует определение склеивания лексем ##
*/
/*
Решения:
1) Программа заменяет контекстно макроопределения как обычные, так и функциональные 
2) Учтены все виды коментариев и констант
3) Реализованны условия #ifdef, #ifndef, #if, #elif, #else, #endif, а также #undef(с ограничениями)
4) Выполнено условие неограниченной вложенности условной трансляции
5) При считывании макроопределения учитывается его перенос на следующую строку.
6) Учтены вложения однго функционального определения в другое (НО только для использования при повторении в одной строке)
7) 
*/

void HUSH(int flag, int i);
void define(int str, int stol);
void rem_def(int str, char* arr_for, char* nam_fun, int* per_pos);
int if_(int str, int stol);
int ifdef (int str, int stol, int one);
void include(int str, int stol);

char*** a = NULL;
int* len_str = NULL;
int qu_str = 0;

int main(int argc, char* argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	FILE* f;
	FILE* fpout = fopen("D:\\Kursovai_text\\Creator.txt", "wt");
	char filename[100] = { 0 }; //имя файла

	if (argc == 2)
		strcpy(filename, argv[1]);
	while ((f = fopen(filename, "r")) == NULL)
	//Открытие файла для чтения
	{
		printf("Cannot open input file.\n");
		printf("Please enter a file address: ");
		gets_s(filename);
	}

	int str = 0; //строки
	int stol = 0; //столбики
	int sim = 0; //символы массива
	int t = 0;//переход по символам считывающей строки

	len_str = (int*)calloc(N, sizeof(int)); //размерности строк 
	a = (char***)calloc(N,sizeof(char**)); // массив слов строк

	while (!feof(f)) 
	{
		bool flag = 0;
		char line[N];// чтение строки
		char* ptr = fgets(line, N, f);
		a[str] = (char**)calloc(N, sizeof(char*));

		if (ptr == NULL)
			break; 

		while (ptr[t] != '\0')
		{
			a[str][stol] = (char*)calloc(M, sizeof(char));

			while (ptr[t] != ' ' && ptr[t] != '(' && ptr[t] != ')' && ptr[t] != ',' && ptr[t] != ';'
				&& ptr[t] != '+' && ptr[t] != '-' && ptr[t] != '=' && ptr[t] != '*' && ptr[t] != '/'
				&& ptr[t] != '^' && ptr[t] != '%' && ptr[t] != '!' && ptr[t] != '@' && ptr[t] != '"'
				&& ptr[t] != '&' && ptr[t] != '|' && ptr[t] != '$' && ptr[t] != '~' && ptr[t] != '.'
				&& ptr[t] != '[' && ptr[t] != ']' && ptr[t] != '\0' && ptr[t] != -1 && ptr[t] != '\n'
				&& ptr[t] != '\t' && ptr[t] != '>' && ptr[t] != '<' && ptr[t] != '#' && ptr[t] != '\\')
			{
				a[str][stol][sim++] = ptr[t++];
				flag = 1;
			}

			if (flag == 1)
			{
				a[str][stol][sim++] = '\0';
				a[str][stol] = (char*)realloc(a[str][stol], sim + 1);
			}
			else
			{
				a[str][stol][0] = ptr[t++];
				a[str][stol][1] = '\0';	
			}

			stol++;
			sim = 0;
			flag = 0;
		}

		a[str] = (char**)realloc(a[str], stol * sizeof(char*));
		len_str[str] = stol;
		str++;
		stol = 0;
		t = 0;
	}
	a = (char***)realloc(a, str * sizeof(char**));
	len_str = (int*)realloc(len_str, str * sizeof(int));
	qu_str = str;
	fclose(f);
	HUSH(0, 0);

	for (int str = 0; str < qu_str; str++)
	{
		for (int stol = 0; stol < len_str[str]; stol++)
		{
			if (a[str][0][0] != '.')
				printf("%s", a[str][stol]);
				//fprintf(fpout, "%s", a[str][stol]);
			else break;
		}
	}
	free(a);
	return 0;
}

void HUSH(int flag, int i) {
	const char* func[] = { "define", "if", "ifdef", "ifndef", "elif", "else", "endif","include" }; // константы #

	bool into = 0; //определение просмотра вложенной директивы (0 - смотри, 1 - пропуск)
	int def = 0; // определитель вложенности
	int coment = 0; //наличие коментария
	/*
	flag = -1 - не выполнилась #elif()
	flag = 0 - выполнилось условие
	flag != 0 && flag != -1 - не выполнилось условие #if()
	*/
	for (int str = 0; str < qu_str; str++)
	{
		for (int stol = 0; stol < len_str[str]; stol++)
		{
			COM
				if (a[str][stol][0] == '#')
				{
					stol++;
					if (into == 0)
						for (int func_elem = 0; func_elem < 8; func_elem++)
						{
							if (!(strcmp(a[str][stol], func[func_elem])))
							{
								switch (func_elem) {
								case 1: if (flag == 0) flag = if_(str, stol);
									  else  into = 1;
									def = 1;
									break;
								case 0: if (flag == 0) define(str, ++stol); break;
								case 2: if (flag == 0) flag = ifdef(str, stol, 0);
									  else  into = 1;
									def = 1;
									break; //ifdef
								case 3: if (flag == 0) flag = ifdef(str, stol, 1);
									  else  into = 1;
									def = 1;
									break; //ifndef
								case 4: if (flag != 0 && flag != -1) flag = if_(str, stol);
									  else flag = -1;
									break; //elif(a)
								case 5: if (flag != 0 && flag != -1) flag = 0;
									  else if (flag == 0) flag = -1;
									break; //else
								case 6: flag = 0;  break; //endif(a)
								case 7: include(str, ++stol); break; //include
								default:  break;
								}
								if(strcmp(a[str][stol-1], func[7]))a[str][0][0] = '.';
								break;
							}
						}
					else if (!(strcmp(a[str][stol], func[6])))
					{
						if (--def == 0)
							into = 0;
					}
					else if (into == 1)
						for (int func_elem = 1; func_elem < 4; func_elem++)
						{
							if (!(strcmp(a[str][stol], func[func_elem])))
							{
								def++;
							}
						}
				}
			if (a[str][stol][0] != '#'  && flag != 0 || into == 1)
			{
				a[str][0][0] = '.';
				continue;
			}
		}
	}
}

void include(int str, int stol) {
	char* nam_fun = (char*)calloc(32, sizeof(char)); // имя функции

	int flag = 0;
	int undef = 0;
	int coment = 0;
	for (str; str < qu_str; str++)
	{
		undef = 0;
		for (stol; stol < len_str[str]; stol++)
		{
			if PROPUSK continue;

			if (flag == 0)
			{
				if (a[str][stol][0] == '"' || a[str][stol][0] == '<') stol++;
				nam_fun = a[str][stol];
				flag = 1;
			}

			COM

				if (!(strcmp(a[str][stol], "ifdef")) || !(strcmp(a[str][stol], "ifndef")) || !(strcmp(a[str][stol], "undef")))
				{
					if (!(strcmp(a[str][stol++], "undef"))) undef = 1;
					for (stol; stol < len_str[str]; stol++)
					{
						if (a[str][stol][0] == '\t' || a[str][stol][0] == ' ' || a[str][stol][0] == ',') continue;
						if (undef == 0 && !(strcmp(a[str][stol], nam_fun))) a[str][stol][0] = '.';
						else if (undef == 1 && !(strcmp(a[str][stol], nam_fun))) undef = 2;
						break;
					}
				}
		}
		if (undef == 2) break;
		stol = 0;
	}

}

void define(int str, int stol)
{
	char* arr_for = (char*)calloc(N, sizeof(char)); // массив с формулой макроопределения
	char** per_fun = (char**)calloc(N, sizeof(char*)); //переменные функции
	int* per_pos = (int*)calloc(M, sizeof(int)); // массив с позициями переменных замены
	char* nam_fun = (char*)calloc(32, sizeof(char)); // имя функции
	char* buffer = (char*)malloc(32); // слово читается до 32 символов

	int pos_for = 0; //позиции переменных в функцим
	int pos_fun = 0; //для занесения в массив переменных
	int funFlag = 0; //начальный определитель функционального макроопределения
	int flag = -1;
	int coment = 0;
	int pos = 0; //per_pos[pos]

	/*
	flag = -1 начало
	flag = 0 поиск имени функции
	flag = 1 переменные функции
	flag = 2 формула для замены макроопределения
	flag = 3 не функция
	*/

	for (stol; stol < len_str[str]; stol++)
	{
		COM
			if (a[str][stol][0] == '\\' && a[str][stol+1][0] == '\n')
			{
				str++; stol = 0;
			}
		funFlag = 0;

		if (flag != 3 && flag != 2 && PROPUSK) continue;
		if (flag == -1) {
			nam_fun = a[str][stol++];
			flag = 0;
			funFlag = 1;
		}

		if ((flag == 2 || flag ==3) && a[str][stol] != "\n") {
			int y = 0;
			int i = 0;
			for (y = 0; y < pos_fun; y++)
				if (!strcmp(a[str][stol], per_fun[y]))
				{
					per_pos[pos++] = pos_for;
					char* buffer = (char*)malloc(32);
					arr_for[pos_for++] = *itoa(y, buffer, 10) + 1;
					free(buffer);
					break;
				}
			if(y == pos_fun)while(a[str][stol][i]!='\0' && a[str][stol][i] != '\n') arr_for[pos_for++] = a[str][stol][i++];
		}

		if (a[str][stol][0] == '(' && flag == 0 && funFlag == 1) {
			flag = 1; stol++; 
		}
		else if (flag == 0) {
			flag = 3;
			continue;
		}
		if (a[str][stol][0] == ')' && flag == 1) { flag = 2; while(a[str][stol+1][0] == '\t' || a[str][stol+1][0] == ' ' || a[str][stol+1][0] == ',') stol++; }
		if (flag == 1) per_fun[pos_fun++] = a[str][stol];
		a[str][0][0] = '.';
	}
	per_fun = (char**)realloc(per_fun, (pos_fun + 1) * sizeof(char*));
	arr_for = (char*)realloc(arr_for, (pos_for + 1) * sizeof(char));
	per_pos = (int*)realloc(per_pos, pos * sizeof(int) + 1);
	/*printf("(%s) ", nam_fun);
	printf("(%s) ", arr_for);
	for (int i = 0; i < pos_fun; i++)
		printf("(%s) ", per_fun[i]);
	printf("\n");
	*/
	rem_def(str + 1, arr_for, nam_fun, per_pos);
	free(buffer);
	free(nam_fun);
	free(per_fun);
	free(arr_for);
	free(per_pos);
}

void rem_def(int str, char* arr_for, char* nam_fun, int* per_pos) {
	char** per_fun = (char**)calloc(M, sizeof(char*)); //имена переменных функции
	char** sav_ost = (char**)calloc(M, sizeof(char*)); // сохранение символов после функции
	int str_stol = 0; // позиция перед заменой макроопределения

	int pos_for = 0; //позиции переменных в функцим
	int pos_fun = 0; //для занесения в массив переменных
	int flag = -1;
	int undef = 0; // определитель #undef с именем макроопределения (1 - true, 0 - false)
	int coment = 0;
	int scob = 0;

	/*
	flag = -1 начало
	flag = 0 найдено имя функции
	flag = 1 переменные функции
	flag = 2 замена на макроопределение
	*/
	for (str; str < qu_str; str++)
	{
		undef = 0;
		for (int stol = 0; stol < len_str[str]; stol++)
		{
			COM
			if PROPUSK continue;

			if (!(strcmp(a[str][stol], "ifdef")) || !(strcmp(a[str][stol], "ifndef")) || !(strcmp(a[str][stol], "undef")))
			{
				if (!(strcmp(a[str][stol++], "undef"))) undef = 1;
				for (stol; stol < len_str[str]; stol++)
				{
					if PROPUSK continue;
					if (undef == 0 && !(strcmp(a[str][stol], nam_fun))) a[str][stol][0] = '.';
					else if (undef == 1 && !(strcmp(a[str][stol], nam_fun))) undef = 2;
					break;
				}
			}
			if (undef == 2) break;

			if (!(strcmp(a[str][stol], nam_fun)) && flag == -1) {
				flag = 0;
				str_stol = stol;
				stol++;
			}

			if (flag == 1) {
				int i = 0, y = 0;
				per_fun[pos_fun] = (char*)calloc(M, 1);
				while (a[str][stol][y] != ',') {
					if (a[str][stol][y] == '(')scob++;
					if (a[str][stol][y] == ')'&& scob == 0) {
						flag = 2; break;
					}
					if (a[str][stol][y] == ')')scob--;
					per_fun[pos_fun][i++] = a[str][stol][y++];
					if (a[str][stol][y] == '\0') {stol++; y=0;}
				}
				per_fun[pos_fun++][i] = '\0';
			}

			if (a[str][stol][0] == '(' && flag == 0) flag = 1; 
			else if (flag == 0) {
				flag = 2;  stol--; pos_fun++;
			}

			if (flag == 2)
			{
				//for (int i = 0; i < pos_fun; i++)
					//printf("(%s) ", per_fun[i]);
				int i = 0;
				int k = 0;
				per_fun = (char**)realloc(per_fun, (pos_fun) * sizeof(char*));
				stol++;
				for (stol; stol < len_str[str]; stol++, i++) // запись символов после замены
				{
					sav_ost[i] = (char*)calloc(M, sizeof(char));
					strcpy(sav_ost[i],a[str][stol]);
					sav_ost[i] = (char*)realloc(sav_ost[i], strlen(a[str][stol])+ 1);
				}
				sav_ost = (char**)realloc(sav_ost, i * sizeof(char*));
				stol = str_stol;
				a[str] = (char**)realloc(a[str], N * sizeof(char*));
				for (int array = 0; array < _msize(arr_for); array++, stol++) // замена на макроопределение
				{
					a[str][stol] = (char*)calloc(M, sizeof(char));
					if (arr_for[array] >= '0' && arr_for[array] <= '9'&& per_pos[k] == array)// замена слов
					{
						strcpy(a[str][stol], per_fun[arr_for[array] - '0' - 1]);//имена переменных функции
						a[str][stol] = (char*)realloc(a[str][stol], strlen(per_fun[arr_for[array] - '0' - 1 ]) + 1); 
						k++;
					}
					else // замена остального
					{
						int y = 0;
						if(tolower(arr_for[array]) >= 'a' && tolower(arr_for[array]) <= 'z' || arr_for[array] == '_'|| arr_for[array] >= '0' && arr_for[array] <= '9')
							do
							a[str][stol][y++] = arr_for[array++];
							while (tolower(arr_for[array]) >= 'a' && tolower(arr_for[array]) <= 'z' || arr_for[array] == '_' || arr_for[array] >= '0' && arr_for[array] <= '9');
						else a[str][stol][y++] = arr_for[array++];
						--array;
					    a[str][stol] = (char*)realloc(a[str][stol], y+1);
					}
				}
				
				for(int k = 0;  k < i; k++, stol++) { // добавление символов остатка
					a[str][stol] = (char*)calloc(M, sizeof(char));
					strcpy(a[str][stol], sav_ost[k]);
					a[str][stol] = (char*)realloc(a[str][stol], strlen(sav_ost[k]) + 1 );
				}
				
				a[str] = (char**)realloc(a[str], (stol) * sizeof(char*));

				len_str[str] = stol;
				stol = str_stol;

				per_fun = (char**)calloc(N, sizeof(char*)); 
				sav_ost = (char**)calloc(N, sizeof(char*)); 

				pos_for = 0; 
				pos_fun = 0; 
				flag = -1;
			}
		}
		if (undef == 2) break;
	}
}

int if_(int str, int stol)
{
	int* arr_for = (int*)calloc(M, sizeof(int)); // массив с формулой условия
	char* znk = (char*)calloc(M, sizeof(char)); // массив со знаками формулы

	int f = 0;
	int z = 0;
	int flag = 0;
	int coment = 0;


	for (int stol = 0; stol < len_str[str]; stol++)
	{
		//printf("%s ", a[str][stol]);
		COM
		if (a[str][stol][0] >= '0' && a[str][stol][0] <= '9')
		{
			//printf("%s ", a[str][stol]);
			if (f > 2 && z > 2) {flag = -1; break;}
			//while(a[str][stol])
			arr_for[f++] = atoi(a[str][stol]);
			//printf("%s ", a[str][stol]);
		}
		else if (a[str][stol][0] == '-' || a[str][stol][0] == '+' || a[str][stol][0] == '*' || a[str][stol][0] == '/' || a[str][stol][0] == '%' || a[str][stol][0] == '^') znk[z++] = a[str][stol][0];

		else if (a[str][stol][0] == '=' || a[str][stol][0] == '>' || a[str][stol][0] == '<' || a[str][stol][0] == '!')
		{
			if (z == 0) z++;
			if (flag == 0)
			switch (znk[--z]) {
			case '*': arr_for[0] *= arr_for[--f]; break;
			case '/': arr_for[0] /= arr_for[--f]; break;
			case '%': arr_for[0] %= arr_for[--f]; break;
			case '+': arr_for[0] += arr_for[--f]; break;
			case '-': arr_for[0] -= arr_for[--f]; break;
			case '^': arr_for[0] -= arr_for[--f]; break;
			default:  break;
			}
			f = 1;
			flag += a[str][stol][0];
		}

		else if (stol + 1 == len_str[str])
		{
			if (z == 0) z++;
			while (f > 2)
			{
				switch (znk[--z]) {
				case '*': arr_for[1] *= arr_for[--f]; break;
				case '/': arr_for[1] /= arr_for[--f]; break;
				case '%': arr_for[1] %= arr_for[--f]; break;
				case '+': arr_for[1] += arr_for[--f]; break;
				case '-': arr_for[1] -= arr_for[--f]; break;
				case '^': arr_for[1] -= arr_for[--f]; break;
				default:  break;
				}
			}

			switch (flag) {
			case 122: if (arr_for[0] == arr_for[1]) flag = 0; break;// ==
			case 62: if (arr_for[0] > arr_for[1])flag = 0; break;// >
			case 60: if (arr_for[0] < arr_for[1]) flag = 0; break; // <
			case 94: if (arr_for[0] != arr_for[1]) flag = 0; break; // !=
			case 123: if (arr_for[0] >= arr_for[1])flag = 0; break;// >=
			case 121: if (arr_for[0] <= arr_for[1]) flag = 0; break; // <=
			default:  break;
			}
		}
	}

	if (f == 1 && arr_for[0] == 0) flag = 122; // любое число кроме -1 и 0

	free(arr_for);
	free(znk);

	return flag;
}

int ifdef(int str, int stol, int one) {
	int flag = 122; // любое число кроме -1 и 0
	int coment = 0;

	for (stol = 0; stol < len_str[str]; stol++)
	{
		COM
		if PROPUSK continue;
		if (a[str][stol][0] == '.')
		{
			flag = 0;
		}
	}
		if(one ==  0)return (flag);
		else return (flag - 122);
}