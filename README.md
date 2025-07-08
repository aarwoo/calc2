**Help Info that print before REPL**
Welcome to use calc 2.0, a calc that you could define a function
() {} [] are equals
binary operator are left associative operator and have the following precedence(higher up):
        ^(pow)
        * /
        + -
        < <= > >= = /=(no equals)
        \/(or) /\(and)
        =>(see below)
        |(see below)
usage: 1 + 2 (note: binary operator should wrap by space)
L => R when L is bool true return R's value, or L is bool false return void(a special value)
L | R when L is void, return R's value or else return L's value
single operator are right associative operator:
         ^(not) + -
usage: -1 (note: single operator should following expr immediately, without space)
number will be parsed by strtold(C function) but you can not use A-Z in number, like 0xAZ is not allowed, use 0xaz instead
special number:
        I(imaginary unit)
        NAN(not a number,special long double value)
        INF(infinity,special long double value)
        PI
        E
define function:
        foo(x,y)=expr
        foo is function reference name which consist by a-z
        x and y are params(note: 0-26 params are supported, but the params should by single alpha a-z)
        call foo:
                foo(0,1)
        you can pass foo into param x, so that:
                x(2,3)
        is equal to
                foo(2,3)
        foo(x,y)=expr
                ^this = has not space around
        function define is a statement,so you can't define in expr
        you can redefine any function in any time,include init function.
        however,redefine is visible for all function:
                #1 foo(x)=x
                #2 goo(x)=foo(x)
                #3 foo(x)=x + 1
                call #2 goo will call #3 foo, not #1 foo
function reference name and params are variable
        for variable v, v is function reference name iff v is not param

[Define] sin(a)=? as init function
[Define] cos(a)=? as init function
[Define] tan(a)=? as init function
[Define] asin(a)=? as init function
[Define] acos(a)=? as init function
[Define] atan(a)=? as init function
[Define] sinh(a)=? as init function
[Define] cosh(a)=? as init function
[Define] tanh(a)=? as init function
[Define] asinh(a)=? as init function
[Define] acosh(a)=? as init function
[Define] atanh(a)=? as init function
[Define] exp(a)=? as init function
[Define] log(a)=? as init function
[Define] sqrt(a)=? as init function
[Define] real(a)=? as init function
[Define] imag(a)=? as init function
[Define] abs(a)=? as init function
[Define] arg(a)=? as init function
[Define] conj(a)=? as init function
[Define] proj(a)=? as init function
[Define] floor(a)=? as init function
[Define] ceil(a)=? as init function
[Define] round(a)=? as init function
