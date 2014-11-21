#include "../robot_fight.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int bin;

int beingShooted(Grid *g, Position p);
int valid(Position p, int m, int n);
Action RunForestRun(Grid *g, Position p);

void prepareGame(Grid *g, Position p, int turnCount){
	bin = 1;
	setName("Defense");
}

Action processTurn(Grid *g, Position p, int turnsLeft){
	if(beingShooted(g, p)){
		return RunForestRun(g, p);
	}

	return STAND;
}

int beingShooted(Grid *g, Position p){
	Tile proj;
	if(p.y%2 == 0){
		if(g->map[p.x-1][p.y-1].type == PROJECTILE || g->map[p.x-1][p.y-1].type == ROBOT ){
			proj = g->map[p.x-1][p.y-1];
			if(proj.object.projectile.dir == BOTTOM_RIGHT || g->map[p.x-1][p.y-1].type == ROBOT)
				return 1;
		}
		
		if(g->map[p.x-1][p.y].type == PROJECTILE ||  g->map[p.x-1][p.y].type == ROBOT){
			proj = g->map[p.x-1][p.y];
			if(proj.object.projectile.dir == RIGHT || g->map[p.x-1][p.y].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x-1][p.y+1].type == PROJECTILE || g->map[p.x-1][p.y+1].type == ROBOT){
			proj = g->map[p.x-1][p.y+1];
			if(proj.object.projectile.dir == TOP_RIGHT || g->map[p.x-1][p.y+1].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x][p.y+1].type == PROJECTILE || g->map[p.x][p.y+1].type == ROBOT){
			proj = g->map[p.x][p.y+1];
			if(proj.object.projectile.dir == TOP_LEFT || g->map[p.x][p.y+1].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x+1][p.y].type == PROJECTILE || g->map[p.x+1][p.y].type == ROBOT){
			proj = g->map[p.x+1][p.y];
			if(proj.object.projectile.dir == LEFT || g->map[p.x+1][p.y].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x][p.y-1].type == PROJECTILE || g->map[p.x][p.y-1].type == ROBOT){
			proj = g->map[p.x][p.y-1];
			if(proj.object.projectile.dir == BOTTOM_LEFT || g->map[p.x][p.y-1].type == ROBOT) 
			return 1;
		}
	}

	else{
		if(g->map[p.x][p.y-1].type == PROJECTILE || g->map[p.x][p.y-1].type == ROBOT){
			proj = g->map[p.x][p.y-1];
			if(proj.object.projectile.dir == BOTTOM_RIGHT || g->map[p.x][p.y-1].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x-1][p.y].type == PROJECTILE || g->map[p.x-1][p.y].type == ROBOT){
			proj = g->map[p.x-1][p.y];
			if(proj.object.projectile.dir == RIGHT || g->map[p.x-1][p.y].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x][p.y+1].type == PROJECTILE || g->map[p.x][p.y+1].type == ROBOT){
			proj = g->map[p.x][p.y+1];
			if(proj.object.projectile.dir == TOP_RIGHT || g->map[p.x][p.y+1].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x+1][p.y+1].type == PROJECTILE || g->map[p.x+1][p.y+1].type == ROBOT){
			proj = g->map[p.x+1][p.y+1];
			if(proj.object.projectile.dir == TOP_LEFT || g->map[p.x+1][p.y+1].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x+1][p.y].type == PROJECTILE || g->map[p.x+1][p.y].type == ROBOT){
			proj = g->map[p.x+1][p.y];
			if(proj.object.projectile.dir == LEFT || g->map[p.x+1][p.y].type == ROBOT) 
			return 1;
		}

		if(g->map[p.x+1][p.y-1].type == PROJECTILE || g->map[p.x+1][p.y-1].type == ROBOT){
			proj = g->map[p.x+1][p.y-1];
			if(proj.object.projectile.dir == BOTTOM_LEFT || g->map[p.x+1][p.y-1].type == ROBOT) 
			return 1;
		}
	}
	return 0;
}

Action RunForestRun(Grid *g, Position p){
	Robot *Me = &g->map[p.x][p.y].object.robot;
	Position aux;
	if(p.y%2 == 0){
		if(g->map[p.x][p.y-1].type == PROJECTILE || g->map[p.x-1][p.y-1].type == PROJECTILE ||
		   g->map[p.x-1][p.y+1].type == PROJECTILE || g->map[p.x][p.y+1].type == PROJECTILE){
			if(Me->dir == 0){
				aux.x = p.x - 1;
				aux.y = p.y;
				if(valid(aux, 12, 10)){
					return WALK;
				}
				else{
					if(Me->obstacles > 0){
						if(g->map[p.x][p.y+1].type == PROJECTILE)
							return OBSTACLE_LEFT;
						if(g->map[p.x][p.y-1].type == PROJECTILE)
							return OBSTACLE_RIGHT;
					}
					return TURN_RIGHT;
				}
			}
			if(Me->dir == 2 || Me->dir == 5) return TURN_RIGHT;
			if(Me->dir == 1 || Me->dir == 4) return TURN_RIGHT;
			if(Me->dir == 3){
				aux.x = p.x + 1;
				aux.y = p.y;
				if(valid(aux, 12, 10)){
					return WALK;
				}
				else{
					if(Me->obstacles > 0){
						if(g->map[p.x-1][p.y+1].type == PROJECTILE)
							return OBSTACLE_RIGHT;
						if(g->map[p.x-1][p.y-1].type == PROJECTILE)
							return OBSTACLE_LEFT;
					}
					return TURN_RIGHT;
				}
			}
		}
			if(g->map[p.x-1][p.y].type == PROJECTILE){
				if(Me->dir == 0){
					if(bin){
					 	return TURN_RIGHT;		
						bin = 0;
					}
					else{
						return WALK;
						bin = 1;
					}
				}
				if(Me->dir == 3){	
					if(Me->obstacles > 0){
						return OBSTACLE_CENTER;
					}
					else{
						aux.x = p.x+1;
						aux.y = p.y;
						if(valid(aux, 12, 10)){
							return WALK;
						}
						else{
							return TURN_RIGHT;
						}
					}
				}
				if(Me->dir == 1){
					aux.x = p.x-1;
					aux.y = p.y-1;
					if(valid(aux, 12, 10))
						return WALK;
					else return TURN_RIGHT;
				}
				if(Me->dir == 2){
					aux.x = p.x;
					aux.y = p.y -1;
					if(valid(aux, 12, 10))
						return WALK;
					else return TURN_RIGHT;
				}
				if(Me->dir == 4){
					aux.x = p.x;
					aux.y = p.y+1;
					if(valid(aux, 12, 10))
						return WALK;
					else return TURN_RIGHT;
				}
				if(Me->dir == 5){
					aux.x = p.x - 1;
					aux.y = p.y +1;
					if(valid(aux, 12, 10))
						return WALK;
					else return TURN_RIGHT;
				}
			}
			if(g->map[p.x+1][p.y].type == PROJECTILE){
				if(Me->dir == 0){
					aux.x = p.x-1;
					aux.y = p.y;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_RIGHT;
				}
				if(Me->dir == 3){
					if(Me->obstacles > 0){
						return OBSTACLE_CENTER;
					}
					else return TURN_RIGHT;
				}
				if(Me->dir == 1){
					aux.x = p.x-1;
					aux.y = p.y-1;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_LEFT;
				}
				if(Me->dir == 2){
					aux.x = p.x;
					aux.y = p.y-1;
					if(valid(aux, 12, 10))
						return WALK;
					else 
						return TURN_LEFT;
				}
				if(Me->dir == 4){
					aux.x = p.x;
					aux.y = p.y + 1;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_RIGHT;
				}
				if(Me->dir == 5){
					aux.x = p.x - 1;
					aux.y = p.y + 1;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_LEFT; 
				}
			}	
		return WALK;
	}

	else{
		if(g->map[p.x][p.y-1].type == PROJECTILE || g->map[p.x][p.y+1].type == PROJECTILE ||
		   g->map[p.x+1][p.y+1].type == PROJECTILE || g->map[p.x+1][p.y-1].type == PROJECTILE){
			if(Me->dir == 0){
				aux.x = p.x-1;
				aux.y = p.y;
				if(valid(aux, 12, 10)){
					return WALK;
				}
				else{
					if(Me->obstacles > 0){
						if(g->map[p.x+1][p.y+1].type == PROJECTILE)
							return OBSTACLE_LEFT;
						if(g->map[p.x+1][p.y-1].type == PROJECTILE)
							return OBSTACLE_RIGHT;
					}
					return TURN_RIGHT;
				}
			}
			if(Me->dir == 2 || Me->dir == 5) return TURN_RIGHT;
			if(Me->dir == 1 || Me->dir == 4) return TURN_RIGHT;
			if(Me->dir == 3){
				aux.x = p.x + 1;
				aux.y = p.y;
				if(valid(aux, 12, 10)){
					return WALK;
				}
				else{
					if(Me->obstacles > 0){
						if(g->map[p.x][p.y-1].type == PROJECTILE)
							return OBSTACLE_LEFT;
						if(g->map[p.x][p.y+1].type == PROJECTILE)
							return OBSTACLE_RIGHT;
					}
					return TURN_RIGHT;
				}
			}

		}

		if(g->map[p.x-1][p.y].type == PROJECTILE){
			if(Me->dir == 1){
				aux.x = p.x;
				aux.y = p.y - 1;
				if(valid(aux, 12, 10))
					return WALK;
				else return TURN_RIGHT;
			}

			if(Me->dir == 2){
				aux.x = p.x + 1;
				aux.y = p.y - 1;
				if(valid(aux, 12, 10))
					return WALK;
				else return TURN_RIGHT;
			}

			if(Me->dir == 4){
				aux.x = p.x + 1;
				aux.y = p.y + 1;
				if(valid(aux, 12, 10))
					return WALK;
				else return TURN_RIGHT;
			}

			if(Me->dir == 5){
				aux.x = p.x;
				aux.y = p.y + 1;
				if(valid(aux, 12, 10))
					return WALK;
				else return TURN_RIGHT;
			}

			if(Me->dir == 3){	
				if(Me->obstacles > 0){
					return OBSTACLE_CENTER;
				}
				else{
					aux.x = p.x+1;
					aux.y = p.y;
					if(valid(aux, 12, 10)){
						return WALK;
					}
					else{
						return TURN_LEFT;
					}
				}
			}
			if(Me->dir == 0){
				if(bin){
				 	return TURN_RIGHT;		
					bin = 0;
				}
				else{
					return WALK;
					bin = 1;
				}
			}	
		}

		if(g->map[p.x+1][p.y].type == PROJECTILE){
				if(Me->dir == 0){
					aux.x = p.x - 1;
					aux.y = p.y;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_RIGHT;
				}
				if(Me->dir == 3){
					if(Me->obstacles > 0){
						return OBSTACLE_CENTER;
					}
					else return TURN_RIGHT;
				}
				if(Me->dir == 1){
					aux.x = p.x;
					aux.y = p.y-1;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_LEFT;
				}
				if(Me->dir == 2){
					aux.x = p.x + 1;
					aux.y = p.y - 1;
					if(valid(aux, 12, 10))
						return WALK;
					else 
						return TURN_LEFT;
				}
				if(Me->dir == 4){
					aux.x = p.x + 1;
					aux.y = p.y + 1;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_RIGHT;
				}
				if(Me->dir == 5){
					aux.x = p.x;
					aux.y = p.y + 1;
					if(valid(aux, 12, 10))
						return WALK;
					else
						return TURN_LEFT; 
				}
			}
		return WALK;
	}
	return WALK;
}

int valid(Position p, int m, int n){
	return p.x >= 0 && p.x < m && p.y >= 0 && p.y < n;
}