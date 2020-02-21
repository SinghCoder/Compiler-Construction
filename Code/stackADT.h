typedef struct {
    int MAX_SIZE = 50;
    symbol[MAX_SIZE] stack;
    int top = 0;
} stack_adt

stack_adt nt_stack;

void push(symbol s);

symbol top();

void pop();void push(symbol s) {
    if(top == MAX_SIZE) {
        perror("Stack full, increase MAX_SIZE\n");
        return;
    } 
    else {
        nt_stack.stack[nt_stack.top] = s;
        top++;
    }
}

void pop() {
    if(nt_stack.top == 0) {
        perror("Tried to pop empty stack\n");
        return;
    }
    else {
        nt_stack.top--;
    }
}

symbol top() {
    if(nt_stack.top == 0) {
        perror("Tried to find top of empty stack\n");
        return;
    }
    else {
        return nt_stack.stack[nt_stack.top];
    }
}