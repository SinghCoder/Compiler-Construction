#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
bool isAlphabet(char c){
	if((c>='a' && c<='z')||(c>='A' && c<='Z')) return true;
	return false;
}
bool isDigit(char c){
	if(c>='0' && c<='9') return true;
	return false;
}

int main(int argc,char* argv[]){
	FILE* fp;
	fp = fopen(argv[1],"r");
	if(fp==NULL){
		printf("FILE OPEN ERROR");
		exit(1);
	}
	char c= getc(fp);
	int state=0;
	int id_count=0;
	while(c!=EOF){
	//for identifer and num
	switch(state){
		case 0:if(isAlphabet(c)) {
			       state=1;
			       id_count++;
				}
				else if(isDigit(c)) state=3;
				c = getc(fp);
				break;
		case 1:if((isAlphabet(c) || isDigit(c) ||'_')&&id_count<20) {
			       state=1;
			       c=getc(fp);
			       id_count++;
				}
				else{
					state=2;
				}
				break;
		case 2: ungetc(c,fp);
				printf("TK_ID");
				state=0;
				break;
		case 3: if(isDigit(c)){
					state=3;
					c = getc(fp);
				}
				else if(c=='.'){
					state = 5;
					c = getc(fp);
				}
				else{
					state = 4;
				}
				break;
		case 4: ungetc(c,fp);
				printf("TK_NUM");
				state=0;
				break;
		case 5: if(isDigit(c)){
					state=6;
					c = getc(fp);
				}
				break;
		case 6: if(isDigit(c)){
					state=6;
					c = getc(fp);
				}
				else if(c=='E' || c=='e'){
					state=9;
					c= getc(fp);
				}
				else state=7;
				break;
		case 7: ungetc(c,fp);
				printf("TK_RNUM");
				state=0;
				break;
		case 8:
		case 9: if(c == '+' || c == '-'){
					state=10;
					c = getc(fp);
				}else if(isDigit(c)){
					state=11;
					c = getc(fp);
				}
				break;
		case 10: if(isDigit(c)){
					state=11;
					c = getc(fp);
				}
				break;
		case 11: if(isDigit(c)){
					state=11;
					c = getc(fp);
				}else state=12;
				break;
		case 12: ungetc(c,fp);
				 printf("TK_RNUM");
				 state=0;
				 break;
		}
	}
}