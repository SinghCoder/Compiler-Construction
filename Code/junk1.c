int parseTable[NUM_OF_NONTERMINALS][NUM_OF_TERMINALS];
//taken long since number of non_terminal or terminal are below 64
/*
i = index of non terminals, j = index of terminals
*/
void createParseTable(long first[],long follow[]){
    for(int i=0; i<NUM_OF_RULES; i++) {
        int nonterminal = (int) grammar[i].sym;
        unsigned long long first_nt = first[nonterminal];
        for(int j=0; j<NUM_OF_TERMINALS; j++) {
            if(1<<j & first_nt) {
                parsetable[nonterminal][j] = i;
            }
        }
    }
	// for(int i=0;i<NUM_OF_NONTERMINALS;i++){
	// 	for(int j=0;j<NUM_OF_TERMINALS;j++)){
    //         if(1<<j & first[i]) parseTable[i][j] = i;
    //     }
    // }
}

/*
to find first of A-> BC
first(BC) = first(B)
if B is Nullable, 
    first(BC) = first(BC) U follow(B)
A->bC
first(bC) = b;
*/

long long getFirst()

terminal t = EPSILON;
if(1<<t & first_nt)