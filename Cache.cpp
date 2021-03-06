#include "Cache.h"
#include <iostream>
#include <fstream>
#include "Campo.h"

using namespace std;

inline Cache::Cache() {

};
inline Cache::Cache(int CSize, int Asso, int LSize) {
	//Seteo de Parametros de la cache
	this->Cache_size = CSize;
	this->Line_size = LSize;
	this->Asso = Asso;
	//this->Replacement_Policy = reppo;
	if(Asso<=2){
		this->SRRIP_Top = 1;
	}
	else {
		this->SRRIP_Top = 3;
	}
	//Seteo de parametros de las instrucciones
	int Csizebyte = CSize * 1024;
	int csizelsize = Csizebyte / LSize;
	this->Sets = csizelsize/this->Asso;
	this->Index = log2(this->Sets);
	this->Offset = log2(LSize);
	this->Tag = 32 - this->Index - this->Offset;

	/*this->Cache_Matrix= new int* [this->Sets];
	this->DBMat= new int* [this->Sets];
	this->SRRIPMat = new int* [this->Sets];*/

	this->Cache_Matrix = new Campo*[this->Sets];
	
	for (int i = 0;i < this->Sets; i++) {
		this->Cache_Matrix[i] = new Campo[this->Asso];
		/*this->Cache_Matrix[i] = new int[this->Asso];
		this->DBMat[i] = new int[this->Asso];
		this->SRRIPMat[i] = new int[this->Asso];*/
	}
	this->TotalInsts = 0;
	this->Total_Loads=0;
	this->Total_Stores=0;
	this->Load_Hits=0;
	this->Load_Misses=0;
	this->Store_Hits=0;
	this->Store_Misses=0; 
	this->DirtyEvictions = 0;
	this->MatrixInit();

};
inline Cache::~Cache() {

};
inline void Cache::MatrixInit() {
	for (int i = 0; i < this->Sets; i++)
	{
		for (int j = 0; j < this->Asso; j++)
		{
			this->Cache_Matrix[i][j].Valor = 0;
			this->Cache_Matrix[i][j].RRPV = this->SRRIP_Top;
			this->Cache_Matrix[i][j].DB = 0;
			/*this->DBMat[i][j] = 0;
			this->SRRIPMat[i][j] = this->SRRIP_Top-1;*/
		}
	}
};
inline string Cache::Hex_Bin(string Leido) {
	string ToBin = "";
	for (int i = 0;i < Leido.length();i++) {
		switch (Leido[i]) {
			case '0':{
				ToBin.append("0000");
				break;
			}
			case '1': {
				ToBin.append("0001");
				break;
			}
			case '2': {
				ToBin.append("0010");
				break;
			}
			case '3': {
				ToBin.append("0011");
				break;
			}
			case '4': {
				ToBin.append("0100");
				break;
			}
			case '5': {
				ToBin.append("0101");
				break;
			}
			case '6': {
				ToBin.append("0110");
				break;
			}
			case '7': {
				ToBin.append("0111");
				break;
			}
			case '8': {
				ToBin.append("1000");
				break;
			}
			case '9': {
				ToBin.append("1001");
				break;
			}
			case 'a': {
				ToBin.append("1010");
				break;
			}
			case 'b': {
				ToBin.append("1011");
				break;
			}
			case 'c': {
				ToBin.append("1100");
				break;
			}
			case 'd': {
				ToBin.append("1101");
				break;
			}
			case 'e': {
				ToBin.append("1110");
				break;
			}
			case 'f': {
				ToBin.append("1111");
				break;
			}

		}
	}
	return ToBin;
}
inline int Cache::Bin_Dec(string Binary) {
	int ToDec = 0;
	int size = Binary.size();
	for (int i = 0; i < size; i++)
	{
		if (Binary.at((size - 1) - i) == '1') {
			ToDec += pow(2, i);
		}
	}
	return ToDec;
}
inline void Cache::ReadInst(string Instru) {
	char inst = Instru.at(2);
	if (inst == '0') {
		this->InstructType = 0;
		this->Total_Loads++;
	}
	else {
		this->InstructType = 1;
		this->Total_Stores++;
	}
}
inline string Cache::GetDirection(string instru) {
	string Direction = instru.substr(4, 8);
	Direction = Hex_Bin(Direction);
	return Direction;
}
inline void Cache::Tag_Index(string dir) {
	this->Data_tag = dir.substr(0, this->Tag-1);
	this->TagContent = this->Bin_Dec(this->Data_tag);
	string Index = dir.substr(this->Tag, this->Index);
	this->Data_Index = this->Bin_Dec(Index);
}
inline void Cache::Check() {
	bool ThisHit = false;
	int HitCol;
	for (int i = 0; i < this->Asso; i++)
	{
		if (this->Cache_Matrix[this->Data_Index][i].Valor == this->TagContent && ThisHit == false) {
			ThisHit = true;
			HitCol = i;
			break;
		}
	}
	if (ThisHit == true) {
		//this->SRRIPMat[this->Data_Index][HitCol] = 0;
		this->Cache_Matrix[this->Data_Index][HitCol].RRPV = 0;
		if (this->InstructType == 0) {
			this->Load_Hits++;
		}
		else {
			this->Store_Hits++;
		}
	}
	else {
		Evict();
		if (this->InstructType == 0) {
			this->Load_Misses++;
		}
		else {
			this->Store_Misses++;
		}
	}
}
inline void Cache::Evict() {
	for (int i = 0; i < this->Asso; i++)
	{
		//if (this->SRRIPMat[this->Data_Index][i] == this->SRRIP_Top) {
		if (this->Cache_Matrix[this->Data_Index][i].RRPV == this->SRRIP_Top) {
			//if (this->DBMat[this->Data_Index][i] == 1) {
			if (this->Cache_Matrix[this->Data_Index][i].DB == 1) {
				this->DirtyEvictions++;
				if (this->InstructType == 0) {
					//this->DBMat[this->Data_Index][i] = 0;
					this->Cache_Matrix[this->Data_Index][i].DB = 0;
				}
				this->Cache_Matrix[this->Data_Index][i].Valor = this->TagContent;
				//this->SRRIPMat[this->Data_Index][i] = this->SRRIP_Top - 1;
				this->Cache_Matrix[this->Data_Index][i].RRPV = this->SRRIP_Top - 1;
			}
			else {
				this->Cache_Matrix[this->Data_Index][i].Valor = this->TagContent;
				//this->SRRIPMat[this->Data_Index][i] = this->SRRIP_Top - 1;
				this->Cache_Matrix[this->Data_Index][i].RRPV = this->SRRIP_Top - 1;
			}
			break;
		}
	}



	/*int ThisCol = 0;
	while (ThisCol<this->Asso) {
		if (this->SRRIPMat[this->Data_Index][ThisCol] != this->SRRIP_Top) {
			ThisCol++;
		}
		else if (this->DBMat[this->Data_Index][ThisCol] == 1) {
			this->DirtyEvictions++;
			if (this->InstructType == 0) {
				this->DBMat[this->Data_Index][ThisCol] = 0;
			}
			this->Cache_Matrix[this->Data_Index][ThisCol] = this->TagContent;
			this->SRRIPMat[this->Data_Index][ThisCol] = this->SRRIP_Top - 1;
		}
		this->Cache_Matrix[this->Data_Index][ThisCol] = this->TagContent;
		this->SRRIPMat[this->Data_Index][ThisCol] = this->SRRIP_Top - 1;
		ThisCol++;
	}

	if (ThisCol <this->Asso) {
		if (this->DBMat[this->Data_Index][ThisCol] == 1) {
			this->DirtyEvictions++;
			if (this->InstructType == 0) {
				this->DBMat[this->Data_Index][ThisCol] = 0;
			}
		}
		this->Cache_Matrix[this->Data_Index][ThisCol] = this->TagContent;
		this->SRRIPMat[this->Data_Index][ThisCol] = this->SRRIP_Top- 1;

	}
	else {
		ThisCol = 0;
		while (ThisCol<this->Asso) {
			this->SRRIPMat[this->Data_Index][ThisCol]++;
			ThisCol++;
		}
		Evict();
	}*/
}
/*inline void Cache::Promote(int Line){
	this->SRRIPMat[this->Data_Index][Line] = 0;
}*/

inline void Cache::Results(/*ofstream& csvfile*/) {
	cout << "********************************************************" << '\n';
	cout << "                    Cache Parameters: " << '\n';
	cout << "********************************************************" << '\n';
	cout << "Cache size:                               " << this->Cache_size << "KB" << '\n';
	cout << "Cache Associativity:                      " << this->Asso << '\n';
	cout << "Cache Block Size:                         " << this->Line_size<< " bytes" << '\n';
	cout << "********************************************************" << '\n';
	cout << "                    Simulation Results: " << '\n';
	cout << "********************************************************" << '\n';
	double Overall_miss_rate = (double)(this->Load_Misses + this->Store_Misses) / (double)(this->Total_Loads+this->Total_Stores);
	cout << "Overall miss rate:                        " << Overall_miss_rate << '\n';
	cout << "Read miss rate:                           " << (double)this->Load_Misses / (double)this->Total_Loads << '\n';
	cout << "Dirty evictions:                          " << this->DirtyEvictions << '\n';
	cout << "Load misses:                              " << this->Load_Misses << '\n';
	cout << "Store misses:                             " << this->Store_Misses << '\n';
	cout << "Total misses:                             " << this->Load_Misses + this->Store_Misses << '\n';
	cout << "Load hits:                                " << this->Load_Hits << '\n';
	cout << "Store hits:                               " << this->Store_Hits << '\n';
	cout << "Total hits:                               " << this->Load_Hits + this->Store_Hits << '\n';
	cout << "********************************************************" << '\n';

	/*csvfile.open("SimulationResults.csv");
	csvfile << '\n';
	csvfile << "Simulation results:,\n";
	csvfile << '\n';
	csvfile << "Overall miss rate: , " << Overall_miss_rate << endl;
	csvfile << "Read miss rate: , " << (double)this->Load_misses / (double)this->Num_loads << endl;
	csvfile << "Dirty evictions: , " << this->dirty_eviction << endl;
	csvfile << "Load misses: , " << this->Load_misses << endl;
	csvfile << "Store misses: , " << this->Store_misses << endl;
	csvfile << "Total misses: , " << this->Load_misses + this->Store_misses << endl;
	csvfile << "Load hits: , " << this->Load_hits << endl;
	csvfile << "Store hits: , " << this->Store_hits << endl;
	csvfile << "Total hits: , " << this->Load_hits + this->Store_hits << endl;
	csvfile << "\n\n";
	csvfile.close();*/


	cout << "Presione cualquier tecla para cerrar.";
	int x;
	cin >> x;
}
inline void Cache::Simulate(string Line) {
	string TestSubject;
	this->ReadInst(Line);
	TestSubject = this->GetDirection(Line);
	this->Tag_Index(TestSubject);
	this->Check();
}