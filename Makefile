PROJ = proj00 proj02-1 proj02-2 proj02-3 proj03 proj04 proj05 proj06-1 proj06-2\
	proj07 proj08 proj09 proj10-1 proj10-2 proj11
all : $(PROJ)
clean : 
	rm -f $(PROJ)

proj00 :
	gcc -Wall -o proj00 p00/testafila.c p00/queue.c
proj02-1 :
	gcc -Wall -o proj02-1 p02/pingpong-tasks1.c p02/pingpong.c p00/queue.c
proj02-2 :
	gcc -Wall -o proj02-2 p02/pingpong-tasks2.c p02/pingpong.c p00/queue.c
proj02-3 :
	gcc -Wall -o proj02-3 p02/pingpong-tasks3.c p02/pingpong.c p00/queue.c
proj03 :
	gcc -Wall -o proj03 p03/pingpong-dispatcher.c p03/pingpong.c p00/queue.c
proj04 :
	gcc -Wall -o proj04 p04/pingpong-scheduler.c p04/pingpong.c p00/queue.c
proj05 :
	gcc -Wall -o proj05 p05/pingpong-preempcao.c p05/pingpong.c p00/queue.c
proj06-1 :
	gcc -Wall -o proj06-1 p06/pingpong-contab.c p06/pingpong.c p00/queue.c
proj06-2 :
	gcc -Wall -o proj06-2 p06/pingpong-contab-prio.c p06/pingpong.c p00/queue.c -DPRIORITY
proj07 :
	gcc -Wall -o proj07 p07/pingpong-maintask.c p07/pingpong.c p00/queue.c
proj08 :
	gcc -Wall -o proj08 p08/pingpong-join.c p08/pingpong.c p00/queue.c
proj09 :
	gcc -Wall -o proj09 p09/pingpong-sleep.c p09/pingpong.c p00/queue.c
proj10-1 :
	gcc -Wall -o proj10-1 p10/pingpong-racecond.c p10/pingpong.c p00/queue.c
proj10-2 :
	gcc -Wall -o proj10-2 p10/pingpong-semaphore.c p10/pingpong.c p00/queue.c
proj11 :
	gcc -Wall -o proj11 p11/pingpong-barrier.c p11/pingpong.c p00/queue.c

