# MallocBugTracker
CS 214: Systems Programming  
Professor Menendez  
Long Tran (netID: lht21)  
Julian Herman (netID: jbh113)  



## Makefile Intructions
- 'make' (default) will run debug mode
- 'make run' will run on official mode

## Testing Plan:
*** Notice ***
  - Testcases are created based on iLab machine's file system. 
  - Please use iLab machines to test these cases for accurate result

### Unit testing:
  - Built-in Commands:
    - pwd: 
      - Single command: output into current STDOUT_FILENO (redirection will set STDOUT_FILENO before executing)
      - Pipeline command: output into pipeline
    - cd: Do not have input/output

  - Exectuting Commands:
    - Assure to be able to run in these directories:
      1. /usr/local/sbin -> 
      2. /usr/local/bin
      3. /usr/sbin
      4. /usr/bin
      5. /sbin
      6. /bin

