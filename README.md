# CITS3002-Project
<p align="center">
  <b> <h1>HIMEKO KILLED BY KIANA HOV?</h1></b><br>
  <img src="https://c.tenor.com/h3zX-_0GoooAAAAM/himeko-murata-himeko.gif" | width=900><br>
</p>

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
- [ ] Optimize C Client (realocation of size exceeding) (fuck C)
- [x] Start of rakeserver
- [x] Rake clients contact rakeserver
- [x] Support Multiple Instances of rake
- [x] Exchange meaningful commands and responses
- [x] Execute Simple Actions on rakeserver
- [x] Dynamic recv() sizing
- [x] Receive errors and terminate on error
- [ ] Send requirements to server, have server check if those are present in directory (implement cost equation via this)
- [ ] Redo entire sending of data process.
- [ ] Send stderr to clients to print
- [ ] Have the server check if any new files were created, if so send them back to the user.
- [ ] Implement sending files back to the user (separate from sending command outputs).
- [ ] Implement sending files to the server (server requires recv code)
- [ ] Perform Compiliation and Linking
- [ ] Test two-tabbed lines that are only a comment w/ C and Python.
- [ ] Check if the python TODOs are fine.
- [ ] Refactor code on Python
- [ ] Complete C code
- [ ] Refactor C code
- [ ] Catch errors
- [ ] Complete report

<!-- Could update this with more links for the future -->
# Useful Links idk
[Socket Programming in C](https://www.geeksforgeeks.org/socket-programming-cc/) <br>
[Socket Programming in Python](https://www.geeksforgeeks.org/socket-programming-python/) <br>


queen
<img src="https://static.wikia.nocookie.net/hoducks/images/b/b9/Azure_Empyrea.png/revision/latest?cb=20201226082505">
