#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "Cache.cpp"

//using namespace std;

int main(int argc, char const *argv[]) {

	int CS,LS,Ass;
	cin >> CS;
	cin >> LS;
	cin >> Ass;
	//Cache CacheInst(atoi(argv[2]), atoi(argv[4]), atoi(argv[6]));
	Cache CacheInst(CS,LS,Ass);
	ifstream Archivo("mcf.trace");
	string Linea;
	while (getline(Archivo, Linea)) {
		CacheInst.Simulate(Linea);
	}
	return 0;
}