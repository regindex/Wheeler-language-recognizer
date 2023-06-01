from string import ascii_letters,digits,whitespace,printable
from typing import List
from .operators import *


def supercharge(rgx:str):
    """Support advanced operators like +, ?, \\\\s, \\\\w, \\\\d ..."""
    def l(s):
        q=[smb['(']]
        for i in s:
            q.append(ord(i))
            q.append(op['|'])
        q.pop()
        q.append(smb[')'])
        return q
    repl={
        '\d':l(digits),
        '\s':l(whitespace),
        '\w':l(ascii_letters+digits+'_'),
        '.': l(printable.replace('\n',''))
    }
    z=iter(rgx)
    a=[]
    for i in z:
        if i=='\\': i+=next(z)
        if i in repl: a.extend(repl[i])
        elif i in opsmb and i!='#': a.append(opsmb[i])
        else: a.append(ord(i[-1]))
    return opr_support(a)


def add_concat(rgx:List[int]):
    """
    adds concat (.) operator to the regex\n
    (a|b)abb -> (a|b).a.b.b
    """
    z=[rgx[0]]
    x,y=[op['*'],smb[')'],op['|']],[smb['('],op['|']]
    for i in range(1,len(rgx)):
        if not (rgx[i] in x or rgx[i-1] in y): z.append(op['.'])
        z.append(rgx[i])
    z+=[op['.'],smb['#']]
    return z


def opr_support(rgx:List[int]):
    """
    Support for optional(?) and kleene plus(+) operator\n
    -> converts ab?c to a(b|$)c ; ($ = epsilon)\n
    -> converts ab+c to abb*c
    """
    b=[]
    for i in rgx:
        if i!=op['?'] and i!=op['+']: b.append(i)
        else:
            c,j=0,[]
            while b:
                j.append(b.pop())
                if j[-1]==smb['(']: c+=1
                if j[-1]==smb[')']: c-=1
                if c==0: break
            if not j or c!=0: raise Exception('Invalid')
            j.reverse()
            b.append(smb['('])
            b.extend(j)
            if i==op['?']:
                b+=[op['|'],smb['$']]
            else: # +
                b.extend(j); b.append(op['*'])
            b.append(smb[')'])
    return b


def infix_to_postfix(rgx:List[int])->List[int]: # shunting yard
    # p={'*':3,'|':2,'.':1} # precedence
    opr,out=[],[]; z={op['*'],op['.'],op['|']}
    for c in rgx:
        if c in z:
            while opr and opr[-1]!=smb['('] and opr[-1]>=c:
                out.append(opr.pop())
            opr.append(c)
        elif c==smb['(']:
            opr.append(c)
        elif c==smb[')']:
            while opr and opr[-1]!=smb['(']:
                out.append(opr.pop())
            if not opr or opr.pop()!=smb['(']: raise Exception('Invalid')
        else:
            out.append(c)
    while opr:
        if opr[-1]==smb['(']: raise Exception('Invalid')
        out.append(opr.pop())
    return out

r_opsmb={v:k for k,v in opsmb.items()}
regex_toStr=lambda rgx:repr(''.join([chr(i) if i<1e5 else r_opsmb[i] for i in rgx]))
