# MORSELAB DSA Modules

## Used by the application

### Hash table

`backend/src/hash_table.c` implements a fixed-size hash table with separate chaining. The production Morse translator uses it as a character-to-code lookup table while encoding text (`morse_encode_text` in `morse.c`).

### Binary Morse tree

`backend/src/morse_tree.c` implements a binary tree where dot follows the `dot` child and dash follows the `dash` child. The production translator builds this tree from the same Morse mappings and uses it to decode each code token.

## Standalone modules and tests

### Stack

`stack.c` implements a singly linked LIFO stack of characters. Its current use is the standalone `tests/test_stack.c`; application translation and account flows do not call it.

### Queue

`queue.c` implements a linked FIFO queue of operation IDs and copied operation strings. Its current use is `tests/test_queue.c`; production request handling does not call it.

### Translation linked list

`linked_list.c` implements a singly linked list of translation records, with append, find, delete, print, and free operations. It is exercised by `tests/test_linked_list.c`; history and saved translations in the application use SQLite directly instead.

## Session list

`auth.c` has a separate private `SessionNode` linked list for active in-memory session tokens. It is not the reusable `TranslationList` in `linked_list.c`.

The backend tests also cover the Morse tree, hash table, Morse translator, authentication, and database modules. The DSA implementations are retained as-is; this documentation reflects where the current production paths call them.
