# MallocBugTracker
CS 214: Systems Programming  
Professor Menendez  
Long Tran (netID: lht21)  
Julian Herman (netID: jbh113)  



## Makefile Intructions
- 'make' (default) will run debug mode
- 'make run' will run on official mode

## Our Shell Extensions:
### Multiple pipes
  - Our shell allows user to piping commands as many as user want

### HOME directory
  - Allow user to traverse to HOME directory using `~`

## Testing Plan:
*** Notice ***
  - Testcases are created based on iLab machine's file system. 
  - Please use iLab machines to test these cases for accurate result

### Unit testing - Single commands:
  - Built-in Programs:
    - pwd: 
      - Single command: output into current STDOUT_FILENO (redirection will set STDOUT_FILENO before executing)
      - Pipeline command: output into pipeline
    - cd: Do not have input/output

  - Exectuting Programs:
    - Assure to be able to run programs in these directories:
      1. /usr/local/sbin 
        - `/usr/local/sbin/showstats.sh`
      2. /usr/local/bin
        - `/usr/local/bin/eclipse`
      3. /usr/sbin
        - `/usr/sbin/zic`
      4. /usr/bin
        - `/usr/bin/ls`
      5. /sbin
        - `/sbin/zic` 
      6. /bin
        - `/bin/pwd` 

  - Program with arguments: (argument test for built-in cd)
    - Being able to recognize the argument list for the progam 
        ### One Arguments
        1. `cd ..`
        2. `echo hello` 
        
        ### Two Arguments
        3. `cd .. hello world` -> ERROR
        4. `echo hello world`
        5. `cat filename1 filename2`

        ### Infinite Arguments
        6. `echo hello world this is a very very long sentence and it aims to have infinite length`

    - Being able to identify argument list for each command in pipeline.
        7. `echo shell should recognize this sentence as argument list | wc`
        8. `ls -l | grep "file"` -> expected SUCCESS: -l is argument of ls, "file" is argument of grep

  - Redirections: 
    - Redirection token is recognized without space
      1.  `echo hello world>echo.txt`
      2.  `wc<filename1`
      3.  `wc<filename1>wc.txt`
        -> Expected wc counts words in filename1 and write to wc.txt
    - Redirection token and its input/output token can be placed anywhere in command line.
      1. `echo hello this is a > echo.txt sentence from echo.`
      2. `echo > echo.txt this is a sentence.`
      3. `echo this is a sentence > echo.txt`

    - Errors Detected:
      * no token after redirection
      1. `echo >` 
      2. `echo <`
      3. `echo <<`
      4. `echo >>`
      5. `echo > |`
      6. `echo < |`

      * Multiple redirections
      1. `echo >echo1.txt >echo2.txt` ----- HEY JULIAN, PLEASE CONFIRM THIS
      2. `wc <filename1 <filename2` --- Ambiguous Inputs
      3. `wc <filename1 >wc.txt <filename2`
      4. `wc <filename1 >wc.txt >wc2.txt`


    - Pipes (Multiple pipes):
      1. Pipes still work even though second command doesn't use input from first command
         - `echo hello world | ls`
      2. Pipes still work even though first command write nothing, and second command use input from pipe
         - `cd .. | wc`  

      3. Two Pipes
         - `ls -l | grep "file" | wc`
         - `ls -l | grep "e" | awk '{print $9}' | wc`

