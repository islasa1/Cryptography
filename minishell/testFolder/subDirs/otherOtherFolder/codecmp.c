Starting sub
Starting trans
Error System call: : Invalid argument
Couldn't verify tag
Decrypted 0 of 1 total files
Decrypted 0 of 1 total encrypted files
Skipped 1 of 1 total encrypted files
islasa1 >  ls
Makefile     minishell.h  nullTest.txt	test2 (another copy).png  testcmp.txt  test.txt.crpt
minishell.c  ms		  obj		test2 (copy).png	  testFolder
islasa1 >  encrypt nullTest.txt
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: n
encrypt: Encryption aborted
islasa1 >  encrypt nullTest.txt -b
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Encrypted 1 of 1 total files
Encrypted 1 of 1 total non-encrypted files
Skipped 0 of 1 total non-encrypted files
islasa1 >  decrypt nullTest.txt.crpt -b
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Starting sub
Starting trans
Error System call: : Invalid argument
Couldn't verify tag
Decrypted 0 of 1 total files
Decrypted 0 of 1 total encrypted files
Skipped 1 of 1 total encrypted files
islasa1 >  cd testFolder
islasa1 >  ls
otherFolder  otherOtherFolder  test1.txt.crpt  test2.txt.crpt  test3.c.crpt  test.txt.crpt
islasa1 >  decrypt test1.txt.crpt
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Starting sub
Starting trans
Couldn't verify tag
Decrypted 0 of 1 total files
Decrypted 0 of 1 total encrypted files
Skipped 1 of 1 total encrypted files
islasa1 >  decrypt test2.txt.crpt
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Starting sub
Starting trans
Couldn't verify tag
Decrypted 0 of 1 total files
Decrypted 0 of 1 total encrypted files
Skipped 1 of 1 total encrypted files
islasa1 >  cd otherOtherFolder
islasa1 >  ls
code.c
islasa1 >  encrypt code.c
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Encrypted 1 of 1 total files
Encrypted 1 of 1 total non-encrypted files
Skipped 0 of 1 total non-encrypted files
islasa1 >  decrypt code.c
decrypt: Could not decrypt. Argument to decrypt is neither file or directory
islasa1 >  decrypt code.c.crpt
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Starting sub
Starting trans
Decrypted 1 of 1 total files
Decrypted 1 of 1 total encrypted files
Skipped 0 of 1 total encrypted files
islasa1 >  exit
anthony minishell 11:49:47 > make
gcc -std=gnu99  -c -o obj/TSHencrypt.o ../Encryption_module/src/TSHencrypt.c -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall
gcc -std=gnu99   -O0 -g -I../login_module -I../Encryption_module/src -I../Encryption_module/include -I../Directory_Search_module -I../Shell_utilities -I../Structures  -Wall -o ms obj/utilities.o obj/transsub.o obj/hillcipher.o obj/TSHencrypt.o obj/queue.o obj/stack.o obj/dir_search.o obj/login.o obj/minishell.o -lcrypt
anthony minishell 11:49:59 > ./ms
 >  cd testFolder/otherOtherFolder
 >  ls
code.c
 >  encrypt code.c
ms: Must be logged in as user to decrypt
 >  login -l
Enter username: islasa1
Password [4-8 chars]: 
Logged in as: islasa1 [ADMIN]
islasa1 >  encrypt code.c
1 files will be encrypted, each file output as file.ext.crpt
	Proceed? [Yn]: y

Encrypted 1 of 1 total files
Encrypted 1 of 1 total non-encrypted files
Skipped 0 of 1 total non-encrypted files
islasa1 >  decrypt code.c.crpt
1 files will be decrypted, each file output as file.ext - .crpt
	Proceed? [Yn]: y
Couldn't verify tag
Decrypted 0 of 1 total files
Decrypted 0 of 1 total encrypted files
Skipped 1 of 1 total encrypted files
islasa1 >  
