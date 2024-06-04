# OS Programming Project
A project about programming in Linux OS using C and bash shell code.

The project has 3 parts:
- Shell scripting in bash shell
- Code optimizations using processes
- Processes time scheduler for Linux
___
### Shell Scripting
The **logparsher.sh** file contains bash code for the management of **access.log** file, which is a Linux system file. Each of its records may have one of the following formats:
- 127.0.0.1 - userid - [10/Feb/2022:13:55:36 +0300] "GET /server-status HTTP/1.1" 200 2326
- 127.0.0.1 - - [29/Mar/2022:17:02:15 +0300] "POST /phpmyadmin/index.php?route=/navigation&ajax_request=1 HTTP/1.1" 200 3941 "-" "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:98.0) Gecko/20100101 Firefox/98.0"
- ::1 - - [19/Apr/2022:10:22:23 +0300] "GET /phpmyadmin/themes/pmahomme/img/b_bookmark.png HTTP/1.1" 200 637 

Depending on the arguments of logparsher.sh the following functions are performed:
- ./logparser.sh &lt;filename&gt;
___
### Processes

___
### Processes Time Scheduler
