
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
