#ifndef CACHE_H
#define CACHE_H



#pragma once
#include <iostream>
#include <fstream>
#include <stdio.h>     
#include <stdlib.h>    
#include <string>
#include "math.h"
#include "Campo.h"

using namespace std;

class Cache {
	public:
		Cache();
		Cache(int CSize, int Asso,int LSize);
		~Cache();

		//Parametros constructivos
		int Cache_size;
		int Line_size;
		int Asso;
		int M;
		int Tag;
		int Index;
		int Offset;
		int Sets;
		int SRRIP_Top;
		

		//Almacenamiento
		Campo** Cache_Matrix;
		/*int ** Cache_Matrix;
		int** SRRIPMat;
		int** DBMat;*/
		int Data_Index;
		int InstructType;
		string Data_tag;
		int TagContent;
		

		//Resultados
		int TotalInsts;
		int Total_Loads;
		int Total_Stores;
		int Load_Hits;
		int Load_Misses;
		int Store_Hits;
		int Store_Misses;
		int DirtyEvictions;
		
		//Manejo de archivos
		

		//Metodos
		void MatrixInit();
		void Check();
		void Evict();
		void Promote(int Line);
		void Simulate(string Line);
		void Results(/*ofstream& csvfile*/);
		string Hex_Bin(string Hex);
		int Bin_Dec(string bin);
		string GetDirection(string Line);
		void ReadInst(string Line);
		void Tag_Index(string Dir);


};

#endif