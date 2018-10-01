PROJ = proj00 proj02-1 proj02-2 proj02-3 proj03 proj04 proj05 proj06-1 proj06-2
all : $(PROJ)
clean : 
	rm -f $(PROJ)

proj00 : 
	gcc -Wall -Wextra -o proj00 p00/testafila.c p00/queue.c
proj02-1 :
	gcc -Wall -Wextra -o proj02-1 p02/pingpong-tasks1.c p02/pingpong.c p00/queue.c 
proj02-2 :
	gcc -Wall -Wextra -o proj02-2 p02/pingpong-tasks2.c p02/pingpong.c p00/queue.c
proj02-3 :
	gcc -Wall -Wextra -o proj02-3 p02/pingpong-tasks3.c p02/pingpong.c p00/queue.c
proj03 : 
	gcc -Wall -Wextra -o proj03 p03/pingpong-dispatcher.c p03/pingpong.c p00/queue.c
proj04 :
	gcc -Wall -Wextra -o proj04 p04/pingpong-scheduler.c p04/pingpong.c p00/queue.c
proj05 :
	gcc -Wall -Wextra -o proj05 p05/pingpong-preempcao.c p05/pingpong.c p00/queue.c
proj06-1 : 
	gcc -Wall -Wextra -o proj06-1 p06/pingpong-contab.c p06/pingpong.c p00/queue.c
proj06-2 :
	gcc -Wall -Wextra -o proj06-2 p06/pingpong-contab-prio.c p06/pingpong.c p00/queue.c -DPRIORITY
