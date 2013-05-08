

#include <memory.h>
#include <string.h>
#include "Scanner.h"

namespace MemeCode {


// string handling, wide character


wchar_t* coco_string_create(const wchar_t* value) {
	return coco_string_create(value, 0);
}

wchar_t* coco_string_create(const wchar_t *value, int startIndex) {
	int valueLen = 0;
	int len = 0;

	if (value) {
		valueLen = wcslen(value);
		len = valueLen - startIndex;
	}

	return coco_string_create(value, startIndex, len);
}

wchar_t* coco_string_create(const wchar_t *value, int startIndex, int length) {
	int len = 0;
	wchar_t* data;

	if (value) { len = length; }
	data = new wchar_t[len + 1];
	wcsncpy(data, &(value[startIndex]), len);
	data[len] = 0;

	return data;
}

wchar_t* coco_string_create_upper(const wchar_t* data) {
	if (!data) { return NULL; }

	int dataLen = 0;
	if (data) { dataLen = wcslen(data); }

	wchar_t *newData = new wchar_t[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		if ((L'a' <= data[i]) && (data[i] <= L'z')) {
			newData[i] = data[i] + (L'A' - L'a');
		}
		else { newData[i] = data[i]; }
	}

	newData[dataLen] = L'\0';
	return newData;
}

wchar_t* coco_string_create_lower(const wchar_t* data) {
	if (!data) { return NULL; }
	int dataLen = wcslen(data);
	return coco_string_create_lower(data, 0, dataLen);
}

wchar_t* coco_string_create_lower(const wchar_t* data, int startIndex, int dataLen) {
	if (!data) { return NULL; }

	wchar_t* newData = new wchar_t[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		wchar_t ch = data[startIndex + i];
		if ((L'A' <= ch) && (ch <= L'Z')) {
			newData[i] = ch - (L'A' - L'a');
		}
		else { newData[i] = ch; }
	}
	newData[dataLen] = L'\0';
	return newData;
}

wchar_t* coco_string_create_append(const wchar_t* data1, const wchar_t* data2) {
	wchar_t* data;
	int data1Len = 0;
	int data2Len = 0;

	if (data1) { data1Len = wcslen(data1); }
	if (data2) {data2Len = wcslen(data2); }

	data = new wchar_t[data1Len + data2Len + 1];

	if (data1) { wcscpy(data, data1); }
	if (data2) { wcscpy(data + data1Len, data2); }

	data[data1Len + data2Len] = 0;

	return data;
}

wchar_t* coco_string_create_append(const wchar_t *target, const wchar_t appendix) {
	int targetLen = coco_string_length(target);
	wchar_t* data = new wchar_t[targetLen + 2];
	wcsncpy(data, target, targetLen);
	data[targetLen] = appendix;
	data[targetLen + 1] = 0;
	return data;
}

void coco_string_delete(wchar_t* &data) {
	delete [] data;
	data = NULL;
}

int coco_string_length(const wchar_t* data) {
	if (data) { return wcslen(data); }
	return 0;
}

bool coco_string_endswith(const wchar_t* data, const wchar_t *end) {
	int dataLen = wcslen(data);
	int endLen = wcslen(end);
	return (endLen <= dataLen) && (wcscmp(data + dataLen - endLen, end) == 0);
}

int coco_string_indexof(const wchar_t* data, const wchar_t value) {
	const wchar_t* chr = wcschr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

int coco_string_lastindexof(const wchar_t* data, const wchar_t value) {
	const wchar_t* chr = wcsrchr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

void coco_string_merge(wchar_t* &target, const wchar_t* appendix) {
	if (!appendix) { return; }
	wchar_t* data = coco_string_create_append(target, appendix);
	delete [] target;
	target = data;
}

bool coco_string_equal(const wchar_t* data1, const wchar_t* data2) {
	return wcscmp( data1, data2 ) == 0;
}

int coco_string_compareto(const wchar_t* data1, const wchar_t* data2) {
	return wcscmp(data1, data2);
}

int coco_string_hash(const wchar_t *data) {
	int h = 0;
	if (!data) { return 0; }
	while (*data != 0) {
		h = (h * 7) ^ *data;
		++data;
	}
	if (h < 0) { h = -h; }
	return h;
}

// string handling, ascii character

wchar_t* coco_string_create(const char* value) {
	int len = 0;
	if (value) { len = strlen(value); }
	wchar_t* data = new wchar_t[len + 1];
	for (int i = 0; i < len; ++i) { data[i] = (wchar_t) value[i]; }
	data[len] = 0;
	return data;
}

char* coco_string_create_char(const wchar_t *value) {
	int len = coco_string_length(value);
	char *res = new char[len + 1];
	for (int i = 0; i < len; ++i) { res[i] = (char) value[i]; }
	res[len] = 0;
	return res;
}

void coco_string_delete(char* &data) {
	delete [] data;
	data = NULL;
}


Token::Token() {
	kind = 0;
	pos  = 0;
	col  = 0;
	line = 0;
	val  = NULL;
	next = NULL;
}

Token::~Token() {
	coco_string_delete(val);
}

Buffer::Buffer(FILE* s, bool isUserStream) {
// ensure binary read on windows
#if _MSC_VER >= 1300
	_setmode(_fileno(s), _O_BINARY);
#endif
	stream = s; this->isUserStream = isUserStream;
	if (CanSeek()) {
		fseek(s, 0, SEEK_END);
		fileLen = ftell(s);
		fseek(s, 0, SEEK_SET);
		bufLen = (fileLen < COCO_MAX_BUFFER_LENGTH) ? fileLen : COCO_MAX_BUFFER_LENGTH;
		bufStart = INT_MAX; // nothing in the buffer so far
	} else {
		fileLen = bufLen = bufStart = 0;
	}
	bufCapacity = (bufLen>0) ? bufLen : COCO_MIN_BUFFER_LENGTH;
	buf = new unsigned char[bufCapacity];	
	if (fileLen > 0) SetPos(0);          // setup  buffer to position 0 (start)
	else bufPos = 0; // index 0 is already after the file, thus Pos = 0 is invalid
	if (bufLen == fileLen && CanSeek()) Close();
}

Buffer::Buffer(Buffer *b) {
	buf = b->buf;
	bufCapacity = b->bufCapacity;
	b->buf = NULL;
	bufStart = b->bufStart;
	bufLen = b->bufLen;
	fileLen = b->fileLen;
	bufPos = b->bufPos;
	stream = b->stream;
	b->stream = NULL;
	isUserStream = b->isUserStream;
}

Buffer::Buffer(const unsigned char* buf, int len) {
	this->buf = new unsigned char[len];
	memcpy(this->buf, buf, len*sizeof(unsigned char));
	bufStart = 0;
	bufCapacity = bufLen = len;
	fileLen = len;
	bufPos = 0;
	stream = NULL;
}

Buffer::~Buffer() {
	Close(); 
	if (buf != NULL) {
		delete [] buf;
		buf = NULL;
	}
}

void Buffer::Close() {
	if (!isUserStream && stream != NULL) {
		fclose(stream);
		stream = NULL;
	}
}

int Buffer::Read() {
	if (bufPos < bufLen) {
		return buf[bufPos++];
	} else if (GetPos() < fileLen) {
		SetPos(GetPos()); // shift buffer start to Pos
		return buf[bufPos++];
	} else if ((stream != NULL) && !CanSeek() && (ReadNextStreamChunk() > 0)) {
		return buf[bufPos++];
	} else {
		return EoF;
	}
}

int Buffer::Peek() {
	int curPos = GetPos();
	int ch = Read();
	SetPos(curPos);
	return ch;
}

// beg .. begin, zero-based, inclusive, in byte
// end .. end, zero-based, exclusive, in byte
wchar_t* Buffer::GetString(int beg, int end) {
	int len = 0;
	wchar_t *buf = new wchar_t[end - beg];
	int oldPos = GetPos();
	SetPos(beg);
	while (GetPos() < end) buf[len++] = (wchar_t) Read();
	SetPos(oldPos);
	wchar_t *res = coco_string_create(buf, 0, len);
	coco_string_delete(buf);
	return res;
}

int Buffer::GetPos() {
	return bufPos + bufStart;
}

void Buffer::SetPos(int value) {
	if ((value >= fileLen) && (stream != NULL) && !CanSeek()) {
		// Wanted position is after buffer and the stream
		// is not seek-able e.g. network or console,
		// thus we have to read the stream manually till
		// the wanted position is in sight.
		while ((value >= fileLen) && (ReadNextStreamChunk() > 0));
	}

	if ((value < 0) || (value > fileLen)) {
		wprintf(L"--- buffer out of bounds access, position: %d\n", value);
		exit(1);
	}

	if ((value >= bufStart) && (value < (bufStart + bufLen))) { // already in buffer
		bufPos = value - bufStart;
	} else if (stream != NULL) { // must be swapped in
		fseek(stream, value, SEEK_SET);
		bufLen = fread(buf, sizeof(unsigned char), bufCapacity, stream);
		bufStart = value; bufPos = 0;
	} else {
		bufPos = fileLen - bufStart; // make Pos return fileLen
	}
}

// Read the next chunk of bytes from the stream, increases the buffer
// if needed and updates the fields fileLen and bufLen.
// Returns the number of bytes read.
int Buffer::ReadNextStreamChunk() {
	int free = bufCapacity - bufLen;
	if (free == 0) {
		// in the case of a growing input stream
		// we can neither seek in the stream, nor can we
		// foresee the maximum length, thus we must adapt
		// the buffer size on demand.
		bufCapacity = bufLen * 2;
		unsigned char *newBuf = new unsigned char[bufCapacity];
		memcpy(newBuf, buf, bufLen*sizeof(unsigned char));
		delete [] buf;
		buf = newBuf;
		free = bufLen;
	}
	int read = fread(buf + bufLen, sizeof(unsigned char), free, stream);
	if (read > 0) {
		fileLen = bufLen = (bufLen + read);
		return read;
	}
	// end of stream reached
	return 0;
}

bool Buffer::CanSeek() {
	return (stream != NULL) && (ftell(stream) != -1);
}

int UTF8Buffer::Read() {
	int ch;
	do {
		ch = Buffer::Read();
		// until we find a utf8 start (0xxxxxxx or 11xxxxxx)
	} while ((ch >= 128) && ((ch & 0xC0) != 0xC0) && (ch != EoF));
	if (ch < 128 || ch == EoF) {
		// nothing to do, first 127 chars are the same in ascii and utf8
		// 0xxxxxxx or end of file character
	} else if ((ch & 0xF0) == 0xF0) {
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		int c1 = ch & 0x07; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F; ch = Buffer::Read();
		int c4 = ch & 0x3F;
		ch = (((((c1 << 6) | c2) << 6) | c3) << 6) | c4;
	} else if ((ch & 0xE0) == 0xE0) {
		// 1110xxxx 10xxxxxx 10xxxxxx
		int c1 = ch & 0x0F; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F;
		ch = (((c1 << 6) | c2) << 6) | c3;
	} else if ((ch & 0xC0) == 0xC0) {
		// 110xxxxx 10xxxxxx
		int c1 = ch & 0x1F; ch = Buffer::Read();
		int c2 = ch & 0x3F;
		ch = (c1 << 6) | c2;
	}
	return ch;
}

Scanner::Scanner(const unsigned char* buf, int len) {
	buffer = new Buffer(buf, len);
	Init();
}

Scanner::Scanner(const wchar_t* fileName) {
	FILE* stream;
	char *chFileName = coco_string_create_char(fileName);
	if ((stream = fopen(chFileName, "rb")) == NULL) {
		wprintf(L"--- Cannot open file %ls\n", fileName);
		exit(1);
	}
	coco_string_delete(chFileName);
	buffer = new Buffer(stream, false);
	Init();
}

Scanner::Scanner(FILE* s) {
	buffer = new Buffer(s, true);
	Init();
}

Scanner::~Scanner() {
	char* cur = (char*) firstHeap;

	while(cur != NULL) {
		cur = *(char**) (cur + COCO_HEAP_BLOCK_SIZE);
		free(firstHeap);
		firstHeap = cur;
	}
	delete [] tval;
	delete buffer;
}

void Scanner::Init() {
	EOL    = '\n';
	eofSym = 0;
	maxT = 70;
	noSym = 70;
	int i;
	for (i = 48; i <= 57; ++i) start.set(i, 12);
	for (i = 65; i <= 90; ++i) start.set(i, 11);
	start.set(116, 217);
	start.set(102, 218);
	start.set(34, 9);
	start.set(112, 219);
	start.set(123, 21);
	start.set(125, 22);
	start.set(111, 23);
	start.set(58, 30);
	start.set(114, 220);
	start.set(40, 34);
	start.set(41, 35);
	start.set(44, 43);
	start.set(59, 44);
	start.set(105, 221);
	start.set(118, 222);
	start.set(91, 56);
	start.set(93, 57);
	start.set(61, 223);
	start.set(108, 224);
	start.set(101, 225);
	start.set(42, 70);
	start.set(47, 71);
	start.set(100, 226);
	start.set(43, 79);
	start.set(45, 80);
	start.set(60, 227);
	start.set(62, 81);
	start.set(38, 84);
	start.set(124, 85);
	start.set(110, 228);
	start.set(97, 229);
	start.set(99, 230);
	start.set(109, 231);
	start.set(98, 137);
	start.set(115, 232);
	start.set(103, 233);
		start.set(Buffer::EoF, -1);


	tvalLength = 128;
	tval = new wchar_t[tvalLength]; // text of current token

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	heap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	firstHeap = heap;
	heapEnd = (void**) (((char*) heap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heapTop = heap;
	if (sizeof(Token) > COCO_HEAP_BLOCK_SIZE) {
		wprintf(L"--- Too small COCO_HEAP_BLOCK_SIZE\n");
		exit(1);
	}

	pos = -1; line = 1; col = 0; charPos = -1;
	oldEols = 0;
	NextCh();
	if (ch == 0xEF) { // check optional byte order mark for UTF-8
		NextCh(); int ch1 = ch;
		NextCh(); int ch2 = ch;
		if (ch1 != 0xBB || ch2 != 0xBF) {
			wprintf(L"Illegal byte order mark at start of file");
			exit(1);
		}
		Buffer *oldBuf = buffer;
		buffer = new UTF8Buffer(buffer); col = 0; charPos = -1;
		delete oldBuf; oldBuf = NULL;
		NextCh();
	}


	pt = tokens = CreateToken(); // first token is a dummy
}

void Scanner::NextCh() {
	if (oldEols > 0) { ch = EOL; oldEols--; }
	else {
		pos = buffer->GetPos();
		// buffer reads unicode chars, if UTF8 has been detected
		ch = buffer->Read(); col++; charPos++;
		// replace isolated '\r' by '\n' in order to make
		// eol handling uniform across Windows, Unix and Mac
		if (ch == L'\r' && buffer->Peek() != L'\n') ch = EOL;
		if (ch == EOL) { line++; col = 0; }
	}

}

void Scanner::AddCh() {
	if (tlen >= tvalLength) {
		tvalLength *= 2;
		wchar_t *newBuf = new wchar_t[tvalLength];
		memcpy(newBuf, tval, tlen*sizeof(wchar_t));
		delete [] tval;
		tval = newBuf;
	}
	if (ch != Buffer::EoF) {
		tval[tlen++] = ch;
		NextCh();
	}
}


bool Scanner::Comment0() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == L'/') {
		NextCh();
		for(;;) {
			if (ch == 13) {
				NextCh();
				if (ch == 10) {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}

bool Scanner::Comment1() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == L'*') {
		NextCh();
		for(;;) {
			if (ch == L'*') {
				NextCh();
				if (ch == L'/') {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == L'/') {
				NextCh();
				if (ch == L'*') {
					level++; NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}


void Scanner::CreateHeapBlock() {
	void* newHeap;
	char* cur = (char*) firstHeap;

	while(((char*) tokens < cur) || ((char*) tokens > (cur + COCO_HEAP_BLOCK_SIZE))) {
		cur = *((char**) (cur + COCO_HEAP_BLOCK_SIZE));
		free(firstHeap);
		firstHeap = cur;
	}

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	newHeap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	*heapEnd = newHeap;
	heapEnd = (void**) (((char*) newHeap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heap = newHeap;
	heapTop = heap;
}

Token* Scanner::CreateToken() {
	Token *t;
	if (((char*) heapTop + (int) sizeof(Token)) >= (char*) heapEnd) {
		CreateHeapBlock();
	}
	t = (Token*) heapTop;
	heapTop = (void*) ((char*) heapTop + sizeof(Token));
	t->val = NULL;
	t->next = NULL;
	return t;
}

void Scanner::AppendVal(Token *t) {
	int reqMem = (tlen + 1) * sizeof(wchar_t);
	if (((char*) heapTop + reqMem) >= (char*) heapEnd) {
		if (reqMem > COCO_HEAP_BLOCK_SIZE) {
			wprintf(L"--- Too long token value\n");
			exit(1);
		}
		CreateHeapBlock();
	}
	t->val = (wchar_t*) heapTop;
	heapTop = (void*) ((char*) heapTop + reqMem);

	wcsncpy(t->val, tval, tlen);
	t->val[tlen] = L'\0';
}

Token* Scanner::NextToken() {
	while (ch == ' ' ||
			(ch >= 9 && ch <= 10) || ch == 13
	) NextCh();
	if ((ch == L'/' && Comment0()) || (ch == L'/' && Comment1())) return NextToken();
	int recKind = noSym;
	int recEnd = pos;
	t = CreateToken();
	t->pos = pos; t->col = col; t->line = line; t->charPos = charPos;
	int state = start.state(ch);
	tlen = 0; AddCh();

	switch (state) {
		case -1: { t->kind = eofSym; break; } // NextCh already done
		case 0: {
			case_0:
			if (recKind != noSym) {
				tlen = recEnd - t->pos;
				SetScannerBehindT();
			}
			t->kind = recKind; break;
		} // NextCh already done
		case 1:
			case_1:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_2;}
			else {goto case_0;}
		case 2:
			case_2:
			recEnd = pos; recKind = 2;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_2;}
			else {t->kind = 2; break;}
		case 3:
			case_3:
			if (ch == L'u') {AddCh(); goto case_4;}
			else {goto case_0;}
		case 4:
			case_4:
			if (ch == L'e') {AddCh(); goto case_8;}
			else {goto case_0;}
		case 5:
			case_5:
			if (ch == L'l') {AddCh(); goto case_6;}
			else {goto case_0;}
		case 6:
			case_6:
			if (ch == L's') {AddCh(); goto case_7;}
			else {goto case_0;}
		case 7:
			case_7:
			if (ch == L'e') {AddCh(); goto case_8;}
			else {goto case_0;}
		case 8:
			case_8:
			{t->kind = 3; break;}
		case 9:
			if (ch <= 65535) {AddCh(); goto case_10;}
			else {goto case_0;}
		case 10:
			case_10:
			if (ch <= L'!' || (ch >= L'#' && ch <= 65535)) {AddCh(); goto case_10;}
			else if (ch == L'"') {AddCh(); goto case_13;}
			else {goto case_0;}
		case 11:
			case_11:
			recEnd = pos; recKind = 5;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z')) {AddCh(); goto case_11;}
			else {t->kind = 5; break;}
		case 12:
			case_12:
			recEnd = pos; recKind = 1;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_12;}
			else if (ch == L'.') {AddCh(); goto case_1;}
			else {t->kind = 1; break;}
		case 13:
			case_13:
			recEnd = pos; recKind = 4;
			if (ch <= L'!' || (ch >= L'#' && ch <= 65535)) {AddCh(); goto case_10;}
			else if (ch == L'"') {AddCh(); goto case_13;}
			else {t->kind = 4; break;}
		case 14:
			case_14:
			if (ch == L'o') {AddCh(); goto case_15;}
			else {goto case_0;}
		case 15:
			case_15:
			if (ch == L'g') {AddCh(); goto case_16;}
			else {goto case_0;}
		case 16:
			case_16:
			if (ch == L'r') {AddCh(); goto case_17;}
			else {goto case_0;}
		case 17:
			case_17:
			if (ch == L'a') {AddCh(); goto case_18;}
			else {goto case_0;}
		case 18:
			case_18:
			if (ch == L'm') {AddCh(); goto case_19;}
			else {goto case_0;}
		case 19:
			case_19:
			if (ch == L'a') {AddCh(); goto case_20;}
			else {goto case_0;}
		case 20:
			case_20:
			{t->kind = 6; break;}
		case 21:
			{t->kind = 7; break;}
		case 22:
			{t->kind = 8; break;}
		case 23:
			if (ch == L'b') {AddCh(); goto case_24;}
			else {goto case_0;}
		case 24:
			case_24:
			if (ch == L't') {AddCh(); goto case_25;}
			else {goto case_0;}
		case 25:
			case_25:
			if (ch == L'i') {AddCh(); goto case_26;}
			else {goto case_0;}
		case 26:
			case_26:
			if (ch == L'e') {AddCh(); goto case_27;}
			else {goto case_0;}
		case 27:
			case_27:
			if (ch == L'n') {AddCh(); goto case_28;}
			else {goto case_0;}
		case 28:
			case_28:
			if (ch == L'e') {AddCh(); goto case_29;}
			else {goto case_0;}
		case 29:
			case_29:
			{t->kind = 9; break;}
		case 30:
			{t->kind = 10; break;}
		case 31:
			case_31:
			if (ch == L'i') {AddCh(); goto case_32;}
			else {goto case_0;}
		case 32:
			case_32:
			if (ch == L'z') {AddCh(); goto case_33;}
			else {goto case_0;}
		case 33:
			case_33:
			{t->kind = 11; break;}
		case 34:
			{t->kind = 12; break;}
		case 35:
			{t->kind = 13; break;}
		case 36:
			case_36:
			if (ch == L't') {AddCh(); goto case_37;}
			else {goto case_0;}
		case 37:
			case_37:
			if (ch == L'e') {AddCh(); goto case_38;}
			else {goto case_0;}
		case 38:
			case_38:
			if (ch == L'n') {AddCh(); goto case_39;}
			else {goto case_0;}
		case 39:
			case_39:
			if (ch == L'c') {AddCh(); goto case_40;}
			else {goto case_0;}
		case 40:
			case_40:
			if (ch == L'i') {AddCh(); goto case_41;}
			else {goto case_0;}
		case 41:
			case_41:
			if (ch == L'a') {AddCh(); goto case_42;}
			else {goto case_0;}
		case 42:
			case_42:
			{t->kind = 14; break;}
		case 43:
			{t->kind = 15; break;}
		case 44:
			{t->kind = 16; break;}
		case 45:
			case_45:
			if (ch == L'p') {AddCh(); goto case_46;}
			else {goto case_0;}
		case 46:
			case_46:
			if (ch == L'r') {AddCh(); goto case_47;}
			else {goto case_0;}
		case 47:
			case_47:
			if (ch == L'i') {AddCh(); goto case_48;}
			else {goto case_0;}
		case 48:
			case_48:
			if (ch == L'm') {AddCh(); goto case_49;}
			else {goto case_0;}
		case 49:
			case_49:
			if (ch == L'e') {AddCh(); goto case_50;}
			else {goto case_0;}
		case 50:
			case_50:
			{t->kind = 17; break;}
		case 51:
			case_51:
			if (ch == L'n') {AddCh(); goto case_52;}
			else {goto case_0;}
		case 52:
			case_52:
			if (ch == L'c') {AddCh(); goto case_53;}
			else {goto case_0;}
		case 53:
			case_53:
			{t->kind = 18; break;}
		case 54:
			case_54:
			if (ch == L'r') {AddCh(); goto case_55;}
			else {goto case_0;}
		case 55:
			case_55:
			{t->kind = 19; break;}
		case 56:
			{t->kind = 20; break;}
		case 57:
			{t->kind = 21; break;}
		case 58:
			case_58:
			if (ch == L'r') {AddCh(); goto case_59;}
			else {goto case_0;}
		case 59:
			case_59:
			if (ch == L'e') {AddCh(); goto case_60;}
			else {goto case_0;}
		case 60:
			case_60:
			if (ch == L's') {AddCh(); goto case_61;}
			else {goto case_0;}
		case 61:
			case_61:
			if (ch == L'a') {AddCh(); goto case_62;}
			else {goto case_0;}
		case 62:
			case_62:
			{t->kind = 22; break;}
		case 63:
			case_63:
			if (ch == L'e') {AddCh(); goto case_64;}
			else {goto case_0;}
		case 64:
			case_64:
			{t->kind = 24; break;}
		case 65:
			case_65:
			if (ch == L'a') {AddCh(); goto case_66;}
			else {goto case_0;}
		case 66:
			case_66:
			if (ch == L'l') {AddCh(); goto case_67;}
			else {goto case_0;}
		case 67:
			case_67:
			if (ch == L'u') {AddCh(); goto case_68;}
			else {goto case_0;}
		case 68:
			case_68:
			if (ch == L'a') {AddCh(); goto case_69;}
			else {goto case_0;}
		case 69:
			case_69:
			{t->kind = 25; break;}
		case 70:
			{t->kind = 26; break;}
		case 71:
			{t->kind = 27; break;}
		case 72:
			case_72:
			if (ch == L't') {AddCh(); goto case_73;}
			else {goto case_0;}
		case 73:
			case_73:
			{t->kind = 28; break;}
		case 74:
			case_74:
			if (ch == L'a') {AddCh(); goto case_75;}
			else {goto case_0;}
		case 75:
			case_75:
			{t->kind = 29; break;}
		case 76:
			case_76:
			{t->kind = 30; break;}
		case 77:
			case_77:
			if (ch == L'g') {AddCh(); goto case_78;}
			else {goto case_0;}
		case 78:
			case_78:
			{t->kind = 31; break;}
		case 79:
			{t->kind = 32; break;}
		case 80:
			{t->kind = 33; break;}
		case 81:
			{t->kind = 35; break;}
		case 82:
			case_82:
			{t->kind = 36; break;}
		case 83:
			case_83:
			{t->kind = 37; break;}
		case 84:
			{t->kind = 38; break;}
		case 85:
			{t->kind = 39; break;}
		case 86:
			case_86:
			if (ch == L'g') {AddCh(); goto case_87;}
			else {goto case_0;}
		case 87:
			case_87:
			if (ch == L'r') {AddCh(); goto case_88;}
			else {goto case_0;}
		case 88:
			case_88:
			if (ch == L'o') {AddCh(); goto case_89;}
			else {goto case_0;}
		case 89:
			case_89:
			{t->kind = 40; break;}
		case 90:
			case_90:
			if (ch == L'u') {AddCh(); goto case_91;}
			else {goto case_0;}
		case 91:
			case_91:
			if (ch == L'l') {AddCh(); goto case_92;}
			else {goto case_0;}
		case 92:
			case_92:
			{t->kind = 41; break;}
		case 93:
			case_93:
			if (ch == L'o') {AddCh(); goto case_94;}
			else {goto case_0;}
		case 94:
			case_94:
			{t->kind = 42; break;}
		case 95:
			case_95:
			if (ch == L'a') {AddCh(); goto case_96;}
			else {goto case_0;}
		case 96:
			case_96:
			if (ch == L'r') {AddCh(); goto case_97;}
			else {goto case_0;}
		case 97:
			case_97:
			if (ch == L'i') {AddCh(); goto case_98;}
			else {goto case_0;}
		case 98:
			case_98:
			if (ch == L'l') {AddCh(); goto case_99;}
			else {goto case_0;}
		case 99:
			case_99:
			if (ch == L'l') {AddCh(); goto case_100;}
			else {goto case_0;}
		case 100:
			case_100:
			if (ch == L'o') {AddCh(); goto case_101;}
			else {goto case_0;}
		case 101:
			case_101:
			{t->kind = 43; break;}
		case 102:
			case_102:
			if (ch == L'd') {AddCh(); goto case_103;}
			else {goto case_0;}
		case 103:
			case_103:
			if (ch == L'e') {AddCh(); goto case_104;}
			else {goto case_0;}
		case 104:
			case_104:
			{t->kind = 44; break;}
		case 105:
			case_105:
			if (ch == L'a') {AddCh(); goto case_106;}
			else {goto case_0;}
		case 106:
			case_106:
			{t->kind = 45; break;}
		case 107:
			case_107:
			if (ch == L'l') {AddCh(); goto case_108;}
			else {goto case_0;}
		case 108:
			case_108:
			if (ch == L'e') {AddCh(); goto case_109;}
			else {goto case_0;}
		case 109:
			case_109:
			if (ch == L's') {AddCh(); goto case_110;}
			else {goto case_0;}
		case 110:
			case_110:
			if (ch == L't') {AddCh(); goto case_111;}
			else {goto case_0;}
		case 111:
			case_111:
			if (ch == L'e') {AddCh(); goto case_112;}
			else {goto case_0;}
		case 112:
			case_112:
			{t->kind = 46; break;}
		case 113:
			case_113:
			if (ch == L'r') {AddCh(); goto case_114;}
			else {goto case_0;}
		case 114:
			case_114:
			if (ch == L'a') {AddCh(); goto case_115;}
			else {goto case_0;}
		case 115:
			case_115:
			if (ch == L'd') {AddCh(); goto case_116;}
			else {goto case_0;}
		case 116:
			case_116:
			if (ch == L'o') {AddCh(); goto case_117;}
			else {goto case_0;}
		case 117:
			case_117:
			{t->kind = 47; break;}
		case 118:
			case_118:
			if (ch == L'r') {AddCh(); goto case_119;}
			else {goto case_0;}
		case 119:
			case_119:
			if (ch == L'a') {AddCh(); goto case_120;}
			else {goto case_0;}
		case 120:
			case_120:
			if (ch == L'n') {AddCh(); goto case_121;}
			else {goto case_0;}
		case 121:
			case_121:
			if (ch == L'j') {AddCh(); goto case_122;}
			else {goto case_0;}
		case 122:
			case_122:
			if (ch == L'a') {AddCh(); goto case_123;}
			else {goto case_0;}
		case 123:
			case_123:
			{t->kind = 48; break;}
		case 124:
			case_124:
			if (ch == L'u') {AddCh(); goto case_125;}
			else {goto case_0;}
		case 125:
			case_125:
			if (ch == L'm') {AddCh(); goto case_126;}
			else {goto case_0;}
		case 126:
			case_126:
			if (ch == L'a') {AddCh(); goto case_127;}
			else {goto case_0;}
		case 127:
			case_127:
			{t->kind = 49; break;}
		case 128:
			case_128:
			if (ch == L'l') {AddCh(); goto case_129;}
			else {goto case_0;}
		case 129:
			case_129:
			if (ch == L'o') {AddCh(); goto case_130;}
			else {goto case_0;}
		case 130:
			case_130:
			if (ch == L'r') {AddCh(); goto case_131;}
			else {goto case_0;}
		case 131:
			case_131:
			{t->kind = 50; break;}
		case 132:
			case_132:
			if (ch == L'm') {AddCh(); goto case_133;}
			else {goto case_0;}
		case 133:
			case_133:
			if (ch == L'a') {AddCh(); goto case_134;}
			else {goto case_0;}
		case 134:
			case_134:
			if (ch == 241) {AddCh(); goto case_135;}
			else {goto case_0;}
		case 135:
			case_135:
			if (ch == L'o') {AddCh(); goto case_136;}
			else {goto case_0;}
		case 136:
			case_136:
			{t->kind = 51; break;}
		case 137:
			if (ch == L'o') {AddCh(); goto case_138;}
			else {goto case_0;}
		case 138:
			case_138:
			if (ch == L'r') {AddCh(); goto case_139;}
			else {goto case_0;}
		case 139:
			case_139:
			if (ch == L'r') {AddCh(); goto case_140;}
			else {goto case_0;}
		case 140:
			case_140:
			if (ch == L'a') {AddCh(); goto case_141;}
			else {goto case_0;}
		case 141:
			case_141:
			{t->kind = 52; break;}
		case 142:
			case_142:
			if (ch == L'r') {AddCh(); goto case_143;}
			else {goto case_0;}
		case 143:
			case_143:
			if (ch == L'i') {AddCh(); goto case_144;}
			else {goto case_0;}
		case 144:
			case_144:
			if (ch == L'b') {AddCh(); goto case_145;}
			else {goto case_0;}
		case 145:
			case_145:
			if (ch == L'a') {AddCh(); goto case_146;}
			else {goto case_0;}
		case 146:
			case_146:
			{t->kind = 53; break;}
		case 147:
			case_147:
			if (ch == L'a') {AddCh(); goto case_148;}
			else {goto case_0;}
		case 148:
			case_148:
			if (ch == L'j') {AddCh(); goto case_149;}
			else {goto case_0;}
		case 149:
			case_149:
			if (ch == L'o') {AddCh(); goto case_150;}
			else {goto case_0;}
		case 150:
			case_150:
			{t->kind = 54; break;}
		case 151:
			case_151:
			if (ch == L'e') {AddCh(); goto case_152;}
			else {goto case_0;}
		case 152:
			case_152:
			if (ch == L't') {AddCh(); goto case_153;}
			else {goto case_0;}
		case 153:
			case_153:
			if (ch == L'i') {AddCh(); goto case_154;}
			else {goto case_0;}
		case 154:
			case_154:
			if (ch == L'r') {AddCh(); goto case_155;}
			else {goto case_0;}
		case 155:
			case_155:
			{t->kind = 55; break;}
		case 156:
			case_156:
			if (ch == L'e') {AddCh(); goto case_157;}
			else {goto case_0;}
		case 157:
			case_157:
			if (ch == L's') {AddCh(); goto case_158;}
			else {goto case_0;}
		case 158:
			case_158:
			{t->kind = 56; break;}
		case 159:
			case_159:
			if (ch == L'e') {AddCh(); goto case_160;}
			else {goto case_0;}
		case 160:
			case_160:
			if (ch == L'n') {AddCh(); goto case_161;}
			else {goto case_0;}
		case 161:
			case_161:
			if (ch == L't') {AddCh(); goto case_162;}
			else {goto case_0;}
		case 162:
			case_162:
			if (ch == L'r') {AddCh(); goto case_163;}
			else {goto case_0;}
		case 163:
			case_163:
			if (ch == L'a') {AddCh(); goto case_164;}
			else {goto case_0;}
		case 164:
			case_164:
			if (ch == L's') {AddCh(); goto case_165;}
			else {goto case_0;}
		case 165:
			case_165:
			{t->kind = 57; break;}
		case 166:
			case_166:
			if (ch == L't') {AddCh(); goto case_167;}
			else {goto case_0;}
		case 167:
			case_167:
			if (ch == L'o') {AddCh(); goto case_168;}
			else {goto case_0;}
		case 168:
			case_168:
			{t->kind = 58; break;}
		case 169:
			case_169:
			if (ch == L'n') {AddCh(); goto case_170;}
			else {goto case_0;}
		case 170:
			case_170:
			if (ch == L'o') {AddCh(); goto case_171;}
			else {goto case_0;}
		case 171:
			case_171:
			{t->kind = 60; break;}
		case 172:
			case_172:
			if (ch == L'a') {AddCh(); goto case_173;}
			else {goto case_0;}
		case 173:
			case_173:
			if (ch == L'n') {AddCh(); goto case_174;}
			else {goto case_0;}
		case 174:
			case_174:
			if (ch == L'z') {AddCh(); goto case_175;}
			else {goto case_0;}
		case 175:
			case_175:
			if (ch == L'a') {AddCh(); goto case_176;}
			else {goto case_0;}
		case 176:
			case_176:
			{t->kind = 61; break;}
		case 177:
			case_177:
			if (ch == L'q') {AddCh(); goto case_178;}
			else {goto case_0;}
		case 178:
			case_178:
			if (ch == L'u') {AddCh(); goto case_179;}
			else {goto case_0;}
		case 179:
			case_179:
			if (ch == L'i') {AddCh(); goto case_180;}
			else {goto case_0;}
		case 180:
			case_180:
			if (ch == L'e') {AddCh(); goto case_181;}
			else {goto case_0;}
		case 181:
			case_181:
			if (ch == L'r') {AddCh(); goto case_182;}
			else {goto case_0;}
		case 182:
			case_182:
			if (ch == L'd') {AddCh(); goto case_183;}
			else {goto case_0;}
		case 183:
			case_183:
			if (ch == L'a') {AddCh(); goto case_184;}
			else {goto case_0;}
		case 184:
			case_184:
			{t->kind = 62; break;}
		case 185:
			case_185:
			if (ch == L'e') {AddCh(); goto case_186;}
			else {goto case_0;}
		case 186:
			case_186:
			if (ch == L'c') {AddCh(); goto case_187;}
			else {goto case_0;}
		case 187:
			case_187:
			if (ch == L'h') {AddCh(); goto case_188;}
			else {goto case_0;}
		case 188:
			case_188:
			if (ch == L'a') {AddCh(); goto case_189;}
			else {goto case_0;}
		case 189:
			case_189:
			{t->kind = 63; break;}
		case 190:
			case_190:
			if (ch == L's') {AddCh(); goto case_191;}
			else {goto case_0;}
		case 191:
			case_191:
			if (ch == L'o') {AddCh(); goto case_192;}
			else {goto case_0;}
		case 192:
			case_192:
			if (ch == L's') {AddCh(); goto case_193;}
			else {goto case_0;}
		case 193:
			case_193:
			{t->kind = 64; break;}
		case 194:
			case_194:
			if (ch == L'r') {AddCh(); goto case_195;}
			else {goto case_0;}
		case 195:
			case_195:
			if (ch == L'a') {AddCh(); goto case_196;}
			else {goto case_0;}
		case 196:
			case_196:
			{t->kind = 65; break;}
		case 197:
			case_197:
			if (ch == L'a') {AddCh(); goto case_198;}
			else {goto case_0;}
		case 198:
			case_198:
			if (ch == L'd') {AddCh(); goto case_199;}
			else {goto case_0;}
		case 199:
			case_199:
			if (ch == L'o') {AddCh(); goto case_200;}
			else {goto case_0;}
		case 200:
			case_200:
			if (ch == L's') {AddCh(); goto case_201;}
			else {goto case_0;}
		case 201:
			case_201:
			{t->kind = 66; break;}
		case 202:
			case_202:
			if (ch == L'p') {AddCh(); goto case_203;}
			else {goto case_0;}
		case 203:
			case_203:
			if (ch == L'e') {AddCh(); goto case_204;}
			else {goto case_0;}
		case 204:
			case_204:
			if (ch == L'r') {AddCh(); goto case_205;}
			else {goto case_0;}
		case 205:
			case_205:
			if (ch == L'a') {AddCh(); goto case_206;}
			else {goto case_0;}
		case 206:
			case_206:
			{t->kind = 67; break;}
		case 207:
			case_207:
			if (ch == L'g') {AddCh(); goto case_208;}
			else {goto case_0;}
		case 208:
			case_208:
			if (ch == L'u') {AddCh(); goto case_209;}
			else {goto case_0;}
		case 209:
			case_209:
			if (ch == L'n') {AddCh(); goto case_210;}
			else {goto case_0;}
		case 210:
			case_210:
			if (ch == L'd') {AddCh(); goto case_211;}
			else {goto case_0;}
		case 211:
			case_211:
			if (ch == L'o') {AddCh(); goto case_212;}
			else {goto case_0;}
		case 212:
			case_212:
			if (ch == L's') {AddCh(); goto case_213;}
			else {goto case_0;}
		case 213:
			case_213:
			{t->kind = 68; break;}
		case 214:
			case_214:
			if (ch == L'm') {AddCh(); goto case_215;}
			else {goto case_0;}
		case 215:
			case_215:
			if (ch == L'e') {AddCh(); goto case_216;}
			else {goto case_0;}
		case 216:
			case_216:
			{t->kind = 69; break;}
		case 217:
			if (ch == L'r') {AddCh(); goto case_3;}
			else if (ch == L'a') {AddCh(); goto case_132;}
			else {goto case_0;}
		case 218:
			if (ch == L'a') {AddCh(); goto case_5;}
			else if (ch == L'u') {AddCh(); goto case_51;}
			else if (ch == L'r') {AddCh(); goto case_74;}
			else {goto case_0;}
		case 219:
			if (ch == L'r') {AddCh(); goto case_14;}
			else if (ch == L'o') {AddCh(); goto case_36;}
			else if (ch == L'l') {AddCh(); goto case_124;}
			else if (ch == L'a') {AddCh(); goto case_190;}
			else {goto case_0;}
		case 220:
			if (ch == L'a') {AddCh(); goto case_31;}
			else if (ch == L'e') {AddCh(); goto case_234;}
			else if (ch == L'o') {AddCh(); goto case_235;}
			else {goto case_0;}
		case 221:
			if (ch == L'm') {AddCh(); goto case_45;}
			else if (ch == L'z') {AddCh(); goto case_177;}
			else {goto case_0;}
		case 222:
			if (ch == L'a') {AddCh(); goto case_54;}
			else if (ch == L'e') {AddCh(); goto case_236;}
			else {goto case_0;}
		case 223:
			recEnd = pos; recKind = 23;
			if (ch == L'=') {AddCh(); goto case_83;}
			else {t->kind = 23; break;}
		case 224:
			if (ch == L'e') {AddCh(); goto case_63;}
			else if (ch == L'o') {AddCh(); goto case_77;}
			else {goto case_0;}
		case 225:
			if (ch == L'v') {AddCh(); goto case_65;}
			else if (ch == L'n') {AddCh(); goto case_72;}
			else if (ch == L's') {AddCh(); goto case_202;}
			else {goto case_0;}
		case 226:
			if (ch == L'e') {AddCh(); goto case_237;}
			else {goto case_0;}
		case 227:
			recEnd = pos; recKind = 34;
			if (ch == L'>') {AddCh(); goto case_82;}
			else {t->kind = 34; break;}
		case 228:
			if (ch == L'e') {AddCh(); goto case_86;}
			else if (ch == L'a') {AddCh(); goto case_118;}
			else {goto case_0;}
		case 229:
			if (ch == L'z') {AddCh(); goto case_90;}
			else if (ch == L'm') {AddCh(); goto case_95;}
			else if (ch == L'r') {AddCh(); goto case_142;}
			else if (ch == L'b') {AddCh(); goto case_147;}
			else if (ch == L'l') {AddCh(); goto case_166;}
			else if (ch == L'v') {AddCh(); goto case_172;}
			else {goto case_0;}
		case 230:
			if (ch == L'e') {AddCh(); goto case_107;}
			else if (ch == L'o') {AddCh(); goto case_128;}
			else {goto case_0;}
		case 231:
			if (ch == L'o') {AddCh(); goto case_113;}
			else if (ch == L'i') {AddCh(); goto case_159;}
			else if (ch == L'e') {AddCh(); goto case_214;}
			else {goto case_0;}
		case 232:
			if (ch == L'i') {AddCh(); goto case_238;}
			else if (ch == L'e') {AddCh(); goto case_207;}
			else {goto case_0;}
		case 233:
			if (ch == L'i') {AddCh(); goto case_194;}
			else if (ch == L'r') {AddCh(); goto case_197;}
			else {goto case_0;}
		case 234:
			case_234:
			if (ch == L'g') {AddCh(); goto case_58;}
			else if (ch == L'p') {AddCh(); goto case_151;}
			else {goto case_0;}
		case 235:
			case_235:
			if (ch == L'j') {AddCh(); goto case_93;}
			else if (ch == L's') {AddCh(); goto case_105;}
			else {goto case_0;}
		case 236:
			case_236:
			if (ch == L'r') {AddCh(); goto case_102;}
			else if (ch == L'c') {AddCh(); goto case_156;}
			else {goto case_0;}
		case 237:
			case_237:
			if (ch == L'c') {AddCh(); goto case_76;}
			else if (ch == L'r') {AddCh(); goto case_185;}
			else {goto case_0;}
		case 238:
			case_238:
			recEnd = pos; recKind = 59;
			if (ch == L'_') {AddCh(); goto case_169;}
			else {t->kind = 59; break;}

	}
	AppendVal(t);
	return t;
}

void Scanner::SetScannerBehindT() {
	buffer->SetPos(t->pos);
	NextCh();
	line = t->line; col = t->col; charPos = t->charPos;
	for (int i = 0; i < tlen; i++) NextCh();
}

// get the next token (possibly a token already seen during peeking)
Token* Scanner::Scan() {
	if (tokens->next == NULL) {
		return pt = tokens = NextToken();
	} else {
		pt = tokens = tokens->next;
		return tokens;
	}
}

// peek for the next token, ignore pragmas
Token* Scanner::Peek() {
	do {
		if (pt->next == NULL) {
			pt->next = NextToken();
		}
		pt = pt->next;
	} while (pt->kind > maxT); // skip pragmas

	return pt;
}

// make sure that peeking starts at the current scan position
void Scanner::ResetPeek() {
	pt = tokens;
}

}
