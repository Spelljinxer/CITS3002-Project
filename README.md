# CITS3002-Project
Project Description:
> Develop a internetworked application that compiles and links C programs across different computers connected using internet protocols.
# Team Members:
[Sckaeth](https://github.com/Sckaeth) <br>
[NEDEN249](https://github.com/NEDEN249) <br>
[Spelljinxer](https://github.com/Spelljinxer)
#
**Compilation and execution for C:**
  ```
  cc -std=c11 -Wall -Werror -o rake-c rake-c.c
  ./rake-c [arguments]
  ```
**Compilation and execution with Python:**
  ```
  python3 rake-p.py
  ```
**Compilaton and execution of rakesever: _(Make sure to have multiple terminals open for this)_**
  ```
  python3 rakeserver.py
  ```
  
<!-- UPDATE THIS AS WE GO ALONG-->
# Task List
- [x] Parsing Rakefile - Python Client 
- [x] Parsing Rakefile - C Client
- [ ] Optimize C Client (realocation of size exceeding)
- [ ] Start of rakeserver
- [ ] Rake clients contact rakeserver
- [ ] Support Multiple Instances of rake
- [ ] Exchange meaningful commands and responses
- [ ] Execute Simple Actions on rakeserver
- [ ] Perform Compiliation and Linking
- [ ] Test two-tabbed lines that are only a comment w/ C and Python.

<!-- Could update this with more links for the future -->
# Useful Links idk
[Socket Programming in C](https://www.geeksforgeeks.org/socket-programming-cc/) <br>
[Socket Programming in Python](https://www.geeksforgeeks.org/socket-programming-python/) <br>


