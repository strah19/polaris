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
- Currently redoing Scopes and using C style symbol table. MAKE A NOTE: At line 440, there is code that needs to be uncommented and fixed. 
- I was able to get the other symbol table to work, I just need to make sure default values are working
#### 6/20/2023
- Fixed issue with default values. I got rid of the Vector and am using a standard array, I should do this with all the values in the AST.h and remove anything using Vectors.
- I realized I was not doing any semantic checks on arguments so right now I am trying to find a solution to getting access to function data from a function call.
- A semantic bug has been found, it crashes when it tries to convert an identifier (I think).
- I think I need to create a CAST opcode. So when the semantic analyzer sees an id that needs to be casted, it adds that info to its AST and then the code generator sees that and adds a cast opcode. 
#### 6/21/2023
- The issue has been resolved, I created a CAST opcode that will change the type of a variable once it is on the stack. Currently adding the semantic checks for arguments.
#### 6/22/2023
- Got rid of more Vectors in the Ast.h
#### 6/23/2023
- Currently I am making sure that String operations are functional, I am adding the ability for the VM to add strings and compare them.
- For some reason, the else in test.pol is not working. Must investigate. I have investigated, seems the issue only occurs when the else is in the main scope....
- The issue with the else is fixed, there was a nullptr that was not being checked causing a crash. Also the string operations are now working well.
- I created a scope.h and a scope.cpp file to put all the code in there.
#### 6/24/2023
- Cleaning up the parser. Cleaned up some return type stuff.
- If there is conditional code in a function and return statement is not called and a value is expected to be returned, the code generator will return a 0 no matter what the type is and there will be a parser warning.
- Added the parser warning for returns in control flow.
#### 7/14/2023
- Code generator supports different equal operands and default values.
- Added hex support but there is a bug for hex and binary during clean up. There is an invalid pointer somewhere...Will be added to [[todo]] list.
#### 7/15/2023
- Fixed a debug issue with when to print stack.
- The bug seems to actually have no connection with bin or hex...It seems it has to do with a memory leak.
- Issue #2 has been resolved. The issue was that the virtual machine initializes 512 slots for random access memory. However, the incorrect capacity was being initialized meaning the incorrect number of Values was being set and reallocations were occurring when they should not have been. 
- Hex and bin are supported!