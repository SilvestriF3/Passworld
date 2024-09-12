# Glossary
"Baseword": a word given as input by user, base point of the next permutations. 

"Candidate": a password that has completed generation process but has not been validated yet.

"Generator": the core engine of the tool.

"Paradigm": a defined form the baseword will follow.

"Permutator": a rule or process a candidate is subjected during its generation.

# How it works
The tool's work is divided in 3 stages/steps. 

In the first stage (PreGeneration) all the input words of the dedicated text box and/or of the supplied wordlist file are permutated according to the defined permutators for basewords. 

In the second stage (Generation) the permutated words of the first stage are used with input numbers and symbols and paradigms to generate a candidate password, on this quite-candidate all the other permutations/substitutions/additions are done.

In the third stage (PostGeneration) each candidate is validated against user's defined rules and appended to a thread stream and eventually to the output file. 
The user can decide what permutators to add and define some configs and rules to filter unwanted results.

The tool is capable of generating tens of thousands of passwords per second with multi-threading, check the performance tab under settings for more information.

# General config
Each input field is parsed as a comma-separated list, whitespaces will be removed.
For basewords an additional input file can be supplied (one word per row).
### Paradigms
A paradigm is parsed as an array of characters, each character represent an element (a number, a word, a symbol) that will be present in the password according to the order.

'n' = numbers from number field

's' = symbols from symbol field

'c' = words from baseword field/file

The length of the paradigm should not be excessive to not genrate too many passwords (3 or 4 is preferrred).

Most common paradigms such as word + number + symbol are suggested.

### Requirement config
Requirements should be tuned based on the password policy (e.g. minimum length, require capital, allowed symbol...)

It is also possible to set a maximum length to discard unlikely/strong password.
# Permutators
### Paradigm permutator
It is a default permutator, not removable.
## Baseword permutators
- ### Capital permutator
  A permutator to capitalize the baseword in different ways (all, first char, last char).
- ### Baseword Cutter
  A permutator to shorten the baseword in various lengths.
- ### Word combiner
  A permutator to combine basewords and additional words.
## Candidate permutators
- ### Leet permutator
  A permutator to replace some characters with similar ones in 1337 style.
- ### Rules
  - Add a list of prefixes to the candidate
  - Add a list suffixes to the candidate
  - Replace a char at an index with an input char

**TIP**: These rules enforce and should not be confused with requirement checks.
