
       
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
                                    if(DEBUG_LEVEL>5) printf("Server %d cap: %.f\n", i, Servers_cap[i] );
           
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
                                    if(DEBUG_LEVEL>5) printf("Client %d cap: %.f\n", i, Clients_cap[i] );
           
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


