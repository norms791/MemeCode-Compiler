

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include "SymbolTable.h"
#include "Quadruple.h"
#include "wchar.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>

#define GBL_ENT 0
#define GBL_DEC 500
#define GBL_LOG 1000
#define GBL_FRA 1500
#define GBL_DIR 2000
#define LCL_ENT 2500
#define LCL_DEC 3000
#define LCL_LOG 3500
#define LCL_FRA 4000
#define LCL_DIR 4500
#define CTE_ENT 5000
#define CTE_DEC 5500
#define CTE_LOG 6000
#define CTE_FRA 6500
#define CTE_DIR 7000
#define TMP_ENT 7500
#define TMP_DEC 8000
#define TMP_FRA 9000
#define TMP_LOG 8500
#define TMP_DIR 9500


#include "Scanner.h"

namespace MemeCode {

class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ent=1,
		_dec=2,
		_log=3,
		_fra=4,
		_id=5
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

int COUNT_GBL_ENT;
	int COUNT_GBL_DEC;
	int COUNT_GBL_FRA;
	int COUNT_GBL_LOG;
	int COUNT_GBL_DIR;
	int COUNT_LCL_ENT;
	int COUNT_LCL_DEC;
	int COUNT_LCL_FRA;
	int COUNT_LCL_LOG;
	int COUNT_LCL_DIR;
	int COUNT_TMP_ENT;
	int COUNT_TMP_DEC;
	int COUNT_TMP_FRA;
	int COUNT_TMP_LOG;
	int COUNT_TMP_DIR;
	int COUNT_CTE_ENT;
	int COUNT_CTE_DEC;
	int COUNT_CTE_FRA;
	int COUNT_CTE_LOG;
	int COUNT_CTE_DIR;
	

    int //types
        undef, ent, fra, log, dec;
	int // object kinds
		var, proc, m_proc;
	int //operators
		plus, minus, times, slash, andpersand, pipe, less, gtr, 
		diff, equ, isequ, false_f;
	
	int //operation codes
		ADD, SUB, MUL, DIV, AND, OR, LSS, GTR, DIF, ASIG, IS_EQU,
		GOTO, GOTOV, GOTOF, ERA, GOSUB, RET, mGIRA, mARRIBA,
		mABAJO, mIZQUIERDA, mDERECHA, mALTO, mESPERA, pARRIBA, 
		pABAJO, pTAM, pCOLOR, pBORRA, PRINT, PARAM, POTENCIA,
		RAIZ, END, VERIF, LEC;	
	
	int //colors
		AMARILLO, AZUL, CELESTE, MORADO, NARANJA, NEGRO, ROJO,
		ROSA, VERDE;
	
	int //socpes
		global_s, local_s, temporal_s, constant_s;
	
	int  cubo [11][5][5];
	
	//Quadruple stuff
	int program_counter;
	vector<Quadruple *> listQuadruple;
	stack<int> pOper;
	stack<int> pOp;
	stack<int> pTipos;
	stack<int> pSaltos;	
	
	//Constants tables
	vector<FraConst *> fraTable;
	vector<BoolConst *> boolTable;
	vector<IntegerConst *> entTable;
	vector<FloatConst *> decTable;
	vector<DirConst *> dirTable;
		
	//Procedures directory stuff
	vector<Procedure *> procs_dir;	

	void Err(wchar_t* msg) {
		errors->Error(la->line, la->col, msg);
	}
	
	/*Function that adds a new quadruple to the list of quadurples*/
	void create_quadruple(int first, int second, int third, int fourth){
		cout << "Check in quadruple" ;
		Quadruple *q = new Quadruple(first, second, third, fourth);
		listQuadruple.push_back(q);
		program_counter ++;
		cout << "Checkout quadruple ";
	}
	
	/*Function that updates a quadruple. It receives as parameters the
	number of a quadruple, the position of the parameter who is gonna be 
	updated and the  new value*/
	void update_quadruple(int number, int position, int value){
		cout << "Check in edit quadruple" << number << " " << position << " " << value ;
		
		Quadruple *q = listQuadruple[number];
		switch (position){
			case 1: q->first = value; 
				break;
			case 2: q->secnd = value;
				break;
			case 3: q->third = value;
				break;
			case 4: q->fourth = value;
				break;
		}
		cout << "Check out edit quadruple" ;
	}

	/*Function to add a new element to the procedures directory*/
	void add_procedure(wchar_t* name, int type){
		cout << "Add prodecure check in ";
		Procedure *p = new Procedure(procs_dir.size(), name, type);
		procs_dir.push_back(p);
		cout << "Add procedure check out ";
	}

	/*Function to add a new variable to the current procedure*/
	void add_variable(wchar_t* name, int type){
		Procedure *p = procs_dir.back();
		int dir = get_var_dir(type, local_s);
		p->add_var(name, type, dir);
	}
	
	/*MODIFICAR LAS COSAS DE LOS ARREGLO LOS INDICES!!*/
	void add_variable_array(wchar_t* name, int type, int arr_length){
		Procedure *p = procs_dir.back();
		int dir = get_var_dir(type, local_s);
		p->add_var_arr(name, type, dir, arr_length);
	}	
	
	void add_parameter(wchar_t* name, int type){
		Procedure *p = procs_dir.back();
		int dir = get_var_dir(type, local_s);
		p->add_param(name, type, dir);
	}
	
	/*Restore the memory counters */
	void restore_counters () {
		COUNT_LCL_ENT = 0;
		COUNT_LCL_DEC = 0;
		COUNT_LCL_FRA = 0;
		COUNT_LCL_LOG = 0;
		COUNT_LCL_DIR = 0;
		COUNT_TMP_ENT = 0;
		COUNT_TMP_DEC = 0;
		COUNT_TMP_FRA = 0;
		COUNT_TMP_LOG = 0;
		COUNT_TMP_DIR = 0;
	}
	
	/*Function to set the direction a variable will have*/
	int get_var_dir(int type, int scope){
		int dir = 0;
		switch(scope) {
			//Global Scope
			case 0: 
				switch(type) {
					case 1:
						dir = GBL_ENT + COUNT_GBL_ENT;
						COUNT_GBL_ENT++;
					break;
					case 2:
						dir = GBL_FRA + COUNT_GBL_FRA;
						COUNT_GBL_FRA++;
					break;
					case 3:
						dir = GBL_LOG + COUNT_GBL_LOG;
						COUNT_GBL_LOG++;
					break;
					case 4:
						dir = GBL_DEC + COUNT_GBL_DEC;
						COUNT_GBL_DEC++;
					break;
				}
			break;
			//Local Scope
			case 1:
				switch(type) {
					case 1:
						dir = LCL_ENT + COUNT_LCL_ENT;
						COUNT_LCL_ENT++;
					break;
					case 2:
						dir = LCL_FRA + COUNT_LCL_FRA;
						COUNT_LCL_FRA++;
					break;
					case 3:
						dir = LCL_LOG + COUNT_LCL_LOG;
						COUNT_LCL_LOG++;
					break;
					case 4:
						dir = LCL_DEC + COUNT_LCL_DEC;
						COUNT_LCL_DEC++;
					break;
				}	
			break;
			//Temporal scope
			case 2:
				switch(type) {
					case 1:
						dir = TMP_ENT + COUNT_TMP_ENT;
						COUNT_TMP_ENT++;
					break;
					case 2:
						dir = TMP_FRA + COUNT_TMP_FRA;
						COUNT_TMP_FRA++;
					break;
					case 3:
						dir = TMP_LOG + COUNT_TMP_LOG;
						COUNT_TMP_LOG++;
					break;
					case 4:
						dir = TMP_DEC + COUNT_TMP_DEC;
						COUNT_TMP_DEC++;
					break;
				}
			break;
			//Constant Scope
			case 3:
				switch(type) {
					case 1:
						dir = CTE_ENT + COUNT_CTE_ENT;
						COUNT_CTE_ENT++;
					break;
					case 2:
						dir = CTE_FRA + COUNT_CTE_FRA;
						COUNT_CTE_FRA++;
					break;
					case 3:
						dir = CTE_LOG + COUNT_CTE_LOG;
						COUNT_CTE_LOG++;
					break;
					case 4:
						dir = CTE_DEC + COUNT_CTE_DEC;
						COUNT_CTE_DEC++;
					break;
				}
			break;
		}
		return dir;
	}
	
	void print_procs_dir(){
		for(int i=0; i<procs_dir.size(); i++ ){
			cout << procs_dir[i]->first_quadruple << "\n" ; 
		}
	}
	
	void print_quadruples_file(){
		ofstream quadruples;
		quadruples.open("quadruples.txt");
		
		/* Formato para las varibles */
		quadruples << entTable.size() << "," << decTable.size() << "," << boolTable.size() << "," << fraTable.size() << "," << dirTable.size() << "\n";
		for (int i = 0; i < entTable.size(); i++ )
			if( i == entTable.size() - 1)
				quadruples << entTable[i]->value;
			else
				quadruples << entTable[i]->value << ",";
		quadruples << "/";
		
		for (int i = 0; i < decTable.size(); i++ )
			if (i == decTable.size() - 1)
				quadruples << decTable[i]->value;
			else 
				quadruples << entTable[i]->value << ",";
		quadruples << "/";
		
		for (int i = 0; i < boolTable.size(); i++ )
			if (i == boolTable.size() - 1 )
				quadruples << boolTable[i]->value;
			else 
				quadruples << boolTable[i]->value << ",";
		quadruples << "/";
		
		for (int i = 0; i < fraTable.size(); i++ )
			if(i == fraTable.size() - 1)
				quadruples << fraTable[i]->value;
			else 
				quadruples << fraTable[i]->value << ",";
		quadruples << "/";
		
		for (int i = 0; i < dirTable.size(); i++ )
			if (i == dirTable.size() - 1)
				quadruples << dirTable[i]->value;
			else 
				quadruples << dirTable[i]->value << ",";			
		quadruples << "\n";
		
		
		
		/* Formato para la memoria global 
			//VARIABLES  se ponen en orden ent, dec, log, fra, dir
		*/
		quadruples << procs_dir[0]->num_ent + procs_dir[0]->num_params_ent << "," << procs_dir[0]->num_dec + procs_dir[0]->num_params_dec << "," <<  procs_dir[0]->num_log + procs_dir[0]->num_params_log << "," << procs_dir[0]->num_fra + procs_dir[0]->num_params_fra << "," << procs_dir[0]->num_dir << "\n" ;
		
		/* Formato para las variables locales y temporales 
			//La cuarta línea es la memoria local  dentro de la local se separa  VARproc1-TEMPproc1/VARproc2-TEMPproc2/VARproc3-TEMPproc3 
			1,0,0,0,0-0,0,0,0,0
		*/
		for(int i = 1; i < procs_dir.size(); i++)
			if( i == procs_dir.size() - 1)
				quadruples << procs_dir[i]->num_ent + procs_dir[i]->num_params_ent << "," << procs_dir[i]->num_dec + procs_dir[i]->num_params_dec << "," <<  procs_dir[i]->num_log + procs_dir[i]->num_params_log << "," << procs_dir[i]->num_fra + procs_dir[i]->num_params_fra << "," << procs_dir[i]->num_dir << "-" << procs_dir[i]->num_tem_ent << "," << procs_dir[i]->num_tem_dec << "," << procs_dir[i]->num_tem_log << "," << procs_dir[i]->num_tem_fra << "," << procs_dir[i]->num_tem_dir << "\n";
			else
				quadruples << procs_dir[i]->num_ent + procs_dir[i]->num_params_ent << "," << procs_dir[i]->num_dec + procs_dir[i]->num_params_dec << "," <<  procs_dir[i]->num_log + procs_dir[i]->num_params_log << "," << procs_dir[i]->num_fra + procs_dir[i]->num_params_fra << "," << procs_dir[i]->num_dir << "-" << procs_dir[i]->num_tem_ent << "," << procs_dir[i]->num_tem_dec << "," << procs_dir[i]->num_tem_log << "," << procs_dir[i]->num_tem_fra << "," << procs_dir[i]->num_tem_dir << "/";
		
		/* Formato para los cuadruplos */
		for(int i= 0; i < listQuadruple.size(); i++) 
			if (i == listQuadruple.size() -1)
				quadruples << listQuadruple[i]->first << "," << listQuadruple[i]->secnd << "," << listQuadruple[i]->third << "," << listQuadruple[i]->fourth ;
			else 
				quadruples << listQuadruple[i]->first << "," << listQuadruple[i]->secnd << "," << listQuadruple[i]->third << "," << listQuadruple[i]->fourth << "/";
		quadruples.close();
	}
	
	void print_quadruples(){
		for(int i= 0; i < listQuadruple.size(); i++) {
			cout << listQuadruple[i]->first << " " << listQuadruple[i]->secnd << " " << listQuadruple[i]->third << " " << listQuadruple[i]->fourth << "; \n" ;
		}
	}
	
	void initDeclarations(){
		// object kinds
		var = 0; proc = 1; m_proc = 2;
		//Códigos de variables
		undef = 0; ent = 1; fra = 2; log = 3; dec = 4;
		//operators
		plus = 0; minus = 1; times = 2; slash = 3; andpersand = 4;
		pipe = 5; less = 6; gtr = 7; diff = 8; equ = 9; isequ = 10;
		false_f = 11;
		//operation codes
		ADD = 0; SUB = 1; MUL = 2; DIV = 3; AND = 4; OR = 5; LSS = 6; 
		GTR = 7; DIF = 8; ASIG = 9; IS_EQU = 10; GOTO = 11; GOTOV = 12;
		GOTOF = 13, ERA = 14; GOSUB = 15; RET = 16; mGIRA = 17; mARRIBA = 18;
		mABAJO = 19; mIZQUIERDA = 20; mDERECHA = 21; mALTO= 22; mESPERA = 23;
		pARRIBA = 24; pABAJO = 25; pTAM = 26; pCOLOR = 27; pBORRA = 28; 
		PRINT = 29; PARAM = 30; POTENCIA = 31; RAIZ = 32; END = 33;
		VERIF = 34; LEC = 35;
		
		//scopes
		global_s = 0; local_s = 1; temporal_s = 2; constant_s = 3;
		//colors
		AMARILLO = 1; AZUL = 2; CELESTE = 3; MORADO = 4; NARANJA = 5;
		NEGRO = 6; ROJO = 7; ROSA = 8; VERDE = 9;
		//program counter
		program_counter = 0;
		//LLENADO DEL CUBO
		for (int i = 0; i < 11; i++ ) {
			for (int j = 0; j < 5; j++ ) {
				for (int k = 0; k < 5; k++) {
					cubo[i][j][k] = 0;
				}
			}
		}
		cubo[0][1][1] = 1; cubo[0][1][4] = 4; cubo[0][4][1] = 4; cubo[0][4][4] = 4;
		cubo[1][1][1] = 1; cubo[1][1][4] = 4; cubo[1][4][1] = 4; cubo[1][4][4] = 4;
		cubo[2][1][1] = 1; cubo[2][1][4] = 4; cubo[2][4][1] = 4; cubo[2][4][4] = 4;
		cubo[3][1][1] = 1; cubo[3][1][4] = 4; cubo[3][4][1] = 4; cubo[3][4][4] = 4;
		cubo[4][1][1] = 3; cubo[4][3][3] = 3;
		cubo[5][3][3] = 3; 
		cubo[6][1][1] = 3; cubo[6][1][4] = 3; cubo[6][4][1] = 3; cubo[6][4][4] = 3;
		cubo[7][1][1] = 3; cubo[7][1][4] = 3; cubo[7][4][1] = 3; cubo[7][4][4] = 3;
		cubo[8][1][1] = 3; cubo[8][1][4] = 3; cubo[8][4][1] = 3; cubo[8][4][4] = 3;
		cubo[8][2][2] = 3; cubo[8][3][3] = 3;
		cubo[9][1][1] = 1; cubo[9][2][2] = 2; cubo[9][3][3] = 3; cubo[9][4][4] = 4;
		cubo[10][1][1] = 3; cubo[10][2][2] = 3; cubo[10][3][3] = 3; cubo[10][4][4] = 3;
	}



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void MemeCode();
	void Ident(wchar_t* &name);
	void DECLARACION();
	void METODO();
	void PRINCIPAL();
	void ESTATUTO();
	void CICLO();
	void CONDICION();
	void MOVIMIENTO();
	void ASIGLEC();
	void IMPRIME();
	void RETORNO();
	void PLUMA();
	void MATH();
	void EXP();
	void TIPO(int &type);
	void DECLARACIONMETODO();
	void SUPEXPRESION();
	void LECTURA();
	void ASIGNA();
	void LLAMADA();
	void TERM();
	void FACTOR();
	void VAR();
	void Log(bool &value);
	void Fra(wchar_t* &value);
	void EXPRESION();
	void COLOR(int &numColor);
	void ALTO();
	void AVANZA();
	void GIRA();
	void ESPERA();

	void Parse();

}; // end Parser

}

#endif

