##Report on Synchronization:


###Which Objects Needed to be Synchronized:

The server and tables are the only objects using synchronization in our implementation, as they are the 
only resources shared between clients (threads), and are therefore the only objects that could be subject 
to synchronization issues between clients.


###How our Objects are Syncronized:

Server- This implementation uses pthread_mutex_t mutexs to make the server innacessible to all but 
one client while tables are being created. Also, tables are never removed from the server in this implementation, 
but because clients could add tables with duplicate names (making them difficult to access accurately), the 
server checks that no client has created a table with the same name before adding new tables.

Tables- In autocommit mode, tables are locked while their entries are being read and edited, as these points
were identified as their critical sections, and are unlocked immediately after their entries are accessed (in GET
and SET requests). During transactions, the server code checks if a table had been locked during the transaction, 
then tries to acquire the lock if not. If the lock can't be acquired, the transaction fails, and if it can, the lock 
is held until the transaction fails in another way or the transaction succeeds and its edits are commited. 


###Why We Are Confident our Server is Free of Race Conditions and Deadlocks:

Race conditions- Because each autocommited command is atomic, these commands access or edit table information at
the time they gain access to it, which is built into the pthread_mutex_t locking and unlocking functions used in this 
implementation. Since transactions allow clients to hold the locks to all of their accessed tables, data races are not
a risk in transactions either, as other clients are not able to edit data that clients in transactions expect to be
left unchanged until their transaction completes. 

Deadlocks- Autocommited commands can only lock one table at a time, so any client waiting on an table that is locked by
a client in autocommit will not have to wait indefinitely- the table will be unlocked as soon as the operation fails or
completes. While deadlocks would be possible between clients in transactions with each other or with autocommiting 
clients if transactions could wait on other operations, because transactions fail as soon as they try to access a locked 
table, clients will never wait indefinitely on transactions either- if a client holds a lock a transacting client needs, 
the transaction will fail, and if a client needs a lock being held by a client in transaction the client will only need to
wait until the transaction completes, successfully or not.