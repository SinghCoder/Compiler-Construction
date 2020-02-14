forward : = forward + 1 ;
if forward ! = eof then begin
    if forward at end of first block then begin
        reload second block ;
        forward : = forward + 1
    end

    else if forward at end of second block then begin
        reload first block ;
        move forward to beginning of first block
    end
    else / * eof within buffer signifying end of input * /
        terminate lexical analysis
end

int lexeme_begin;
int forward_ptr;
int curr_bfr_no; // Takes only 2 values -> 0 or 1
char *curr_bfr = buffer1;

char get_char()
{
    forward_ptr = forward_ptr + 1;
    if(forward)
}






