anthony ~ 14:01:03 > cd Documents/Classes/Cryptography/minishell/
anthony minishell 14:01:16 > make clean
rm -f ./obj/*.o *~ core 
rm ms 
anthony minishell 14:01:19 > make
gcc -std=gnu99  -c -o obj/utilities.o ../Encryption_module/src/utilities.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/transsub.o ../Encryption_module/src/transsub.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/hillcipher.o ../Encryption_module/src/hillcipher.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/TSHencrypt.o ../Encryption_module/src/TSHencrypt.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/queue.o ../Structures/queue.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/stack.o ../Structures/stack.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/dir_search.o ../Directory_Search_module/dir_search.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/login.o ../login_module/login.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/minishell.o minishell.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
minishell.c:384:6: error: conflicting types for ?encryptFiles?
 void encryptFiles(const char* file, bool recursive, bool verbose, bool mode, bool stats)
      ^
In file included from minishell.c:36:0:
minishell.h:32:6: note: previous declaration of ?encryptFiles? was here
 void encryptFiles(const char*, bool, bool, bool);
      ^
minishell.c: In function ?encryptFiles?:
minishell.c:559:80: error: ?microseconds? undeclared (first use in this function)
                 printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microseconds, bytes, (float) bytes / microseconds * 1000000);
                                                                                ^
minishell.c:559:80: note: each undeclared identifier is reported only once for each function it appears in
make: *** [obj/minishell.o] Error 1
anthony minishell 14:01:35 > make
gcc -std=gnu99  -c -o obj/minishell.o minishell.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
minishell.c: In function ?main?:
minishell.c:199:29: error: too few arguments to function ?decryptFiles?
                             decryptFiles(command.argv[argPaths[j]], recursive, verbose, mode);
                             ^
In file included from minishell.c:36:0:
minishell.h:33:6: note: declared here
 void decryptFiles(const char*, bool, bool, bool, bool);
      ^
minishell.c:202:29: error: too few arguments to function ?encryptFiles?
                             encryptFiles(command.argv[argPaths[j]], recursive, verbose, mode);
                             ^
In file included from minishell.c:36:0:
minishell.h:32:6: note: declared here
 void encryptFiles(const char*, bool, bool, bool, bool);
      ^
minishell.c: In function ?encryptFiles?:
minishell.c:559:80: error: ?microseconds? undeclared (first use in this function)
                 printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microseconds, bytes, (float) bytes / microseconds * 1000000);
                                                                                ^
minishell.c:559:80: note: each undeclared identifier is reported only once for each function it appears in
minishell.c: At top level:
minishell.c:576:6: error: conflicting types for ?decryptFiles?
 void decryptFiles(const char* file, bool recursive, bool verbose, bool mode)
      ^
In file included from minishell.c:36:0:
minishell.h:33:6: note: previous declaration of ?decryptFiles? was here
 void decryptFiles(const char*, bool, bool, bool, bool);
      ^
minishell.c: In function ?decryptFiles?:
minishell.c:706:20: error: ?stats? undeclared (first use in this function)
                 if(stats)
                    ^
minishell.c:759:80: error: ?microseconds? undeclared (first use in this function)
                 printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microseconds, bytes, (float) bytes / microseconds * 1000000);
                                                                                ^
make: *** [obj/minishell.o] Error 1
anthony minishell 14:09:35 > make
gcc -std=gnu99  -c -o obj/minishell.o minishell.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
minishell.c: In function ?encryptFiles?:
minishell.c:560:80: error: ?microseconds? undeclared (first use in this function)
                 printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microseconds, bytes, (float) bytes / microseconds * 1000000);
                                                                                ^
minishell.c:560:80: note: each undeclared identifier is reported only once for each function it appears in
minishell.c: In function ?decryptFiles?:
minishell.c:760:80: error: ?microseconds? undeclared (first use in this function)
                 printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microseconds, bytes, (float) bytes / microseconds * 1000000);
                                                                                ^
make: *** [obj/minishell.o] Error 1
anthony minishell 14:11:47 > make
gcc -std=gnu99  -c -o obj/minishell.o minishell.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
minishell.c: In function ?encryptFiles?:
minishell.c:560:114: error: ?microseconds? undeclared (first use in this function)
                 printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microsecs, bytes, (float) bytes / microseconds * 1000000);
                                                                                                                  ^
minishell.c:560:114: note: each undeclared identifier is reported only once for each function it appears in
minishell.c: In function ?decryptFiles?:
minishell.c:760:114: error: ?microseconds? undeclared (first use in this function)
                 printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microsecs, bytes, (float) bytes / microseconds * 1000000);
                                                                                                                  ^
make: *** [obj/minishell.o] Error 1
anthony minishell 14:12:22 > make
gcc -std=gnu99  -c -o obj/minishell.o minishell.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99   -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall -o ms obj/utilities.o obj/transsub.o obj/hillcipher.o obj/TSHencrypt.o obj/queue.o obj/stack.o obj/dir_search.o obj/login.o obj/minishell.o -lcrypt
anthony minishell 14:12:48 > make clean
rm -f ./obj/*.o *~ core 
rm ms 
anthony minishell 14:12:51 > clear

anthony minishell 14:12:53 > make
gcc -std=gnu99  -c -o obj/utilities.o ../Encryption_module/src/utilities.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/transsub.o ../Encryption_module/src/transsub.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/hillcipher.o ../Encryption_module/src/hillcipher.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/TSHencrypt.o ../Encryption_module/src/TSHencrypt.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/queue.o ../Structures/queue.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/stack.o ../Structures/stack.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/dir_search.o ../Directory_Search_module/dir_search.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/login.o ../login_module/login.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99  -c -o obj/minishell.o minishell.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99   -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall -o ms obj/utilities.o obj/transsub.o obj/hillcipher.o obj/TSHencrypt.o obj/queue.o obj/stack.o obj/dir_search.o obj/login.o obj/minishell.o -lcrypt
anthony minishell 14:12:56 > ./ms
 >  
 >  
 >  
 >  
 >  
 >  
 >  login -l
Enter username: islasa1
Password [4-8 chars]: 
Logged in as: islasa1 [ADMIN]
islasa1 >  encrypt -s -b test.txt
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Stats:
	Time: 3769
	Total Bytes: 777
	Bytes/sec: 206155.48
	Encrypted 1 of 1 total files
	Encrypted 1 of 1 total non-encrypted files
	Skipped 0 of 1 total non-encrypted files
islasa1 >  decrypt test.txt.crpt
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Error: Bad Return
islasa1 >  decrypt test.txt.crpt -b -s
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Stats:
	Time: 4462
	Total Bytes: 777
	Bytes/sec: 174137.16
Decrypted 1 of 1 total files
Decrypted 1 of 1 total encrypted files
Skipped 0 of 1 total encrypted files
islasa1 >  encrypt test.txt -s
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Stats:
	Time: 9601
	Total Bytes: 777
	Bytes/sec: 80929.07
	Encrypted 1 of 1 total files
	Encrypted 1 of 1 total non-encrypted files
	Skipped 0 of 1 total non-encrypted files
islasa1 >  decrypt test.txt.crpt -s
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Stats:
	Time: 10368
	Total Bytes: 778
	Bytes/sec: 75038.59
Decrypted 1 of 1 total files
Decrypted 1 of 1 total encrypted files
Skipped 0 of 1 total encrypted files
islasa1 >  encrypt -b 4k_test.png
encrypt: Could not encrypt. Argument to encrypt is neither file or directory
islasa1 >  encrypt -b 4k_test.jpg
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: n
encrypt: Encryption aborted
islasa1 >  encrypt -s -v 4k_test.jpg
4k_test.jpg
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Stats:
	Time: 8876325
	Total Bytes: 2716443
	Bytes/sec: 306032.38
	Encrypted 1 of 1 total files
	Encrypted 1 of 1 total non-encrypted files
	Skipped 0 of 1 total non-encrypted files
islasa1 >  encrypt -v -s -b 4k.jpg
4k.jpg
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Stats:
	Time: 15449065
	Total Bytes: 2716443
	Bytes/sec: 175832.20
	Encrypted 1 of 1 total files
	Encrypted 1 of 1 total non-encrypted files
	Skipped 0 of 1 total non-encrypted files
islasa1 >  decrypt -v -s -b 4k.jpg.crpt
4k.jpg.crpt
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Stats:
	Time: 17664892
	Total Bytes: 2716443
	Bytes/sec: 153776.38
Decrypted 1 of 1 total files
Decrypted 1 of 1 total encrypted files
Skipped 0 of 1 total encrypted files
islasa1 >  ls
4k.jpg	  minishell.c  ms	     obj	testcmp.txt  test.txt		  transsub.txt
Makefile  minishell.h  nullTest.txt  test2.png	testFolder   transsub (copy).txt  TSHencrypt.txt
islasa1 >  encrypt transsub.txt -s
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Stats:
	Time: 17858118
	Total Bytes: 59236
	Bytes/sec: 3317.03
	Encrypted 1 of 1 total files
	Encrypted 1 of 1 total non-encrypted files
	Skipped 0 of 1 total non-encrypted files
islasa1 >  decrypt transsub.txt.crpt -s
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Stats:
	Time: 17904496
	Total Bytes: 59236
	Bytes/sec: 3308.44
Decrypted 1 of 1 total files
Decrypted 1 of 1 total encrypted files
Skipped 0 of 1 total encrypted files
islasa1 >  exit
anthony minishell 14:26:59 > make
gcc -std=gnu99  -c -o obj/minishell.o minishell.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99   -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall -o ms obj/utilities.o obj/transsub.o obj/hillcipher.o obj/TSHencrypt.o obj/queue.o obj/stack.o obj/dir_search.o obj/login.o obj/minishell.o -lcrypt
