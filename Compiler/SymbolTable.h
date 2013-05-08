#if !defined(MEMECODE_SYMBOLTABLE_H__)
#define MEMECODE_SYMBOLTABLE_H__


#include <vector>
#include <iostream>
#include "wchar.h"

using namespace std;

namespace MemeCode{

class Parser;
class Errors;

class Variable {
	public:
		wchar_t* name;
		int type;
		int dir;
		int is_array;
		int arr_length;
		
		Variable(wchar_t* name, int type, int dir){
			this->name = name;
			this->type = type;
			this->dir = dir;
			this->is_array = 0;
			this->arr_length = 0;
		}
		
		Variable(wchar_t* name, int type, int dir, int arr_length){
			this->name = name;
			this->type = type;
			this->dir = dir;
			this->is_array = 1;
			this->arr_length = arr_length; 
		}
};

class IntegerConst {
	public:
		int dir;
		int value;
		
		IntegerConst(int dir, int value){
			this->dir = dir;
			this->value = value;
		}
};

class FloatConst {
	public:
		int dir;
		float value;
		
		FloatConst (int dir, float value){
			this->dir = dir;
			this->value = value;
		}
};

class BoolConst {
	public:
		int dir;
		bool value;
		
		BoolConst (int dir, bool value){
			this->dir = dir;
			this->value = value;
		}
};

class FraConst {
	public:
		int dir;
		wchar_t* value;
		
		FraConst(int dir, wchar_t* value){
			this->dir = dir;
			this->value = value;
		}
};

class DirConst {
	public:
		int dir;
		int value;
		
		DirConst(int dir, int value){
			this->dir = dir;
			this->value = value;
		}
};


class Procedure {
	public:
		int id;
		wchar_t* name;
		int type;
		int first_quadruple;
		int num_params_ent;
		int num_params_fra;
		int num_params_log;
		int num_params_dec;
		int num_ent;
		int num_fra;
		int num_log;
		int num_dec;
		int num_dir;
		int num_tem_ent;
		int num_tem_fra;
		int num_tem_log;
		int num_tem_dec;
		int num_tem_dir;
		vector<Variable *> params;
		vector<Variable *> variables;
		
		Procedure(int id, wchar_t* name, int type){
			this->id = id;
			this->name = name;
			this->type = type;
			this->first_quadruple = -1;
			this->num_ent = 0;
			this->num_fra = 0;
			this->num_log = 0;
			this->num_dec = 0;
			this->num_dir = 0;
			this->num_params_ent = 0;
			this->num_params_fra = 0;
			this->num_params_log = 0;
			this->num_params_dec = 0;
			this->num_tem_ent = 0;
			this->num_tem_fra = 0;
			this->num_tem_log = 0;
			this->num_tem_dec = 0;
			this->num_tem_dir = 0;
		}
		
		/*Function to add a new variable in a procedure*/
		void add_var(wchar_t* name, int type, int dir){
			cout << "Add variable check in ";
			/*Create a new variable*/
			Variable *v = new Variable(name, type, dir);
			variables.push_back(v);
			switch (type) {
				case 1: num_ent ++;
					break;
				case 2: num_fra ++;
					break;
				case 3: num_log ++;
					break;
				case 4: num_dec ++;
					break;	
			}
			cout << "Add variable check out ";
		}

		/*Function to add a new variable in a procedure*/
		void add_var_arr(wchar_t* name, int type, int dir, int arr_length){
			cout << "Add variable check in ";
			/*Create a new variable*/
			Variable *v = new Variable(name, type, dir, arr_length);
			variables.push_back(v);
			switch (type) {
				case 1: num_ent += arr_length;
					break;
				case 2: num_fra += arr_length;
					break;
				case 3: num_log += arr_length;
					break;
				case 4: num_dec += arr_length;
					break;	
			}
			cout << "Add variable check out ";
		}		
		
		/*Function to add a new variable in a procedure*/
		void add_param(wchar_t* name, int type, int dir){
			//cout << "Add parameter check in ";
			/*Create a new variable*/
			Variable *v = new Variable(name, type, dir);
			params.push_back(v);
			switch (type) {
				case 1: num_params_ent ++;
					break;
				case 2: num_params_fra ++;
					break;
				case 3: num_params_log ++;
					break;
				case 4: num_params_dec ++;
					break;	
			}
		}
};

} //namespace

#endif // !defined(MEMECODE_SYMBOLTABLE_H__)
