#include <pthread.h>

struct estacao {
    pthread_mutex_t lock;
    pthread_cond_t vagao_c;
    pthread_cond_t vagao_embarque;
    int assentos_disponiveis;
    int passageiros_espera;
    int passageiros_embarque;
};


void estacao_init(struct estacao *estacao) {
    pthread_mutex_init(&estacao->lock, NULL);
    pthread_cond_init(&estacao->vagao_c, NULL);
    pthread_cond_init(&estacao->vagao_embarque, NULL);
    estacao->assentos_disponiveis = 0;
    estacao->passageiros_espera = 0;
    estacao->passageiros_embarque = 0;
}

void estacao_preencher_vagao(struct estacao *estacao, int assentos) {
    pthread_mutex_lock(&estacao->lock);
    
    while (estacao->passageiros_embarque < assentos && estacao->passageiros_espera > 0) {
        pthread_cond_signal(&estacao->vagao_c);
        pthread_cond_broadcast(&estacao->vagao_embarque);
        pthread_cond_wait(&estacao->vagao_embarque, &estacao->lock);
    }

    estacao->passageiros_embarque = 0;
    estacao->assentos_disponiveis = 0;

    pthread_mutex_unlock(&estacao->lock);
}

void estacao_espera_pelo_vagao(struct estacao *estacao) {
    pthread_mutex_lock(&estacao->lock);

    estacao->passageiros_espera++;
    pthread_cond_wait(&estacao->vagao_c, &estacao->lock);
   
    estacao->passageiros_espera--;
    estacao->assentos_disponiveis--;

    pthread_mutex_unlock(&estacao->lock);
}

void estacao_embarque(struct estacao *estacao) {
    pthread_mutex_lock(&estacao->lock);
    
    estacao->passageiros_embarque++;

    pthread_cond_signal(&estacao->vagao_embarque);
    
    pthread_mutex_unlock(&estacao->lock);
}