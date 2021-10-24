# The Note Sharer

This is a simple note-sharer program made in C, that allows users at different machines to commincate with each other. 

**Overview**

There are two main modes of running this program

**1) User Mode**

Say you want to send a couple of notes to multiple users on various other machines. Look no further, the   ***NOTE-SHARER***   has your back! 

First you will need to write some notes to all users you wish to commincate with. To do so, the main file will need to be executed with the `user` parameter.
Immediately, you will be prompted to provide a user name like so: 

```
Please enter a user name (max 19 characters):
```
You have two options here: 

**1.** If you want to read/edit notes that someone wrote to you / or you wrote for yourself

  In this case, you would enter your own user name. This will allow you to view notes that other people / or you yourself have written for you.

**2.** If you want to read/edit notes for someone else

  In this case, you would enter the name of the person that your notes are directed to, and add notes under their name so when they run the program, they can       enter their own name and read what others have written for them. 


Next, multiple options will appear on the screen, displaying the your options
```
1) view (print your notes to stdout)
2) add (add to your notes)
3) quit (exit the program and update notes) 
```
**Details**

- *view*

   Print the notes of user name provided to STDOUT
- *add*

   Add notes under this user name 
- *quit*

   Save the notes and try to update them on the first 10 Machines listed in **config.txt**
   
**2) Reciever Mode**

This mode allows the machine to update the notes. To do so, the main file will need to be executed with the `recv` parameter at the same time as the other machine running `user` mode (where the notes are currently being edited). The ip of this machine and port that will be provided by reciever here should correlate to the ones provided in **config.txt** at the `user` mode's side

When the `user` mode machine quits, it will send the update files to the machine/s running in `recv` mode. Now with these updated notes, anyone on this machine can execute the program in `user` mode and view/add notes. 

**Files that the program edits**

data: This binary file contains very important user information such as user names, length of notes (in bytes), and number of total users, and should not be modified in any way. 

notes: This file contains the notes. Viewing it can display each user and their notes. 

**Files that you edit**

config.txt: This file contains ip addresses and port numbers of all the machines that you want to update your notes at. The format is ```[ip],[port]```

Sample config.txt
```
127.0.0.1,55555
```

The computer at the `[ip]` (reciever machine) will have to have executed the main file in `recv` mode and used the same port as outlined in **config.txt** at the sender machine to be able to update the notes and data file. 


