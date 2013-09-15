using namespace std;

FILE *output;
int t_print, flag_estatisticas;
float t, t_last, t_last_fi, t_inicio_stats;

float server_arrive_time[NUMBER_SERVERS];
float server_exit_time[NUMBER_SERVERS];

float client_arrive_time[NUMBER_CLIENTS];
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
int process_exit_event(event *, event *, event *, client[NUMBER_CLIENTS], int[][NUMBER_CLIENTS], int[NUMBER_CLIENTS], int[NUMBER_SERVERS], int *, int * , int, int, int * );
int process_arrival_event(event *, event *, client[NUMBER_CLIENTS], int[][KNOWLEDGE], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], int[NUMBER_CLIENTS], int[NUMBER_SERVERS], int *, int *, int, int, int * );
int process_flow_event(event *, event *, client[NUMBER_CLIENTS], int[][KNOWLEDGE], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], int[NUMBER_SERVERS], int, int * );
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
int algoritmo_overhead(client[NUMBER_CLIENTS], int, int[][KNOWLEDGE], int[][NUMBER_CLIENTS], float[][NUMBER_CLIENTS], int[NUMBER_SERVERS], int );  //seleciona servidores, retorna 1 se erro
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
