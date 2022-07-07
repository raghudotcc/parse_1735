def grammar(description):
    """Convert a description to a grammar. """
    G = {}
    for line in split(description, '\n'):
        lhs, rhs = split(line, ' => ')
        alternatives = split(rhs, ' | ')
        G[lhs] = tuple(map(split, alternatives))
    return G

def split(text, sep=None, maxsplit=-1):
    "Like str.split applied to text, but strips whitespace from each piece."
    return [t.strip() for t in text.split(sep, maxsplit) if t]


G = grammar("""
Exp   => Term [+-] Exp | Term
Term  => Factor [*/] Term | Factor
Factor => Funcall | Var | Num | [(] Exp [)]
Funcall => Funname [(] Exps [)]
Exps => Exp [,] Exps | Exp
Var => [a-zA-Z_]\w*
Num => [+-]?[0-9]+([.][0-9]*)?
""")


print(G)