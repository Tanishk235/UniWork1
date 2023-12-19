#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct connection connection; // defining the structure and the descriptions of the room
typedef struct room {
  char* name;
  char* description;
  connection* connections;
  int numConnections;
} room;

typedef struct connection {
  room* room1;
  room* room2;
} connection;

typedef struct player {
  room* currentRoom;
} player;

typedef struct game {
  room* rooms;
  int numRooms;
  player* player;
  time_t startTime; // New feature: Timer
} game;

int getMenuChoice() { // displays the initial options to choose
  int choice;
  printf("What would you like to do?\n");
  printf("1. Engage in a room reconnaissance tour\n");
  printf("2. Move to another room\n");
  printf("3. Quit the game\n");
  scanf("%d", &choice);

  while (choice < 1 || choice > 3) {
    printf("Invalid choice, please try again\n");
    scanf("%d", &choice);
  }
  return choice;
}

void printRoomDescription(room* room) { // 
  printf("You are in the %s.\n", room->name);
  printf("%s\n", room->description);
}

int getRoomChoice(room* currentRoom) {
  int choice;
  printf("Which room would you like to move to?\n");
  for (int i = 0; i < currentRoom->numConnections; i++) {
    printf("%d. %s\n", i+1, currentRoom->connections[i].room2->name);
  }
  scanf("%d", &choice);

  while (choice < 1 || choice > currentRoom->numConnections) {
    printf("Invalid choice, please try again\n");
    scanf("%d", &choice);
  }
  return choice;
}

void movePlayer(player* player, int choice) {
  player->currentRoom = player->currentRoom->connections[choice-1].room2;
  printRoomDescription(player->currentRoom);
}

room* loadRooms() {
  FILE* roomsFile = fopen("rooms.csv", "r");
  FILE* connectionsFile = fopen("connections.csv", "r");

  if (roomsFile == NULL || connectionsFile == NULL) {
    printf("Error opening files\n");
    exit(1);
  }

  int numLines = 0;
  char line[500];
  while (fgets(line, 500, roomsFile) != NULL) {
    numLines++;
  }

  rewind(roomsFile);

  room* rooms = malloc(sizeof(room) * numLines);

  for (int i = 0; i < numLines; i++) {
    fgets(line, 500, roomsFile);
    line[strlen(line)-1] = '\0';
    char* name = strtok(line, "\"");
    char* endofname = strtok(NULL, "\"");
    char* description = strtok(NULL, "\"");
    char* endofdesc = strtok(NULL, "\0");
    name[endofname - name] = '\0';

    room room;
    room.name = malloc(sizeof(char) * strlen(name) + 1);
    strcpy(room.name, name);
    room.description = malloc(sizeof(char) * strlen(description) + 1);
    strcpy(room.description, description);
    room.connections = NULL;
    room.numConnections = 0;
    rooms[i] = room;
  }

  for (int i = 0; i < numLines; i++) { // function to read, comprehend and connect rooms per the connections.csv file
    fgets(line, 500, connectionsFile);
    int connections[3];
    sscanf(line, "%d,%d,%d", &connections[0], &connections[1], &connections[2]);

    for (int j = 0; j < 3; j++) {
      connection newConnection;
      newConnection.room1 = &rooms[i];
      newConnection.room2 = &rooms[connections[j]];

      rooms[i].connections = realloc(rooms[i].connections, sizeof(connection) * (rooms[i].numConnections + 1));
      rooms[i].connections[rooms[i].numConnections] = newConnection;
      rooms[i].numConnections++;
    }
  }

  fclose(roomsFile);
  fclose(connectionsFile);

  return rooms;
}

player* createPlayer(room* currentRoom) {
  player* player = malloc(sizeof(player));
  player->currentRoom = currentRoom;
  return player;
}

game* createGame() {
  game* game = malloc(sizeof(game));
  game->rooms = loadRooms();
  game->numRooms = 10;
  game->player = createPlayer(&game->rooms[0]);
  game->startTime = time(NULL); // New feature: Start the timer
  return game;
}

void printElapsedTime(game* game) {
  time_t currentTime = time(NULL);
  int elapsedSeconds = difftime(currentTime, game->startTime);
  int minutes = elapsedSeconds / 60;
  int seconds = elapsedSeconds % 60;
  printf("Elapsed Time: %d minutes and %d seconds\n", minutes, seconds);
}

void playGame() {
  printf("Welcome to the game\n");
  game* game = createGame();
  printRoomDescription(game->player->currentRoom);

  bool quit = false;
  while (!quit) {
    int choice = getMenuChoice();
    if (choice == 1) {
      printRoomDescription(game->player->currentRoom);
    } else if (choice == 2) {
      int choice = getRoomChoice(game->player->currentRoom);
      movePlayer(game->player, choice);
    } else if (choice == 3) {
      quit = true;
      printElapsedTime(game); // New feature: Display elapsed time when quitting
    }
  }
}

int main() { // entry point to start the game
  playGame();
  return 0;
}
