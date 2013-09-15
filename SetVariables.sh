 #!/bin/bash
        # Author: fabricio@land.ufrj.br
        # Date: 2009-11-10
        # Last modified: 2010-02-21
        # Description: This script sets many variables in
        #              the tangram-II model of BitTorrent Luiz_Torrent_V_21*.obj
        
        #CODIGO DE DIVISAO DE GRUPOS
               
        #      -1: CLIENTES E SERVIDORES COM MESMA BANDA - so com random == 1
        #       0: TODOS DO MESMO GRUPO COM CAPACIDADES DISTINTAS (aceita no caso randm == 0, se quiser fazer clientes random e serv. nao, ou vice versa)
        #       1: TODOS DO MESMO GRUPO COM A MESMA CAPACIDADE
        #       2: 1/2 - 1/2
        #       3: 1/3 - 1/3 - 1/3
        #       4: 1/3 - 2/3
        
        # BEGIN of changeable variables #
        DEBUG_LEVEL=1
        
        NUMBER_SERVERS=100 
        NUMBER_CLIENTS=250
        KNOWLEDGE=100
        MAX_CONNECTIONS_ATTEMPT=100
        NUMBER_SERVERS_DOWN=50
        TIME_SERVERS_DOWN=3000
        TIME_SERVERS_UP=7000
        
        RANDOM_CAP=0
        SERVERS_BAND_DIVISION=1
        CLIENTS_BAND_DIVISION=1
        
        #setar variaveis se RANDOM == 0 / inicio
        CAP_SERVER_1=1000
        CAP_SERVER_2=100
        CAP_SERVER_3=100
        
        CAP_CLIENT_1=100
        CAP_CLIENT_2=100
        CAP_CLIENT_3=100
        #setar variaveis se RANDOM == 0 / fim
        
        #setar variaveis se RANDOM == 1 ou BAND_DIVISION = 0/ inicio
        MAX_CAP_SERVER=100
        MAX_CAP_CLIENT=100
        
        MIN_CAP_SERVER=100
        MIN_CAP_CLIENT=100
        #setar variaveis se RANDOM == 1 ou BAND_DIVISION = 0 / fim

        #CONNLIMIT 1
        #OVERHEAD 2
        ALGORITHM=2


        
        
        CONNECTION_LIMIT=1
        DROP_SERVERS=1
        SET_SIZE=2
        INITIAL_SET_SIZE=1
        FILE_SIZE=1000000
        MAX_TIME=100000
        
        #DELTA2>>DELTA3 - media
        #DELTA1 entrada-saida 
        #DELTA2 mudanca de fluxo (media pelo menos duas vezes maior)
        #DELTA3 des-conexao 
        
        DELTA1=10
        DELTA2=200
        DELTA3=10
        
        #porcentagem da variacao: (fluxo antigo)*VARIACAO/100 = maximo de diferenca entre fluxo antigo e atual
        VARIACAO=10 
        FLOW_PERCEPTION=3 
        CONNECTION_COST=12
        
        
        
        lista="DEBUG_LEVEL NUMBER_SERVERS NUMBER_CLIENTS RANDOM_CAP SERVERS_BAND_DIVISION CLIENTS_BAND_DIVISION \
        CAP_SERVER_1 CAP_SERVER_2 CAP_SERVER_3 CAP_CLIENT_1 CAP_CLIENT_2 CAP_CLIENT_3 TIME_SERVERS_UP \
        MAX_CAP_SERVER MAX_CAP_CLIENT MIN_CAP_SERVER MIN_CAP_CLIENT ALGORITHM KNOWLEDGE TIME_SERVERS_DOWN \
        CONNECTION_LIMIT DROP_SERVERS SET_SIZE INITIAL_SET_SIZE FILE_SIZE MAX_TIME NUMBER_SERVERS_DOWN MAX_CONNECTIONS_ATTEMPT \
        DELTA1 DELTA2 DELTA3 VARIACAO FLOW_PERCEPTION CONNECTION_COST"
        
        # Passo 1: altera as variaveis do modelo #
        rm -f sed_cmd.txt
        for i in $lista; do
        echo "s/#define ${i} \+[0-9]\+/#define ${i} $(($i))/g" >> sed_cmd.txt
        #echo "s/${i}=[0-9]\+/${i}=$(($i))/g" >> sed_cmd.txt    
        done
        
        sed -f sed_cmd.txt defines.cpp > defines.cpp.new
        mv defines.cpp.new defines.cpp
        
        rm -f simulador.cpp
        awk '{print $0}' comentarios.cpp >> simulador.cpp
        awk '{print $0}' includes.cpp >> simulador.cpp
        awk '{print $0}' defines.cpp >> simulador.cpp
        awk '{print $0}' declarations.cpp >> simulador.cpp
        awk '{print $0}' main.cpp >> simulador.cpp
        awk '{print $0}' gera_aleatorio.cpp >> simulador.cpp
        awk '{print $0}' manipula_listas.cpp >> simulador.cpp
        awk '{print $0}' manipula_eventos.cpp >> simulador.cpp
        awk '{print $0}' capacities.cpp >> simulador.cpp
        awk '{print $0}' alocacao.cpp >> simulador.cpp
        awk '{print $0}' algoritmo_connlimit.cpp >> simulador.cpp
        awk '{print $0}' algoritmo_overhead.cpp >> simulador.cpp

        awk '{print $0}' process_flow_event.cpp >> simulador.cpp
        awk '{print $0}' process_exit_event.cpp >> simulador.cpp
        awk '{print $0}' process_arrival_event.cpp >> simulador.cpp
        awk '{print $0}' process_connection_event.cpp >> simulador.cpp
        
        g++ simulador.cpp -o simulador #&& \
        # for (( y=1; y<=2; y++)); do nice ./simulador > /storage/alejandra/resultados-tese/resultados/simulador-tese-flash/saidas/setsize/caso1/saida_modelo-$y.txt && ./data.awk /storage/alejandra/resultados-tese/resultados/simulador-tese-flash/saidas/setsize/caso1/saida_modelo-$y.txt >> /storage/alejandra/resultados-tese/resultados/simulador-tese-flash/saidas/setsize/caso1/saida_erro.txt ; done
        #grep " S \| R \|Transitions" saida_modelo.txt > saida.txt
       
      
     
    
   
  
 

