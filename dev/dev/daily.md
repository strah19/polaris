6/18/2023
- Redid the semantics. Made the code easier to read and covered more cases.
- Currently at 11:58 PM working on issue on returns. The semantic analyzer thinks that all functions are void type and causing a compiler error. It seems that when I am trying to save the expected return type in AST, it is somehow being lost, possibly when accessing it through the hash map. 
6/19/2023
- It is 12:02 AM, I solved the above issue by just having a variable keep track of the current function return type being parsed (it is only one at any given time).
- I would like to clean up the parser some day, probably not today (maybe at work...) but it could use some house keeping. It has been added to [[todo]].
- I also want to put some copyright comments in each file ([[todo]]).