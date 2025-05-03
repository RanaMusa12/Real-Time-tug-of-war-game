#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void init_player(Player *p) {
    p->energy = rand() % (ENERGY_MAX - ENERGY_MIN + 1) + ENERGY_MIN;
    p->is_fallen = 0;
}

void decay_energy(Player *p) {
    int decay = rand() % (ENERGY_DECAY_MAX - ENERGY_DECAY_MIN + 1) + ENERGY_DECAY_MIN;
    p->energy -= decay;
    if (p->energy < 0) p->energy = 0;
}

void check_falling(Player *p) {
    if (!p->is_fallen && (rand() % 100 < FALL_PROBABILITY)) {
        p->is_fallen = 1;
        p->energy = 0;
        int recovery = rand() % (RECOVERY_TIME_MAX - RECOVERY_TIME_MIN + 1) + RECOVERY_TIME_MIN;
        sleep(recovery);
        p->energy = rand() % (ENERGY_MAX - ENERGY_MIN + 1) + ENERGY_MIN;
        p->is_fallen = 0;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <id> <pipe_fd>\n", argv[0]);
        return 1;
    }

    srand(time(NULL) + atoi(argv[1]));
    
    Player p;
    p.id = atoi(argv[1]);
    p.pipe_fd = atoi(argv[2]);
    init_player(&p);

    while (1) {
        decay_energy(&p);
        check_falling(&p);
        
        if (write(p.pipe_fd, &p.energy, sizeof(int)) != sizeof(int)) {
            perror("Player write failed");
            break;
        }
        sleep(1);
    }
    return 0;
}
