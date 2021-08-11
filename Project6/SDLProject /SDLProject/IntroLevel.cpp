#include "IntroLevel.h"

#define OBJECT_COUNT 80
#define ENEMY_COUNT 0

void IntroLevel::Initialize() {
    
    state.nextScene = -1;
    
    // INITIALIZE PLAYER
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-9, -0.25, -7.4);
    state.player->rotation = glm::vec3(0, -50.0f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->scale = glm::vec3(0.2, 2.0, 0.2);
    state.player->speed = 1.0f;
    state.player->weight = 40.0f;
    state.player->energy = 50.0f;
    state.player->health = 10.0f;
    
    // INITIALIZE WORLD
    
    state.objects = new Entity[OBJECT_COUNT];
    
    Mesh *floorMesh = new Mesh();
    floorMesh->LoadOBJ("Assets/cube.obj", 20);
    
    Mesh *cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("Assets/cube.obj", 1);
    
    Mesh *leftCornerRoadMesh = new Mesh();
    leftCornerRoadMesh->LoadOBJ("Assets/City OBJ format/roadAsphalt_cornerInner.obj", 1);
    
    Mesh *leftCornerInnerRoadMesh = new Mesh();
    leftCornerInnerRoadMesh->LoadOBJ("Assets/City OBJ format/roadAsphalt_cornerOuter.obj", 1);
    
    Mesh *straightRoadMesh = new Mesh();
    straightRoadMesh->LoadOBJ("Assets/City OBJ format/roadAsphalt_straight.obj", 1);
    
    Mesh *sideRoadMesh = new Mesh();
    sideRoadMesh->LoadOBJ("Assets/City OBJ format/roadAsphalt_side.obj", 1);
    
    Mesh *pavementMesh = new Mesh();
    pavementMesh->LoadOBJ("Assets/City OBJ format/roadAsphalt_pavement.obj", 1);
    
    Mesh *bigTreeMesh = new Mesh();
    bigTreeMesh->LoadOBJ("Assets/City OBJ format/treePine_large.obj", 1);
    
    Mesh *smallTreeMesh = new Mesh();
    smallTreeMesh->LoadOBJ("Assets/City OBJ format/treePine_large.obj", 1);
    
    Mesh *shrub1Mesh = new Mesh();
    shrub1Mesh->LoadOBJ("Assets/City OBJ format/tree_shrub.obj", 1);
    
    //    Mesh *shrub2Mesh = new Mesh();
    //    shrub2Mesh->LoadOBJ("Assets/City OBJ format/treePine_large.obj", 1);
    
    Mesh *openDumptserMesh = new Mesh();
    openDumptserMesh->LoadOBJ("Assets/City OBJ format/detailDumpster_open.obj", 1);
    
    Mesh *closedDumptserMesh = new Mesh();
    closedDumptserMesh->LoadOBJ("Assets/City OBJ format/detailDumpster_closed.obj", 1);
    
    Mesh *truckMesh = new Mesh();
    truckMesh->LoadOBJ("Assets/City OBJ format/truck_grey.obj", 1);
    
    Mesh *gasStationPolesMesh = new Mesh();
    gasStationPolesMesh->LoadOBJ("Assets/City OBJ format/scaffolding_poles.obj", 1);
    
    Mesh *barrierMesh = new Mesh();
    barrierMesh->LoadOBJ("Assets/City OBJ format/detailBarrierStrong_typeA.obj", 1);
    
    GLuint backgroundTextureID = Util::LoadTexture("Assets/atlanta-backgrd.jpeg");
        
    GLuint dirtTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/dirt.png");
    GLuint concreteTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/concreteSmooth.png");
    GLuint concrete2TextureID = Util::LoadTexture("Assets/City OBJ format/Textures/concrete.png");
    GLuint asphaltTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/asphalt.png");
    GLuint grassTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/grass.png");
    GLuint metalTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/metal.png");
    
    GLuint brickWallTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/wall_lines.png");
    GLuint garageDoorTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/wall_garage.png");
    GLuint roofTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/roof_plates.png");
    //GLuint barsTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/bars.png");
    
    GLuint pineTreeTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/treeB.png");
    GLuint shrub1TextureID = Util::LoadTexture("Assets/City OBJ format/Textures/treeB.png");
    GLuint shrub2TextureID = Util::LoadTexture("Assets/City OBJ format/Textures/treeA.png");
    
    GLuint dumpsterTextureID = Util::LoadTexture("Assets/City OBJ format/Textures/wall_metal.png");
    GLuint detailTruck1TextureID = Util::LoadTexture("Assets/City OBJ format/Textures/truck.png");
    GLuint detailTruck2TextureID = Util::LoadTexture("Assets/City OBJ format/Textures/truck_alien.png");

    // dirt floor
    state.objects[0].textureID = dirtTextureID;
    state.objects[0].mesh = floorMesh;
    state.objects[0].position = glm::vec3(0, -0.5f, 0);
    state.objects[0].entityType = FLOOR;
    state.objects[0].scale = glm::vec3(20, 0.1f, 20);
    
    // gas station FLOOR
    state.objects[1].textureID = concreteTextureID;
    state.objects[1].mesh = cubeMesh;
    state.objects[1].position = glm::vec3(0, -0.48, -9.25);
    state.objects[1].entityType = PAVEMENT;
    state.objects[1].scale = glm::vec3(7, 0.1, 1.5);
    
    // gas station center drive up
    state.objects[2].textureID = concreteTextureID;
    state.objects[2].mesh = straightRoadMesh;
    state.objects[2].position = glm::vec3(0, -0.48, -7.75);
    state.objects[2].entityType = PAVEMENT;
    state.objects[2].scale = glm::vec3(3, 1, 1);
    
    // left corners
    state.objects[3].textureID = concreteTextureID;
    state.objects[3].mesh = leftCornerRoadMesh;
    state.objects[3].position = glm::vec3(-3, -0.48, -8.25);
    state.objects[3].entityType = PAVEMENT;
    
    state.objects[4].textureID = concreteTextureID;
    state.objects[4].mesh = leftCornerInnerRoadMesh;
    state.objects[4].position = glm::vec3(-2, -0.48, -7.25);
    state.objects[4].entityType = PAVEMENT;
    
    // right corners
    state.objects[5].textureID = concreteTextureID;
    state.objects[5].mesh = leftCornerRoadMesh;
    state.objects[5].position = glm::vec3(3, -0.48, -8.25);
    state.objects[5].entityType = PAVEMENT;
    state.objects[5].scale = glm::vec3(-1, 1, 1);
    
    state.objects[6].textureID = concreteTextureID;
    state.objects[6].mesh = leftCornerInnerRoadMesh;
    state.objects[6].position = glm::vec3(2, -0.48, -7.25);
    state.objects[6].entityType = PAVEMENT;
    state.objects[6].scale = glm::vec3(-1, 1, 1);
    
    // center extention
    state.objects[7].textureID = concreteTextureID;
    state.objects[7].mesh = sideRoadMesh;
    state.objects[7].position = glm::vec3(2, -0.48, -8.75);
    state.objects[7].entityType = PAVEMENT;
    
    state.objects[8].textureID = concreteTextureID;
    state.objects[8].mesh = sideRoadMesh;
    state.objects[8].position = glm::vec3(-2, -0.48, -8.75);
    state.objects[8].entityType = PAVEMENT;
    
    // corner extentions
        // left
    state.objects[9].textureID = concreteTextureID;
    state.objects[9].mesh = sideRoadMesh;
    state.objects[9].position = glm::vec3(-3.5, -0.48, -6.75);
    state.objects[9].entityType = PAVEMENT;
    state.objects[9].rotation = glm::vec3(0, 90, 0);
    state.objects[9].scale = glm::vec3(1, 1, 2);
        // right top corner
    state.objects[10].textureID = concreteTextureID;
    state.objects[10].mesh = leftCornerInnerRoadMesh;
    state.objects[10].position = glm::vec3(3, -0.48, -7.25);
    state.objects[10].entityType = PAVEMENT;
    state.objects[10].rotation = glm::vec3(0, 90, 0);
        // right bottom corner
    state.objects[11].textureID = concreteTextureID;
    state.objects[11].mesh = leftCornerInnerRoadMesh;
    state.objects[11].position = glm::vec3(3, -0.48, -6.25);
    state.objects[11].entityType = PAVEMENT;
    state.objects[11].rotation = glm::vec3(0, 270, 0);
    state.objects[11].scale = glm::vec3(-1, 1, 1);
    
    // big pavement bottom center
    state.objects[12].textureID = concreteTextureID;
    state.objects[12].mesh = cubeMesh;
    state.objects[12].position = glm::vec3(0, -0.48, -6.25);
    state.objects[12].entityType = PAVEMENT;
    state.objects[12].scale = glm::vec3(4, 0.1, 1);
    
    // big pavement left
    state.objects[13].textureID = concreteTextureID;
    state.objects[13].mesh = cubeMesh;
    state.objects[13].position = glm::vec3(-5.5, -0.48, -7.875);
    state.objects[13].entityType = PAVEMENT;
    state.objects[13].scale = glm::vec3(4, 0.1, 4.25);
    // big pavement right
    state.objects[14].textureID = concreteTextureID;
    state.objects[14].mesh = cubeMesh;
    state.objects[14].position = glm::vec3(5.5, -0.48, -8.625);
    state.objects[14].entityType = PAVEMENT;
    state.objects[14].scale = glm::vec3(4, 0.1, 2.75);
    
    // trees
    state.objects[15].textureID = pineTreeTextureID;
    state.objects[15].mesh = bigTreeMesh;
    state.objects[15].position = glm::vec3(-8.5, -0.48, -7);
    state.objects[15].entityType = HIDE;
    
    state.objects[16].textureID = pineTreeTextureID;
    state.objects[16].mesh = bigTreeMesh;
    state.objects[16].position = glm::vec3(-8.5, -0.48, -9);
    state.objects[16].entityType = HIDE;
    
    // shrubs
    state.objects[17].textureID = shrub1TextureID;
    state.objects[17].mesh = shrub1Mesh;
    state.objects[17].position = glm::vec3(-7.5, -0.48, -9);
    state.objects[17].entityType = HIDE;
    state.objects[17].scale = glm::vec3(1, 1, 5);
    
    state.objects[18].textureID = shrub1TextureID;
    state.objects[18].mesh = shrub1Mesh;
    state.objects[18].position = glm::vec3(-7.5, -0.48, -7);
    state.objects[18].entityType = HIDE;
    state.objects[18].scale = glm::vec3(1, 1, 5);
    
    // dumpsters
    
    state.objects[19].textureID = dumpsterTextureID;
    state.objects[19].mesh = openDumptserMesh;
    state.objects[19].position = glm::vec3(5, -0.47, -9);
    state.objects[19].rotation = glm::vec3(0, 135, 0);
    state.objects[19].entityType = DUMPSTER;
    state.objects[19].weight = 10000.0f;
    state.objects[19].width = 0.75;
    state.objects[19].depth = 0.75;
    
    state.objects[20].textureID = dumpsterTextureID;
    state.objects[20].mesh = closedDumptserMesh;
    state.objects[20].position = glm::vec3(6, -0.47, -9.25);
    state.objects[20].rotation = glm::vec3(0, 45, 0);
    state.objects[20].entityType = DUMPSTER;
    state.objects[20].weight = 10000.0f;
    state.objects[20].width = 0.75;
    state.objects[20].depth = 0.75;
    
    // detail trucks
    
    state.objects[21].textureID = detailTruck1TextureID;
    state.objects[21].mesh = truckMesh;
    state.objects[21].position = glm::vec3(-4.5, -0.47, -8);
    state.objects[21].entityType = OBSTACLE;
    state.objects[21].weight = 10000.0f;
    state.objects[21].depth = 2.0f;
    
    state.objects[22].textureID = detailTruck2TextureID;
    state.objects[22].mesh = truckMesh;
    state.objects[22].position = glm::vec3(-6.25, -0.47, -8.25);
    state.objects[22].entityType = OBSTACLE;
    state.objects[22].weight = 10000.0f;
    state.objects[22].depth = 2.0;
    
    // gas station
    
    state.objects[23].textureID = metalTextureID;
    state.objects[23].mesh = gasStationPolesMesh;
    state.objects[23].position = glm::vec3(-2.45, -0.47, -9.5);
    state.objects[23].entityType = FLOOR;
    state.objects[23].scale = glm::vec3(1, 1.25, 1);
    
    state.objects[24].textureID = metalTextureID;
    state.objects[24].mesh = gasStationPolesMesh;
    state.objects[24].position = glm::vec3(2.45, -0.47, -9.5);
    state.objects[24].entityType = FLOOR;
    state.objects[24].scale = glm::vec3(1, 1.25, 1);
    
    state.objects[25].textureID = roofTextureID;
    state.objects[25].mesh = cubeMesh;
    state.objects[25].position = glm::vec3(0, 0.75, -9.25);
    state.objects[25].entityType = FLOOR;
    state.objects[25].scale = glm::vec3(7, 0.25, 1.5);
    
    state.objects[26].textureID = brickWallTextureID;
    state.objects[26].mesh = cubeMesh;
    state.objects[26].position = glm::vec3(0, 0.1, -9.5);
    state.objects[26].entityType = OBSTACLE;
    state.objects[26].scale = glm::vec3(4, 1.25, 1);
    state.objects[26].weight = 10000.0f;
    state.objects[26].depth = 1.5f;

    state.objects[27].textureID = garageDoorTextureID;
    state.objects[27].mesh = cubeMesh;
    state.objects[27].position = glm::vec3(0, 0.025, -9.45);
    state.objects[27].entityType = FLOOR;
    
    // no where road
    state.objects[28].textureID = concreteTextureID;
    state.objects[28].mesh = sideRoadMesh;
    state.objects[28].position = glm::vec3(5.5, -0.48, -6.25);
    state.objects[28].entityType = PAVEMENT;
    state.objects[28].scale = glm::vec3(4, 1, 1);
    
    state.objects[29].textureID = concreteTextureID;
    state.objects[29].mesh = straightRoadMesh;
    state.objects[29].position = glm::vec3(8.75, -0.48, -6.75);
    state.objects[29].entityType = PAVEMENT;
    state.objects[29].scale = glm::vec3(2.5, 1, 1);
    
    // asphalt
    state.objects[30].textureID = asphaltTextureID;
    state.objects[30].mesh = floorMesh;
    state.objects[30].position = glm::vec3(0, -0.49f, -7.88);
    state.objects[30].entityType = FLOOR;
    state.objects[30].scale = glm::vec3(20, 0.1f, 4.25);
    
    // grass
    state.objects[31].textureID = grassTextureID;
    state.objects[31].mesh = floorMesh;
    state.objects[31].position = glm::vec3(-8.75, -0.48f, -7.88);
    state.objects[31].entityType = FLOOR;
    state.objects[31].scale = glm::vec3(2.5, 0.1f, 4.25);

    state.objects[32].textureID = grassTextureID;
    state.objects[32].mesh = floorMesh;
    state.objects[32].position = glm::vec3(8.75, -0.48f, -8.88);
    state.objects[32].entityType = FLOOR;
    state.objects[32].scale = glm::vec3(2.5, 0.1f, 2.25);
    
    // barriers
    for (int i = 0; i < 9; i++) {
        state.objects[33 + i].textureID = concrete2TextureID;
        state.objects[33 + i].mesh = barrierMesh;
        state.objects[33 + i].position = glm::vec3(-2.5 - i, -0.44f, -9.75);
        state.objects[33 + i].entityType = OBSTACLE;
        state.objects[33 + i].weight = 10000.0f;
        state.objects[33 + i].depth = 0.5;
    }
    for (int i = 0; i < 8; i++) {
        state.objects[41 + i].textureID = concrete2TextureID;
        state.objects[41 + i].mesh = barrierMesh;
        state.objects[41 + i].position = glm::vec3(2.5 + i, -0.44f, -9.75);
        state.objects[41 + i].entityType = OBSTACLE;
        state.objects[41 + i].weight = 10000.0f;
        state.objects[41 + i].depth = 0.5;
    }
    for (int i = 0; i < 4; i++) {
        state.objects[49 + i].textureID = concrete2TextureID;
        state.objects[49 + i].mesh = barrierMesh;
        state.objects[49 + i].position = glm::vec3(-9.75, -0.44f, -6.5 - i);
        state.objects[49 + i].entityType = OBSTACLE;
        state.objects[49 + i].weight = 10000.0f;
        state.objects[49 + i].width = 0.5;
        state.objects[49 + i].rotation.y = 90.0f;
    }
    for (int i = 0; i < 3; i++) {
        state.objects[53 + i].textureID = concrete2TextureID;
        state.objects[53 + i].mesh = barrierMesh;
        state.objects[53 + i].position = glm::vec3(9.75, -0.44f, -9.65 + i);
        state.objects[53 + i].entityType = OBSTACLE;
        state.objects[53 + i].weight = 10000.0f;
        state.objects[53 + i].width = 0.5;
        state.objects[53 + i].rotation.y = 90.0f;
    }
    for (int i = 0; i < 7; i++) {
        state.objects[56 + i].textureID = concrete2TextureID;
        state.objects[56 + i].mesh = barrierMesh;
        state.objects[56 + i].position = glm::vec3(-9.5 + i, -0.44f, -6);
        state.objects[56 + i].entityType = OBSTACLE;
        state.objects[56 + i].weight = 10000.0f;
        state.objects[56 + i].depth = 0.5;
    }
    for (int i = 0; i < 7; i++) {
        state.objects[63 + i].textureID = concrete2TextureID;
        state.objects[63 + i].mesh = barrierMesh;
        state.objects[63 + i].position = glm::vec3(3.5 + i, -0.44f, -6);
        state.objects[63 + i].entityType = OBSTACLE;
        state.objects[63 + i].weight = 10000.0f;
        state.objects[63 + i].depth = 0.5;
    }
    for (int i = 0; i < 3; i++) {
        state.objects[70 + i].textureID = concrete2TextureID;
        state.objects[70 + i].mesh = barrierMesh;
        state.objects[70 + i].position = glm::vec3(-1 + i, -0.44f, -6);
        state.objects[70 + i].entityType = OBSTACLE;
        state.objects[70 + i].weight = 10000.0f;
        state.objects[70 + i].depth = 0.5;
    }
    // background
    state.objects[73].textureID = backgroundTextureID;
    state.objects[73].mesh = cubeMesh;
    state.objects[73].position = glm::vec3(0, 6, 0);
    state.objects[73].entityType = FLOOR;
    state.objects[73].scale = glm::vec3(25, 15, 25);
    
    state.objects[74].textureID = asphaltTextureID;
    state.objects[74].mesh = cubeMesh;
    state.objects[74].position = glm::vec3(10, -0.49, -6.75);
    state.objects[74].entityType = FLOOR;
    state.objects[74].scale = glm::vec3(3.5, 0.1, 1);
    state.objects[74].rotation = glm::vec3(0, 0, 45);

    // INITIALIZE FOOD
    
    GLuint foodTextureID = Util::LoadTexture("Assets/green.jpeg");
    
//    int totalTrash = 5;
//
//    int numMeat = 1;
//    int numHotDog = 1;
//    totalTrash -= numMeat + numHotDog;
//
//    float max = totalTrash;
//    float min = 0;
//    float r = ((float)rand() / RAND_MAX) * (max - min) + min;
//
//    int numFishBones = r;
//    totalTrash -= numFishBones;
//    int numCheese = 0;
//    totalTrash -= numCheese;
//    int numBanana = totalTrash;
    
    // dumpster loction:
    glm::vec3 dumpster1 = state.objects[19].position;
    glm::vec3 dumpster2 = state.objects[20].position;
    glm::vec3 avgdumpster = glm::vec3((dumpster1.x + dumpster2.x) / 2, (dumpster1.y + dumpster2.y) / 2, (dumpster1.z + dumpster2.z) / 2);
    
    // food types
    
    Mesh *bananaMesh = new Mesh();
    bananaMesh->LoadOBJ("Assets/Food OBJ format/banana.obj", 1);
    
    Mesh *cheeseMesh = new Mesh();
    cheeseMesh->LoadOBJ("Assets/Food OBJ format/cheeseCut.obj", 1);
    
    Mesh *fishBonesMesh = new Mesh();
    fishBonesMesh->LoadOBJ("Assets/Food OBJ format/fishBones.obj", 1);
    
    Mesh *meatMesh = new Mesh();
    meatMesh->LoadOBJ("Assets/Food OBJ format/meatCooked.obj", 1);
    
    Mesh *hotDogMesh = new Mesh();
    hotDogMesh->LoadOBJ("Assets/Food OBJ format/hotDog.obj", 1);
    
    state.objects[75].textureID = foodTextureID;
    state.objects[75].mesh = bananaMesh;
    state.objects[75].position = avgdumpster;
    state.objects[75].position.z += 1.2;
    state.objects[75].position.y += 0.07;
    state.objects[75].entityType = TRASH;
    state.objects[75].scale = glm::vec3(0.25);
    state.objects[75].depth = 1;
    state.objects[75].width = 1;
    state.objects[75].height = 1;
    state.objects[75].rotation = glm::vec3(0, 65, 90);
    state.objects[75].weight = 5.0f;
    state.objects[75].energy = 15.0f;
    
    state.objects[76].textureID = foodTextureID;
    state.objects[76].mesh = cheeseMesh;
    state.objects[76].position = avgdumpster;
    state.objects[76].position.z += 0.8;
    state.objects[76].position.y += 0.07;
    state.objects[76].position.x += 0.5;
    state.objects[76].entityType = TRASH;
    state.objects[76].scale = glm::vec3(0.25);
    state.objects[75].depth = 1;
    state.objects[75].width = 1;
    state.objects[75].height = 1;
    state.objects[76].rotation = glm::vec3(0, 145, 0);
    state.objects[76].weight = 5.0f;
    state.objects[76].energy = 12.0f;
    
    state.objects[77].textureID = foodTextureID;
    state.objects[77].mesh = fishBonesMesh;
    state.objects[77].position = avgdumpster;
    state.objects[77].position.z += 0.9;
    state.objects[77].position.y += 0.07;
    state.objects[77].position.x -= 0.5;
    state.objects[77].entityType = TRASH;
    state.objects[77].scale = glm::vec3(0.25);
    state.objects[75].depth = 1;
    state.objects[75].width = 1;
    state.objects[75].height = 1;
    state.objects[77].rotation = glm::vec3(0, 45, 0);
    state.objects[77].weight = 5.0f;
    state.objects[77].energy = 5.0f;
    
    state.objects[78].textureID = foodTextureID;
    state.objects[78].mesh = meatMesh;
    state.objects[78].position = avgdumpster;
    state.objects[78].position.z += 0.6;
    state.objects[78].position.y += 0.07;
    state.objects[78].position.x += 0.3;
    state.objects[78].entityType = TRASH;
    state.objects[78].scale = glm::vec3(0.25);
    state.objects[75].depth = 1;
    state.objects[75].width = 1;
    state.objects[75].height = 1;
    state.objects[78].rotation = glm::vec3(0, 120, 0);
    state.objects[78].weight = 5.0f;
    state.objects[78].energy = 25.0f;
    
    state.objects[79].textureID = foodTextureID;
    state.objects[79].mesh = hotDogMesh;
    state.objects[79].position = avgdumpster;
    state.objects[79].position.z += 1.0;
    state.objects[79].position.y += 0.07;
    state.objects[79].position.x -= 0.3;
    state.objects[79].entityType = TRASH;
    state.objects[79].scale = glm::vec3(0.25);
    state.objects[75].depth = 1;
    state.objects[75].width = 1;
    state.objects[75].height = 1;
    state.objects[79].rotation = glm::vec3(0, 275, 0);
    state.objects[79].weight = 5.0f;
    state.objects[79].energy = 20.0f;
    
    // INITIALIZE ANIMAL CONTROL
    
    // INITIALIZE RATS
    state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint enemyTextureID = Util::LoadTexture("Assets/billb.png");
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
         state.enemies[i].billboard = true;
         state.enemies[i].textureID = enemyTextureID;
         state.enemies[i].position = glm::vec3(rand() % 20 - 10, 0, rand() % 20 - 10);
         state.enemies[i].rotation = glm::vec3(0, 0, 0);
         state.enemies[i].acceleration = glm::vec3(0, 0, 0);
    }
}

void IntroLevel::Update(float deltaTime) {
    
    state.player->Update(deltaTime, state.player, state.objects, OBJECT_COUNT);
    
    for(int i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT);
    }
    for(int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.objects, OBJECT_COUNT);
    }
}

void IntroLevel::Render(ShaderProgram *program) {
    
    for(int i = 0; i < OBJECT_COUNT; i++) {
        state.objects[i].Render(program);
    }
    for(int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}
