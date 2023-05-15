# Polaris Grammar

The grammar of Polaris is still being formed and editing but this is the basic outline of it. This one will be using BNF.

<translation-unit> ::= {<declaration>}*

<declaration> ::= <variable-declaration> | <function-declaration>

<function-declaration> ::= <identifier> : ()
