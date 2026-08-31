# PizzaHut

This repo is mainly design for me to learn and grow as a C++ developer.

PizzaHut is a small, key-value database aimed to be as performant as possible that is written by me. I know there are databases like TigerBeetle and RockDb that will be way faster than PizzaHut, but the point of this project is not to be faster than them.

Now where do I begin? I do not even know what a database. 

While searching on the web, I came by a paper written by Justin Sheehy and David Smith: Bitcask.

Here is the paper: https://riak.com/assets/bitcask-intro.pdf?

I will see what I can do with this.

## Phase 1 - Create a walmart version of Bitcask

In phase 1, we will be building a single-file version of Bitcask which supports the following operations, `open()`, `put()`, and `get()`, with an in-memory keydir rebuilt by scanning the single data file we will store.

We are intentionally excluding all the complicated crap:
- multiple data files
- merging/compaction
- hint files
- delete/tombstones
- threading
- multiple processes
- file locking
- compression
- caching
- workers
- crash recovery
- sync policies

At least that is what I am thinking on top of my head.

General Flow:
```
// 1. Open the database
open("db")

// 2. Put whatever key/values (any type work)
put(1, "Veggie Lover")
put(21, "Pepperoni")
put(333, true)

// 3. Getting a value should be based on its key.
get(1) -> "Veggie Lover"
get(21) -> "Pepperoni"
get(333) -> true
get(111) -> KeyNotFound

// 4. When we close and then re-open the database, keydir should be derived from that one data file.
close("db")

// 5. There should be no keydir in-memory.
open("db")

// 6. We should see keydir constructed correctly in-memory.
1 -> "Veggie Lover"
21 -> "Pepperoni"
333 -> true
```