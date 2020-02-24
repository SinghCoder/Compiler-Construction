** This testcase demonstrates correctness of module structure with declaration and simple statements**
<<<driver program>>>
start
	declare v_1_2_3:integer;
	get_value(v_1_2_3);
	declare result_:integer;
	[result_]:=use module square with parameters v_1_2_3;
	print(result);
end

<<module square>>
takes input [x:integer];
returns [value:integer];
start
	value:= x*x;
end

**notice that this test case does not declare the module before the driver module, still it is syntactically correct as moduleDeclarations is optional. Testing whether a module is declared before its use is a matter of semantic analysis**

** test the correctness of all lexemes and parse tree created
and print the following message on successful compilation on the console
      " Input source code is syntactically correct.........."
**

**make sure to produce only relevant output of your compiler code...
Also remove all unnecessary printf statements and make the code more professional before submitting**
