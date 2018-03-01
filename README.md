# Esolang-WIP
A reverse polish notation, stack-based esolang I'm writing for fun.

RPN is the binary

RPN.cpp contains all the interesting stuff.

RPN_util.cpp and RPN_util.h contain basic utility functions.

RPN_example is an example program, which prompts the user to input a number, and listing all primes below the input.

Literals:

  String literals start with double quotes and end with either double quotes or a new line, and may not span more than one line. Characters can be escaped, but note that \n, \t, etc. will just result in n, t, or so on.
  
  Number literals are strings of digits. Only integers are currently supported, so they may not contain decimals.

Operations:

  2 to 1 Operations:
  
    +, -, *, /, %, =, >, <, <=, >=, !=, ^, max, min : pop a and b off the stack, push a operator b
    
  1 to 1 Operations:
  
    | (abs), ~ (negate) : perform the relevant operation on the top of the stack
    
  Control Flow:
  
    jump : Pop a and b off the stack, and jump to word a in line b
    
    if   : Pop a off the stack. If a is zero, jump to the corresponding else or end (nested if's are allowed)

    else : Jump to the corresponding end
    
    end  : Does nothing other than contributing to program structure.
  
  Others:
    
    :    : Duplicate the top of the stack
    
    !    : Delete the top of the stack
    
    sum  : Pop a off the stack, and sum the first a elements of the stack
    
    prod : Pop a off the stack, and multiply the first a elements of the stack
    
    size : Push the size of the stack to the stack
    
    prn  : Pop the top of the stack, and print it as a number
    
    prc  : Pop the top of the stack, and print it as a character
    
    prw  : Print the top of the stack as a char and pop while the top of the stack is printable ascii (31 < x < 127)
    
    in   : Get a number from cin and push it to the stack (no prompt is given to the user)
    
    rev  : Reverse the stack
    
    get  : Pop a off the stack. Move the ath element of the stack to the top
    
    $    : Swap the top two elements of the stack
    
Assignments:
  
    def  : Macro defintion. Must be on its own line, and of the form
            
            def macro_name one or more operations
         
         macro_name will be replaced with the operations given everywhere in the code. Note that this will affect jumps.
  
    let  : Variable assignment. May be anywhere, but must be of the form
            
            let variable_name zero or more operations end
         
         The zero or more operations are performed on the stack, then variable_name is assigned the value of the top of the stack, popping it.
  
    func : Function definition. Must be of the form
         
             func arg_count func_name operations end
         
         When the function is called, arg_count values are moved from the main stack to the function stack, on which the function body is executed.
         
         Once execution has completed, the function stack is merged onto the main stack.
         
         Note that jumps inside a function are relative to the function body, with the line containing func_name being line 0.
         
         Furthermore, you cannot jump out of a function call back into the main code, but may jump into function code from the main file.
