# Note Sharer

This is a simple note-sharer program made in C, that allows users at different machines to commincate with each other. 

**Overview**

Say you want to send a couple of notes to multiple users on various other machines. Look no further, the   ***NOTE-SHARER***   has your back! 

First you will need to write some notes to all users you wish to commincate with. To do so, the main file will need to be executed with the `user` parameter.
Immediately, you will be prompted to provide a user name like so: 

```
Please enter a user name (max 19 characters):
```
You have two options here: 

*1.* If you want to read/edit notes that someone wrote to you / or you wrote for yourself

  In this case, you would enter your own user name. This will allow you to view notes that other people / or you yourself have written for you.

*2.* If you want to read/edit notes for someone else

  In this case, you would enter the name of the person that your notes are directed to, and add notes under their name so when they run the program, they can       enter their own name and read what others have written for them. 


Next, multiple options will appear on the screen, displaying the your options
```
1) view (print your notes to stdout)
2) add (add to your notes)
3) quit (exit the program and update notes) 
```
**Detials**

- *view*
   Print the notes of user name provided to STDOUT
- *add*
   Add notes under this user name 
- *quit*
   Save the notes 

**Overview**

This program allows multiple users to save notes for later use. To do so, the main file will need to be executed with the `user` parameter
When the program is run, it will prompt the user to proide a user name like so:

```
Please enter a user name (max 19 characters):
```

Next, multiple options will appear on the screen, displaying the user's options
```
1) view (print your notes to stdout)
2) add (add to your notes)
3) quit (exit the program and update notes) 
```
If the user has previously used the program to write any notes, they can choose to view them again. They can also add notes and exit the program. Upon exiting the program, the first 10 machines listed in **config.txt** by the user will be updated with the latest notes, meaning that on that machine, running the main file with the `user` parameter will allow users on that machine also be able to 

**Instructions for running**

After cloning the repo, running "make all" command should complie the necessary files. If you have already been using the program and want to start all over (deleting all saved data), you can run "make hard_clean" (you will have to run "make all" again after this to recomplie the C file)

Executiong of the main file will require 


Executing the main file, should prompt you to provide a user name. You will be given three options: 
```
1) view (print your notes to stdout)
2) add (add to your notes)
3) quit (exit the program and update notes) 
```
**Files that the program edits**

data: This binary file contains very important user information such as user names, length of notes (in bytes), and number of total users, and should not be modified in any way. 

notes: This file contains the notes. Viewing it can display each user and their notes. 

**Files that the you edit**

config.txt: This file contains ip addresses and port numbers of all the machines that you want to update your notes at. The format is ```[ip],[port]```

Sample config.txt
```
127.0.0.1,55555
```

The computer at the `[ip]` (reciever machine) will have to have executed the main file in `recv` mode and used the same port as outlined in **config.txt** at the sender machine to be able to update the notes and data file. 

**Conclusion**

This is not a very "user secure" program by any means. To mitigate this, I might add passowrd functionality and edit the file permissions in the future. Thank you for reading this :)
