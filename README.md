# Login-Notepad

This is a simple notetaker program made in C with a login system.

**Instructions for running**

After cloning the repo, running "make all" command should complie the necessary files. If you have already been using the program and want to start all over (deleting all saved data), you can run "make hard_clean" (you will have to run "make all" again after this to recomplie the C file)

Executing the main file, should prompt you to provide a user name. You will be given three options: 

1) view (print your notes to stdout)
2) add (add to your notes)
3) quit (exit the program) 

**Files**

data: This binary file contains very important user information such as user names, length of notes (in bytes), and number of total users, and should not be modified in any way. 

notes: This file contains the notes. Viewing it can display each user and their notes. 

**Conclusion**

This is not a very "user secure" program by any means. To mitigate this, I might add passowrd functionality and edit the file permissions in the future. Thank you for reading this :)
