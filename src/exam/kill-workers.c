/* kill-workers.c */

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t pool[30][30];
int thr_left_per_team[30];
int thrcnt, n, m;
int teamid,id;

struct thrid { int teamid, id; }; 
void
print_teams(void)
{
	int i;
	printf("teams ");
	for (i = 0; i < n; ++i)
		printf("%d:%d ", i, thr_left_per_team[i]);
	printf("\n");
}

void *
f(void *arg)
{
	struct thrid *this = (struct thrid *)arg;
	int rnd_team, error;
	
	if ((error = pthread_mutex_lock(&mutex)) != 0)
		errx(1, "pthread_mutex_lock: %s", strerror(error));
	while (teamid != this->teamid || id != this->id) {

		/* Signal to others int team that this is now the working thread. */
		if (teamid == this->teamid) {
			id = this->id;
			printf("== %d/%d thread picked ==\n", teamid, id);
			break;
		}
		if ((error = pthread_cond_wait(&cond, &mutex)) != 0)
			errx(1, "pthread_cond_wait: %s", strerror(error));
	}

	--thrcnt;
	thr_left_per_team[this->teamid] -= 1;

	printf("%d/%d remaining threads now %d\n", this->teamid, this->id, thrcnt);

	/* Now pick random nonemty team and set global id and broadcast it. */
	while (thrcnt != 0) {
		rnd_team = random() % n;
		
		if (thr_left_per_team[rnd_team] > 0) {
			printf("%d/%d chose next team %d\n", this->teamid, this->id,
					rnd_team);
			teamid = rnd_team;

			/* Reset id so that it doestn't randomly match */
			id = -1;
			break;
		}
	}


	if ((error = pthread_cond_broadcast(&cond)) != 0)
		errx(1, "pthread_cond_broadcast: %s", strerror(error));
	if ((error = pthread_mutex_unlock(&mutex)) != 0)
		errx(1, "pthread_mutex_unlock: %s", strerror(error));

	return NULL;
}

int
main(int argc, char **argv)
{
	int i, j, rnd_team, error;
	struct thrid ids[30][30];

	if (argc != 3)
		errx(1, "usage: ./a.out <n> <m>");

	if ((n = atoi(argv[1])) > 30) 
		errx(1, "agrv[1] is more than limit 30");

	if ((m = atoi(argv[2])) > 30) 
		errx(1, "agrv[2] is more than limit 30");

	thrcnt = n * m;

	if ((error = pthread_mutex_lock(&mutex)) != 0)
		errx(1, "pthread_mutex_lock: %s", strerror(error));
	for (i = 0; i < n; ++i) {
		thr_left_per_team[i] = m;
		for (j = 0; j < m; ++j) {
			ids[i][j].teamid = i;
			ids[i][j].id = j;
			if ((error = pthread_create(&pool[i][j], NULL,
				f, &ids[i][j])) != 0)
				errx(1, "pthread_create: %s", strerror(error));

			printf("%d/%d started.\n", i, j);
		}
	}

	/* Pick random team */
	while (1) {
		rnd_team = random() % n;
		
		if (thr_left_per_team[rnd_team] > 0) {
			printf("main picking team %d.\n", rnd_team);
			teamid = rnd_team;
			id = -1;
			break;
		}
	}

	if ((error = pthread_cond_broadcast(&cond)) != 0)
		errx(1, "pthread_cond_broadcast: %s", strerror(error));
	if ((error = pthread_mutex_unlock(&mutex)) != 0)
		errx(1, "pthread_mutex_unlock: %s", strerror(error));

	/* Wait for thrcnt to drop to 0 */
	if ((error = pthread_mutex_lock(&mutex)) != 0)
		errx(1, "pthread_mutex_lock: %s", strerror(error));

	while (thrcnt != 0)
		if ((error = pthread_cond_wait(&cond, &mutex)) != 0)
			errx(1, "pthread_cond_wait: %s", strerror(error));

	printf("main woke up");

	/* Now we can just unlock the mutex all threads are finished */
	if ((error = pthread_mutex_unlock(&mutex)) != 0)
		errx(1, "pthread_mutex_unlock: %s", strerror(error));

	/* Join all threads */
	for (i = 0; i < n; ++i)
		for (j = 0; j < m; ++j)
			if ((error = pthread_join(pool[i][j], NULL)) != 0)
				errx(1, "pthread_join: %s", strerror(error));

	return (0);
}
