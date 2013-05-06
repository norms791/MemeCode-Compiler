

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"

namespace MemeCode {


void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::MemeCode() {
		wchar_t* name; int type; initDeclarations();
		Expect(6 /* "programa" */);
		type = 5; 
		Ident(name);
		cout << "Entre al programa! " ;
		/*Create the main procedure*/
		add_procedure(name, proc);
		
		Expect(7 /* "{" */);
		Procedure *p = procs_dir.back();
		p->first_quadruple = program_counter;
		
		while (la->kind == 13 /* "lista" */ || la->kind == 15 /* "var" */) {
			DECLARACION();
		}
		while (la->kind == 9 /* "func" */) {
			METODO();
		}
		PRINCIPAL();
		print_procs_dir(); 
		print_quadruples_file();
		
		Expect(8 /* "}" */);
}

void Parser::Ident(wchar_t* &name) {
		Expect(_id);
		name= coco_string_create(t->val); 
}

void Parser::DECLARACION() {
		cout << "Check in declaracion variable ";
		wchar_t* name; int type; 
		if (la->kind == 13 /* "lista" */) {
			Get();
			Expect(14 /* "con" */);
			EXP();
		}
		Expect(15 /* "var" */);
		TIPO(type);
		Ident(name);
		cout << "Add a new variable ";
		add_variable(name, type); 
		while (la->kind == 11 /* "," */) {
			Get();
			Ident(name);
			cout << "Add a new variable 2";
			add_variable(name, type); 
		}
		Expect(16 /* ";" */);
}

void Parser::METODO() {
		Expect(9 /* "func" */);
		wchar_t* name; int type;
		TIPO(type);
		Ident(name);
		cout << "Check in Metodo ";
		add_procedure(name, type); 
		Expect(10 /* "(" */);
		if (la->kind == 25 /* "ent" */ || la->kind == 26 /* "dec" */ || la->kind == 27 /* "log" */) {
			DECLARACIONMETODO();
			while (la->kind == 11 /* "," */) {
				Get();
				DECLARACIONMETODO();
			}
		}
		Expect(12 /* ")" */);
		Procedure *p = procs_dir.back();
		p->first_quadruple = program_counter;
		
		Expect(7 /* "{" */);
		while (la->kind == 13 /* "lista" */ || la->kind == 15 /* "var" */) {
			DECLARACION();
		}
		while (StartOf(1)) {
			ESTATUTO();
		}
		Expect(8 /* "}" */);
}

void Parser::PRINCIPAL() {
		wchar_t* name; 
		Ident(name);
		cout << "Check in principal"; 
		if (coco_string_equal(name, L"Principal")){
		add_procedure(name, m_proc); 
		} else {
		Err(L"Debe existir un metodo principal y parece que tu no lo tienes...");
		exit(1);
		}	
		
		Expect(7 /* "{" */);
		Procedure *p = procs_dir.back();
		p->first_quadruple = program_counter;
		
		while (la->kind == 13 /* "lista" */ || la->kind == 15 /* "var" */) {
			DECLARACION();
		}
		while (StartOf(1)) {
			ESTATUTO();
		}
		Expect(8 /* "}" */);
		print_quadruples();
		
}

void Parser::ESTATUTO() {
		switch (la->kind) {
		case 52 /* "repetir" */: case 54 /* "mientras" */: {
			CICLO();
			break;
		}
		case 58 /* "si" */: {
			CONDICION();
			break;
		}
		case 68 /* "meme" */: {
			MOVIMIENTO();
			break;
		}
		case _id: {
			ASIGLEC();
			break;
		}
		case 17 /* "regresa" */: {
			RETORNO();
			break;
		}
		case 45 /* "pluma" */: {
			PLUMA();
			break;
		}
		default: SynErr(70); break;
		}
}

void Parser::CICLO() {
		if (la->kind == 52 /* "repetir" */) {
			Get();
			pSaltos.push(program_counter);
			
			EXP();
			Expect(53 /* "veces" */);
			int resultType = pTipos.top();
			pTipos.pop();
			int result;
			if(resultType == ent){
			result = pOp.top();
			pOp.pop();
			create_quadruple(GOTOF, result, -1, -1);
			pSaltos.push(program_counter);
			} else {
			Err(L"Tienes que repetir un numero entero de veces");
			exit(1);
			}
			
			Expect(7 /* "{" */);
			while (StartOf(1)) {
				ESTATUTO();
			}
			Expect(8 /* "}" */);
			int false_dir = pSaltos.top();
			pSaltos.pop();
			int ret_dir = pSaltos.top();
			pSaltos.pop();
			create_quadruple(GOTO, ret_dir, -1, -1);
			update_quadruple(false_dir -1, 4, program_counter);
			
		} else if (la->kind == 54 /* "mientras" */) {
			Get();
			pSaltos.push(program_counter);
			
			SUPEXPRESION();
			int resultType = pTipos.top();
			pTipos.pop();
			int result;
			if(resultType == log){
			result = pOp.top();
			pOp.pop();
			create_quadruple(GOTOF, result, -1, -1);
			pSaltos.push(program_counter);
			} else {
			Err(L"Los ciclos mientras tienen que tener una expresion logica");
			exit(1);
			}
			
			Expect(7 /* "{" */);
			while (StartOf(1)) {
				ESTATUTO();
			}
			Expect(8 /* "}" */);
			int false_dir = pSaltos.top();
			pSaltos.pop();
			int ret_dir = pSaltos.top();
			pSaltos.pop();
			create_quadruple(GOTO, ret_dir, -1, -1);
			update_quadruple(false_dir -1, 4, program_counter);
			
		} else SynErr(71);
}

void Parser::CONDICION() {
		Expect(58 /* "si" */);
		Expect(10 /* "(" */);
		SUPEXPRESION();
		Expect(12 /* ")" */);
		int tempR;
		if(pTipos.top() != log){
		Err(L"La expresion de la condicion tiene que ser logica");
		exit(1);
		}else {
		pTipos.pop();
		tempR = pOp.top();
		pOp.pop();
		create_quadruple(GOTOF, tempR, -1, -1);
		pSaltos.push(program_counter -1);
		}
		
		Expect(7 /* "{" */);
		while (StartOf(1)) {
			ESTATUTO();
		}
		Expect(8 /* "}" */);
		if (la->kind == 59 /* "si_no" */) {
			create_quadruple(GOTO,-1,-1,-1);
			int quadruplePosition = pSaltos.top();
			pSaltos.pop();
			update_quadruple(quadruplePosition -1, 4, program_counter);
			pSaltos.push(program_counter -1);
			
			Get();
			Expect(7 /* "{" */);
			while (StartOf(1)) {
				ESTATUTO();
			}
			int qToUpdate = pSaltos.top();
			pSaltos.pop();
			update_quadruple(qToUpdate-1, 4, program_counter);
			
			Expect(8 /* "}" */);
		}
}

void Parser::MOVIMIENTO() {
		Expect(68 /* "meme" */);
		Expect(46 /* ":" */);
		if (la->kind == 64 /* "gira" */) {
			GIRA();
		} else if (la->kind == 60 /* "avanza" */) {
			AVANZA();
		} else if (la->kind == 66 /* "espera" */) {
			ESPERA();
		} else if (la->kind == 57 /* "alto" */) {
			ALTO();
		} else SynErr(72);
}

void Parser::ASIGLEC() {
		wchar_t* name; 
		Ident(name);
		
		if (la->kind == 18 /* "[" */) {
			Get();
			Expect(_ent);
			Expect(19 /* "]" */);
		}
		Expect(20 /* "=" */);
		if (la->kind == 21 /* "lee" */) {
			int typeLec; 
			LECTURA(typeLec);
			
		} else if (StartOf(2)) {
			ASIGNA();
		} else SynErr(73);
}

void Parser::RETORNO() {
		Expect(17 /* "regresa" */);
		Expect(10 /* "(" */);
		SUPEXPRESION();
		Expect(12 /* ")" */);
		Expect(16 /* ";" */);
}

void Parser::PLUMA() {
		Expect(45 /* "pluma" */);
		Expect(46 /* ":" */);
		if (la->kind == 47 /* "color" */) {
			Get();
			Expect(20 /* "=" */);
			int numColor; 
			COLOR(numColor);
			create_quadruple(pCOLOR, numColor, -1, -1); 
		} else if (la->kind == 48 /* "tamaño" */) {
			Get();
			Expect(20 /* "=" */);
			EXP();
			int DirTam = pOp.top();
			pOp.pop();
			int typeTam = pTipos.top();
			pTipos.pop();
			if(typeTam == ent){
			create_quadruple(pTAM, DirTam, -1, -1);
			} else {
			Err(L"El tamanio se espera que sea de tipo entero");
			exit(1);	
			}
			
		} else if (la->kind == 49 /* "borra" */) {
			Get();
			create_quadruple(pBORRA, -1, -1, -1); 
		} else if (la->kind == 50 /* "arriba" */) {
			Get();
			create_quadruple(pARRIBA, -1, -1, -1); 
		} else if (la->kind == 51 /* "abajo" */) {
			Get();
			create_quadruple(pABAJO, -1, -1, -1); 
		} else SynErr(74);
		Expect(16 /* ";" */);
}

void Parser::TIPO(int &type) {
		if (la->kind == 25 /* "ent" */) {
			type = undef;
			Get();
			type = ent;
		} else if (la->kind == 26 /* "dec" */) {
			Get();
			type = dec;
		} else if (la->kind == 27 /* "log" */) {
			Get();
			type = log;
		} else SynErr(75);
}

void Parser::DECLARACIONMETODO() {
		cout << "Check in declaracion metodo ";
		wchar_t* name; int type;  
		TIPO(type);
		Ident(name);
		add_parameter(name, type); 
}

void Parser::EXP() {
		cout << "Checkin EXP"; 
		TERM();
		if( !(pOper.empty()) && (pOper.top() == plus || 
		pOper.top() == minus )) {
		int el3 = pTipos.top();
		pTipos.pop();
		int el2 = pTipos.top();
		pTipos.pop();
		int el1 = pOper.top();
		pOper.pop();
		/*Check if the operation is possible and if it, generate the quadruple*/
		int result = cubo[el1][el2][el3];
		int dir = 0;
		int op2 = pOp.top();
		pOp.pop();
		int op1 = pOp.top();
		pOp.pop();
		if(result != 0){
			dir = get_var_dir(result, temporal_s);
			switch (el1){
				case 0: create_quadruple(ADD, op1, op2, dir);
					break;
				case 1: create_quadruple(SUB, op1, op2, dir);
					break;
			}
			pOp.push(dir);
			pTipos.push(result);
			/*undef = 0; ent = 1; fra = 2; log = 3; dec = 4;*/
			/* Incrementar el numero de temporales solicitados en el procedimiento */
			Procedure *p = procs_dir.back();
			switch (result) {
				case 0: p->num_tem_ent ++;
					break;
				case 4: p->num_tem_dec ++;
					break;		
			}
		}else{
			Err(L"La operacion no se puede realizar");
			exit(1);
		}
		} 
		
		if (la->kind == 28 /* "+" */ || la->kind == 29 /* "-" */) {
			if (la->kind == 28 /* "+" */) {
				Get();
				pOper.push(plus); 
			} else {
				Get();
				pOper.push(minus); 
			}
			EXP();
		}
		cout << "checkout EXP"; 
}

void Parser::SUPEXPRESION() {
		EXPRESION();
		if (la->kind == 34 /* "&" */ || la->kind == 35 /* "|" */) {
			if (la->kind == 34 /* "&" */) {
				Get();
				pOper.push(andpersand); 
			} else {
				Get();
				pOper.push(pipe); 
			}
			EXPRESION();
			if( !(pOper.empty()) && (pOper.top() == andpersand || 
			pOper.top() == pipe )) {
			int el3 = pTipos.top();
			pTipos.pop();
			int el2 = pTipos.top();
			pTipos.pop();
			int el1 = pOper.top();
			pOper.pop();
			/*Check if the operation is possible and if it, generate the quadruple*/
			int result = cubo[el1][el2][el3];
			int dir = 0;
			int op2 = pOp.top();
			pOp.pop();
			int op1 = pOp.top();
			pOp.pop();
			if(result != 0){
				dir = get_var_dir(result, temporal_s);
				switch (el1) {
					case 4 : create_quadruple(AND, op1, op2, dir);
						break;
					case 5 : create_quadruple(OR, op1, op2, dir);
						break;	
				}
				pOp.push(dir);
				pTipos.push(result);
				/*undef = 0; ent = 1; fra = 2; log = 3; dec = 4;*/
				/* Incrementar el numero de temporales solicitados en el procedimiento */
				Procedure *p = procs_dir.back();
				p->num_tem_log ++;
			}else{
				Err(L"La operacion no se puede realizar");
				exit(1);
			}
			}
			
		}
}

void Parser::LECTURA(int &typeLec) {
		int type; 
		Expect(21 /* "lee" */);
		Expect(10 /* "(" */);
		TIPO(type);
		typeLec = type; 
		Expect(12 /* ")" */);
		Expect(16 /* ";" */);
}

void Parser::ASIGNA() {
		if (StartOf(3)) {
			SUPEXPRESION();
		} else if (la->kind == 22 /* "evalua" */) {
			LLAMADA();
		} else SynErr(76);
		Expect(16 /* ";" */);
}

void Parser::LLAMADA() {
		Expect(22 /* "evalua" */);
		wchar_t* name; 
		Ident(name);
		Expect(10 /* "(" */);
		if (StartOf(3)) {
			SUPEXPRESION();
			while (la->kind == 11 /* "," */) {
				Get();
				SUPEXPRESION();
			}
		}
		Expect(12 /* ")" */);
		for ( int i = 0; i < procs_dir.size(); i++ ) { 
		Procedure *p = procs_dir[i];
		if(coco_string_equal(name, p->name)){
			/*Si encontro el directorio de procedimiento, generar los cuadruplos necesarios*/
			create_quadruple(ERA, p->id, -1, -1);
			for ( int i = 0; i < p->params.size(); i++ ) {
				if(p->params[i]->type == pTipos.top() ){
					pTipos.pop();
					create_quadruple(PARAM, pOp.top(), -1, -1);
					pOp.top();
				} else {
					Err(L"Al parecer hubo un error con los parametros");
					exit(1);
				}
			}
			create_quadruple(GOSUB, p->first_quadruple, -1, -1);
			break;
		} else if ( i ==  procs_dir.size() -1) {
			/*No lo encontro, hay que marcar un error indicando que el procedimiento no esta*/
			Err(L"El procedimiento que quieres llamar no ha sido declarado ");
			exit(1);
		}
		}
		
}

void Parser::TERM() {
		cout << "check in term"; 
		FACTOR();
		if( !(pOper.empty()) && ( pOper.top() == times || 
		pOper.top() == slash )) {
		int el3 = pTipos.top();
		pTipos.pop();
		int el2 = pTipos.top();
		pTipos.pop();
		int el1 = pOper.top();
		pOper.pop();
		/*Check if the operation is possible and if it is, generate the quadruple*/
		int result = cubo[el1][el2][el3];
		int dir = 0;
		int op2 = pOp.top();
		pOp.pop();
		int op1 = pOp.top();
		pOp.pop();
		cout << "Todo bien hasta aqui";
		if(result != 0){
			dir = get_var_dir(result, temporal_s);
			switch (el1) {
				case 2: create_quadruple(MUL, op1, op2, dir);	
					break;
				case 3: create_quadruple(DIV, op1, op2, dir);
					break;
			}
			/*Incrementar el numero de temporales del procedimeinto dependiendo del tipo*/
			Procedure *p = procs_dir.back();
			switch (result) {
				case 0: p->num_tem_ent ++;
					break;
				case 4: p->num_tem_dec ++;
					break;		
			}
			pOp.push(dir);
			pTipos.push(result);
		} else {
			Err(L"La operacion no puede ser realizada, revisa los tipos de variables");
			exit(1);
		}
		}
		
		if (la->kind == 23 /* "*" */ || la->kind == 24 /* "/" */) {
			if (la->kind == 23 /* "*" */) {
				Get();
				pOper.push(times); 
				TERM();
			} else {
				Get();
				pOper.push(slash); 
				TERM();
			}
		}
		cout << "checkout term"; 
}

void Parser::FACTOR() {
		if (la->kind == 55 /* "pot" */) {
			cout << "checkin factor"; 
			Get();
			Expect(10 /* "(" */);
			EXP();
			Expect(11 /* "," */);
			EXP();
			Expect(12 /* ")" */);
		} else if (la->kind == 10 /* "(" */ || la->kind == 56 /* "raiz" */) {
			if (la->kind == 56 /* "raiz" */) {
				Get();
			}
			Expect(10 /* "(" */);
			pOper.push(false_f);	
			EXP();
			Expect(12 /* ")" */);
			pOper.pop();
			cout << "Si quite el parentesis";
		} else if (StartOf(4)) {
			VAR();
			cout << "checkout factor"; 
		} else SynErr(77);
}

void Parser::VAR() {
		if (StartOf(5)) {
			if (la->kind == 28 /* "+" */ || la->kind == 29 /* "-" */) {
				if (la->kind == 28 /* "+" */) {
					Get();
				} else {
					Get();
				}
			}
			if (la->kind == _id) {
				wchar_t* name; 
				Ident(name);
				Procedure *p = procs_dir.back();
				for ( int i = 0; i < p->variables.size(); i++ ) {
				Variable *v = p->variables[i];
				if(coco_string_equal(name, v->name)){
					pOp.push(v->dir);									
					cout << "Meti en la pila de op el valor " << v->dir;
					pTipos.push(v->type);
					break;
				}else if (i == p->variables.size() -1){
					Err(L"Parece que la variable no ha sido declarada ");
					exit(1);
				}
				}
				
				
			} else if (la->kind == _ent) {
				int value; 
				Get();
				swscanf(t->val, L"%d", &value);
				/*Validar que el arreglo no este vacio, si esta vacio agregarlo sin buscar*/
				if( !entTable.empty() ) {
				/*Buscar el elemento en la tabla de constantes enteras para ver si no lo han agregado*/
				for(int i = 0; i < entTable.size(); i++){	
				IntegerConst *ic = entTable[i];
				if(ic->value == value){
				pOp.push(ic->dir);
				pTipos.push(ent);
				break;
				} else if (i == entTable.size()-1) {
				/*No existe, hay que agregarla*/
				int dir = get_var_dir(constant_s, ent);
				IntegerConst *nic = new IntegerConst(dir, value);
				entTable.push_back(nic);
				pOp.push(dir);
				pTipos.push(ent);
				}
				}
				} else {
				int dir = get_var_dir(constant_s, ent);
				IntegerConst *nic = new IntegerConst(dir, value);
				entTable.push_back(nic);
				pOp.push(dir);
				pTipos.push(ent);
				}	
				
			} else if (la->kind == _dec) {
				float value; 
				Get();
				swscanf(t->val, L"%f", &value);
				/*Validar que el arreglo no este vacio, si esta vacio agregarlo sin buscar*/
				if( !decTable.empty() ) {	
				/*Buscar el elemento en la tabla de constantes decimales para ver si no lo han agregado*/
				for(int i = 0; i < decTable.size(); i++){	
				FloatConst *fc = decTable[i];
				if(fc->value == value){
				pOp.push(fc->dir);
				pTipos.push(dec);
				break;
				} else if (i == decTable.size()-1) {
				/*No existe, hay que agregarla*/
				int dir = get_var_dir(constant_s, dec);
				FloatConst *nfc = new FloatConst(dir, value);
				decTable.push_back(nfc);
				pOp.push(dir);
				pTipos.push(dec);
				}
				}
				} else {
				/*No existe, hay que agregarla*/
				int dir = get_var_dir(constant_s, dec);
				FloatConst *nfc = new FloatConst(dir, value);
				decTable.push_back(nfc);
				pOp.push(dir);
				pTipos.push(dec);
				}	
				
			} else SynErr(78);
		} else if (la->kind == _log) {
			bool value; 
			Log(value);
			if( !boolTable.empty() ) {	
			/*Buscar el elemento en la tabla de constantes logicas para ver si no lo han agregado*/
			for(int i = 0; i < boolTable.size(); i++){	
				BoolConst *bc = boolTable[i];
				if(bc->value == value){
					pOp.push(bc->dir);
					pTipos.push(log);
					break;
				} else if (i == boolTable.size()-1) {
					/*No existe, hay que agregarla*/
					int dir = get_var_dir(constant_s, log);
					BoolConst *nbc = new BoolConst(dir, value);
					boolTable.push_back(nbc);
					pOp.push(dir);
					pTipos.push(log);
				}
			}
			} else {
			/*No existe, hay que agregarla*/
			int dir = get_var_dir(constant_s, log);
			BoolConst *nbc = new BoolConst(dir, value);
			boolTable.push_back(nbc);
			pOp.push(dir);
			pTipos.push(log);
			}	
			
		} else if (la->kind == _fra) {
			wchar_t* value; 
			Fra(value);
			if( !fraTable.empty() ) {	
			/*Buscar el elemento en la tabla de constantes logicas para ver si no lo han agregado*/
			for(int i = 0; i < fraTable.size(); i++){	
				FraConst *frc = fraTable[i];
				if(coco_string_equal(value, frc->value)){
					pOp.push(frc->dir);
					pTipos.push(fra);
					break;
				} else if (i == fraTable.size()-1) {
					/*No existe, hay que agregarla*/
					int dir = get_var_dir(constant_s, fra);
					FraConst *nfrc = new FraConst(dir, value);
					fraTable.push_back(nfrc);
					pOp.push(dir);
					pTipos.push(fra);
				}
			}
			} else {
			/*No existe, hay que agregarla*/
			int dir = get_var_dir(constant_s, fra);
			FraConst *nfrc = new FraConst(dir, value);
			fraTable.push_back(nfrc);
			pOp.push(dir);
			pTipos.push(fra);
			}	
			
		} else SynErr(79);
}

void Parser::Log(bool &value) {
		Expect(_log);
		if(coco_string_equal(coco_string_create(t->val), L"false")){
		value = false;	
		} else {
		value = true;
		} 
}

void Parser::Fra(wchar_t* &value) {
		Expect(_fra);
		value = coco_string_create(t->val); 
}

void Parser::EXPRESION() {
		cout << "Checkin expression"; 
		EXP();
		if (StartOf(6)) {
			if (la->kind == 30 /* "<" */) {
				Get();
				pOper.push(less); 
			} else if (la->kind == 31 /* ">" */) {
				Get();
				pOper.push(gtr); 
			} else if (la->kind == 32 /* "<>" */) {
				Get();
				pOper.push(diff); 
			} else {
				Get();
				pOper.push(isequ); 
			}
			EXP();
			if( !(pOper.empty()) && (pOper.top() == less || 
				pOper.top() == gtr ||  pOper.top() == diff || pOper.top() == isequ )) {
			int el3 = pTipos.top();
			pTipos.pop();
			int el2 = pTipos.top();
			pTipos.pop();
			int el1 = pOper.top();
			pOper.pop();
			/*Check if the operation is possible and if it, generate the quadruple*/
			int result = cubo[el1][el2][el3];
			int dir = 0;
			int op2 = pOp.top();
			pOp.pop();
			int op1 = pOp.top();
			pOp.pop();
			if(result != 0){
				dir = get_var_dir(result, temporal_s);
				switch (el1) {
					case 6: create_quadruple(LSS, op1, op2, dir);
						break;
					case 7: create_quadruple(GTR, op1, op2, dir);
						break;	
					case 8: create_quadruple(DIF, op1, op2, dir);
						break;
					case 10: create_quadruple(IS_EQU, op1, op2, dir);
						break;
				}
				/* Incrementar el tipo de temporales solicitado*/
				Procedure *p = procs_dir.back();
				p->num_tem_log ++;
				pOp.push(dir);
				pTipos.push(result);
			}else{
				Err(L"La operacion no se puede realizar");
				exit(1);
			}
			} 
			
		}
		cout << "Checkout Expression"; 
}

void Parser::COLOR(int &numColor) {
		switch (la->kind) {
		case 36 /* "negro" */: {
			Get();
			numColor = NEGRO; 
			break;
		}
		case 37 /* "azul" */: {
			Get();
			numColor = AZUL; 
			break;
		}
		case 38 /* "rojo" */: {
			Get();
			numColor = ROJO; 
			break;
		}
		case 39 /* "amarillo" */: {
			Get();
			numColor = AMARILLO; 
			break;
		}
		case 40 /* "verde" */: {
			Get();
			numColor = VERDE; 
			break;
		}
		case 41 /* "rosa" */: {
			Get();
			numColor = ROSA; 
			break;
		}
		case 42 /* "celeste" */: {
			Get();
			numColor = CELESTE; 
			break;
		}
		case 43 /* "morado" */: {
			Get();
			numColor = MORADO; 
			break;
		}
		case 44 /* "naranja" */: {
			Get();
			numColor = NARANJA; 
			break;
		}
		default: SynErr(80); break;
		}
}

void Parser::ALTO() {
		Expect(57 /* "alto" */);
		Expect(16 /* ";" */);
}

void Parser::AVANZA() {
		int tipoMovimiento; 
		Expect(60 /* "avanza" */);
		if (la->kind == 50 /* "arriba" */) {
			Get();
			tipoMovimiento = 0; 
		} else if (la->kind == 51 /* "abajo" */) {
			Get();
			tipoMovimiento = 1; 
		} else if (la->kind == 61 /* "izquierda" */) {
			Get();
			tipoMovimiento = 2; 
		} else if (la->kind == 62 /* "derecha" */) {
			Get();
			tipoMovimiento = 2; 
		} else SynErr(81);
		EXP();
		int tipoAvance = pTipos.top();
		pTipos.pop();
		int dirAvance = pOp.top();
		pOp.pop();
		if(tipoAvance == ent){
		switch(tipoMovimiento){
		case 0: create_quadruple(mARRIBA, dirAvance, -1, -1);
		break;
		case 1: create_quadruple(mABAJO, dirAvance, -1, -1);
		break;	
		case 2: create_quadruple(mIZQUIERDA, dirAvance, -1, -1);
		break;	
		case 3: create_quadruple(mDERECHA, dirAvance, -1, -1);
		break;		
		}
		} else {
		Err(L"Solo puedes moverte un numero entero de pasos");
		exit(1);
		}	
		
		Expect(63 /* "pasos" */);
		Expect(16 /* ";" */);
}

void Parser::GIRA() {
		Expect(64 /* "gira" */);
		int tipoGira = pTipos.top(); 
		pTipos.pop();
		int dirGira = pOp.top();
		pOp.pop();
		
		EXP();
		if(tipoGira == ent){
		create_quadruple(mGIRA, dirGira, -1, -1);
		} else {
		Err(L"Solo puedes girar un numero entero de grados");
		exit(1);
		}
		
		Expect(65 /* "grados" */);
		Expect(16 /* ";" */);
}

void Parser::ESPERA() {
		Expect(66 /* "espera" */);
		EXP();
		int tipoEspera = pTipos.top();
		pTipos.pop();
		int dirEspera = pOp.top();
		pOp.pop();
		if(tipoEspera == ent){
		create_quadruple(mESPERA, dirEspera, -1, -1);
		} else {
		Err(L"Solo puedes esperar una cantidad entera de segundos");
		exit(1);
		}
		
		Expect(67 /* "segundos" */);
		Expect(16 /* ";" */);
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	MemeCode();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 69;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[7][71] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, T,x,T,x, x,x,T,x, x,x,x,x, x,x,x,x, T,x,x},
		{x,T,T,T, T,T,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, T,T,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ent expected"); break;
			case 2: s = coco_string_create(L"dec expected"); break;
			case 3: s = coco_string_create(L"log expected"); break;
			case 4: s = coco_string_create(L"fra expected"); break;
			case 5: s = coco_string_create(L"id expected"); break;
			case 6: s = coco_string_create(L"\"programa\" expected"); break;
			case 7: s = coco_string_create(L"\"{\" expected"); break;
			case 8: s = coco_string_create(L"\"}\" expected"); break;
			case 9: s = coco_string_create(L"\"func\" expected"); break;
			case 10: s = coco_string_create(L"\"(\" expected"); break;
			case 11: s = coco_string_create(L"\",\" expected"); break;
			case 12: s = coco_string_create(L"\")\" expected"); break;
			case 13: s = coco_string_create(L"\"lista\" expected"); break;
			case 14: s = coco_string_create(L"\"con\" expected"); break;
			case 15: s = coco_string_create(L"\"var\" expected"); break;
			case 16: s = coco_string_create(L"\";\" expected"); break;
			case 17: s = coco_string_create(L"\"regresa\" expected"); break;
			case 18: s = coco_string_create(L"\"[\" expected"); break;
			case 19: s = coco_string_create(L"\"]\" expected"); break;
			case 20: s = coco_string_create(L"\"=\" expected"); break;
			case 21: s = coco_string_create(L"\"lee\" expected"); break;
			case 22: s = coco_string_create(L"\"evalua\" expected"); break;
			case 23: s = coco_string_create(L"\"*\" expected"); break;
			case 24: s = coco_string_create(L"\"/\" expected"); break;
			case 25: s = coco_string_create(L"\"ent\" expected"); break;
			case 26: s = coco_string_create(L"\"dec\" expected"); break;
			case 27: s = coco_string_create(L"\"log\" expected"); break;
			case 28: s = coco_string_create(L"\"+\" expected"); break;
			case 29: s = coco_string_create(L"\"-\" expected"); break;
			case 30: s = coco_string_create(L"\"<\" expected"); break;
			case 31: s = coco_string_create(L"\">\" expected"); break;
			case 32: s = coco_string_create(L"\"<>\" expected"); break;
			case 33: s = coco_string_create(L"\"==\" expected"); break;
			case 34: s = coco_string_create(L"\"&\" expected"); break;
			case 35: s = coco_string_create(L"\"|\" expected"); break;
			case 36: s = coco_string_create(L"\"negro\" expected"); break;
			case 37: s = coco_string_create(L"\"azul\" expected"); break;
			case 38: s = coco_string_create(L"\"rojo\" expected"); break;
			case 39: s = coco_string_create(L"\"amarillo\" expected"); break;
			case 40: s = coco_string_create(L"\"verde\" expected"); break;
			case 41: s = coco_string_create(L"\"rosa\" expected"); break;
			case 42: s = coco_string_create(L"\"celeste\" expected"); break;
			case 43: s = coco_string_create(L"\"morado\" expected"); break;
			case 44: s = coco_string_create(L"\"naranja\" expected"); break;
			case 45: s = coco_string_create(L"\"pluma\" expected"); break;
			case 46: s = coco_string_create(L"\":\" expected"); break;
			case 47: s = coco_string_create(L"\"color\" expected"); break;
			case 48: s = coco_string_create(L"\"tama\0x00f1o\" expected"); break;
			case 49: s = coco_string_create(L"\"borra\" expected"); break;
			case 50: s = coco_string_create(L"\"arriba\" expected"); break;
			case 51: s = coco_string_create(L"\"abajo\" expected"); break;
			case 52: s = coco_string_create(L"\"repetir\" expected"); break;
			case 53: s = coco_string_create(L"\"veces\" expected"); break;
			case 54: s = coco_string_create(L"\"mientras\" expected"); break;
			case 55: s = coco_string_create(L"\"pot\" expected"); break;
			case 56: s = coco_string_create(L"\"raiz\" expected"); break;
			case 57: s = coco_string_create(L"\"alto\" expected"); break;
			case 58: s = coco_string_create(L"\"si\" expected"); break;
			case 59: s = coco_string_create(L"\"si_no\" expected"); break;
			case 60: s = coco_string_create(L"\"avanza\" expected"); break;
			case 61: s = coco_string_create(L"\"izquierda\" expected"); break;
			case 62: s = coco_string_create(L"\"derecha\" expected"); break;
			case 63: s = coco_string_create(L"\"pasos\" expected"); break;
			case 64: s = coco_string_create(L"\"gira\" expected"); break;
			case 65: s = coco_string_create(L"\"grados\" expected"); break;
			case 66: s = coco_string_create(L"\"espera\" expected"); break;
			case 67: s = coco_string_create(L"\"segundos\" expected"); break;
			case 68: s = coco_string_create(L"\"meme\" expected"); break;
			case 69: s = coco_string_create(L"??? expected"); break;
			case 70: s = coco_string_create(L"invalid ESTATUTO"); break;
			case 71: s = coco_string_create(L"invalid CICLO"); break;
			case 72: s = coco_string_create(L"invalid MOVIMIENTO"); break;
			case 73: s = coco_string_create(L"invalid ASIGLEC"); break;
			case 74: s = coco_string_create(L"invalid PLUMA"); break;
			case 75: s = coco_string_create(L"invalid TIPO"); break;
			case 76: s = coco_string_create(L"invalid ASIGNA"); break;
			case 77: s = coco_string_create(L"invalid FACTOR"); break;
			case 78: s = coco_string_create(L"invalid VAR"); break;
			case 79: s = coco_string_create(L"invalid VAR"); break;
			case 80: s = coco_string_create(L"invalid COLOR"); break;
			case 81: s = coco_string_create(L"invalid AVANZA"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}


}