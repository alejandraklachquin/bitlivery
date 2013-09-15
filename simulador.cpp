
/*
fase 1: re-executar algoritmo de alocacao entre todos os servidores
fase 2: achar componente conexo para alocacao de banda, pensar em outros algorimos de selecao de servidores
*/

/*
fazer testes e tirar estimativas:

homogeneo:
por todos com mesma banda
dps aumentar a banda dos clientes

depois clientes homogeneos , servidores hetero: servidores mais banda
depois aumenta banda dos clientes

fazer alguns testes e comparar com resultados analiticos

MEDIA 0.1 (DETAL 10) ENTRADA DOS CLIENTES


EXPERIMENTOS:


100 CLIENTES E 100 SERVIDORES 
DROP_CONNECTION

CONNECTION_COST: PAGA 2% DA BANDA DO CLIENTE
SET_SIZE: SEMPRE COMEÇAM COM 2 E PODEM AUMENTAR ATE {4, 8, 12}

TAMANHO ARQUIVO : 1 MILHAO (1GB)

CAPACIDADE DOS SERVIDORES: 1000 (1 MEGA)
CAPACIDADE DOS CLIENTES: 
1) TODOS 100
2) TODOS 300
3) TODOS 600
4) TODOS 1000
5) TODOS 1300
6) TODOS 1500

CAPACIDADE DOS CLIENTES: 1000 (1 MEGA)
CAPACIDADE DOS SERVIDORES: 
1) TODOS 100
2) TODOS 300
3) TODOS 600
4) TODOS 1000
5) TODOS 1300
6) TODOS 1500

TENTAR CALCULAR O LIMITE INFERIOR DAS COISAS.

TESTAR CLIENTES: SET_SIZE = 4, COMEÇANDO COM 2, TODOS COM MESMA CAPACIDA
NUMERO DE CLIENTES:
50
100
150
200

100 SERVIDORES, ARQUIVO 1MILHAO, BANDA DE CLIENTES E SERVIDORES MIL
CALCULAR O TEMPO MINIMO QUE DEVERIA LEVAR.

*/


/*

CODIGO DE DIVISAO DE GRUPOS
       
      -1: CLIENTES E SERVIDORES COM MESMA BANDA - so com random == 1
       0: TODOS DO MESMO GRUPO COM CAPACIDADES DISTINTAS
       1: TODOS DO MESMO GRUPO COM A MESMA CAPACIDADE
       2: 1/2 - 1/2
       3: 1/3 - 1/3 - 1/3
       4: 1/3 - 2/3
       
       NAO IMPLEMENTADO:
       5: 1/4 - 1/4 - 1/4 - 1/4
       6: 1/4 - 1/4 - 1/2
       7: 1/4 - 3/4
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cassert>
#include <ctype.h>
#include <math.h>
//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int_distribution.hpp>
#define DEBUG_LEVEL 3

#define NUMBER_SERVERS 100 
#define NUMBER_CLIENTS 100
#define KNOWLEDGE 100

#define RANDOM_CAP 0
#define SERVERS_BAND_DIVISION 1
#define CLIENTS_BAND_DIVISION 1

//setar variaveis se RANDOM == 0 / inicio
#define CAP_SERVER_1 100
#define CAP_SERVER_2 100
#define CAP_SERVER_3 100

#define CAP_CLIENT_1 1000
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
#define INF 999999999999
#define MAX_BAND_COST 100
using namespace std;

FILE *output;
int t_print;
float t, t_last, t_last_fi;
float server_arrive_time[NUMBER_SERVERS];
float client_arrive_time[NUMBER_CLIENTS];
float server_exit_time[NUMBER_SERVERS];
float client_exit_time[NUMBER_CLIENTS];

float soma_t_c[NUMBER_CLIENTS];
float soma_t_s[NUMBER_SERVERS];
float t_last_fi_c[NUMBER_CLIENTS];
float t_last_fi_s[NUMBER_SERVERS];
float Servers_cap[NUMBER_SERVERS];
float Clients_cap[NUMBER_CLIENTS];
float Servers_connection_cost[NUMBER_CLIENTS];

int nsamples;
int global_nsamples_c[NUMBER_CLIENTS];
int global_nsamples_s[NUMBER_SERVERS];
 long double mean_c[NUMBER_CLIENTS];
 long double mean_s[NUMBER_SERVERS];
 long double var_c[NUMBER_CLIENTS];
 long double var_s[NUMBER_SERVERS];
 long double oldmean_c[NUMBER_CLIENTS];
 long double oldmean_s[NUMBER_SERVERS];
 long double oldvar_c[NUMBER_CLIENTS];
 long double oldvar_s[NUMBER_SERVERS];

long double mean_clients_flow[NUMBER_CLIENTS];  
long double mean_servers_flow[NUMBER_SERVERS];
long double mean_system_flow;
long double oldmean_clients_flow[NUMBER_CLIENTS];  
long double oldmean_servers_flow[NUMBER_SERVERS];
long double oldmean_system_flow;
//long double var_clients_flow[NUMBER_CLIENTS];  
//long double var_servers_flow[NUMBER_SERVERS];
//long double var_system_flow;

    
long double mean_servers_number_connections[NUMBER_SERVERS]; 
long double mean_clients_number_connections[NUMBER_CLIENTS];    
long double oldmean_servers_number_connections[NUMBER_SERVERS]; 
long double oldmean_clients_number_connections[NUMBER_CLIENTS]; 
//long double var_servers_number_connections[NUMBER_SERVERS]; 
//long double var_clients_number_connections[NUMBER_CLIENTS];

struct event{
       float time; //tempo de ocorrencia do evento
       int event_id; //tipo de evento
       int client_id; //id do cliente que gerou evento
       int server_id; //no caso de conexao/desconexao, com qual servidor
       struct event *next; //ponteiro para proximo evento
       struct event *prev; //ponteiro para evento anterior
       
       bool operator <(const event& rhs)const{
                 return time < rhs.time;} //caso seja necessario ordenar a lista de eventos usando a funcao sort, exemplo: sort(events,events+number_events)
       
       };
       
       
struct client{
       int client_id; //id do cliente
       int flow_event_alloc; //flag para saber se ha um evento de fluxo alocado na lista de eventos
       long double band_filled; //flag para saber se banda do cliente esta sendo toda utilizada
       float last_connection_scheduled; //variavel auxiliar, guarda tempo do evento de conexao/desconexao gerado que ocorrera mais no futuro
       struct event* exit_event; //ponteiro para o evento de saida desse cliente
       struct event* flow_event; //ponteiro para o evento de mudanca de fluxo desse cliente - nem sempre esta alocado em uma lista
       int own_connections[NUMBER_SERVERS]; //lista de servidores que o cliente esta conectado/pediu conexao (conexao pode ou nao jÃ¡ estar estabelecida)
       int own_connections_old[NUMBER_SERVERS]; //idem, vetor auxiliar
       long double own_flow[NUMBER_SERVERS];   //fluxo observado pelo cliente vindo de cada servidor 
           
       };
//funcoes que processam os eventos - retornam 1 se ocorrer um erro
int process_exit_event(event *, event *, event *, client[NUMBER_CLIENTS], int[][NUMBER_CLIENTS], int[NUMBER_CLIENTS], int[NUMBER_SERVERS], int *, int * ,int, int);
int process_arrival_event(event *, event *, client[NUMBER_CLIENTS], int[][KNOWLEDGE], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], int[NUMBER_CLIENTS], int[NUMBER_SERVERS], int *, int *, int, int, int * );
int process_flow_event(event *, event *, client[NUMBER_CLIENTS], int[][KNOWLEDGE], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], int, int * );
int process_connection_event(event *, event *, event *, client[NUMBER_CLIENTS], int[][NUMBER_CLIENTS], int[NUMBER_SERVERS], int[NUMBER_CLIENTS], float[][NUMBER_CLIENTS], long double[NUMBER_CLIENTS], long double[NUMBER_SERVERS], long double[][NUMBER_CLIENTS], int[NUMBER_CLIENTS], int[NUMBER_SERVERS], int , int, int, int * , int );

int erase  ( event *, event*); //remove evento da lista e desaloca evento da memoria 
void remove_event(event *); //somente remove evento da lista - nÃ£o desaloca da memoria
void recoloca  ( event *, int, float,int, int,  event *); //insere evento na lista no devido lugar - ordenado em ordem crescente no tempo

event * insere ( event *, int, float, int, int); //cria e insere evento numa lista, retorna endereco da estrutura(evento)
void busca ( event *, float , int, int, event **,  event **); //busca evento na lista de eventos: fluxo, conexao, desconexao
void busca2 ( event *, int ,  event **,  event **); //busca evento na lista de saidas
void busca_tempo ( event *, float , event **, event **);

void listar (struct event *); //lista eventos de uma lista
void print_system_state(int, int[NUMBER_CLIENTS], int[NUMBER_SERVERS],long double[NUMBER_CLIENTS], long double[NUMBER_SERVERS]);

double gera_aleatorio(double parametro); //gera numero aleatorio com distribuicao exponencial
int algoritmo_overhead(client[NUMBER_CLIENTS], int, int[][KNOWLEDGE], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], int );  //seleciona servidores, retorna 1 se erro
int algoritmo_connlimit(client[NUMBER_CLIENTS], int, int [][KNOWLEDGE], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], int );  //seleciona servidores, retorna 1 se erro
int alocacao(client[NUMBER_CLIENTS], int[NUMBER_CLIENTS], int[NUMBER_SERVERS], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], long double[NUMBER_CLIENTS], long double[NUMBER_SERVERS], int[NUMBER_CLIENTS], int[NUMBER_SERVERS], int ); //aloca banda dos servidores para os clientes, retorna numero de controle, 1 se erro


struct event * malloc_flow(int); //aloca evento de fluxo na memoria, passa o endereco


struct event * get_next(event *); //retorna endereco do proximo evento na lista
float get_time(event *); //retorna tempo do evento
int get_client_id(event *); //retorna id do cliente envolvido com o  evento
int get_server_id(event *); //retorna id do servidor envolvido com o evento
int get_event_id(event *); //retorna o tipo do evento

void set_servers_capacite_random();
void set_clients_capacite_random();
void set_servers_capacite();
void set_clients_capacite();
int main(int argc, char *argv[]) {
    
        
    int i, j, k;
    int number_events, number, servers_up, clients_up;
    int event_id, client_id, server_id;
    double when, exp_paramenter;

    struct event **ant, **pont;
    
    struct client clients_vector[NUMBER_CLIENTS]; //vetor de instancias de clientes
    struct event *exits; //lista de saidas, duplamente enc. ordenada no tempo
    struct event *events; //lista de eventos: des/conexao, fluxo, duplamente enc. ordenada no tempo
    struct event *temp_event_exits, *temp_event_events; //ponteiro para primeiro evento a ser processados de cada lista
    
    int clients_state[NUMBER_CLIENTS];
    int servers_state[NUMBER_SERVERS];
	int knowledge_temp[NUMBER_SERVERS];
	int knowledge_servers[NUMBER_CLIENTS][KNOWLEDGE];
    int connections[NUMBER_SERVERS][NUMBER_CLIENTS]; //1 se servidor i estiver contado a cliente j
    int servers_number_connections[NUMBER_SERVERS]; //numero de conexoes estabelecidas por cada servidor
    int clients_number_connections[NUMBER_CLIENTS]; //numero de conexoes estabelecidas por cada cliente
    
    char randstate[256];
        
    float flow[NUMBER_SERVERS][NUMBER_CLIENTS]; //(X) fluxo atual do servidor i para cliente j
    long double data[NUMBER_SERVERS][NUMBER_CLIENTS]; //(B) total de dados transmitidos ate o tempo t. do servidor i para cliente j
    long double clients_flow[NUMBER_CLIENTS]; //fluxo real total alocado 
    long double servers_flow[NUMBER_SERVERS]; 
    

    long double ic_clients_flow[NUMBER_CLIENTS];  
    long double dp_clients_flow[NUMBER_CLIENTS]; 
    long double ic_servers_flow[NUMBER_SERVERS];
    long double dp_servers_flow[NUMBER_SERVERS];
    long double ic_system_flow;  
    long double dp_system_flow;
     
    long double  ic_servers_number_connections[NUMBER_SERVERS];
    long double  ic_clients_number_connections[NUMBER_CLIENTS]; 
    long double  dp_servers_number_connections[NUMBER_SERVERS];
    long double  dp_clients_number_connections[NUMBER_CLIENTS]; 
        
    if(initstate(time(NULL), randstate, 256)==NULL) return 1;
    srandom( (int)time(NULL) );

    
    //printf("%d %d %d\n", NUMBER_SERVERS, NUMBER_CLIENTS, CONNECTION_LIMIT);

    
    exits = NULL;
    events = NULL;

    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
    
    //Criando no cabeca para lista de eventos 'events' que contem eventos de fluxo, conexao e desconexao
    if (( events = ( event *) malloc(sizeof( event )) ) == NULL )
    {
        printf("ERROR: Nao foi possivel alocar memoria para events\n");
        return 1;
    }
    events->event_id = -1;
    events->time = 0;
    events->next = NULL;
    events->prev = NULL;
    
    //Criando no cabeca para lista de eventos 'exits' que contem eventos de saida
    if (( exits = ( event *) malloc(sizeof( event)) ) == NULL )
    {
        printf("ERROR: Nao foi possivel alocar memoria para exits\n");
        return 1;
    }
    exits->event_id = -1;
    exits->time = 0;
    exits->next = NULL;
    exits->prev = NULL;    
    

   //Gerando capacidades
   if(RANDOM_CAP) {
	set_servers_capacite_random();     
  	set_clients_capacite_random();}
   else{
	set_servers_capacite();     
  	set_clients_capacite();}
      
    
    for(i=0; i<NUMBER_SERVERS ; i++)
    {       
        servers_number_connections[i]=0;
        servers_state[i]=1;
        servers_flow[i]=0;
		knowledge_temp[i]=NUMBER_CLIENTS;  
        global_nsamples_s[i]=0; 
        for(j=0; j<NUMBER_CLIENTS; j++)
        {
           if(i==0) //para realizar esta operacao apenas uma vez
           {    
    		    //setando custo de conexao
    	        Servers_connection_cost[j] = (((float)(CONNECTION_COST))/100)*Clients_cap[j];
            	if(DEBUG_LEVEL>1) printf("Client %d connection cost: %f\n", j,Servers_connection_cost[j] );

                //setando valores iniciais dos clientes
                clients_vector[j].client_id=j;
                clients_number_connections[j]=0;
                clients_vector[j].band_filled = 0; 
                clients_vector[j].last_connection_scheduled = 0;
		        clients_flow[j] = 0;
  
            }
            global_nsamples_c[j]=0;
           //setando valores iniciais
            flow[i][j] = 0;
            data[i][j] = 0;
            connections[i][j] = 0;
            clients_state[j]=0;
            clients_vector[j].own_flow[i]=0;
            clients_vector[j].own_connections[i]=0;
            clients_vector[j].own_connections_old[i]=0;   
            
 
            mean_c[j]=0;
            mean_s[i]=0;
            var_c[j]=0;
            var_s[i]=0;
            mean_clients_flow[j] =0;  
            mean_servers_flow[i] =0;
            mean_system_flow =0;
            mean_servers_number_connections[i]=0; 
            mean_clients_number_connections[j]=0;  
                  
        }
        
    }
    if(DEBUG_LEVEL>1) printf("\n");

    //INICIANDO CRIACAO E PROCESSAMENTO DE EVENTOS
    
    t = 0;
    t_last = 0;
    t_print = 0;
    t_last_fi = 0;
    number_events = 0;
    nsamples = 0;
    mean_system_flow=0;
    servers_up = NUMBER_SERVERS;

    if(DEBUG_LEVEL>0) printf( "%f BEGIN\n", t);

    //Criando eventos de chegada, saida e fluxo
    for(j=0; j<NUMBER_CLIENTS ; j++)
    {                                
             //criando evento de chegada para cliente no vetor de eventos
            while(true)
            {
                 exp_paramenter = (float)(1/(float)DELTA1);
                 when = t+gera_aleatorio(exp_paramenter);
                 if( when < t )
                 {
                     printf("ERROR: %f ERROR_TIME_EVENT - CLIENT_ARRIVAL_EVENT %d %f\n", t, j, when);
                     return 1;
                     
                 }
                 busca_tempo (events, when, ant, pont);
                 if(*pont==NULL) break;
            }
             insere(events,CLIENT_ARRIVAL_EVENT,when,0,j);

             number_events++;
             if(DEBUG_LEVEL>3) printf("%f PUSH CLIENT_ARRIVAL_EVENT %d %f\n", t, j, when);
             
             //criando evento de saida para cliente no vetor de saidas e passando endereÃ§o para o cliente
             when=INF;
             clients_vector[j].exit_event = insere(exits,CLIENT_EXIT_EVENT,when,0,j); 
             number_events++;
             if(DEBUG_LEVEL>3) printf("%f PUSH CLIENT_EXIT_EVENT %i %f\n", t, j, when);
             
             //criando evento de fluxo para cliente e passando endereÃ§o para o cliente
             when = INF;
             clients_vector[j].flow_event = malloc_flow(j); 
             clients_vector[j].flow_event_alloc=0;

             if(KNOWLEDGE < NUMBER_SERVERS ) {
        			for(i=0; i<KNOWLEDGE ; i++)
        			{
        				while(true)
        				{
        					number = rand() % NUMBER_SERVERS;
        					if (knowledge_temp[number]!=j) break;
        				}
        				knowledge_servers[j][i]=number;
        				knowledge_temp[number] = j;
        			}
             }
             else{
                for(i=0; i<KNOWLEDGE ; i++){
                    knowledge_servers[j][i]=i;

                }


             }
    }    
    
    
    //Processando a lista de eventoS     
    temp_event_events = get_next(events);
    temp_event_exits = get_next(exits);
    
    if(temp_event_events == NULL)
    {
                  printf("ERROR: Nao ha eventos para comecar a simulacao\n");
                  return 1;                           
    }

    t_print++;
    
    while( number_events )
    {

           if(DEBUG_LEVEL>3) printf( "\n%f REMAINING EVENTS TO PROCESS %d\n", t, number_events);
           
           if(DEBUG_LEVEL>3) listar(exits);
           if(DEBUG_LEVEL>3) listar(events);
           if(DEBUG_LEVEL>3) printf("\n");
           
           if(get_time(temp_event_events) >= get_time(temp_event_exits)) //verificando qual evento ocorre primeiro: events ou exits
           {
                //EXIT_EVENT OCORRE PRIMEIRO
                
                t= get_time(temp_event_exits); 
                client_id = get_client_id(temp_event_exits);
                
                process_exit_event(temp_event_exits, exits, events, clients_vector, connections, clients_state, servers_state, &servers_up, &number_events, CLIENT_EXIT_EVENT,client_id );
                
                
           }
           else{
                
               //EVENTO DA LISTA EVENTS OCORRE PRIMEIRO QUE UMA SAIDA 

               t = get_time(temp_event_events);
               event_id = get_event_id(temp_event_events);
               client_id = get_client_id(temp_event_events);
               server_id = get_server_id(temp_event_events);
               
               switch(event_id)
               {
                               case CLIENT_ARRIVAL_EVENT:
                                    
                                    i = process_arrival_event(temp_event_events, events, clients_vector, knowledge_servers, connections, flow, clients_state, servers_state, &servers_up, &number_events, CLIENT_ARRIVAL_EVENT, client_id, &clients_up);
                                    if( i == 1) return 1;
                                    break;

                                case SERVER_ARRIVAL_EVENT:
                                    
                                    i = process_arrival_event(temp_event_events, events, clients_vector, knowledge_servers, connections, flow, clients_state, servers_state, &servers_up, &number_events, SERVER_ARRIVAL_EVENT, server_id, &clients_up );
                                    if( i == 1) return 1;
                                    break;

                                    
                               case FLOW_EVENT:
                                    
                                    i = process_flow_event(temp_event_events, events, clients_vector, knowledge_servers, connections, flow, client_id, &number_events);
                                    if( i == 1) return 1;
                                    break;
                                    
                               case CONNECTION_EVENT:
                                    
                                    i = process_connection_event(temp_event_events, events, exits, clients_vector, connections, servers_number_connections, clients_number_connections, flow, clients_flow, servers_flow, data, clients_state, servers_state, event_id, server_id, client_id, &number_events, clients_up);     
                                    if( i == 1) return 1;
                                    break;
                                    
                               case DISCONNECTION_EVENT:
                                                                        
                                    i = process_connection_event(temp_event_events, events, exits, clients_vector, connections, servers_number_connections, clients_number_connections, flow, clients_flow, servers_flow, data, clients_state, servers_state, event_id, server_id, client_id, &number_events,clients_up);     
                                    if( i == 1) return 1;
                                    break;

                                case SERVER_EXIT_EVENT:
                                                                        
                                    i = process_exit_event(temp_event_exits, exits, events, clients_vector, connections, clients_state, servers_state, &servers_up, &number_events, SERVER_EXIT_EVENT, server_id );
                                    if( i == 1) return 1;
                                    break;

               
               }

        }//END else event time < exit time
    
       //atualizando evento de saida mais proximo               
       temp_event_exits = get_next(exits);
       temp_event_events = get_next(events);
       
    }//END while - ENQUANTO HOUVER EVENTOS 
    
    

	for(j=0;j<NUMBER_CLIENTS;j++)
    {
        //dp_clients_number_connections[j] = sqrt(var_clients_number_connections[j]);
        //ic_clients_number_connections[j] = ((1.96*dp_clients_number_connections[j])/sqrt(nsamples));
        //dp_clients_flow[j] = sqrt(var_clients_flow[j]);
        //ic_clients_flow[j] = ((1.96*dp_clients_flow[j])/sqrt(nsamples));

		mean_clients_number_connections[j] +=  oldmean_clients_number_connections[j]*(t-t_last_fi);
		mean_clients_flow[j] +=  oldmean_clients_flow[j]*(t-t_last_fi);
		mean_clients_number_connections[j] = mean_clients_number_connections[j]/t;        
		mean_clients_flow[j] = mean_clients_flow[j]/t;
        printf( "CLIENT_CONNECTION_STATISTIC %d %Lf\n", j, mean_clients_number_connections[j]);
        printf( "CLIENT_FLOW_STATISTIC %d %Lf\n", j, mean_clients_flow[j]);
        
        
        if(DEBUG_LEVEL>2) {
			mean_c[j]+=oldmean_c[j]*(t-t_last_fi);
			mean_c[j] = mean_c[j]/t;
			
        	var_c[j]+=oldvar_c[j]*(t-t_last_fi);
			var_c[j] = var_c[j]/t;
				
			printf( "CLIENT_FLOW_MAT %d %Lf %Lf\n", j, mean_c[j], var_c[j]);
			}
    }
	for(i=0; i< NUMBER_SERVERS ; i++)
	{
        //dp_servers_number_connections[i] = sqrt(var_servers_number_connections[i]);
        //ic_servers_number_connections[i] = ((1.96*dp_servers_number_connections[i])/sqrt(nsamples));
        //dp_servers_flow[i] = sqrt(var_servers_flow[i]);
        //ic_servers_flow[i] = ((1.96*dp_servers_flow[i])/sqrt(nsamples));
        
		mean_servers_number_connections[i] += oldmean_servers_number_connections[i]*(t-t_last_fi);
        mean_servers_flow[i] += oldmean_servers_flow[i]*(t-t_last_fi);
		mean_servers_number_connections[i] = mean_servers_number_connections[i]/t;        
		mean_servers_flow[i] = mean_servers_flow[i]/t;
		
		printf( "SERVER_CONNECTION_STATISTIC %d %Lf\n", i, mean_servers_number_connections[i]);
		printf( "SERVER_FLOW_STATISTIC %d %Lf\n", i, mean_servers_flow[i]);
        
        
        if(DEBUG_LEVEL>2) {
			mean_s[i]+=oldmean_s[i]*(t-t_last_fi);
			mean_s[i] = mean_s[i]/t;
			
			var_s[i]+=oldvar_s[i]*(t-t_last_fi);
			var_s[i] = var_s[i]/t;
			printf( "SERVER_FLOW_MAT %d %Lf %Lf\n",  i, mean_s[i], var_s[i]);			
		}
	}
	
    //dp_system_flow = sqrt(var_system_flow);
    //ic_system_flow = ((1.96*dp_system_flow)/sqrt(nsamples));
    mean_system_flow += oldmean_system_flow*(t-t_last_fi);
	mean_system_flow = mean_system_flow/t;
    printf( "SYSTEM_FLOW_STATISTIC 0 %Lf\n", mean_system_flow);        

    
    if(DEBUG_LEVEL>0) printf( "%f END", t);
    return 0;
    
}//END main
//gera numero aleatorio com distribuicao exponencial 
double gera_aleatorio(double parametro)
{
      
      double number;
      
      while(true)
      {
          number = (double)random()/RAND_MAX;
          if(number != 0) break;
      }

      return (double)log(1-number)/(-parametro);
}
//remove evento da lista de eventos, mas nao desaloca a memoria
void remove_event(event *event_toremove)
{
     struct event *prev, *next;
     
     prev = event_toremove->prev;
     next = event_toremove->next;
     
     prev->next=next;
     if(next != NULL) next->prev=prev;
     
}

//reinsere evento na lista de eventos
void recoloca ( event *list, int event_id, float time, int server_id, int client_id, event *fix_event ) 
{ 
    struct event *pont, *ant;
    
    ant = list;
    pont = list->next;
    
    //procura posicao correta
    while(pont)
    {
         if( pont->time < time ) 
         {   ant=pont;
             pont=pont->next;}
         else{ 
              break;} 
                
    }
    
    fix_event->time = time;
    
    fix_event->next = (ant)->next;
    fix_event->prev = (ant);
    (ant)->next = fix_event;
    if((fix_event->next) != NULL) (fix_event->next)->prev = fix_event;
           
}
  
 void busca_tempo ( event *events, float when, event **ant, event **ponte) {

    struct event *ptr;

    *ant = events; // aponta no anterior 
    *ponte = NULL;  // aponta no procurado, se nao achar retorna nulo 
    ptr = events->next; // aponta para candidato 

    while (ptr)
    {  
        if ((ptr->time) < when) 
        { // ainda nao chegou no no
            *ant = ptr;
             ptr = ptr->next;
        }
        else 
        {       
               //a principio enocontramos o evento, ou no caso de insercao, a posicao a ser inserido       
               if (ptr->time == when) 
               { 
                
                      *ponte = ptr; // achou evento
                      
               }
               //CASO DE INSERCAO: (ou evento deveria estar na lista e nao esta - ERROR)
               ptr = NULL; //esta deveria ser a posicao do evento
               //no caso de insercao, "ant" fica apontando para evento que ira antecer novo evento
               //laco eh quebrado aqui pois ptr recebe valor null
               
        }
    }
}

// funcao usada para busca de eventos lista "events" - ou para encontrar posicao de insercao
void busca ( event *events, float when, int server_id, int client_id, event **ant, event **ponte) {

    struct event *ptr;

    *ant = events; // aponta no anterior 
	*ponte = NULL;  // aponta no procurado, se nao achar retorna nulo 
    ptr = events->next; // aponta para candidato 

    while (ptr)
    {  
        if ((ptr->time) < when) 
        { // ainda nao chegou no no
            *ant = ptr;
             ptr = ptr->next;
        }
        else 
        {       
               //a principio enocontramos o evento, ou no caso de insercao, a posicao a ser inserido       
               if (ptr->time == when) 
               {
                  if((client_id == ptr->client_id) && (server_id == ptr->server_id))
                  {
                      *ponte = ptr; // achou evento
                      break;
                  }
                  else {
                       //temos um evento com mesmo horario, mas nao eh o evento que buscamos
                       *ant = ptr;
                       ptr = ptr->next;
                       continue;}
               }
               //CASO DE INSERCAO: (ou evento deveria estar na lista e nao esta - ERROR)
               ptr = NULL; //esta deveria ser a posicao do evento
               //no caso de insercao, "ant" fica apontando para evento que ira antecer novo evento
               //laco eh quebrado aqui pois ptr recebe valor null
               
        }
    }
}

// funcao usada para busca de eventos lista "exits" - ou para encontrar posicao de insercao
//equivalente a funcao "busca"
void busca2 ( event *events, int who,  event **ant, event **ponte) {

    event *ptr;

    *ant = events; 
	  *ponte = NULL;  
    ptr = events->next; 

    while (ptr)
    {  
        if ((ptr->client_id) != who) 
        { // ainda nao chegou no no 
            *ant = ptr;
             ptr = ptr->next;
        }
        else 
        {             
               if (ptr->client_id == who) 
               {
                  *ponte = ptr; // achou 
                  break;
               }
               ptr = NULL; // nao estava na lista - ou: posicao para insercao, "ant" aponta para evento que o antecede
               //laco eh rompido aqui
        }
    }
}
    
event * insere ( event *list, int event_id, float time, int server_id, int client_id ) 
{
    
    struct event *temp_event1, 	// ponteiro para o novo no a inserir
    	**ant,      // ponteiro para ponteiro na lista - vai apontar para evento que ja existe na lista e antecede imediatamente novo evento 
        **pont;     // ponteiro para ponteiro, auxilia busca
    
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
    temp_event1 = (event *)malloc(1*sizeof(event));
        
        
    if(event_id==CLIENT_EXIT_EVENT){
                             busca2 (list, client_id, ant, pont);}
    else{
         busca (list, time, server_id, client_id, ant, pont);} 

    temp_event1->event_id = event_id;
    temp_event1->client_id = client_id;
    temp_event1->server_id = server_id;
    temp_event1->time = time;
    temp_event1->next = (*ant)->next;
    temp_event1->prev = (*ant);
    (*ant)->next = temp_event1;
    if(temp_event1->next!=NULL){
        (temp_event1->next)->prev = temp_event1;}
        
     return    temp_event1;        
}

//remove e desaloca evento de listas - eventos de fluxo somente sao removidos da lista
int erase ( event *list, event * event_remove )
{     

     
    int event_id, client_id, server_id;
    struct event *pont, *ant;
    float time;
    
    event_id = event_remove->event_id;
    client_id = event_remove->client_id;
    server_id = event_remove->server_id;
    time = event_remove->time;

    ant = list;
    pont = list->next;
    
    //buscando evento utilizando posicao da memoria como base de comparacao
    while(pont)
    {
         if( pont == event_remove ) 
         {break;}   
         else
         {   ant=pont;
             pont=pont->next;}      
                
    }
    
    //arrumando ponteiros
    if (pont) {
        (ant)->next = (pont)->next;
        if(((pont)->next) != NULL) ((pont)->next)->prev = (pont)->prev;
        if(event_id!=FLOW_EVENT) free(pont);
     }
     else 
     {
          
           if(event_id == CLIENT_EXIT_EVENT){
           printf("DELETE_ERROR CLIENT_EXIT_EVENT | Time: %f | Client id: %d \n", time, client_id);}
           else{
           if(event_id == SERVER_EXIT_EVENT){
           printf("DELETE_ERROR SERVER_EXIT_EVENT | Time: %f | Server id: %d \n", time, server_id);} 
           if(event_id == FLOW_EVENT){
           printf("DELETE_ERROR FLOW_EVENT | Time: %f | Server id: %d | Client id: %d \n", time, server_id, client_id);}
           else if(event_id == DISCONNECTION_EVENT){
           printf("DELETE_ERROR DISCONNECTION_EVENT | Time: %f | Server id: %d | Client id: %d \n", time, server_id, client_id);}
           else if(event_id == CONNECTION_EVENT){
           printf("DELETE_ERROR CONNECTION_EVENT | Time: %f | Server id: %d | Client id: %d \n", time, server_id, client_id);
           if(event_id == SERVER_ARRIVAL_EVENT){
           printf("DELETE_ERROR SERVER_ARRIVAL_EVENT | Time: %f | Server id: %d \n", time, server_id);}
           if(event_id == CLIENT_ARRIVAL_EVENT){
           printf("DELETE_ERROR CLIENT_ARRIVAL_EVENT | Time: %f | Server id: %d \n", time, client_id);}
         }
           }
	   return 1;
     }
     return 0;
}    

void listar (struct event *ptlista) {

     
     struct event *pont;

     pont = ptlista->next;
     
     while (pont) 
     {
           if(pont->event_id == CLIENT_EXIT_EVENT){
           printf("QUEUE_CLIENT_EXIT_EVENT          | Time: %f | Client id: %d \n", pont->time, pont->client_id);}
           else{
          if(pont->event_id == CLIENT_ARRIVAL_EVENT){
           printf("QUEUE_CLIENT_ARRIVAL_EVENT       | Time: %f | Client id: %d \n", pont->time, pont->client_id);}
           if(pont->event_id == SERVER_ARRIVAL_EVENT){
           printf("QUEUE_SERVER_ARRIVAL_EVENT       | Time: %f | Server id: %d \n", pont->time, pont->server_id);}
           if(pont->event_id == CLIENT_ARRIVAL_EVENT){
           printf("QUEUE_SERVER_EXIT_EVENT       | Time: %f | Server id: %d \n", pont->time, pont->server_id);}
           if(pont->event_id == FLOW_EVENT){
           printf("QUEUE_FLOW_EVENT          | Time: %f | Client id: %d \n", pont->time, pont->client_id);}
           else if(pont->event_id == DISCONNECTION_EVENT){
           printf("QUEUE_DISCONNECTION_EVENT | Time: %f | Server id: %d | Client id: %d \n", pont->time, pont->server_id, pont->client_id);}
           else if(pont->event_id == CONNECTION_EVENT){
           printf("QUEUE_CONNECTION_EVENT    | Time: %f | Server id: %d | Client id: %d \n", pont->time, pont->server_id, pont->client_id);}
           }
           pont = pont->next;      
     }
     
}

void print_system_state(int t_print, int clients_number_connections[NUMBER_CLIENTS], int servers_number_connections[NUMBER_SERVERS], long double client_flow[NUMBER_CLIENTS], long double server_flow[NUMBER_SERVERS])
{

	int i, j;
	long double flow_accumulator, system_total_flow;

	system_total_flow = 0;

	for(j=0;j<NUMBER_CLIENTS;j++)
	{
		 system_total_flow += client_flow[j];
 		 printf("%d CLIENT_STATE %d %d %Lf\n", t_print, j, clients_number_connections[j], client_flow[j]);
	}        
	for(j=0;j<NUMBER_SERVERS;j++)
	{
		 //system_total_flow += client_flow[j];
 		 printf("%d SERVER_STATE %d %d %Lf\n", t_print, j, servers_number_connections[j], server_flow[j]);
	}
	printf("%d SYSTEM_STATE %Lf\n", t_print, system_total_flow );

}
struct event * malloc_flow(int client_id)
{
           struct event *temp_event1; 	// ponteiro para o novo no a inserir
    

    temp_event1 = (event *)malloc(1*sizeof(event));
        
    temp_event1->event_id = FLOW_EVENT;
    temp_event1->client_id = client_id;
    temp_event1->server_id = 0;
    temp_event1->time = INF;
    temp_event1->next = NULL;
    temp_event1->prev = NULL;
    
    //retorna endereco do evento de fluxo alocado    
    return    temp_event1;       
       
       
}

struct event * get_next(event *list)
{
       return list->next;
}

float get_time(event *temp)
{
      if(temp == NULL) return INF;
      return temp->time;
}
                        
int get_client_id(event *temp)
{
      
      return temp->client_id;
}

int get_server_id(event *temp)
{
      
      return temp->server_id;
}

int get_event_id(event *temp)
{
      
      return temp->event_id;
}

       
void set_servers_capacite_random()
{
       // CODIGO DE DIVISAO DE GRUPOS:
       // 0: TODOS COM CAPACIDADES DISTINTAS
       // 1: TODOS COM A MESMA CAPACIDADE
       // 2: 1/2 - 1/2
       // 3: 1/3 - 1/3 - 1/3
       // 4: 1/3 - 2/3
       // 5: 1/4 - 1/4 - 1/4 - 1/4
       // 6: 1/4 - 1/4 - 1/2
       // 7: 1/4 - 3/4
       
       int i, j;
       float cap1, cap2, cap3, cap4;
       float num_servers;
       
       num_servers = (float)NUMBER_SERVERS;
       
       switch(SERVERS_BAND_DIVISION)
       {
                      case -1:
                           // -1: TODOS COM CAPACIDADES IGUAIS - CLIENTES E SERVIDORES
                            while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_SERVER) && (cap1 < MAX_CAP_SERVER) && (cap1 > MIN_CAP_CLIENT) && (cap1 < MAX_CAP_CLIENT)) break;
                            }
                            for(i=0; i<NUMBER_SERVERS ; i++)
                            {    
                                    Servers_cap[i] = cap1;
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            break;
                      
                      case 0:
                           // 0: TODOS COM CAPACIDADES DISTINTAS
                            for(i=0; i<NUMBER_SERVERS ; i++)
                            {    
                                    while(true)
                                    {          
                                               Servers_cap[i] = rand();
                                               if((Servers_cap[i] > MIN_CAP_SERVER) && (Servers_cap[i] < MAX_CAP_SERVER) ) break;
                                    }
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i,Servers_cap[i] );
           
                            }
                            break;
                      
                      case 1:
                           // 1: TODOS COM A MESMA CAPACIDADE
                            while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_SERVER) && (cap1 < MAX_CAP_SERVER) ) break;
                            }
                            for(i=0; i<NUMBER_SERVERS ; i++)
                            {    
                                    Servers_cap[i] = cap1;
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }                           
                           break;
                      
                      case 2:
                           // 2: 1/2 - 1/2
                            while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_SERVER) && (cap1 < MAX_CAP_SERVER) ) break;
                            }
                            while(true)
                            {          
                                       cap2 = rand();
                                       if((cap2 > MIN_CAP_SERVER) && (cap2 < MAX_CAP_SERVER) && (cap1 != cap2)) break;
                            }
                            
                            for(i=0; i<(int)floor(num_servers/2) ; i++)
                            {    
                                    Servers_cap[i] = cap1;
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            for(i=(int)floor(num_servers/2); i<NUMBER_SERVERS ; i++)
                            {    
                                    Servers_cap[i] = cap2;
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            
                           break;
                      
                      case 3:
                           // 3: 1/3 - 1/3 - 1/3
                           while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_SERVER) && (cap1 < MAX_CAP_SERVER) ) break;
                            }
                            while(true)
                            {          
                                       cap2 = rand();
                                       if((cap2 > MIN_CAP_SERVER) && (cap2 < MAX_CAP_SERVER) && (cap1 != cap2)) break;
                            }
                            while(true)
                            {          
                                       cap3 = rand();
                                       if((cap2 > MIN_CAP_SERVER) && (cap2 < MAX_CAP_SERVER) && (cap1 != cap3)  && (cap2 != cap3)) break;
                            }
                            if( ((NUMBER_SERVERS%3) == 0 ) || ((NUMBER_SERVERS%3) == 1 ))
                            {
                                for(i=0; i<(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=(int)floor(num_servers/3); i<2*(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=2*(int)floor(num_servers/3); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = cap3;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                           }
                           else{
                                for(i=0; i<(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                
                                j = NUMBER_SERVERS - (int)floor(num_servers/3);
                                j = (int)j/2;
                                
                                for(i=(int)floor(num_servers/3); i < ((int)floor(num_servers/3)+ j) ; i++)
                                {    
                                        Servers_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=((int)floor(num_servers/3)+ j); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = cap3;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
   
                           } 
                           break;
                      
                      case 4:
                           // 4: 1/3 - 2/3
                            while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_SERVER) && (cap1 < MAX_CAP_SERVER) ) break;
                            }
                            while(true)
                            {          
                                       cap2 = rand();
                                       if((cap2 > MIN_CAP_SERVER) && (cap2 < MAX_CAP_SERVER) && (cap1 != cap2)) break;
                            }
                            if(NUMBER_SERVERS%3 == 0 || NUMBER_SERVERS%3 ==1)
                            {
                                for(i=0; i<(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=(int)floor(num_servers/3); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                           }
                           else{
                                for(i=0; i<(int)ceil(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=(int)ceil(num_servers/3); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                           }                                
                                
                           
                           break;
                      /*
                      case 5:
                           // 5: 1/4 - 1/4 - 1/4 - 1/4
                            while(true)
                            {          
                                       cap1 = floor(gera_aleatorio(DELTA4));
                                       if((cap1 > MIN_CAP_SERVER) && (cap1 < MAX_CAP_SERVER) ) break;
                            }
                            while(true)
                            {          
                                       cap2 = floor(gera_aleatorio(DELTA4));
                                       if((cap2 > MIN_CAP_SERVER) && (cap2 < MAX_CAP_SERVER) && (cap1 != cap2)) break;
                            }
                            while(true)
                            {          
                                       cap3 = floor(gera_aleatorio(DELTA4));
                                       if((cap3 > MIN_CAP_SERVER) && (cap3 < MAX_CAP_SERVER) && (cap3 != cap1) && (cap3 != cap2)) break;
                            }
                            while(true)
                            {          
                                       cap4 = floor(gera_aleatorio(DELTA4));
                                       if((cap4 > MIN_CAP_SERVER) && (cap4 < MAX_CAP_SERVER) && (cap4 != cap1) && (cap4 != cap2) && (cap4 != cap3)) break;
                            }
                            
                            for(i=0; i<(int)floor(num_servers/4) ; i++)
                            {    
                                    Servers_cap[i] = cap1;
                                    if(DEBUG_LEVEL>2) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            for(i=(int)floor(num_servers/4); i<(int)floor(num_servers/2) ; i++)
                            {    
                                    Servers_cap[i] = cap2;
                                    if(DEBUG_LEVEL>2) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            for(i=(int)floor(num_servers/2); i<3*(int)floor(num_servers/4) ; i++)
                            {    
                                    Servers_cap[i] = cap1;
                                    if(DEBUG_LEVEL>2) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            for(i= 3*(int)floor(num_servers/4) ; i<NUMBER_SERVERS ; i++)
                            {    
                                    Servers_cap[i] = cap2;
                                    if(DEBUG_LEVEL>2) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            
                           break;
                      
                      case 6:
                           // 6: 1/4 - 1/4 - 1/2
                           break;
                      
                      case 7:
                           // 7: 1/4 - 3/4
                           break;*/
                      
       }  
       
       if(DEBUG_LEVEL>1) printf("\n" ); 
     
}

void set_clients_capacite_random()
{
       // CODIGO DE DIVISAO DE GRUPOS:
       // 0: TODOS COM CAPACIDADES DISTINTAS
       // 1: TODOS COM A MESMA CAPACIDADE
       // 2: 1/2 - 1/2
       // 3: 1/3 - 1/3 - 1/3
       // 4: 1/3 - 2/3
       // 5: 1/4 - 1/4 - 1/4 - 1/4
       // 6: 1/4 - 1/4 - 1/2
       // 7: 1/4 - 3/4  
              int i, j;
       float cap1, cap2, cap3, cap4;
       float num_clients;
       
       num_clients = (float)NUMBER_CLIENTS;
       
       switch(CLIENTS_BAND_DIVISION)
       {
                      case -1:
                           // -1: TODOS COM CAPACIDADES IGUAIS - CLIENTES E SERVIDORES
                            for(i=0; i<NUMBER_CLIENTS; i++)
                            {    
                                    Clients_cap[i] = Servers_cap[0];
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
                            }
                            break;
                            
                      case 0:
                           // 0: TODOS COM CAPACIDADES DISTINTAS
                            for(i=0; i<NUMBER_CLIENTS ; i++)
                            {    
                                    while(true)
                                    {          
                                               Clients_cap[i] = rand();
                                               if((Clients_cap[i] > MIN_CAP_CLIENT) && (Clients_cap[i] < MAX_CAP_CLIENT) ) break;
                                    }
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i,Clients_cap[i] );
           
                            }
                            break;
                      
                      case 1:
                           // 1: TODOS COM A MESMA CAPACIDADE
                            while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_CLIENT) && (cap1 < MAX_CAP_CLIENT) ) break;
                            }
                            for(i=0; i<NUMBER_CLIENTS ; i++)
                            {    
                                    Clients_cap[i] = cap1;
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
                            }                           
                           break;
                      
                      case 2:
                           // 2: 1/2 - 1/2
                            while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_CLIENT) && (cap1 < MAX_CAP_CLIENT) ) break;
                            }
                            while(true)
                            {          
                                       cap2 = rand();
                                       if((cap2 > MIN_CAP_CLIENT) && (cap2 < MAX_CAP_CLIENT) && (cap1 != cap2)) break;
                            }
                            
                            for(i=0; i<(int)floor(num_clients/2) ; i++)
                            {    
                                    Clients_cap[i] = cap1;
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
                            }
                            for(i=(int)floor(num_clients/2); i<NUMBER_CLIENTS ; i++)
                            {    
                                    Clients_cap[i] = cap2;
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
                            }
                            
                           break;
                      
                      case 3:
                           // 3: 1/3 - 1/3 - 1/3
                           while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_CLIENT) && (cap1 < MAX_CAP_CLIENT) ) break;
                            }
                            while(true)
                            {          
                                       cap2 = rand();
                                       if((cap2 > MIN_CAP_CLIENT) && (cap2 < MAX_CAP_CLIENT) && (cap1 != cap2)) break;
                            }
                            while(true)
                            {          
                                       cap3 = rand();
                                       if((cap2 > MIN_CAP_CLIENT) && (cap2 < MAX_CAP_CLIENT) && (cap1 != cap3)  && (cap2 != cap3)) break;
                            }
                            if( ((NUMBER_CLIENTS%3) == 0 ) || ((NUMBER_CLIENTS%3) == 1 ))
                            {
                                for(i=0; i<(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=(int)floor(num_clients/3); i<2*(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=2*(int)floor(num_clients/3); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = cap3;
                                        if(DEBUG_LEVEL>1) printf("client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                           }
                           else{
                                for(i=0; i<(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                
                                j = NUMBER_CLIENTS - (int)floor(num_clients/3);
                                j = (int)j/2;
                                
                                for(i=(int)floor(num_clients/3); i < ((int)floor(num_clients/3)+ j) ; i++)
                                {    
                                        Clients_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=((int)floor(num_clients/3)+ j); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = cap3;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
   
                           } 
                           break;
                      
                      case 4:
                           // 4: 1/3 - 2/3
                            while(true)
                            {          
                                       cap1 = rand();
                                       if((cap1 > MIN_CAP_CLIENT) && (cap1 < MAX_CAP_CLIENT) ) break;
                            }
                            while(true)
                            {          
                                       cap2 = rand();
                                       if((cap2 > MIN_CAP_CLIENT) && (cap2 < MAX_CAP_CLIENT) && (cap1 != cap2)) break;
                            }
                            
                            if(NUMBER_CLIENTS%3 == 0 || NUMBER_CLIENTS%3==1)
                            {
                                for(i=0; i<(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=(int)floor(num_clients/3); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                            }
                            else{
                                for(i=0; i<(int)ceil(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = cap1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=(int)ceil(num_clients/3); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = cap2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                 
                                 
                            }
                           
                           break;
                      
       }  
       
       if(DEBUG_LEVEL>1) printf("\n" ); 
    
}

void set_servers_capacite()
{
       // CODIGO DE DIVISAO DE GRUPOS:
       // 0: TODOS COM CAPACIDADES DISTINTAS
       // 1: TODOS COM A MESMA CAPACIDADE
       // 2: 1/2 - 1/2
       // 3: 1/3 - 1/3 - 1/3
       // 4: 1/3 - 2/3
       // 5: 1/4 - 1/4 - 1/4 - 1/4
       // 6: 1/4 - 1/4 - 1/2
       // 7: 1/4 - 3/4
       
       int i, j;
       float cap1, cap2, cap3, cap4;
       float num_servers;
       
       num_servers = (float)NUMBER_SERVERS;


       switch(SERVERS_BAND_DIVISION)
       {                      
                      case 0:
                           // 0: TODOS COM CAPACIDADES DISTINTAS
                            for(i=0; i<NUMBER_SERVERS ; i++)
                            {    
                                    while(true)
                                    {          
                                               Servers_cap[i] = rand();
                                               if((Servers_cap[i] > MIN_CAP_SERVER) && (Servers_cap[i] < MAX_CAP_SERVER) ) break;
                                    }
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i,Servers_cap[i] );
           
                            }
                            break;
                      
                      case 1:
                           // 1: TODOS COM A MESMA CAPACIDADE
                            for(i=0; i<NUMBER_SERVERS ; i++)
                            {    
                                    Servers_cap[i] = CAP_SERVER_1;
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }                           
                           break;
                      
                      case 2:
                           // 2: 1/2 - 1/2
                            for(i=0; i<(int)floor(num_servers/2) ; i++)
                            {    
                                    Servers_cap[i] = CAP_SERVER_1;
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            for(i=(int)floor(num_servers/2); i<NUMBER_SERVERS ; i++)
                            {    
                                    Servers_cap[i] = CAP_SERVER_2;
                                    if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
                            }
                            
                           break;
                      
                      case 3:
                           // 3: 1/3 - 1/3 - 1/3
                            if( ((NUMBER_SERVERS%3) == 0 ) || ((NUMBER_SERVERS%3) == 1 ))
                            {
                                for(i=0; i<(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=(int)floor(num_servers/3); i<2*(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=2*(int)floor(num_servers/3); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_3;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                           }
                           else{
                                for(i=0; i<(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                
                                j = NUMBER_SERVERS - (int)floor(num_servers/3);
                                j = (int)j/2;
                                
                                for(i=(int)floor(num_servers/3); i < ((int)floor(num_servers/3)+ j) ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=((int)floor(num_servers/3)+ j); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_3;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
   
                           } 
                           break;
                      
                      case 4:
                           // 4: 1/3 - 2/3
                            if(NUMBER_SERVERS%3 == 0 || NUMBER_SERVERS%3 ==1)
                            {
                                for(i=0; i<(int)floor(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=(int)floor(num_servers/3); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                           }
                           else{
                                for(i=0; i<(int)ceil(num_servers/3) ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_1;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                                for(i=(int)ceil(num_servers/3); i<NUMBER_SERVERS ; i++)
                                {    
                                        Servers_cap[i] = CAP_SERVER_2;
                                        if(DEBUG_LEVEL>1) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
               
                                }
                           }                                
                                
                           
                           break;
                      
       }  
       
       if(DEBUG_LEVEL>1) printf("\n" ); 
     
}

void set_clients_capacite()
{
       // CODIGO DE DIVISAO DE GRUPOS:
       // 0: TODOS COM CAPACIDADES DISTINTAS
       // 1: TODOS COM A MESMA CAPACIDADE
       // 2: 1/2 - 1/2
       // 3: 1/3 - 1/3 - 1/3
       // 4: 1/3 - 2/3
       // 5: 1/4 - 1/4 - 1/4 - 1/4
       // 6: 1/4 - 1/4 - 1/2
       // 7: 1/4 - 3/4  
              int i, j;
       float cap1, cap2, cap3, cap4;
       float num_clients;
       
       num_clients = (float)NUMBER_CLIENTS;
       
       switch(CLIENTS_BAND_DIVISION)
       {
                            
                      case 0:
                           // 0: TODOS COM CAPACIDADES DISTINTAS
                            for(i=0; i<NUMBER_CLIENTS ; i++)
                            {    
                                    while(true)
                                    {          
                                               Clients_cap[i] = rand();
                                               if((Clients_cap[i] > MIN_CAP_CLIENT) && (Clients_cap[i] < MAX_CAP_CLIENT) ) break;
                                    }
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i,Clients_cap[i] );
           
                            }
                            break;
                      
                      case 1:
                           // 1: TODOS COM A MESMA CAPACIDADE
                            for(i=0; i<NUMBER_CLIENTS ; i++)
                            {    
                                    Clients_cap[i] = CAP_CLIENT_1;
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
                            }                           
                           break;
                      
                      case 2:
                           // 2: 1/2 - 1/2
                            for(i=0; i<(int)floor(num_clients/2) ; i++)
                            {    
                                    Clients_cap[i] = CAP_CLIENT_1;
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
                            }
                            for(i=(int)floor(num_clients/2); i<NUMBER_CLIENTS ; i++)
                            {    
                                    Clients_cap[i] = CAP_CLIENT_2;
                                    if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
                            }
                            
                           break;
                      
                      case 3:
                           // 3: 1/3 - 1/3 - 1/3
                            if( ((NUMBER_CLIENTS%3) == 0 ) || ((NUMBER_CLIENTS%3) == 1 ))
                            {
                                for(i=0; i<(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=(int)floor(num_clients/3); i<2*(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=2*(int)floor(num_clients/3); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_3;
                                        if(DEBUG_LEVEL>1) printf("client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                           }
                           else{
                                for(i=0; i<(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                
                                j = NUMBER_CLIENTS - (int)floor(num_clients/3);
                                j = (int)j/2;
                                
                                for(i=(int)floor(num_clients/3); i < ((int)floor(num_clients/3)+ j) ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=((int)floor(num_clients/3)+ j); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_3;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
   
                           } 
                           break;
                      
                      case 4:
                           // 4: 1/3 - 2/3
                            if(NUMBER_CLIENTS%3 == 0 || NUMBER_CLIENTS%3==1)
                            {
                                for(i=0; i<(int)floor(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=(int)floor(num_clients/3); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                            }
                            else{
                                for(i=0; i<(int)ceil(num_clients/3) ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_1;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                for(i=(int)ceil(num_clients/3); i<NUMBER_CLIENTS ; i++)
                                {    
                                        Clients_cap[i] = CAP_CLIENT_2;
                                        if(DEBUG_LEVEL>1) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
               
                                }
                                 
                                 
                            }
                           
                           break;
                      
       }  
       
       if(DEBUG_LEVEL>1) printf("\n" ); 
    
}


int alocacao(client clients_vector[NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS],  int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], long double clients_flow[NUMBER_CLIENTS], long double servers_flow[NUMBER_SERVERS], int clients_number_connections[NUMBER_CLIENTS], int servers_number_connections[NUMBER_SERVERS], int clients_up)
{
      
    int i, j, k, keep;  
    int servers_number_connections_temp[NUMBER_SERVERS]; //possui o numero de conexoes que o servidor possui com clientes que nao tem a banda totalmente preenchida ainda
    long double mean_flow;
    long double clients_cap_rem[NUMBER_CLIENTS]; //inicia com o total da capacidade do cliente, Ã© subtraido o custo das conexoes, e a medida que os servidores alocam banda, vai sendo decrementado (indica quanta banda livre o cliente tem para receber mais fluxo)
    long double servers_cap_rem[NUMBER_SERVERS]; //inicia com o total da capacidade do servidor, Ã© subtraido a medida que a banda vai sendo alocada (ou seja, indica quanta banda o servidor ainda possui para distribuir)
    long double clients_cap_filled[NUMBER_CLIENTS]; //inicia zerada e indica quanta banda jÃ¡ foi preenchida, os custos de conexao preenchem a banda
    long double servers_cap_filled[NUMBER_SERVERS]; //indica quanta banda jÃ¡ foi distribuida pelo servidor
    long double servers_flow_test[NUMBER_SERVERS], clients_flow_test[NUMBER_CLIENTS];
    long double system_flow;

     int nsamples_s[NUMBER_SERVERS], nsamples_c[NUMBER_CLIENTS];
     long double oldmean_s_temp[NUMBER_SERVERS], mean_s_temp[NUMBER_SERVERS], var_s_temp[NUMBER_SERVERS];
     long double oldmean_c_temp[NUMBER_CLIENTS], mean_c_temp[NUMBER_CLIENTS], var_c_temp[NUMBER_CLIENTS];                    
                        
    
	
	
    for(i=0;i<NUMBER_SERVERS;i++)
    {
                                 servers_number_connections_temp[i] =  servers_number_connections[i]; //inicialmente todos os clientes conectados aos servidores ainda tem banda disponivel. A medida que a banda dos clientes vai sendo saturada, esta variavel eh decrementada.
                                 servers_cap_rem[i] = Servers_cap[i]; //servidores comecam com toda a capacidade para distribuir
                                 servers_cap_filled[i] = 0;
                                 servers_flow_test[i] = 0;
                                 mean_s_temp[i]=0;
                                 var_s_temp[i]=0;
                                 for(j=0; j< NUMBER_CLIENTS ; j++)
                                 {
                                          nsamples_c[j]=0;
                                          nsamples_s[i]=0;  
                                          flow[i][j]=0;
                                          mean_c_temp[j]=0;
                                          var_c_temp[j]=0;
                                          if(i==0){ //atribuicoes feitas so no primeiro laco
					      					                    clients_flow_test[j]=0;	
                                              clients_cap_filled[j] = 0;
                                              clients_vector[j].band_filled =0; 
                                              clients_cap_rem[j] = Clients_cap[j];
                                          }
                                 
								 		  if(connections[i][j] == 1)
								 		  {
												clients_cap_rem[j] -= Servers_connection_cost[j]; //substraimos da capacidade livre do cliente o custo das conexoes ativas
												clients_cap_filled[j] += Servers_connection_cost[j];
					  			 		  }
    							 }
    }
     
    keep = 1;    
      
    while(keep)
    { 
        //Este loop eh executado atÃ© que a distribuicao do fluxo nao mude mais
        keep = 0;
        
        for(i=0;i<NUMBER_SERVERS;i++) //FOR#1
        {
              if(servers_number_connections_temp[i] == 0) continue; //caso o servidor nao esteja conectado a nenhum cliente ou todos seus clientes ja estejam com a banda saturada, pular para o proximo servidor
              
              mean_flow = floor(servers_cap_rem[i]/servers_number_connections_temp[i]);
              
              if( mean_flow > 1 ) //se o servidor tem banda para distribuir
              {
                  for(j=0; j< NUMBER_CLIENTS ; j++) //FOR#2
                  {                            
                           if( connections[i][j] == 1 ) //se cliente e servidor estao conectados
                           {
                               if(clients_vector[j].band_filled != 1) //se ainda ha banda para preecher nesse cliente
                               {
                                                if(clients_cap_rem[j] > mean_flow ){
                                                             //se o fluxo medio que o servidor esta reservando eh menor que a banda livre do cliente, aloca o fluxo medio
                                                             flow[i][j] += mean_flow;
                                                             
                                                             //mean_flow foi gasto na banda do cliente e do servidor
                                                             servers_cap_rem[i] -= mean_flow;
                                                             clients_cap_rem[j] -= mean_flow;
                                                             
                                                             //banda sendo utilizada pelo cliente ; quanto o servidor ja alocou
                                                             servers_cap_filled[i] += mean_flow;
                                                             clients_cap_filled[j] += mean_flow; }
                                                    else{    
                                                             //neste caso, o cliente nao tem banda suficiente para absorver todo o fluxo que estava disponivel nesta rodada, alocar so ate aonde ele tinha disponivel                                         
                                                             flow[i][j] += clients_cap_rem[j];
                                                             //o servidor sÃ³ gastou parte do fluxo esperado, o cliente nao tem mais capacidade de receber fluxo
                                                             servers_cap_rem[i] -= clients_cap_rem[j];                                                     
                                                             
                                                             //atualizando quanto de banda cliente esta utilizando no total, que eh == a sua capacidade
                                                             //atualizando quanto o servidor ja alocou de banda
                                                             servers_cap_filled[i] += clients_cap_rem[j];
                                                             clients_cap_filled[j] += clients_cap_rem[j];
                                                          
                                                             clients_cap_rem[j] = 0; //o cliente nao tem mais banda livre
                                                             clients_vector[j].band_filled =1; //a banda do cliente esta saturada
                                                             
                                                             //atualizando numero de clientes conectados e com banda disponivel para cada servidor
                                                             for(k=0;k<NUMBER_SERVERS;k++)
                                                             {                            //como o cliente nao tem mais banda livre, nos proximos loops os servidores nao precisarao mais considerar este cliente (nao sera necessario reservar uma fatia da banda livre do servidor nas proximas iteracoes do while)
                                                                                          if(connections[k][j] == 1) servers_number_connections_temp[k]--;
                                                             }
                                                             
                                                             //if(DEBUG_LEVEL>3) printf("%f CLIENT_BAND_FILLED %d (confirmando id) %d (filled =1) %d\n", t, j, clients_vector[j].client_id, clients_vector[j].band_filled);
                                   
                                                         }
     
                                                    //houve alocacao de banda, talvez ainda haja clientes com banda livre e servidores com banda disponivel para alocar
                                                    keep = 1;                   
                                                    if(DEBUG_LEVEL > 4 ) printf( "TEMP_FLOW_ALOCCATED %d %d %Lf\n", i, j, flow[i][j]);                   
                                                         
                               }
                               
                               if( ( (flow[i][j]+Servers_connection_cost[i]) >  ( Clients_cap[j]) )   || ( flow[i][j] >  Servers_cap[i] ) || (servers_cap_filled[i] > Servers_cap[i]) || (clients_cap_filled[j] > Clients_cap[j]) )
                               {
                                   
                                   printf( "ERROR: %f ERROR_FLOW_ALLOCATION %d %d %f %f %Lf\n", t, i, j, Servers_cap[i],  Clients_cap[j], flow[i][j]);
                                   return 1;    
                               }
                           }               
                  }
              }     
        }
        
    }
        
        
    
	for(j=0;j<NUMBER_CLIENTS;j++)
	{
		 for(i=0; i< NUMBER_SERVERS ; i++)
		 {
	
			      servers_flow_test[i]+=flow[i][j];
			      clients_flow_test[j]+=flow[i][j];
		          if( ((flow[i][j] != 0) && (connections[i][j] == 0)) || (flow[i][j] < 0) || (servers_flow_test[i] > Servers_cap[i]) || (clients_flow_test[j] > Clients_cap[j]) )
		          {
		              printf( "ERROR: %f ERROR_FLOW_ALLOCATION - Server: %d Capacitie: %f Total_flow: %Lf Client: %d Capacitie: %f Total_flow: %Lf Connected:%d Flow: %Lf\n", t, i ,Servers_cap[i], servers_flow_test[i], j, Clients_cap[j], clients_flow_test[j], connections[i][j], flow[i][j]);
		              return 1;
		          }
		          servers_flow[i] = servers_flow_test[i];
		          if(DEBUG_LEVEL > 3 )  
		          { 
		             if (flow[i][j] != 0) printf("%f FLOW_ALOCCATED %d %d %Lf\n", t, i, j, flow[i][j]);
		            
		          }
		          if( (DEBUG_LEVEL > 2) && (clients_up==NUMBER_CLIENTS))
		          {
		                if(flow[i][j] != 0)
		                {
		                    nsamples_s[i]++;
		                    nsamples_c[j]++;
		                        if( nsamples_s[i] == 1 )
                            {
                                mean_s_temp[i]=flow[i][j];
                            }
                            else{
                                    oldmean_s_temp[i]=mean_s_temp[i];
                                    mean_s_temp[i] = (( mean_s_temp[i] *(nsamples_s[i]-1))+flow[i][j])/nsamples_s[i];
                                    var_s_temp[i] =  ((1.0 - (1.0/(nsamples_s[i]-1))) * var_s_temp[i]) + (nsamples_s[i]) * ( (mean_s_temp[i] - oldmean_s_temp[i])*(mean_s_temp[i] - oldmean_s_temp[i]) );
                            }
                          
                            if( nsamples_c[j] == 1 )
                            {
                                mean_c_temp[j]=flow[i][j];
                            }
                            else{
                                    oldmean_c_temp[j]=mean_c_temp[j];
                                    mean_c_temp[j] = (( mean_c_temp[j] *(nsamples_c[j]-1))+flow[i][j])/nsamples_c[j];
                                    var_c_temp[j] =  ((1.0 - (1.0/(nsamples_c[j]-1))) * var_c_temp[j]) + (nsamples_c[j]) * ( (mean_c_temp[j] - oldmean_c_temp[j])*(mean_c_temp[j] - oldmean_c_temp[j]) );
                            }
                            
                            
                        }
                  }
		          

		 }
		 clients_flow[j] = clients_flow_test[j];
                 
	}    
	
	    
  if (clients_up==NUMBER_CLIENTS)
  {    
	//variavel auxiliar para calcular fluxo medio por conexao
        	nsamples++;

        	
                system_flow = 0;
             	   for(j=0;j<NUMBER_CLIENTS;j++)
                 {
          	              system_flow += clients_flow[j];
          	              if(global_nsamples_c[j] == 0)
                          {
                             global_nsamples_c[j]++;
                             oldmean_clients_number_connections[j]=clients_number_connections[j];
                             oldmean_clients_flow[j]=clients_flow[j];
                          }
                          else
                          {
                             global_nsamples_c[j]++;

                    				 mean_clients_number_connections[j] +=  oldmean_clients_number_connections[j]*(t-t_last_fi);
                    				 oldmean_clients_number_connections[j]=clients_number_connections[j];
                    			
                    				 mean_clients_flow[j] +=  oldmean_clients_flow[j]*(t-t_last_fi);
                    				 oldmean_clients_flow[j]=clients_flow[j];
            				
                          }    

                 }        
                 for(i=0;i<NUMBER_SERVERS;i++)
                 {
                    if(servers_state[i]==1)
                    {
                        if(global_nsamples_s[i] == 0)
                        {
                          global_nsamples_s[i]++;
                           oldmean_servers_number_connections[i] = servers_number_connections[i];
                           oldmean_servers_flow[i] = servers_flow[i];
                        }
                        else
                        {
                          global_nsamples_s[i]++;
                          mean_servers_number_connections[i] += oldmean_servers_number_connections[i]*(t-t_last_fi);
                          oldmean_servers_number_connections[i] = servers_number_connections[i]; 

                          mean_servers_flow[i] += oldmean_servers_flow[i]*(t-t_last_fi);
                          oldmean_servers_flow[i] = servers_flow[i];
                        }
                    }
                 }
                 if(nsamples == 1)
                 {
                      oldmean_system_flow=system_flow;
                 }
                 else
                 {
                     mean_system_flow += oldmean_system_flow*(t-t_last_fi);
                     oldmean_system_flow=system_flow;
                 }
        // LDBL_MAX
        	
            if(DEBUG_LEVEL > 2 )  
            {
             /***************     CALCULANDO MEDIA AMOSTRAL TEMPORAL DA { MEDIA e sua VARIANCIA } DO FLUXO: ponderado no tempo!!!  ***************/
            for(j=0;j<NUMBER_CLIENTS;j++)
        	  {
              
              	   if( global_nsamples_c[j] == 1 )
                   {
                      oldmean_c[j]=mean_c_temp[j];
                      oldvar_c[j] = var_c_temp[j]; 
                   }
                  else{
                      //oldmean_c[j]=mean_c[j];
                      //mean_c[j] = ((( mean_c[j] *(nsamples-1))*t_last_fi)+(mean_c_temp[j]*(t-t_last_fi))/t;

                      mean_c[j]+=oldmean_c[j]*(t-t_last_fi);
                      oldmean_c[j]=mean_c_temp[j];
                      //oldvar_c[j]=var_c[j];
                      //var_c[j] = ((( var_c[j] *(nsamples-1))*t_last_fi)+(var_c_temp[j]*(t-t_last_fi))/t;
                      var_c[j]+=oldvar_c[j]*(t-t_last_fi);
                      oldvar_c[j]=var_c_temp[j];
              	    }
                    if ((var_c[j] < 0) || (mean_c[j]<0)) {
                      printf(" ERROR: %f PARCIAIS_MEDIA_VAR_TEMPORAL_CLIENT %d %Lf %Lf\n", t, j, mean_c[j], var_c[j]);
                    }
                    //printf("%f PARCIAIS_MEDIA_VAR_TEMPORAL_CLIENT %d %Lf %Lf\n", t, j, mean_c[j], var_c[j]);  
                	
            }    
            for(i=0; i< NUMBER_SERVERS ; i++)
            {
              if(servers_state[i]==1){
            	    if( global_nsamples_s[i] == 1 )
                     {
                        oldmean_s[i]=mean_s_temp[i];
                        oldvar_s[i] = var_s_temp[i]; 
                     }
                     else{
                        //oldmean_c[j]=mean_c[j];
                        //mean_c[j] = ((( mean_c[j] *(nsamples-1))*t_last_fi)+(mean_c_temp[j]*(t-t_last_fi))/t;
                        mean_s[i]+=oldmean_s[i]*(t-t_last_fi);
                        oldmean_s[i]=mean_s_temp[i];

                        //oldvar_c[j]=var_c[j];
                        //var_c[j] = ((( var_c[j] *(nsamples-1))*t_last_fi)+(var_c_temp[j]*(t-t_last_fi))/t;
                        var_s[i]+=oldvar_s[i]*(t-t_last_fi);
                        oldvar_s[i]=var_s_temp[i];

                        if ((var_s[i] < 0) || (mean_s[i]<0)) {
                      printf("ERROR: %f PARCIAIS_MEDIA_VAR_TEMPORAL_SERVER %d %Lf %Lf\n", t, i, mean_s[i], var_s[i]);
                    }
            	    }
               //printf("%f PARCIAIS_MEDIA_VAR_TEMPORAL_SERVER %d %Lf %Lf\n", t, j, mean_s[i], var_s[i]);
               }  
        	}
        }
            t_last_fi = t;
	}

    if(DEBUG_LEVEL > 3 )   
    {
       printf("\n");
       
    }
    return 0;
     
}

int algoritmo_connlimit(client clients_vector[NUMBER_CLIENTS], int client_id, int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], long double flow[][NUMBER_CLIENTS], int event_id)
{
     int min_flow_id;
     int i;
     int number;
     float min_flow, total_band_used;
     float cost, max_cost;
     int number_connections, number_connections_plus;
     number_connections = 0;
     
     min_flow_id = 0;
     min_flow = Clients_cap[client_id];
     total_band_used = 0;
     
     if(CONNECTION_LIMIT == 1) //tirando no caso de chegada, somente 1 servidor Ã© removido e outro eh posto no lugar - numero de servidores conectados eh mantido
     {
          if(event_id == CLIENT_ARRIVAL_EVENT)
          {
              //escolher aleatoriamente SET_SIZE servidores e setar own_connections[]=1 para servidores escolhidos
              for(i=0; i < INITIAL_SET_SIZE; i++)        
              {
	   				   i = rand() % KNOWLEDGE;
		 			   number = knowledge_servers[client_id][i];
                       if(clients_vector[client_id].own_connections[number] == 0){
                            clients_vector[client_id].own_connections[number]=1;
                            if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);                            
                       }
                       else{
                            i--;
                       }
                                     
              }
                      
          }
          else
          {               
               //atualizar vetor de fluxos 
               for(i=0; i<NUMBER_SERVERS; i++)
               {
                      //guardando como era o vetor de conexoes (nem todas as conexoes estao estabelecidas pois o pedido de conexao pode nao ter sido processado ainda)
                      clients_vector[client_id].own_connections_old[i] = clients_vector[client_id].own_connections[i];  
                      clients_vector[client_id].own_flow[i]=flow[i][client_id];  
                      
                      if(clients_vector[client_id].own_connections[i] == 1){
                                                                      
                            number_connections++; //numero de conexoes estabelecidas + as que irao se concretizar (pedido de conexao ja enviado)
                      }
                      
                      if( (connections[i][client_id] == 1)  ) 
                      {
                          //procurando menor fluxo recebido
                          //utiliza connections pq o cliente sabe se a conexao ja foi estabelecida ou nao - TCP 3wayhandshake 
                          //caso contrario, o cliente consideraria servidores cuja conexao ainda nÃ£o foi estabelecida, e por isso ainda possuem fluxo zero
                          
                          if(flow[i][client_id]<min_flow)
                          {
                              min_flow = flow[i][client_id];
                              min_flow_id = i;
                          }
                      }
               }
               
               //para clientes que nao estao com a banda saturada:
	       if( clients_vector[client_id].band_filled == 0 )
               {
                   max_cost = Clients_cap[client_id]*(((float)(MAX_BAND_COST))/100) ;
                   number_connections_plus = number_connections + 1;
                   cost = number_connections_plus*Servers_connection_cost[client_id] ;
                   if( (number_connections < (SET_SIZE && KNOWLEDGE)) && ( cost < max_cost))
                   {        
                    
                        //o cliente nao esta com a banda totalmente preenchida, tem direito a pelo menos mais uma conexao, e nao ira saturar a banda somente com o custo das conexoes
                        //number_connections eh o numero de conexoes estabelecidas mais os servidores que possuem um pedido de coxexao na lista de evento a partir daquele cliente
                        //o cliente pode estabelecer ate SET_SIZE conexoes. Realizar mais uma conexao para tentar garantir uma banda saturada (se vericarmos que nao eram necessarias tanta conexoes, podemos descartar o pior servidor posteriomente) 
                        while(true)
                        {
                             //escolher mais um servidor aleatoriamente 
	             			i = rand() % KNOWLEDGE;
				 			number = knowledge_servers[client_id][i];
				            if( connections[number][client_id] == 0)
                             {
                                 clients_vector[client_id].own_connections[number] = 1;
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);  
                                 break;                             
                             }
                        }
               			
           	       }
               	   else
               	   {
               			//nao esta com banda saturada, esta com set_size conexoes ou vai saturar a banda so com conexoes
               			//algoritmo simples: troca servidor que envia menor fluxo por outro qualquer (lembrando que o servidor de menor fluxo ja possui uma conexao estabelecida, ou seja, jÃ¡ foi execurado algortimo de alocacao daquele servidor para aquele cliente)
                    	while(true)
                   	{
             				i = rand() % KNOWLEDGE;
			 				number = knowledge_servers[client_id][i];
                            if( connections[number][client_id] == 0)
                             {
                                 clients_vector[client_id].own_connections[number] = 1;
                                 clients_vector[client_id].own_connections[min_flow_id] = 0;
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, min_flow_id, client_id);
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);  
                                 break;                             
                             }
                   	}	
	               			
	            }
               	}    
               else{
                    //banda do cliente esta totalmente preenchida
                    if( (DROP_SERVERS == 1) && (number_connections>1) )
                    {   
                        //jogar fora servidor de menor fluxo caso cliente esteja conectado a mais de um servidor, para verificar se eh possivel manter a banda saturada com menos conexoes (conexoes custam banda)
                        clients_vector[client_id].own_connections[min_flow_id] = 0;
                        if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, min_flow_id, client_id);
                        
                    }
                    else{
                         //manter todos os servidores (ou pq so temos um servidor conectado ou pq nao vamos utilizar a politica de descarte)
                         if(DEBUG_LEVEL>3) printf("%f ALL_SERVERS_KEEPED %d\n", t, client_id);}
                    
                    }
          }//end else - caso !ARRIVAL_EVENT
                    
     }//end if  CONNECTION_LIMIT==1
    
    return 0;
}
int algoritmo_overhead(client clients_vector[NUMBER_CLIENTS], int client_id, int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], int event_id)
{
     int min_flow_id;
     int i;
     int number;
     float min_flow, total_band_used;
     float cost, max_cost;
     int number_connections, number_active_connections, number_connections_plus, servers_dropped;
     number_connections = 0;
     number_active_connections = 0;
	   servers_dropped = 0;
     
     
     min_flow_id = 0;
     min_flow = Clients_cap[client_id];
     total_band_used = 0;
     

          if(event_id == CLIENT_ARRIVAL_EVENT)
          {
              //escolher aleatoriamente 1 servidore e setar own_connections[]=1 para servidor escolhido
             i = rand() % KNOWLEDGE;
			       number = knowledge_servers[client_id][i];
             clients_vector[client_id].own_connections[number]=1;
             if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);                            
                                     
          }
          else
          {               
               //atualizar vetor de fluxos 
               for(i=0; i<NUMBER_SERVERS; i++)
               {
                      //guardando como era o vetor de conexoes (nem todas as conexoes estao estabelecidas pois o pedido de conexao pode nao ter sido processado ainda)
                      clients_vector[client_id].own_connections_old[i] = clients_vector[client_id].own_connections[i];  
                      clients_vector[client_id].own_flow[i]=flow[i][client_id];  
                      
                      if(clients_vector[client_id].own_connections[i] == 1){
                                                                      
                            number_connections++; //numero de conexoes estabelecidas + as que irao se concretizar (pedido de conexao ja enviado) - as que serao desfeitas
                      }
                      
                      if( (connections[i][client_id] == 1)  ) 
                      {
						              number_active_connections++;
                          //procurando menor fluxo recebido
                          //utiliza connections pq o cliente sabe se a conexao ja foi estabelecida ou nao - TCP 3wayhandshake 
                          //caso contrario, o cliente consideraria servidores cuja conexao ainda nÃ£o foi estabelecida, e por isso ainda possuem fluxo zero
                          if(flow[i][client_id]<min_flow)
                          {
								              min_flow = flow[i][client_id];
                            	min_flow_id = i;
                          }
                          if( flow[i][client_id] < Servers_connection_cost[client_id] )
                          {
            								//fluxo recebido pelo cliente eh menor que o overhead gasto com a conexao
            								//servidor recebera pedido de desconexao
            								clients_vector[client_id].own_connections[i] = 0;
            								servers_dropped++;
            								if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, i, client_id);
                            
                          }
                      }
               }
               
           //para clientes que nao estao com a banda saturada:
	         if( clients_vector[client_id].band_filled == 0 )
           {
          				if (number_connections < KNOWLEDGE)
          				{
          					   while(true)
                    	{
	                         //escolher um servidor aleatoriamente 
                            i = rand() % KNOWLEDGE;
                            number = knowledge_servers[client_id][i];
                            if( (connections[number][client_id] == 0 ) && (clients_vector[client_id].own_connections[number] == 0) )
                             {
                                 clients_vector[client_id].own_connections[number] = 1;
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);  
                                 break;                             
                             }            
                   			}
          				}
            }
            else{
                  //banda do cliente esta totalmente preenchida
                  if( (servers_dropped == 0) && (number_active_connections>1) )
                  {   
                      //jogar fora servidor de menor fluxo
                      clients_vector[client_id].own_connections[min_flow_id] = 0;
                      if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, min_flow_id, client_id);
                      
                  }     
            }     
      }//end else - caso !ARRIVAL_EVENT
                    

    
    return 0;
}
int process_flow_event(event *temp_event_events, event *events, client clients_vector[NUMBER_CLIENTS], int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], int client_id, int *number_events )
{
     
     int event_id, i;
     float when, exp_paramenter;
     struct event *temp_event; 

    struct event **ant, **pont;
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
     
           event_id = FLOW_EVENT;

           //if(DEBUG_LEVEL>0) printf("%f FLOW_EVENT %d\n", t, client_id);
                    
           //retornar nova lista de conexoes para cliente + atualizar o vetor de fluxos dele (own_connections[])
				   if(ALGORITHM == OVERHEAD )                   i = algoritmo_overhead(clients_vector,client_id, knowledge_servers, connections, flow, event_id);
				   if(ALGORITHM == CONNLIMIT)                   i = algoritmo_connlimit(clients_vector,client_id, knowledge_servers, connections, flow, event_id);
				
                   if (i == 1) return 1;
                   
                   for(i=0;i<NUMBER_SERVERS;i++)
                   {
                                             //vetor connections[] possui as conexoes sob perspectiva dos servidores  - conexoes que estao de fato estabelecidas - considerando protocolo tcp, cliente sabe quando a conexao foi estabelecida
                                             //caso  own_connections[] de client_id estiver diferente apos algortimo de escolha de conexoes, enviar pedido de des/conexao para servidor
                                             if ( clients_vector[client_id].own_connections[i] != clients_vector[client_id].own_connections_old[i] )
                                             {
                                                  if(clients_vector[client_id].own_connections[i]==1)
                                                  {
                                                    while(true)
                                                    {
                                                          //criando evento de conexao para cliente no vetor de eventos
    						      						                        exp_paramenter = (float)(1/(float)DELTA3);
                                                          when = t+gera_aleatorio(exp_paramenter);
                                                          if( when < t )
                                                          {
                                                              printf("ERROR: %f ERROR_TIME_EVENT - CONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                              return 1;
                                                          }
                                                          busca_tempo (events, when, ant, pont);
                                                          if(*pont==NULL) break;
                                                      }
                                                      insere(events,CONNECTION_EVENT,when,i,client_id);
                                                      (*number_events)++;
                                                      if(DEBUG_LEVEL>3) printf("%f PUSH CONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                      //atualizando tempo do ultimo evento de conexao
                                                      if( when > clients_vector[client_id].last_connection_scheduled ) clients_vector[client_id].last_connection_scheduled = when;

                                                  }
                                                  else
                                                  {
                                                    while(true)
                                                    {
                                                          //criando evento de desconexao para cliente no vetor de eventos
     						      						                        exp_paramenter = (float)(1/(float)DELTA3);
                                                          when = t+gera_aleatorio(exp_paramenter);
                                                          if( when < t )
                                                          {
                                                              printf("ERROR: %f ERROR_TIME_EVENT - DISCONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                              return 1;
                                                          }
                                                          busca_tempo (events, when, ant, pont);
                                                          if(*pont==NULL) break;
                                                    }
                                                      insere(events,DISCONNECTION_EVENT,when,i,client_id);
                                                      (*number_events)++;
                                                      if(DEBUG_LEVEL>3) printf("%f PUSH DISCONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                      //atualizando tempo do ultimo evento de conexao
                                                      if( when > clients_vector[client_id].last_connection_scheduled ) clients_vector[client_id].last_connection_scheduled = when;
                                                  }
                                             }
                   }
                   
                   //evento de mudanca de fluxo foi processado
                   //setar variavel flow_event_alloc em zero, indicando que nao ha mais evento de mudanca de fluco alocado no vetor de eventos para este cliente
                   clients_vector[client_id].flow_event_alloc=0; 
                   
                   //tirando da lista evento que foi processado (temp_event_events)
                   i = erase(events,temp_event_events);                 
                   if( i == 1 ) return 1;
		   (*number_events)--;    
                   
                   return 0;
     
}


int process_exit_event(event *temp_event_exits, event *exits, event *events, client clients_vector[NUMBER_CLIENTS], int connections[][NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int *servers_up, int *number_events, int event_id, int end_host )
{
    int i,j, client_id, server_id;
    float when,exp_paramenter;
    struct event *temp_event, *temp_event2;
                
    struct event **ant, **pont;
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
               
        if(event_id==SERVER_EXIT_EVENT)
        {

                server_id=end_host;
                //if(DEBUG_LEVEL>0) printf("%f SERVER_EXIT_EVENT %d\n", t, server_id);
                server_exit_time[server_id]=t;
                //gerar eventos de desconexao com delta 3 para todos os servidores i conectados a client
                for(j=0;j<NUMBER_CLIENTS;j++)
                {
                                          //cada cliente possui um vetor own_connections[]
                                          //se os dois valores sao iguais a 1, a conexao esta ativa e nao ha pedidos de desconexao ja alocados
                                          if((connections[server_id][j]==1) && (clients_vector[j].own_connections[server_id]==1))
                                          {
                                              //criando evento de desconexao para cliente no vetor de eventos
                                                while(true)
                                                {  
                                                    exp_paramenter = (float)(1/(float)DELTA3);
                                                    when = t+gera_aleatorio(exp_paramenter);
                                                    if( when < t )
                                                    {
                                                       printf("ERROR: %f ERROR_TIME_EVENT - DISCONNECTION_EVENT %d %d %f\n", t, server_id, j, when);
                                                       return 1;
                                                       
                                                    }
                                                    busca_tempo (events, when, ant, pont);
                                                    if(*pont==NULL) break;
                                                }

                                            }
                                              insere(events,DISCONNECTION_EVENT,when,server_id,j);
                                              (*number_events)++;
                                              if(DEBUG_LEVEL>3) printf("%f PUSH DISCONNECTION_EVENT %d %d %f\n", t, server_id, j, when);
                                                                                                                            
                                          }
                                          
                }

                (*servers_up)--;
                servers_state[server_id]=0;

                //deletar evento de saida da lista de saidas
                i = erase(exits,temp_event_exits) ;
                if( i == 1 ) return 1;                
                (*number_events)--;

                //verificar se ficou algum pedido de conexao pendente e cancelar
                temp_event = get_next(events);
                while(temp_event != NULL)
                {
                                 event_id = get_event_id(temp_event);
                                 i = get_server_id(temp_event);
                                 
                                 if( (event_id == CONNECTION_EVENT) && ( i == server_id) )
                                 {
                                     temp_event2 = temp_event;
                                     temp_event = get_next(temp_event);
                                     
                                     i = erase(events,temp_event2);
                                     if( i == 1 ) return 1;
                                     (*number_events)--;
                                     
                                 }
                                 else{
                                     temp_event = get_next(temp_event);}
                
                }


        }
        else if(event_id==CLIENT_EXIT_EVENT)
               {

                  client_id=end_host; 
                  clients_state[client_id]=0;
                  client_exit_time[client_id]=t;

                //if(DEBUG_LEVEL>0) printf("%f CLIENT_EXIT_EVENT %d\n", t, client_id);

                //gerar eventos de desconexao com delta 3 para todos os servidores i conectados a client
                for(i=0;i<NUMBER_SERVERS;i++)
                {
                                          //cada cliente possui um vetor own_connections[]
                                          //se os dois valores sao iguais a 1, a conexao esta ativa e nao ha pedidos de desconexao ja alocados
                                          if((connections[i][client_id]==1) && (clients_vector[client_id].own_connections[i]==1))
                                          {
                                              //criando evento de desconexao para cliente no vetor de eventos
                                                while(true)
                                                {
                                                      exp_paramenter = (float)(1/(float)DELTA3);
                                                      when = t+gera_aleatorio(exp_paramenter);
                                                      if( when < t )
                                                      {
                                                         printf("ERROR: %f ERROR_TIME_EVENT - DISCONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                         return 1;
                                                         
                                                      }
                                                      busca_tempo (events, when, ant, pont);
                                                      if(*pont==NULL) break;
                                                 }

                                                  insere(events,DISCONNECTION_EVENT,when,i,client_id);
                                                  (*number_events)++;
                                                  if(DEBUG_LEVEL>3) printf("%f PUSH DISCONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                                                                                                
                                          }
                                          
                }

        

                //deletar evento de saida da lista de saidas
                i = erase(exits,temp_event_exits) ;
                if( i == 1 ) return 1;                
                (*number_events)--;
                
                //deletar evento de fluxo relativo ao cliente que saiu do sistema
                //passar endereco da estrura
                temp_event = clients_vector[client_id].flow_event;
                if(clients_vector[client_id].flow_event_alloc==1)
                {
                         //neste caso, o evento de fluxo estava alocado na lista de eventos
                         //tirar da lista de eventos, eventos de fluxo nao sao desalocados pela funcao erase
                        i = erase(events,temp_event);
                        if( i == 1 ) return 1;
                        (*number_events)--;
                        clients_vector[client_id].flow_event_alloc=0;
                                                                  
                }
                free(temp_event);
                
                clients_vector[client_id].flow_event=NULL;
                clients_vector[client_id].exit_event=NULL; 
                
                //verificar se ficou algum pedido de conexao pendente e cancelar
                temp_event = get_next(events);
                while(temp_event != NULL)
                {
                                 event_id = get_event_id(temp_event);
                                 i = get_client_id(temp_event);
                                 
                                 if( (event_id == CONNECTION_EVENT) && ( i == client_id) )
                                 {
                                     temp_event2 = temp_event;
                                     temp_event = get_next(temp_event);
                                     
                                     i = erase(events,temp_event2);
                                     if( i == 1 ) return 1;
                                     (*number_events)--;
                                     
                                 }
                                 else{
                                     temp_event = get_next(temp_event);}
                
                }
                                
                
          
        } 
                return 0;   
    
}

int process_arrival_event(event *temp_event_events, event *events, client clients_vector[NUMBER_CLIENTS], int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int *servers_up, int *number_events, int event_id, int end_host, int *clients_up)
{
    int client_id, server_id, i;
    float when,exp_paramenter;
    struct event *temp_event; 

    struct event **ant, **pont;
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
               

    if(event_id==SERVER_ARRIVAL_EVENT)
    {
          server_id = end_host;
          //if(DEBUG_LEVEL>0) printf("%f SERVER_ARRIVAL_EVENT %d\n", t, server_id);
          servers_state[server_id]=1;
          (*servers_up)++;
          server_arrive_time[server_id]=t;

        
    }
    else if(event_id==CLIENT_ARRIVAL_EVENT)
    {
          client_id = end_host;
          clients_state[client_id]=1;
          (*clients_up)++;
               client_arrive_time[client_id]=t;

               //if(DEBUG_LEVEL>0) printf("%f CLIENT_ARRIVAL_EVENT %d\n", t, client_id);
                   
               //escolher a quais servidores conectar 
			   if(ALGORITHM == OVERHEAD )                   i = algoritmo_overhead(clients_vector,client_id, knowledge_servers, connections, flow, event_id);
			   if(ALGORITHM == CONNLIMIT)                   i = algoritmo_connlimit(clients_vector,client_id, knowledge_servers, connections, flow, event_id);
               if( i == 1 ) return 1;
               
               for(i=0;i<NUMBER_SERVERS;i++)
               {                            
                                         //enviar eventos de conexao para servidores escolhidos
                                         //isto eh, posicoes que possuem valor 1 no vetor own_connections[] do cliente que entrou (client_id)
                                         if ( clients_vector[client_id].own_connections[i] == 1 )
                                         {
                                              while(true)
                                              {
                                                    exp_paramenter = (float)(1/(float)DELTA3);
                                                    when = t+gera_aleatorio(exp_paramenter);
                                                    if( when < t )
                                                    {
                                                       printf("ERROR: %f ERROR_TIME_EVENT - CONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                       return 1;
                                                       
                                                    }
                                                   busca_tempo (events, when, ant, pont);
                                                   if(*pont==NULL) break;
                                              }
                                              //criando evento de conexao para cliente no vetor de eventos

                                              insere(events,CONNECTION_EVENT,when,i,client_id);
                                              (*number_events)++;
                                              if(DEBUG_LEVEL>3) printf("%f PUSH CONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                              //atualizando tempo do ultimo evento de conexao
                                              if( when > clients_vector[client_id].last_connection_scheduled ) clients_vector[client_id].last_connection_scheduled = when;
                                              
                                         }
               }    
    
    }

               //tirando da lista evento que foi processado (temp_event_events)
    i = erase(events,temp_event_events); 
    if( i == 1) return 1;                
    (*number_events)--;


               
               return 0;
}

int process_connection_event(event *temp_event_events, event *events, event *exits, client clients_vector[NUMBER_CLIENTS], int connections[][NUMBER_CLIENTS], int servers_number_connections[NUMBER_SERVERS], int clients_number_connections[NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], long double client_flow[NUMBER_CLIENTS], long double server_flow[NUMBER_SERVERS], long double data[][NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int event_id, int server_id, int client_id, int *number_events, int clients_up )
{
     int i,j, k;
     int change_flow;
     long double flow_accumulator, data_accumulator, data_accumulator_old, cost_accumulator;
     float when, epsilon, exp_paramenter;
     struct event *temp_event; 
     struct event **ant, **pont;

    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));

                    // if(event_id == CONNECTION_EVENT){
                    //              if(DEBUG_LEVEL>0) printf("%f CONNECTION_EVENT %d %d\n", t, server_id, client_id);}
                    // if(event_id == DISCONNECTION_EVENT){
                    //              if(DEBUG_LEVEL>0) printf("%f DISCONNECTION_EVENT %d %d\n", t, server_id, client_id);}
                    
                    
                    //atualiza dados recebidos de todos os clientes usando fluxo atual, t e t-ultima-att
                    for (j=0;j<NUMBER_CLIENTS;j++)
                    {
                        data_accumulator = 0; //iterador para total de dados recebidos - de todos os servidores
			            data_accumulator_old = 0;
                        for(i=0;i<NUMBER_SERVERS;i++)
                        {
						     data_accumulator_old += data[i][j];	
                                                     
						     data[i][j] += (t- t_last)*flow[i][j];

                             data_accumulator += data[i][j];

                             if( (data[i][j] < 0) || ((data_accumulator >  FILE_SIZE) && (clients_vector[j].exit_event != NULL)) || ((data_accumulator_old >  FILE_SIZE) && (clients_vector[j].exit_event != NULL)) )
                             {
                                printf("ERROR: %f ERROR_DATA_AMOUNT - %d %d %Lf %Lf %Lf \n", t, i, j, data[i][j], data_accumulator_old, data_accumulator);
	                            printf( "\n%f REMAINING EVENTS TO PROCESS %d\n", t, (*number_events));

    							listar(exits); listar(events); printf("\n");

    							printf( "CLIENT %d DATA_VECTOR:\n", j);
    							for(k=0; k<NUMBER_SERVERS; k++)
    							{
    								printf( "DATA_AMOUNT_FROM_SERVER %d %Lf\n", k, data[k][j]);
    							}
    							printf("\nCLIENT %d FLOW_VECTOR:\n", j);
    							for(k=0; k<NUMBER_SERVERS; k++)
    							{
    								printf( "FLOW_AMOUNT_FROM_SERVER %d %Lf\n", k, flow[k][j]);
    							}
                                 //return 1;
                            }                          
                        }
                           
                    }
                    //atualiza t_last, marca quando foi a ultima atualizacao nos fluxos dos servidores
                    t_last = t;
                    
                    //atualizando vetor de conexoes dos servidores
        	                    if(event_id == CONNECTION_EVENT){
                                if(servers_state[server_id] == 0)
                                {
                                            printf("ERROR: %f ERROR_CONNECTION_ATTEMPT - SERVER_DOWN %d %d\n", t, server_id, client_id);
                                            return 1; 

                                }
                                if (connections[server_id][client_id] != 0)
                                {
                                            printf("ERROR: %f ERROR_CONNECTION_ATTEMPT - ALREADY_CONNECTED %d %d\n", t, server_id, client_id);
                                            return 1;                                       
                                }
                                connections[server_id][client_id] = 1;
                                servers_number_connections[server_id]++;
                                clients_number_connections[client_id]++;
                }
        	    else{	
                                if (connections[server_id][client_id] != 1)
                                {
                                            printf("ERROR: %f ERROR_DISCONNECTION_ATTEMPT - NOT_CONNECTED %d %d\n", t, server_id, client_id);
                                            return 1;                                       
                                }
                                connections[server_id][client_id] = 0;
                                servers_number_connections[server_id]--;
                                clients_number_connections[client_id]--;
                }

                // if(DEBUG_LEVEL>1) printf("%f SERVER_NUMBER_CONNECTIONS %d %d\n", t, server_id, servers_number_connections[server_id]);
                // if(DEBUG_LEVEL>1) printf("%f CLIENT_NUMBER_CONNECTIONS %d %d\n", t, client_id, clients_number_connections[client_id]);
                 
                //roda algoritmo de alocacao de banda para determinar novo flow
                //ja retorna com flow atualizado
                i = alocacao(clients_vector, clients_state, servers_state, connections, flow, client_flow, server_flow, clients_number_connections, servers_number_connections, clients_up);
                if( i == 1) return 1;
   
                    //reescalona eventos de saida e fluxo para os clientes usando matriz de dados e fluxo atual
                    for (j=0;j<NUMBER_CLIENTS;j++)
                    {
                        //alguns clientes ja podem ter saido, mas o servidor ainda nao recebeu a notificacao
                        //para esses clientes, nao eh necessario reescalonar evento de saida/fluxo, pois eles nao estao mais no sistema
                        //clientes que ja sairam do sistema possuem respectivos exit_event e flow_event com valor NULL (estrutara desses eventos foi desalocada)
                        if(clients_vector[j].exit_event!=NULL)
                        { 
                                                                
                            flow_accumulator = 0; //iterador para fluxo total recebido por um cliente - de todos os servidores
                            data_accumulator = 0; //iterador para total de dados recebidos - de todos os servidores
                            cost_accumulator = 0; //iterador para total do custo das conexoes
                            change_flow = 0;
                            for(i=0;i<NUMBER_SERVERS;i++)
                            {
                                                         flow_accumulator += flow[i][j];
                                                         data_accumulator += data[i][j];
                                                         if(connections[i][j] == 1) cost_accumulator += Servers_connection_cost[j];
                                                         //se o fluxo alocado pelo servidor tiver mudado
                                                         //isto eh, flow[][] (fluxo efetivo) passou a ter um valor diferente de own_flow[] (fluxo enxergado pelos clientes)
                                                         //verificar se diferenca Ã© perceptivel, caso seja, escalonar evento de mudanca de fluxo para respectivo cliente
                                                         if( flow[i][j] != clients_vector[j].own_flow[i] )
                                                         {
                                                             epsilon = flow[i][j]-clients_vector[j].own_flow[i];
                                                             if(epsilon < 0){
                                                                      epsilon = -epsilon;}                                                         
                                                             if( epsilon > (((float)VARIACAO)/100)*(clients_vector[j].own_flow[i]) ){
                                                                       change_flow = 1;}
                                                         }                       
                            }
                            
                            if( cost_accumulator < 0 )
                            {
                                printf("ERROR: %f ERROR_FLOW_ALLOCATION - NEGATIVE_COST %d %Lf\n", t, j, cost_accumulator);
                                return 1;
                            }
                            if( (flow_accumulator < 0) || (flow_accumulator > (Clients_cap[j]-cost_accumulator)) )
                            {
                                printf("ERROR: %f ERROR_TOTAL_FLOW_ALLOCATION %d %Lf %Lf\n", t, j, flow_accumulator, (Clients_cap[j]-cost_accumulator));
                                return 1;
                            }
                            if( (data_accumulator < 0) || (data_accumulator > FILE_SIZE) )
                            {
                                printf("ERROR: %f ERROR_DATA_AMOUNT %d %Lf\n", t, j, data_accumulator);
				
                				printf( "\n%f REMAINING EVENTS TO PROCESS %d\n", t, (*number_events));

                				listar(exits); listar(events); printf("\n");

                				printf( "CLIENT %d DATA_VECTOR:\n", j);
                				for(k=0; k<NUMBER_SERVERS; k++)
                				{
                					printf( "DATA_AMOUNT_FROM_SERVER %d %Lf\n", k, data[k][j]);
                				}
                				printf("\nCLIENT %d FLOW_VECTOR:\n", j);
                				for(k=0; k<NUMBER_SERVERS; k++)
                				{
                					printf( "FLOW_AMOUNT_FROM_SERVER %d %Lf\n", k, flow[k][j]);
                				}           
                                                //return 1;
                            }
                            
                            
                            if(DEBUG_LEVEL>3){
                                               
                                               if(clients_vector[j].band_filled) printf("%f BAND_FILLED %d %Lf\n", t, j, flow_accumulator);
                                               if(!clients_vector[j].band_filled) printf("%f TOTAL_FLOW %d %Lf\n", t, j, flow_accumulator);
                                               printf("%f DATA_AMOUNT %d %Lf\n", t, j, data_accumulator);
                            }
                            
                            //REESCALONAR EVENTO DE SAIDA BASEADO NO FLUXO ATUAL E DADOS JA RECEBIDOS
                            //tempo estimado para fim do download
                            //when = ((FILE_SIZE-data_accumulator)/flow_accumulator)+t;
                            if( flow_accumulator != 0)
                            {
                                when = ((((long double)FILE_SIZE)-data_accumulator)/flow_accumulator)+t;
                                //printf("%f ESTIMATED_TIME_EXIT %d %Lf\n", t, j, when);

				//if( ( ((when - t)*flow_accumulator)+data_accumulator ) > FILE_SIZE ) 
				//{ printf("ERROR: %f ERROR_TIME_EVENT - EXIT_EVENT - TRUNKED %d exit: %f data_accumulator: %Lf flow_accumulator: %Lf \n", t, j, when, data_accumulator, flow_accumulator); return 1;}
                            }
                            else {
                                 when = INF;}
                                 
                            if( when < t )
                            {
                                 printf("ERROR: %f ERROR_TIME_EVENT - EXIT_EVENT %d %f\n", t, j, when);
                                 return 1;
                                 
                            }
                            
                            temp_event = clients_vector[j].exit_event;
                            if(get_time(temp_event) != when)
                            {
                                //removendo antigo evento de saida                        
                                remove_event(temp_event);
                                //reinserindo
                                recoloca(exits,CLIENT_EXIT_EVENT,when,0,j,temp_event);
                                
                                if(DEBUG_LEVEL>3) printf("%f REPUSH CLIENT_EXIT_EVENT %d %f\n", t, j, when);
                                                      
                            }
                            
                            //reescalona eventos de mudanca de fluxo no tempo t + 2 para clientes que tiveram alguma mudanca significativa e ainda nao possuem evento mudanca de fluxo escalonado
                            if(change_flow && (clients_vector[j].flow_event_alloc==0))
                            {
                                
                                clients_vector[j].flow_event_alloc=1;
                                
                                temp_event = clients_vector[j].flow_event;
                                // while(true)
                                // {          //gambiarra para que evento mudanca de fluxo nao ocorra antes de conexoes/desconexoes
                                //            pode ser usado para estabelecer limites do tempo que demora para percepcao do fluxo
                                //            ideia: estabelecer distancias entre servidores e clientes
                                //            estabelecer limites superiores e inferiores para o tempo necessario para perceber a mudanca de fluxo
                            while(true)
                            {                                //            dadas as distancias dos servidores que tiveram mudancas significativas de fluxo
            					   exp_paramenter = (float)(1/(float)DELTA2);
            					   if(clients_vector[j].last_connection_scheduled > t) when = clients_vector[j].last_connection_scheduled+gera_aleatorio(exp_paramenter);
            					   if(clients_vector[j].last_connection_scheduled <= t) when = t+gera_aleatorio(exp_paramenter); 	

                                               //if( when > clients_vector[j].last_connection_scheduled ) break;
                                               //IDEIA: cliente ira perceber mudanca de fluxo depois de um tempo pelo menos FLOW_PERCEPTION vezes maior que "o maior tempo atual para estabelecer uma conexao"
                                               //if( (when-t) >= FLOW_PERCEPTION*(clients_vector[j].last_connection_scheduled - t)  ) break;
                                    //}
                                    
                                    if( when < t )
                                    {
                                        printf("ERROR: %f ERROR_TIME_EVENT - FLOW_EVENT %d %f\n", t, j, when);
                                        return 1;
                                     
                                    }
                                     
                                    busca_tempo (events, when, ant, pont);
                                    if(*pont==NULL) break;
                              }              
                                recoloca(events,FLOW_EVENT,when,0,j,temp_event);
                                
                                if(DEBUG_LEVEL>3) printf( "%f PUSH FLOW_EVENT %d %f\n", t, j, when);
                                
                                (*number_events)++;
                            
                            }
                        }//end if client exit != null  
                                
                    }
                    
                    //tirando da lista evento que foi processado (temp_event_events)
                    i = erase(events,temp_event_events);                 
                    if( i == 1 ) return 1;
		            (*number_events)--;
                    
                    return 0;     
}

