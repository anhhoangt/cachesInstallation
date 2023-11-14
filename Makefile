all: messageStore

messageStore: message.c messageStore.c LRUCache.c randomCache.c genRand.c
	gcc -o messageStore message.c messageStore.c LRUCache.c randomCache.c genRand.c

clean:
	rm -f messageStore *.o
