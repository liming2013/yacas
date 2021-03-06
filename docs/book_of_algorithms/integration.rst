===========
Integration
===========

Integration can be performed by the function {Integrate}, which has
two calling conventions:

* Integrate(variable) expression
* Integrate(variable, from, to) expression

Integrate can have its own set of rules for specific integrals, which
might return a correct answer immediately. Alternatively, it calls the
function AntiDeriv, to see if the anti-derivative can be determined
for the integral requested. If this is the case, the anti-derivative
is used to compose the output.

If the integration algorithm cannot perform the integral, the
expression is returned unsimplified.

The integration algorithm
-------------------------

This section describes the steps taken in doing integration.

General structure
^^^^^^^^^^^^^^^^^

The integration starts at the function {Integrate}, but the task is
delegated to other functions, one after the other.  Each function can
deem the integral unsolvable, and thus return the integral
unevaluated. These different functions offer hooks for adding new
types of integrals to be handled.

Expression clean-up
^^^^^^^^^^^^^^^^^^^

Integration starts by first cleaning up the expression, by calling
{TrigSimpCombine} to simplify expressions containing multiplications
of trigonometric functions into additions of trigonometric functions
(for which the integration rules are trivial), and then passing the
result to {Simplify}.

Generalized integration rules
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For the function {AntiDeriv}, which is responsible for finding the
anti-derivative of a function, the code splits up expressions
according to the additive properties of integration, eg. integration
of $ a+b $ is the same as integrating $ a $ + integrating $ b $.

* Polynomials which can be expressed as univariate polynomials in the
  variable to be integrated over are handled by one integration rule.
* Expressions of the form $ p*f(x) $, where $ p $ represents a
  univariate polynomial, and $ f(x) $ an integrable function, are
  handled by a special integration rule. This transformation rule has
  to be designed carefully not to invoke infinite recursion.
* Rational functions, $ f(x)/g(x) $ with both $ f(x) $ and $ g(x) $
  univariate polynomials, is handled separately also, using partial
  fraction expansion to reduce rational function to a sum of simpler
  expressions.

Integration tables
^^^^^^^^^^^^^^^^^^

For elementary functions, Yacas uses integration tables. For instance,
the fact that the anti-derivative of $ Cos(x) $ is $ Sin(x) $ is
declared in an integration table.

For the purpose of setting up the integration table, a few declaration
functions have been defined, which use some generalized pattern
matchers to be more flexible in recognizing expressions that are
integrable.

Integrating simple functions of a variable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For functions like $ Sin(x) $ the anti-derivative can be declared with
the function {IntFunc}.

The calling sequence for {IntFunc} is::

    IntFunc(variable,pattern,antiderivative)

For instance, for the function $ Cos(x) $ there is a declaration::

    IntFunc(x,Cos(_x),Sin(x));

The fact that the second argument is a pattern means that each
occurrence of the variable to be matched should be referred to as
{_x}, as in the example above.

IntFunc generalizes the integration implicitly, in that it will set up
the system to actually recognize expressions of the form $ Cos(a*x+b)
$, and return $ Sin(a*x+b)/a $ automatically.  This means that the
variables $ a $ and $ b $ are reserved, and can not be used in the
pattern. Also, the variable used (in this case, {_x} is actually
matched to the expression passed in to the function, and the variable
{var} is the real variable being integrated over. To clarify: suppose
the user wants to integrate $ Cos(c*y+d) $ over $ y $, then the
following variables are set:

* {a} = $c$
* {b} = $d$
* {x} = $a*y+b$
* {var} = $x$

When functions are multiplied by constants, that situation is handled
by the integration rule that can deal with univariate polynomials
multiplied by functions, as a constant is a polynomial of degree zero.


Integrating functions containing expressions of the form $ a*x^2+b $
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are numerous expressions containing sub-expressions of the form
$ a*x^2+b $ which can easily be integrated.

The general form for declaring anti-derivatives for such expressions
is::

  IntPureSquare(variable, pattern, sign2, sign0, antiderivative)

Here {IntPureSquare} uses {MatchPureSquared} to match the expression.

The expression is searched for the pattern, where the variable can
match to a sub-expression of the form $ a*x^2+b $, and for which both
$ a $ and $ b $ are numbers and $ a*sign2>0 $ and $ b*sign0>0 $.

As an example::

    IntPureSquare(x,num_IsFreeOf(var)/(_x),
      1,1,(num/(a*Sqrt(b/a)))*
      ArcTan(var/Sqrt(b/a)));

declares that the anti-derivative of $ c/(a*x^2+b) $ is $$
(c/(a*Sqrt(b/a)))*ArcTan(x/Sqrt(b/a)) $$, if both $ a $ and $ b $ are
positive numbers.
