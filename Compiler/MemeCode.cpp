#include "stdio.h"
#include "SymbolTable.h"
#include "Parser.h"
#include "Scanner.h"
#include <sys/timeb.h>
#include <wchar.h>

using namespace MemeCode;

int main (int argc, char *argv[]) {

	if (argc == 2) {
		wchar_t *fileName = coco_string_create(argv[1]);
		Scanner *scanner = new Scanner(fileName);
		Parser *parser = new Parser(scanner);
		parser->Parse();
		int x = parser->errors->count;
		if (x != 0) {
			printf("%i errors detected", x);
		}
		// Hacer los delete's
	} else{
		printf("-- No source file specified\n");
	}
	return 0;
}