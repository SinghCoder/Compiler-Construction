## Names

- Function performs an action, so name should make it clear what it does.
    - ```check_for_errors() instead of error_check(), dump_data_to_file() instead of data_file()```

    - Suffixes are sometimes useful
        - max - to mean the maximum value something can have.
        - cnt - the current count of a running count variable.
        - key - key value.
            - ```retry_max to mean the maximum number of retries, retry_cnt to mean the current retry count```

    - Prefixes are sometimes useful:
        - is - to ask a question about something. Whenever someone sees Is they will know it's a question.
        - get - get a value.
        - set - set a value
            - ```e.g. is_hit_retry_limit ```

- Include units in names (for variables representing time/weight/etc)
    - ``` 
            uint32 timeout_msecs;
            uint32 my_weight_lbs;
      ```

- Structure names
    - Underbars ('_') to separate name components
    - Declare at the top of the file in which they are used, or in separate header files, if they are used in multiple source files.
    - Use of the structures should be by separate declarations and should be "extern" if they are declared in a header file
    - Can use meaningful prefix for each member name. E.g, for ```"struct softc" the prefix could be "sc_"```.

- Pointer variables
    - place the * close to the variable name not pointer type
    - ```
              char *name= NULL;
              char *name, address;
      ```

- Global variables
    - Should be prepended with a ```g_```   (Important to know the scope of variable)
    - should be avoided whenever possible
    - ``` 
          Logger  g_log;
          Logger* g_plog;
      ```

- Global constants
    - should be all caps with '_' separators.
    - ``` 
          const int A_GLOBAL_CONSTANT= 5;
      ```

- #define and Macro Names
    - in all upper using '_' separators
    - Macros are capitalized, parenthesized, and should avoid side-effects
    - If the macro is more than a single statement, use ```do { ... } while (0)```, so that a trailing semicolon works
    - ``` 
            #define MAX(a,b) blah
            #define IS_ERR(err) blah
            #define	MACRO(v, w, x, y)					\
            do {									    \
                v = (x) + (y);							\
                w = (y) + 2;							\
            } while (0)
      ```

- Enum Names
    - Labels All Upper Case with '_' Word Separators
    - ```
            enum PinStateType {
                PIN_OFF,
                PIN_ON
            };
      ```
    - Make a Label for an Error State
        - To say an enum is not in any of its valid states, make a label for an uninitialized or error state
        - ``` enum { STATE_ERR,  STATE_OPEN, STATE_RUNNING, STATE_DYING}; ```


## Formatting

- Brace placement
    - All if, while and do statements require braces even if there is only a single statement within the braces.
        - Will help to add more lines of code easily 
        - ```
            if (1 == somevalue) {
                somevalue = 2;
            }
          ```

- Add Comments to Closing Braces
    - will help when you are reading code because you don't have to find the begin brace to know what is going on.
    - ```
        while(1) {
            if (valid) {
            
            } /* if valid */
            else {
            } /* not valid */

        } /* end forever */
      ```

- Parens () with Key Words and Functions Policy
    - Do not put parens next to keywords. Put a space between.
    - Do put parens next to function names.
    - Do not use parens in return statements when it's not necessary.
    - ```
        if (condition) {
        }

        while (condition) {
        }

        strcpy(s, s1);

        return 1;
      ```

- If Then Else Formatting
    - Layout
        - Upto you how you use bracing styles. A common approach -:
            - ```
                if (condition) 
                {
                } 
                else if (condition) 
                {
                } 
                else 
                {
                }
              ```
            - If you have else if statements then it is usually a good idea to always have an else block for finding unhandled cases. Maybe put a log message in the else even if there is no corrective action taken.
            - Condition Format
            - Put the constant on the left hand side of an equality/inequality comparison
            - If you miss one = sign, compiler will throw error.
            - ``` if ( 6 == errorNum ) ... ```

- Switch formatting

    - Falling through a case statement into the next case statement shall be permitted as long as a comment is included.
    - The default case should always be present and trigger an error if it should not be reached, yet is reached.
    - If you need to create variables put all the code in a block.
    - ```
            switch (...)
            {
                case 1:
                    ...
                /* comments */

                case 2:
                {        
                    int v;
                    ...
                }
                break;

                default:
            }
      ```

- Use of ?:

    - Put the condition in parens so as to set it off from other code
    - If possible, the actions for the test should be simple functions.
    - Put the action for the then and else statement on a separate line unless it can be clearly put on one line.
    - ```
        (condition) ? funct1() : func2();

        or

        (condition)
            ? long statement
            : another long statement;
      ```

- One Variable Per Line
    - ```
            Not:
            char **a, *x;

            Do:
            char **a = 0;  /* add doc */
            char  *x = 0;  /* add doc */
      ```

- Use Header File Guards
    - Use macros to guard the include files
    - do not use "_" as first character for compatibility reasons
    - ```
            #ifndef sys_socket_h
            #define sys_socket_h  /* NOT _sys_socket_h_ */
            #endif 
      ```

## Macros

- Replace Macros with Inline Functions
    - ```#define  MAX(x,y)	(((x) > (y) ? (x) : (y))	// Get the maximum```
            - The macro above can be replaced for integers with the following inline function with no loss of efficiency:
            - ```
                    inline int 
                    max(int x, int y) {
                        return (x > y ? x : y);
                    }
      ```
- Always Wrap the Expression in Parenthesis
    - ```#define ADD(x,y) x + y``` must be written as  ```#define ADD(x,y) ((x) + (y))```

- Make Macro Names Unique
    - Prepend with package name

## Initialize all Variables
    gcc with the flag -W may catch operations on uninitialized variables, but it may also not.


## Do Not Default If Test to Non-Zero
- ```if (FAIL != f())``` is better than ```if (f())```
- An explicit test will help you out later when somebody decides that a failure return should be -1 instead of 0

## Commenting Out Large Code Blocks
- Sometimes large blocks of code need to be commented out for testing
    - **Using #if 0**
            - The easiest way to do this is with an #if 0 block:
            - ```    
                    void example()
                    {
                        great looking code

                        #if 0
                        lots of code
                        #endif

                        more code
                    }
              ```
            - can't use /* */ style comments because comments can't contain comments

## No Data Definitions in Header Files

- /*

   \* aheader.h

   */

  ```int x = 0;```

- Consider defining the variable once in a .c file and use an extern statement to reference it.

## Error Return Check Policy

- Check every call to malloc or realloc. You might want to have your own wrapper for these calls, so you can do the right thing always and don't have to make memory checks everywhere.
- Lint is a C program checker that examines C source files to detect and report type incompatibilities, inconsistencies between function definitions and calls, potential program bugs, etc.    
    - The use of lint on all programs is strongly recommended, and it is expected that most projects will require programs to use lint as part of the official acceptance procedure.
