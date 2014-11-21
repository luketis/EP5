#include "../robot_fight.h"
#include<math.h>

#define PI 3.14159265
typedef enum {ATIROU, ANDOU, VIROU, BLOQUEOU, NADA} Comportamento;

/* Feito por Yan, O Monitor Mais Rápido do Oeste */

static char charging;

typedef struct in{
	Position pos;
	Direction dir;
	int distanciaAtual;
	float distanciaMedia;
	int somaDist;
	int balas;
	int vida;
	int obstaculos;
	int score;
	int isInCp;
	int mirandoEmMim;
	int somaAtirou;
	int somaAndou;
	int somaBlocks;
	int somaVirou;
	float taxaDefesa;
	int tirosQueLevaria;	
	int tirosQueLevou;
	int vivo;
}inimigo;

struct ponto{
	Position pos;
	int distancia;
};


typedef struct ponto *cponto;

inimigo inimigos[3];

static Position controles[10];
int turno;

int isRobot(Tile *t) {
	return t->type == ROBOT;
}

int isProjectile(Tile *t) {
	return t->type == PROJECTILE;
}

int isBlock(Tile *t) {
	return t->type == BLOCK;
}

int isEmptyControl(Tile *t) {
	return t->isControlPoint && t->type == NONE;
}

int isControl(Tile *t) {
	return t->isControlPoint;
}

int isValid(Position p, int m, int n) {
	return p.x >= 0 && p.x < m && p.y >= 0 && p.y < n;
}


int MaxInt(int x, int y){
	if(x > y)
		return x;
	return y;
}

int distancia(Position from, Position to){
	return (MaxInt(abs(to.x - from.x), abs(to.y - from.y)) +1 );
}

Direction shortestPath(Grid *g, Position from, Position to){

	float angle;
	int deltaY, deltaX;
	Direction dir;

	deltaY = to.y - from.y;
	deltaX = to.x - from.x;
	
	if(!deltaX){
		
		if((from.x)%2)
			angle = ((to.y - from.y) < 0)*170 + 1.0;
		else
			angle = ((to.y - from.y) < 0)*(-170) - 1.0;
	}
	else
		angle = atan2(deltaX, deltaY)*180.0/PI;
	if(angle >= 0)
		dir = floor((179.999999 - angle)/60.0) + 2;
	else{
		dir = ((int)floor((-angle)/60.0) +5)%6;
		//printf("h%f\n", angle);
	}

	return dir;

}

int rayCast(Grid *g, Position me, Position him, Direction his, int d, int *dir){
	int i, j, result = 0;
	Position aux;
	
	for(i = -1; i < 2; i++){
		//printf("sera\n");
		aux = getNeighbor(him, (his + i)%6);
		for(j = 0; j < d && isValid(aux, g->m, g->n); j++){
			if(aux.x == me.x && aux.y == me.y){
				result = 1;
				if(dir != NULL)
					*dir = i;
				break;
			}
			aux = getNeighbor(aux, (his + i)%6);
		}
	}
	return result;
}

void findOfensiveSpots(Grid *g, Position me, Position op[], inimigo ini){
	Position aux;
	int i, j;

	for(i = 0; i < 6; i++){
		aux = ini.pos;
		for(j = 0; j < 5 && isValid(getNeighbor(aux, i), g->m, g->n); j++)
			aux = getNeighbor(aux, i);
		if(j == 5)
			op[i] = aux;
		else{
			op[i].x = 500;
			op[i].y = 500;
		}
	}

}

void buscaMapa(Grid *g, Position p, Direction myDir){
	Robot *r;
	int i, j, k, l;
	Position aux;
	
	k = 0;
	l = 0;

	for(i = 0; i < g->m; i++)
		for(j = 0 ; j < g->n; j++){
			//printf("halo\n");
			aux.x = i;
			aux.y = j;
			if(isValid(aux, g->m, g->n) && isRobot(&g->map[i][j]) && !(i == p.x && j == p.y)){
				
				inimigos[k].pos = aux;
				inimigos[k].distanciaAtual = distancia(p, aux) + (6 + myDir - shortestPath(g, p, aux))%6;
				inimigos[k].distanciaMedia = distancia(p, aux);
				inimigos[k].somaDist = distancia(p, aux);
				//printf("halo2\n");
				r = &g->map[aux.x][aux.y].object.robot;
				inimigos[k].dir = r->dir;
				inimigos[k].balas = r->bullets;
				inimigos[k].score = r->score;
				inimigos[k].vida = r->hp;
				inimigos[k].obstaculos = r->obstacles;
				
				inimigos[k].isInCp = g->map[aux.x][aux.y].isControlPoint;
				
				inimigos[k].somaAndou = 0;
				inimigos[k].somaAtirou = 0;
				inimigos[k].somaVirou = 0;
				inimigos[k].somaBlocks = 0;
				inimigos[k].taxaDefesa = -1;
				inimigos[k].tirosQueLevaria = 0;
				inimigos[k].tirosQueLevou = 0;
				inimigos[k].vivo = 1;
				inimigos[k].mirandoEmMim = rayCast(g, p, inimigos[k].pos, inimigos[k].dir, 4, NULL);
				k++;
				//printf("halo3\n");
			}
			else if(isControl(&g->map[i][j]) && l < 10){  
				//printf("halo4\n");
				 //Aqui nao ta funcionando por alguma razao
				aux.x = i;
				aux.y = j;
				controles[l] = aux;
				l++;
			}	
		}
}
void atualiza(Grid *g, Position p, Direction myDir){
	int i, j, l = 0;
	Robot *r;
	Position pos;
	turno += 1;

	for(i = 0; i < 3; i++){
		if(inimigos[i].vivo && !isRobot(&g->map[inimigos[i].pos.x][inimigos[i].pos.y])){ //QUer dizer que andou
			for(j = 0; j < 6; j++){	
				pos = getNeighbor(inimigos[i].pos, j);
				if(isValid(pos, g->m, g->n) && isRobot(&g->map[pos.x][pos.y])){	
					inimigos[i].pos = pos;
					inimigos[i].distanciaAtual = distancia(p, pos) + (6 + myDir - shortestPath(g, p, pos))%6;
					inimigos[i].somaDist += distancia(p, pos);
					inimigos[i].distanciaMedia = (inimigos[i].somaDist)/((float) turno);
					inimigos[i].somaAndou += 1;
					inimigos[i].isInCp = g->map[pos.x][pos.y].isControlPoint;
					inimigos[i].mirandoEmMim = rayCast(g, p, inimigos[i].pos, inimigos[i].dir, inimigos[i].distanciaAtual, NULL);
					break;
				}
				
			}
			if(j == 6)
				inimigos[i].vivo = 0;
		}
		else if(inimigos[i].vivo){
			r = &g->map[inimigos[i].pos.x][inimigos[i].pos.y].object.robot;
			if(r->dir != inimigos[i].dir){
				inimigos[i].dir = r->dir;
				inimigos[i].somaVirou += 1;
			}
			for(j = 0; j < 6; j++){
				pos = getNeighbor(inimigos[i].pos, j);
				if(isProjectile(&g->map[pos.x][pos.y])){
					Projectile *pr = &g->map[inimigos[i].pos.x][inimigos[i].pos.y].object.projectile;
					if(pr->dir == inimigos[i].dir)
						inimigos[i].somaAtirou += 1;
					else if(abs(pr->dir - j) == 3) //Significa que o tiro esta direcionado para o inimigo sendo espiado
						inimigos[i].tirosQueLevaria += 1;
						
				}
				else if(isBlock(&g->map[pos.x][pos.y])){
					
					//TODO
					
				}
			}

		}
		if(inimigos[i].vivo){
			r = &g->map[inimigos[i].pos.x][inimigos[i].pos.y].object.robot;
			inimigos[i].balas = r->bullets;
			inimigos[i].score = r->score;
			if(inimigos[i].vida != r->hp)
				inimigos[i].tirosQueLevou +=1;
			inimigos[i].vida = r->hp;
			inimigos[i].obstaculos = r->obstacles;
			if(inimigos[i].tirosQueLevaria)
				inimigos[i].taxaDefesa = inimigos[i].tirosQueLevou/((float)inimigos[i].tirosQueLevaria);
		}		
	}
	if(turno == 1){
		for(i = 0; i < g->m; i++)
			for(j = 0 ; j < g->n; j++){
				if(isControl(&g->map[i][j]) && l < 10){  
					 //Aqui nao ta funcionando por alguma razao
					pos.x= i;
					pos.y = j;
					controles[l] = pos;
					l++;
				}	
			}
	}
	

}

int sendoAlvejado(Grid *g, Position p, Direction dirs[], int n){

	Position pos;
	Projectile *r;
	int i, alvejado = 0;
	
	if(n == 2)
		for(i = 0; i < 6; i++){
			pos = getNeighbor(getNeighbor(p, i), i);
			if(isValid(pos, g->m, g->n) && isProjectile(&g->map[pos.x][pos.y])){
				r = &g->map[pos.x][pos.y].object.projectile;
				if(abs(r->dir - i) == 3){
					dirs[i] = 1;
					alvejado = 1;
				}
			}
			else
				dirs[i] = 0;
		}
	
	else
		for(i = 0; i < 6; i++){
			pos = getNeighbor(p, i);
			if(isValid(pos, g->m, g->n) && isProjectile(&g->map[pos.x][pos.y])){
				r = &g->map[pos.x][pos.y].object.projectile;
				if(abs(r->dir - i) == 3){
					dirs[i] = 1;
					alvejado = 1;
				}
			}
			else if(isValid(pos, g->m, g->n) && isRobot(&g->map[pos.x][pos.y])){
				
				dirs[i] = 1;
				alvejado = 1;
				

			}
			else
				dirs[i] = 0;
		}
	
	return alvejado;
}

//Fazer uma funcao BFS que acha melhor caminho ateh certo ponto. tem q considerar viradas como passos

Action turn(Grid *g, Position p, Direction dirs[], Direction myDir){

	Position pos;
	int i, lucroLeft = 0, lucroRight = 0, j;
	
	for(i = 0; i < 3; i++){
		pos = p;
		j = 0;
		while(isValid(pos, g->m, g->n)){
			pos = getNeighbor(pos, (myDir - i)%6);
			if(isRobot(&g->map[pos.x][pos.y])){
				if(j > 0)
					lucroLeft+=10;
				else
					lucroLeft-=10000;
				break;
			}
			else  if(isControl(&g->map[pos.x][pos.y]))
				lucroLeft+=5;

			j++;
		}
	}
	if(!isValid(getNeighbor(pos, (myDir - 1)%6), g->m, g->n))
		lucroLeft-=10000;
	if(dirs[(myDir - 1)%6])
		lucroLeft-=100;
	for(i = 0; i < 3; i++){
		pos = p;
		j = 0;
		while(isValid(pos, g->m, g->n)){
			pos = getNeighbor(pos, (myDir + i)%6);
			if(isRobot(&g->map[pos.x][pos.y])){
				if(j > 0)
					lucroRight+=10;
				else
					lucroRight-=10000;
				break;
			}
			else  if(isControl(&g->map[pos.x][pos.y]))
				lucroRight+=5;

			
			j++;
		}
		
	}
	if(!isValid(getNeighbor(pos, (myDir + 1)%6), g->m, g->n))
		lucroRight-=10000;
	if(dirs[(myDir + 1)%6])
		lucroRight-=100;
	if(lucroRight > lucroLeft)
		return TURN_RIGHT;
	return TURN_LEFT;

}

Position nearest(Grid *g, Position from, Position v[], int t, Direction myDir){
	Position n;
	int dist = 1000, aux, i;

	for(i = 0; i < t; i++){
		aux = distancia(from, v[i]) + (6 + myDir - shortestPath(g, from, v[i]))%6;
		if(aux < dist){
			dist = aux;
			n = v[i];
		}
	}
	return n;
}

inimigo nearestEnemy(Grid *g, Position from, Direction myDir, int mode){
	int aux, dist = 10000, i;
	inimigo titular;
	for(i = 0; i < 3; i++){
		aux = distancia(from, inimigos[i].pos);
		if(mode)
			aux += (6 + myDir - shortestPath(g, from, inimigos[i].pos))%6;
		if(aux < dist){
			dist = aux;
			titular = inimigos[i];
		}
	}
	return titular;
}

int danoEstar(Grid *g, Position p, int isntMe){
	Direction dirs[6];
	int dano = 0, i;
	if (sendoAlvejado(g, p, dirs,  1)){}
	for(i = 0; i < 6; i++)
		dano += 10*dirs[i];
	
	return (dano + isntMe*5*isRobot(&g->map[p.x][p.y]));
}

void prepareGame(Grid *g, Position p, int turnCount) {
	
	charging = 0;
	setName("3");
	
	
	
	turno = 0;
	
	
	
}

Action bestTurn(Direction from, Direction to) {
	if(((6 + from - to) % 6) < 3) return TURN_LEFT;
	else return TURN_RIGHT;
}

Action shoot(Direction from, Direction to) {
	if(to == ((from + 1) % 6))
		return SHOOT_RIGHT;
	else if(to == from)
		return SHOOT_CENTER;
	else
		return SHOOT_LEFT;
}


Action processTurn(Grid *g, Position p, int turnsLeft) {
	//printf("here we go!\n\n");
	Position posOfensiva[6];
	Direction dirs[6];	
	Direction myDir;
	Direction obj;
	inimigo inimigoPerto;
	int di;
	//printf("%d %d\n", controles[0].x, controles[0].y)
	
	Robot *eu = &g->map[p.x][p.y].object.robot;
	myDir = eu->dir;
	if(turno == 0)
		buscaMapa(g, p, myDir);
	inimigoPerto = nearestEnemy(g, p, myDir, 0);
	atualiza(g, p, myDir);
	//printf("here we go2!\n\n");
	if(sendoAlvejado(g, p, dirs, 2)){

		if(dirs[myDir]){
			printf("Virar para nao morrer\n");
			return turn(g, p, dirs, myDir);
			
		}
		else if(isValid(getNeighbor(p, myDir), g->m, g->m) && danoEstar(g, p, 0) < danoEstar(g, getNeighbor(p, myDir), 1)){
			printf("Ficar que nao vale a pena lutar\n");
			return STAND;
		}
		else if(isValid(getNeighbor(p, myDir), g->m, g->n)){
			printf("Corra para as colinas!\n");
			return WALK;
		}
		else{
			printf("Virar, fazer o que\n");
			return TURN_LEFT;
		}
		
	} 
	else if(sendoAlvejado(g, p, dirs, 1)){
		if(danoEstar(g, p, 0) < danoEstar(g, getNeighbor(p, myDir), 1)){
			printf("Virar para nao morrer2\n"); //ta dando errado
			return turn(g, p, dirs, myDir);
		}
		else if(isValid(getNeighbor(p, myDir), g->m, g->n)){
			printf("Corra para as colinas2!\n");
			return WALK;
		}
		else{
			printf("Ficar que nao vale a pena lutar\n");
			return STAND;
		}
	}
	else if(distancia(p, inimigoPerto.pos) < 4 && inimigoPerto.balas >=4 && inimigoPerto.mirandoEmMim){
		if(isValid(getNeighbor(p, myDir), g->m, g->n)){
			printf("Fuja dele!\n");
			return WALK;
		}
		else{
			printf("Virando para fugir dele\n");
			return turn(g, p, dirs, myDir);
		}
	}
	inimigoPerto = nearestEnemy(g, p, myDir, 0);
	findOfensiveSpots(g, p, posOfensiva, inimigoPerto);
	if(rayCast(g, inimigoPerto.pos, p, myDir, 4, &di)){
		printf("MATAAR\n");
		if(di == 0)
			return SHOOT_CENTER;
		else if(di == 1)
			return SHOOT_RIGHT;
		else
			return SHOOT_LEFT;
	}
	else if(g->map[p.x][p.y].isControlPoint){
		printf("Coletando uns pontinhos\n");
		return STAND;
	}
	inimigoPerto = nearestEnemy(g, p, myDir, 0);
	findOfensiveSpots(g, p, posOfensiva, inimigoPerto);
	if(rayCast(g, inimigoPerto.pos, p, myDir, 4, &di)){
		printf("MATAAR\n");
		if(di == 0)
			return SHOOT_CENTER;
		else if(di == 1)
			return SHOOT_RIGHT;
		else
			return SHOOT_LEFT;
	}
	if(eu->bullets < 4){
		printf("preciso recarregar\n");
		obj = shortestPath(g, p, nearest(g, p, controles, 10, myDir));
		//printf("Quero %d  %d\n", nearest(g, p, controles, 10, myDir).x, nearest(g, p, controles, 10, myDir).y);
		//printf("Estou %d  %d\n", p.x, p.y);
		//printf("%d\n", obj);
		
		//printf("%d %d\n", controles[0].x, controles[0].y)
		if(obj == myDir && isValid(getNeighbor(p, myDir), g->m, g->n)){
			printf("indo para cp\n");
			return WALK;
		}
		else{
			printf("virando para cp\n");
			return bestTurn(myDir, obj);
		}		
	}
	else if(distancia(p, nearest(g, p, posOfensiva, 6, myDir)) <= distancia(p, nearest(g, p, controles, 10, myDir)) && eu->bullets > 0){
		printf("Avante\n");
		obj = shortestPath(g, p, nearest(g, p, posOfensiva, 6, myDir));
		if(obj == myDir && isValid(getNeighbor(p, myDir), g->m, g->n)){
			printf("indo ateh ele\n");
			return WALK;
		}
		else{
			printf("virando apra ele\n");
			return bestTurn(myDir, obj);
		}
	}
	
	
	obj = shortestPath(g, p, nearest(g, p, controles, 10, myDir));
		
		
	//printf("%d %d\n", controles[0].x, controles[0].y)
	if(obj == myDir && isValid(getNeighbor(p, myDir), g->m, g->n)){
		printf("indo para cp\n");
		return WALK;
	}
	else{
		printf("virando para cp  %d\n", obj);
		return bestTurn(myDir, obj);
	}	
	
}
