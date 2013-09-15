#define DEBUG_LEVEL 3

#define NUMBER_SERVERS 100 
#define NUMBER_CLIENTS 250
#define KNOWLEDGE 100
#define MAX_CONNECTIONS_ATTEMPT 100
#define NUMBER_SERVERS_DOWN 0
#define TIME_SERVERS_DOWN 5000
#define TIME_SERVERS_UP 10000

#define RANDOM_CAP 0
#define SERVERS_BAND_DIVISION 1
#define CLIENTS_BAND_DIVISION 1

//setar variaveis se RANDOM == 0 / inicio
#define CAP_SERVER_1 1000
#define CAP_SERVER_2 100
#define CAP_SERVER_3 100

#define CAP_CLIENT_1 100
#define CAP_CLIENT_2 1000
#define CAP_CLIENT_3 1000
//setar variaveis se RANDOM == 0 / fim

//setar variaveis se RANDOM == 1 / inicio
#define MAX_CAP_SERVER 100
#define MAX_CAP_CLIENT 1000

#define MIN_CAP_SERVER 100
#define MIN_CAP_CLIENT 1000
//setar variaveis se RANDOM == 1 / fim


#define CONNECTION_LIMIT 1
#define DROP_SERVERS 1
#define SET_SIZE 50
#define INITIAL_SET_SIZE 1
#define FILE_SIZE 1000000
#define MAX_TIME 100000

#define CONNLIMIT 1
#define OVERHEAD 2
#define ALGORITHM 2

// DELTA2>>DELTA3 
#define DELTA1 1 //entrada-saida
#define DELTA2 20 //mudanca de fluxo (media pelo menos duas vezes maior)
#define DELTA3 1 //des-conexao


#define VARIACAO 10 //porcentagem da variacao: (fluxo antigo)*(VARIACAO/100) = maximo de diferenca entre fluxo antigo e atual
#define FLOW_PERCEPTION 3 
#define CONNECTION_COST 12

#define FLOW_EVENT 0
#define CLIENT_EXIT_EVENT 1
#define CLIENT_ARRIVAL_EVENT 2
#define SERVER_ARRIVAL_EVENT 5
#define SERVER_EXIT_EVENT 6
#define CONNECTION_EVENT 3
#define DISCONNECTION_EVENT 4
#define INF 999999999
#define MAX_BAND_COST 100
