#project1: prolific.c generation.c explorer.c slug.c slugrace.c
 ##	gcc -o prolific prolific.c
 ##	gcc -g prolific.c -o prolific
 ##	gcc -o generation generation.c
 ##	gcc -g generation.c -o generation
 ##	gcc -o explorer explorer.c
 ##	gcc -g explorer.c -o explorer
 ##	gcc -o slug slug.c
 ##	gcc -g slug.c -o slug
 ##	gcc -o slugrace slugrace.c
 ##	gcc -g slugrace.c -o slugrace

all: taking_flight

taking_flight: taking_flight.c
	gcc -o taking_flight taking_flight.c
	gcc -g taking_flight.c -o taking_flight

clean:
	rm -f taking_flight