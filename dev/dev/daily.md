#### 6/18/2023
- Redid the semantics. Made the code easier to read and covered more cases.
- Currently at 11:58 PM working on issue on returns. The semantic analyzer thinks that all functions are void type and causing a compiler error. It seems that when I am trying to save the expected return type in AST, it is somehow being lost, possibly when accessing it through the hash map. 
#### 6/19/2023
- It is 12:02 AM, I solved the above issue by just having a variable keep track of the current function return type being parsed (it is only one at any given time).
- I would like to clean up the parser some day, probably not today (maybe at work...) but it could use some house keeping. It has been added to [[todo]].
- I also want to put some copyright comments in each file ([[todo]]).
- I want to start to test the speed of the compiler and virtual machine, I need to figure out how to set up a debug and release mode from cmake [[todo]].
- Added copyright and license notice in all files.
- I think I should start working on lists. I'm not sure if I want to represent them as pointers in polaris but that's how it will be internally.
	- Possible syntax of lists: a : int [5] = {1, 2, 3, 4, 5};
- Currently redoing Scopes and using C style symbol table. MAKE A NOTE: At line 401, there is code that needs to be uncommented and fixed. 