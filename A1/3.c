#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct _params {	// 토큰 구조체
	char cur_char;			// 현재 인덱스의 문자
	char *m_input;			// 공백 제거 된 수식
	int cur_idx;			// 현재 인덱스
	int expect_parens;		// 괄호 카운트
} _params;

enum token{		// 토큰 종류
	NUMBER,		// 숫자
	LP,			// (
	RP,			// )
	PLUS,		// +
	MINUS,		// -
	MULTIPLY,	// *
	DIVIDE,		// /
	POW,		// ^
	UNKNOWN		// ?
};

enum token _symbol(char c) {			// 토큰 구분 함수
	switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':	
		case '7':
		case '8':
		case '9':	return NUMBER;
		case '(': 	return LP;
		case ')': 	return RP;
		case '+': 	return PLUS;
		case '-':	return MINUS;
		case '*': 	return MULTIPLY;
		case '/':	return DIVIDE;
		case '^':	return POW;
		default:	return UNKNOWN;
	}
};

char input[1024] = {0};		// 입력 된 수식
char output[1024] = {0};	// 공백 제거 된 수식

double _expr(_params *ptr);	
double _term(_params *ptr);
double _factor(_params *ptr);
enum token _peek(_params *ptr);
void get_next_token(_params *ptr);
double _exponent(_params *ptr);
double _number(_params *ptr);
void _error(char *str);
void remove_space(void);

int main(void) {
	printf("Input expression: ");					// 수식 입력
	scanf("%s", input);
	remove_space();									// 공백 제거
	_params params = { output[0], output, 0, 0 };	// 파라미터 생성
	printf("%.f\n", _expr(&params));				// 출력
}

double _expr(_params *ptr) {				// 재귀 하향 파서
	double term = _term(ptr);				// 우선순위(*, /, ^) 연산
	while(1){							
		if(_peek(ptr) == PLUS) {			// + 경우
			get_next_token(ptr);			// 토큰 이동
			term = term + _term(ptr);		// 결과 갱신
		} else if(_peek(ptr) == MINUS) {	// - 경우
			get_next_token(ptr);			// 토큰 이동
			term = term - _term(ptr);		// 결과 갱신
		} else return term;					// 결과 반환
	} 
}

double _term(_params *ptr) {						
	double exponent = _exponent(ptr);				// 우선순위(^) 연산
	while(1) {
		if(_peek(ptr) == MULTIPLY) {				// * 경우
			get_next_token(ptr);					// 토큰 이동
			exponent = exponent * _exponent(ptr);	// 결과 갱신
		} else if(_peek(ptr) == DIVIDE) {			// / 경우
			get_next_token(ptr);					// 토큰 이동
			exponent = exponent / _exponent(ptr);	// 결과 갱신
		} else return exponent;						// 결과 반환
	}
}

double _factor(_params *ptr) {								
	int sign = 1;											// 부호 초기화
	double expression = 0;									// 결과
	if (_peek(ptr) == MINUS) {								// - 경우
		get_next_token(ptr);								// 토큰 이동
		sign = -1;											// 부호 음수 전환
	} else sign = 1;										// 부호 양수 유지

	if (_peek(ptr) == NUMBER) {								// 숫자 경우
		return sign * _number(ptr);							// 부호에 따른 결과 반환
	} else if (_peek(ptr) == LP) {							// ( 경우
		ptr->expect_parens = 1;								// 괄호 카운트 증가
		get_next_token(ptr);								// 토큰 이동
		expression = _expr(ptr);							// 내부 수식 연산
		if (_peek(ptr) != RP) {								// ) 가 없는 경우
			_error("Syntax: Expected closed parenthesis");	
		}
		get_next_token(ptr);								// 토큰 이동
		ptr->expect_parens = 0;								// 괄호 카운트 초기화
		return sign * expression;							// 결과 반환
	} else {												// 이 외의 경우
		_error("Syntax");					
		return 0;
	}
}

enum token _peek(_params *ptr) {					// 현재 인덱스의 토큰 문자 형식 반환
	return _symbol(ptr->m_input[ptr->cur_idx]);
}

void get_next_token(_params *ptr) {				// 다음 문자 이동
	ptr->cur_idx = ptr->cur_idx + 1;			// 인덱스 증가
	ptr->cur_char = ptr->m_input[ptr->cur_idx]; // 문자 갱신
}

double _exponent(_params *ptr) {				// 지수 연산
	double factor = _factor(ptr);				
	while (1) {
		if (_peek(ptr) == POW) {				// ^ 경우
			get_next_token(ptr);				// 토큰 이동
			factor = pow(factor, _factor(ptr)); // 결과 갱신
		} else {
			return factor;						// 결과 반환
		}
	}
}

double _number(_params *ptr) {							// 숫자 판별
	double result = 0;									// 최종 숫자 값
	double fractional = 0;								// 분수 확인
	int length = 0;
	while(_peek(ptr) == NUMBER) {						// 숫자 문자 경우
		result = result * 10 + (ptr->cur_char - '0');	// 숫자 갱신
		get_next_token(ptr);							// 토큰 이동
	}
	if (fractional > 0) 								// 분수일 경우
		result = result + (fractional/pow(10, length)); // 숫자 갱신
	if (_peek(ptr) == LP)								// ( 경우
		_error("Syntax: Unexpected open parenthesis");
	else if (_peek(ptr) == RP && !ptr->expect_parens)	// ) 이고 괄호 카운트가 0이 아닌 경우
		_error("Syntax: Unexpected closed parenthesis");
	return result;										// 결과 반환
}

void _error(char *str) {					// 오류 구문 출력
	fprintf(stderr, "ERROR:  %s\n", str);
	exit(1);
}

void remove_space(void) {								// 공백 제거
	int i, j;
	for (i = 0, j = 0; i < strlen(input); i++, j++) {
		if (input[i]!=' ') {
			output[j]=input[i];
		} else {
			j--;
		}			
	}
	output[j]=0;										// 끝에 NULL값 추가
}
