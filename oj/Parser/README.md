## 语法分析器

### 文法

```
<statement> ::=
     <variable> ':=' <expression>
    |'IF' <expression> 'THEN' <statement> 'ELSE' <statement>
    |'IF' <expression> 'THEN' <statement>
<expression> ::=
     <term>
    |<expression>'+'<term>
<term> ::=
     <factor>
    |<term>'*'<factor>
<factor> ::=
     <variable>
    |'('<expression>')'
<variable> ::=
     <identifier>
    |<identifier>'['<expression>']'
<identifier> ::=
     <alpha>
    |<identifier><alpha>
<alpha> ::=
     'A'|'B'|...|'Z'|'a'|'b'|...|'z'
<keyword> ::=
    'IF' | 'THEN' | 'ELSE'
```

在不改变文法定义的语言的情况下，可以对文法进行改写

### 输入

一个长ascii字节流，保证其中的每一个字符的ascii值`b`都满足$(b \in \{9,10,13\}) \lor (32 \le b \le 126)$

### 输出

如果输入是一个符合文法的`<statement>`，那么输出T，否则输出F