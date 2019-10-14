## OPG

### 描述

给出`OPG`文法，编写`OPG`分析器，判定给定的句子是否符合文法

保证文法没有有害规则，没有多余规则，一定是`OPG`文法

保证文法的非终结符不出现`#`

### 输入

第一行是三个整数`o`、`p`和`g`，代表这个`OPG`文法有`o`个非终结符，`p`个终结符和`g`条文法推导式

第二行是`o`个字符用空格隔开，保证只有字母

第三行是`p`个字符用空格隔开，保证只有字母和* / - + ( ) = < >

第四行只有一个字符，表示文法的开始字符

接下来的`g`行每行一个文法推导式，格式为`^[a-zA-Z]->[a-zA-Z\*\/\-\+\(\)=<>]+$`

接下来是一个整数`T`，代表接下来有`T (0<T<10000)`个句子

接下来的`T`行，每行一个句子，保证由终结符组成

### 输出

如果可以推导输出`T`

如果不能推导输出`F`