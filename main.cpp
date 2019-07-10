#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <ctime>
#include <fstream>
using namespace std;

sf::RenderWindow window;
const int offsetY = 50;
const int tileSizeX = 30, tileSizeY = 30;
const int mapSizeX = 18, mapSizeY = 13;
const int windowSizeX = mapSizeX*tileSizeX, windowSizeY = offsetY + mapSizeY*tileSizeY;
const double updateTime = 20;

sf::Font font;
sf::Text text;
sf::Texture buttonFrameTexture;
sf::Sprite buttonFrameSprite;

struct ButtonClass {
    int posX, posY, sizeX, sizeY;
    int fontSize;
    string buttonText;
    bool hovered;

    ButtonClass () {}

    ButtonClass ( int posX_, int posY_, int sizeX_, int sizeY_, string text_ ) {

        posX = posX_;
        posY = posY_;
        sizeX = sizeX_;
        sizeY = sizeY_;
        buttonText = text_;
        hovered = 0;
        fontSize = 20;
    }
    void load () {
        hovered = 0;
    }

    void mouseMoveEvent ( int mouseX, int mouseY ) {
        if ( posX <= mouseX && mouseX <= posX + sizeX &&
             posY <= mouseY && mouseY <= posY + sizeY ) {
            hovered = 1;
         }else {
            hovered = 0;
         }
    }
    bool isClicked ( int mouseX, int mouseY ) {
        return hovered;
    }

    void draw () {

        buttonFrameSprite.setColor(sf::Color(255, 255, 255, 255) );
        buttonFrameSprite.setScale( sizeX/85., sizeY/85. );
        buttonFrameSprite.setPosition(sf::Vector2f(posX, posY));

        window.draw( buttonFrameSprite );


        if ( hovered ) {
            buttonFrameSprite.setColor(sf::Color(0, 0, 0, 128) );
            window.draw( buttonFrameSprite );
        }

        text.setString( buttonText );
        text.setPosition( posX + sizeX/2 - fontSize*buttonText.size()/4,
                            posY + sizeY/2 - fontSize/2 );
        text.setCharacterSize(fontSize);
        text.setFillColor(sf::Color::Black);
        window.draw(text);

    }

};

string toString ( int x, int cntDigits = -1 ) {
    string ret = "";
    if ( cntDigits == -1 ) {
        for ( int i = 0; x ; i ++, x /= 10 ) {
            ret += '0' + x%10;
        }
    }else {
        for ( int i = 0; i < cntDigits ; i ++, x /= 10 ) {
            ret += '0' + x%10;
        }
    }
    reverse( ret.begin(), ret.end() );
    return ret;
}

struct MapClass {
    string m[20];
    int minTo[20][20];

    int dotsLeft = 0;

    sf::Texture wallTexture, lineTexture, partialWallTexture, horizontalWallTexture, verticalWallTexture, dotTexture, powerPelletTexture;
    sf::Sprite wallSprite, lineSprite, partialWallSprite, horizontalWallSprite, verticalWallSprite, dotSprite, powerPelletSprite;

    MapClass () {
        if (!wallTexture.loadFromFile("images/wall.png") ) { cout << "Unable to load image\n"; }
        wallSprite.setTexture(wallTexture);

        if (!lineTexture.loadFromFile("images/line.png") ) { cout << "Unable to load image\n"; }
        lineSprite.setTexture(lineTexture);

        if (!partialWallTexture.loadFromFile("images/partialWall.png") ) { cout << "Unable to load image\n"; }
        partialWallSprite.setTexture(partialWallTexture);

        if (!horizontalWallTexture.loadFromFile("images/horizontalWall.png") ) { cout << "Unable to load image\n"; }
        horizontalWallSprite.setTexture(horizontalWallTexture);

        if (!verticalWallTexture.loadFromFile("images/verticalWall.png") ) { cout << "Unable to load image\n"; }
        verticalWallSprite.setTexture(verticalWallTexture);

        if (!dotTexture.loadFromFile("images/dot.png") ) { cout << "Unable to load image\n"; }
        dotSprite.setTexture(dotTexture);

        if (!powerPelletTexture.loadFromFile("images/powerPellet.png") ) { cout << "Unable to load image\n"; }
        powerPelletSprite.setTexture(powerPelletTexture);

    }

    void bfs ( int x, int y ) {
        x += mapSizeX;
        y += mapSizeY;
        x %= mapSizeX;
        y %= mapSizeY;

        for ( int i = 0 ; i < mapSizeY ; i ++ ) {
            for ( int j = 0 ; j < mapSizeX ; j ++ ) {
                minTo[i][j] = 1<<20;
            }
        }
        int dx[4] = { 0, 1, 0, -1 };
        int dy[4] = { 1, 0, -1, 0 };
        queue < pair < int, int > > reached;
        reached.push( make_pair( x,y ) );

        while ( !reached.empty() ) {
            int cx = reached.front().first, cy = reached.front().second;
            reached.pop();
            if ( minTo[ cy ][ cx ] < (1<<20) ) { continue; }

            if ( cy == y  && cx == x ) { minTo[y][x] = 0; }

            for ( int i = 0 ; i < 4 ; i ++ ) {
                if( cx + dx[i] >= 0 && cx + dx[i] < mapSizeX
                && cy + dy[i] >= 0 && cy + dy[i] < mapSizeY &&
                m[ cy+dy[i] ][ cx + dx[i] ] != '#' && m[ cy+dy[i] ][ cx + dx[i] ] != '-' ) {

                    minTo[cy][cx] = min( minTo[cy][cx], minTo[ cy+dy[i] ][ cx+dx[i] ] +1 );
                    reached.push( make_pair( cx+dx[i], cy+dy[i] ) );
                }
            }
        }

    }

    void genOne () {
        for ( int i = 0 ; i < mapSizeY ; i ++ ) {
            m[i] = "";
            for ( int j = 0 ; j < mapSizeX ; j ++ ) {
                m[i] += ' ';
            }
        }

        m[4] = "       ----       ";
        m[5] = "      #BBBB#      ";
        m[6] = "      ######      ";

        for ( int i = 0 ; i < mapSizeY ; i ++ ) {
            for ( int j = 0 ; j < mapSizeX ; j ++ ) {
                if ( m[i][j] != ' ' ) { continue; }

                if ( rand()%100 < 25 || i == 0 || j == 0 || j == mapSizeX-1 || i == mapSizeY-1 ) {
                    m[i][j] = '#';
                }else {
                    m[i][j] = ' ';
                }
            }
        }
        m[6][0] = ' ';
        m[6][mapSizeX-1] = ' ';
        m[3][7] = ' ';
        m[3][8] = ' ';
        m[3][9] = ' ';
        m[3][10] = ' ';

        bfs ( 0, 6 );

        for ( int i = 0 ; i < mapSizeY ; i ++ ) {
            for ( int j = 0 ; j < mapSizeX ; j ++ ) {
                if ( minTo[i][j] == (1<<20) && m[i][j] == ' ' ) {
                    m[i][j] = '#';
                }
            }
        }
    }
    bool isMapOk () {
        bfs ( 0, 6 );
        if ( minTo[3][7] == (1<<20) ) { return 0; }
        if ( minTo[6][mapSizeX-1] == (1<<20) ) { return 0; }

        string row5 = m[5], row7 = m[7];

        for ( int i = 0 ; i < mapSizeX ; i ++ ) { m[5][i] = '#'; }
        bfs( 0, 6 );
        if ( minTo[6][mapSizeX-1] == (1<<20) ) { return 0; }
        m[5] = row5;

        for ( int i = 0 ; i < mapSizeX ; i ++ ) { m[7][i] = '#'; }
        bfs( 0, 6 );
        if ( minTo[6][mapSizeX-1] == (1<<20) ) { return 0; }
        m[7] = row7;

        for ( int i = 1 ; i+1 < mapSizeY ;i ++ ) {
            for ( int j = 1 ; j+1 < mapSizeX ; j ++ ) {
                if ( m[i][j] == '#' && m[i-1][j] == '#' && m[i+1][j] == '#'
                    && m[i][j-1] == '#' && m[i][j+1] == '#' && m[i-1][j-1] == '#' && m[i+1][j-1] == '#'
                    && m[i-1][j+1] == '#' && m[i+1][j+1] == '#' ) {

                    return 0;
                }
            }
        }

        return 1;
    }

    int cntEmpty () {
        int ret = 0;
        for ( int i = 0 ; i < mapSizeY ; i ++ ) {
            for ( int j = 0 ; j < mapSizeX ; j ++ ) {
                if ( m[i][j] == ' ' ) { ret ++; }
            }
        }
        return ret;
    }

    void printMap () {

    }

    void generateRandom () {
        while ( 1 ) {
            genOne();
            if ( isMapOk() && cntEmpty() < 120 ) { break; }
        }
        int cEmpty = cntEmpty();
        int ppCnt = 4, ppId[20] = {};
        for ( int i = 0 ; i < ppCnt ; i ++ ) {
            ppId[i] = rand()%cEmpty;
        }


        for ( int i = 0 ; i < mapSizeY ; i ++ ) {
            for ( int j = 1 ; j < mapSizeX ; j ++ ) {
                if ( m[i][j] == ' ' ) {
                    m[i][j] = '.';

                    for ( int k = 0 ; k < ppCnt ; k ++ ) {
                        if ( ppId[k] == 0 ) {
                            m[i][j] = 'o';
                        }
                        ppId[k] --;
                    }

                }
            }
        }
    }
    void generateNormal () {

        m[0] = "##################";
        m[1] = "#o..#........#..o#";
        m[2] = "#.#...##..##...#.#";
        m[3] = "#.##.#......#.##.#";
        m[4] = "#..#...----...#..#";
        m[5] = "##.##.#BBBB#.##.##";
        m[6] = "......######......";
        m[7] = "##.##........##.##";
        m[8] = "##.#####..#####.##";
        m[9] = "#...##......##...#";
        m[10] ="#.#....####....#.#";
        m[11] ="#o..##......##..o#";
        m[12] ="##################";

    }

    void construct ( bool randomMap ) {

        if ( randomMap ) {
            generateRandom() ;
        }else {
            generateNormal() ;
        }

        for ( int i = 0 ; i < mapSizeY ; i ++ ) {
            for ( int j = 0 ; j < mapSizeX ; j ++ ) {
                if ( m[i][j] == '.' || m[i][j] == 'o' ) { dotsLeft ++; }
            }
        }
    }

    int playerOn ( int y, int x ) {
        if ( m[y][x] == '.' ) {
            m[y][x] = ' ';
            dotsLeft --;
            return 10;
        }
        if ( m[y][x] == 'o' ) {
            m[y][x] = ' ';
            dotsLeft --;
            return 50;
        }

        return 0;
    }

    void draw () {
        for ( int i = 0; i < mapSizeY ; i ++ ) {
            for ( int j = 0 ; j < mapSizeX ; j ++ ) {
                if ( m[i][j] == '#' ) {
                    bool hasHor = 0, hasVer = 0;

                    if ( ( i && m[i-1][j] == '#' ) || ( i +1 < mapSizeY && m[i+1][j] == '#' ) ) { hasVer = 1; }
                    if ( ( j && m[i][j-1] == '#' ) || ( j +1 < mapSizeX && m[i][j+1] == '#' ) ) { hasHor = 1; }

                    if ( !hasHor && !hasVer ) {
                        partialWallSprite.setPosition(sf::Vector2f( j*tileSizeX, i*tileSizeY + offsetY ));
                        window.draw( partialWallSprite );
                    }else if ( !hasHor && hasVer ) {
                        verticalWallSprite.setPosition(sf::Vector2f( j*tileSizeX, i*tileSizeY+ offsetY ));
                        window.draw( verticalWallSprite );
                    }else if ( hasHor && !hasVer ) {
                        horizontalWallSprite.setPosition(sf::Vector2f( j*tileSizeX, i*tileSizeY+ offsetY ));
                        window.draw( horizontalWallSprite );
                    }else {
                        wallSprite.setPosition(sf::Vector2f( j*tileSizeX, i*tileSizeY+ offsetY ));
                        window.draw( wallSprite );
                    }

                }else if ( m[i][j] == '.' ) {
                    dotSprite.setPosition(sf::Vector2f( j*tileSizeX, i*tileSizeY+ offsetY ));
                    window.draw( dotSprite );
                }else if ( m[i][j] == 'o' ) {
                    powerPelletSprite.setPosition(sf::Vector2f( j*tileSizeX, i*tileSizeY+ offsetY ));
                    window.draw( powerPelletSprite );
                }else if ( m[i][j] == '-' ) {
                    lineSprite.setPosition(sf::Vector2f( j*tileSizeX, i*tileSizeY+ offsetY ));
                    window.draw( lineSprite );
                }
            }
        }
    }
};

struct CharacterClass {

    bool difficultMode;
    int posX, posY, sizeX, sizeY;
    double frameInd;
    int frameCnt;
    int currentDirection, possibleNewDirection;
    int directionX, directionY, speed, defaultSpeed;
    int charId, timeWithPP, timeDead;
    bool inBox, isDead, withPowerPellet;

    sf::Texture texture, deadTexture, powerPelletTexture;
    sf::Sprite sprite, deadSprite, powerPelletSprite;

    CharacterClass () {
        if (!deadTexture.loadFromFile("images/dead.png") ) {
            cout << "Unable to load image\n";
        }
        deadSprite.setTexture(deadTexture);

        if (!powerPelletTexture.loadFromFile("images/powerPelletGhost.png") ) {
            cout << "Unable to load image\n";
        }
        powerPelletSprite.setTexture(powerPelletTexture);
    }

    void construct ( int startY, int startX, int id, string textureName = "" ) {

        frameInd = 0;
        frameCnt = 1;
        if ( id == 1 ) { frameCnt = 3; }

        isDead = 0;
        timeDead = 0;

        withPowerPellet = 0;
        timeWithPP = 0;

        charId = id;
        if ( id != 1 ) {
            inBox = 1;
        }

        defaultSpeed = 2;
        posX = startX*tileSizeX;
        posY = startY*tileSizeY;

        speed = 0;

        currentDirection = 0;
        possibleNewDirection = -1;

        directionX = 0;
        directionY = 0;

        if( id != 1 ) {
            speed = 1;
            defaultSpeed = 1;
            directionY = -1;
            currentDirection = 1;
        }

        if ( textureName.size() ) {
            if (!texture.loadFromFile(textureName) ) {
                cout << "Unable to load image\n";
            }
            sprite.setTexture(texture);
        }

    }

    void keyPressEvent ( int keyCode ) {

        if ( keyCode == 71 ) { possibleNewDirection = 2; }
        if ( keyCode == 72 ) { possibleNewDirection = 0; }
        if ( keyCode == 73 ) { possibleNewDirection = 1; }
        if ( keyCode == 74 ) { possibleNewDirection = 3; }

    }

    void startMovingInD( int d ) {
        if ( d == 2 ){
            directionX = -1;
            directionY = 0;
            currentDirection = 2;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
        if ( d == 0 ){
            directionX = 1;
            directionY = 0;
            currentDirection = 0;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
        if ( d == 1 ){
            directionX = 0;
            directionY = -1;
            currentDirection = 1;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
        if ( d == 3 ){
            directionX = 0;
            directionY = 1;
            currentDirection = 3;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
    }

    void changeDirectionPl ( MapClass &myMap ){
        int mapX = posX / tileSizeX, mapY = posY / tileSizeY;
        if ( possibleNewDirection == 2 && myMap.m[ mapY +0 ][ mapX -1 ] != '#' && myMap.m[ mapY +0 ][ mapX -1 ] != '-' ){
            directionX = -1;
            directionY = 0;
            currentDirection = 2;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
        if ( possibleNewDirection == 0 && myMap.m[ mapY +0 ][ mapX +1 ] != '#' && myMap.m[ mapY +0 ][ mapX +1 ] != '-' ){
            directionX = 1;
            directionY = 0;
            currentDirection = 0;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
        if ( possibleNewDirection == 1 && myMap.m[ mapY -1 ][ mapX -0 ] != '#' && myMap.m[ mapY -1 ][ mapX -0 ] != '-' ){
            directionX = 0;
            directionY = -1;
            currentDirection = 1;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
        if ( possibleNewDirection == 3 && myMap.m[ mapY +1 ][ mapX -0 ] != '#' && myMap.m[ mapY +1 ][ mapX -0 ] != '-' ){
            directionX = 0;
            directionY = 1;
            currentDirection = 3;
            speed = defaultSpeed;
            possibleNewDirection = -1;
        }
    }

    void changeDirectionGh ( MapClass &myMap, bool difficultMode ) {
        int mapX = posX / tileSizeX, mapY = posY / tileSizeY;
        if ( myMap.m[mapY][mapX] == '.' || myMap.m[mapY][mapX] == ' ' ) { inBox = 0; }
        if ( inBox ) { return; }


        int cntPosDir = 0;
        bool posD[4] = {0, 0, 0, 0 };

        if ( mapX -1 > 0 && myMap.m[ mapY +0 ][ mapX -1 ] != '#' && myMap.m[ mapY +0 ][ mapX -1 ] != '-' ){
            cntPosDir ++;
            posD[2] = 1;
        }
        if ( mapX +2 < mapSizeX && myMap.m[ mapY +0 ][ mapX +1 ] != '#' && myMap.m[ mapY +0 ][ mapX +1 ] != '-' ){
            cntPosDir ++;
            posD[0] = 1;
        }
        if ( myMap.m[ mapY -1 ][ mapX -0 ] != '#' && myMap.m[ mapY -1 ][ mapX -0 ] != '-' ){
            cntPosDir ++;
            posD[1] = 1;
        }
        if ( myMap.m[ mapY +1 ][ mapX -0 ] != '#' && myMap.m[ mapY +1 ][ mapX -0 ] != '-' ){
            cntPosDir ++;
            posD[3] = 1;
        }
        if ( cntPosDir == 1 ) {
            int p = -1;
            for ( int i = 0 ; i < 4 ; i ++ ) {
                if ( posD[i] ) { p = i; }
            }
            startMovingInD( p );

        }else {

            if ( !difficultMode ) {

                if ( posD[ (currentDirection+2)%4 ] ) { cntPosDir --; }
                posD[ (currentDirection+2)%4 ] = 0;

                int c = rand()%cntPosDir, p = -1;

                for ( int i = 0 ; p < 0 && i < 4 ; i ++ ) {
                    if ( !posD[i] ) { continue; }
                    if ( c == 0 ) { p = i; break; }
                    c --;
                }
                startMovingInD ( p );
            }else {

                int dx[4] = { 1, 0, -1, 0 };
                int dy[4] = { 0, -1, 0, 1 };
                int minV = 1<<20, minD = -1;
                for( int i = 0 ; i < 4 ; i ++ ) {
                    if ( !posD[i] ) { continue; }

                    if ( minV > myMap.minTo[ mapY + dy[i] ][ mapX + dx[i] ] ) {
                        minV = myMap.minTo[ mapY + dy[i] ][ mapX + dx[i] ];
                        minD = i;
                    }
                }
                if ( minD >= 0 && posD[ minD ] ) {
                    startMovingInD( minD );
                }else {
                    cout << minD << " " << dx[minD] << " " << dy[minD] << " <-- \n";
                    cout << posD[0] << " " << posD[1] <<" " << posD[2] << " " << posD[3] << "\n";
                }
            }
        }
    }

    void update ( MapClass &myMap ) {

        if ( isDead ) {
            timeDead --;
            if ( timeDead == 0 ) {
                isDead = 0;
                timeWithPP = 0;
                withPowerPellet = 0;
            }
        }

        if ( timeWithPP ) {
            timeWithPP --;
            if ( timeWithPP == 0 ) {
                withPowerPellet = 0;
            }
        }

        if ( posX % tileSizeX == 0 && posY % tileSizeY == 0 ) {

            int mapX = posX / tileSizeX, mapY = posY / tileSizeY;

            if ( myMap.m[ mapY + directionY ][ mapX + directionX ] == '#' ||
                ( !inBox && myMap.m[ mapY + directionY ][ mapX + directionX ] == '-' ) ) {
                speed = 0;
            }

            if ( charId == 1 ) {
                changeDirectionPl( myMap );
            }else {
                changeDirectionGh( myMap, difficultMode );
            }
        }

        posX += directionX*speed;
        posY += directionY*speed;

        if ( speed ) {
            frameInd += 1.*frameCnt*speed/tileSizeX;
            while ( frameInd >= frameCnt ) {
                frameInd -= frameCnt;
            }
        }


        if ( posX < -tileSizeX/2 ) { posX += (mapSizeX+1)*tileSizeX; }
        if ( posX > mapSizeX*tileSizeX + tileSizeX/2 ) { posX -= (mapSizeX+1)*tileSizeX; }

        sprite.setPosition(sf::Vector2f( posX, posY+ offsetY ));
        deadSprite.setPosition(sf::Vector2f( posX, posY+ offsetY ));
        powerPelletSprite.setPosition(sf::Vector2f( posX, posY+ offsetY ));

    }

    void draw () {
        int fInd = floor( frameInd );
        if ( charId == 1 ){

            sprite.setTextureRect(sf::IntRect(fInd*30, 0, 30, 30));

            sprite.setOrigin( 15, 15 );
            sprite.rotate( -90.*currentDirection );
            sprite.move( 15, 15 );

            window.draw( sprite );

            sprite.move ( -15, -15 );
            sprite.rotate( 90.*currentDirection );

        }else {
            if ( isDead ) {
                window.draw( deadSprite );
            }else if ( withPowerPellet ) {
                window.draw ( powerPelletSprite );
            }else {
                window.draw( sprite );
            }
        }
    }

};

struct levelClass {

    int *screenId;

    CharacterClass player, npc[4];
    MapClass myMap;

    sf::Texture lifeTexture;
    sf::Sprite lifeSprite;

    bool isPaused, pausedTextDrawn, difficultMode, randomMap;
    int levelInd, lives, score ;

    string playerName;
    ButtonClass saveScoreBnt;

    int maxNameSize;

    bool levelWon, isPlayerDead, deadTextDrawn;

    levelClass () {
        maxNameSize = 10;
    }

    void startGame ( int *screenId_, bool difficultMode_, bool randomMap_ ) {

        if ( !lifeTexture.loadFromFile( "images/life.png") ) { cout << "Unable to load\n"; }
        lifeSprite.setTexture( lifeTexture );

        difficultMode = difficultMode_;
        randomMap = randomMap_;

        srand( time( nullptr ) );

        screenId = screenId_;

        saveScoreBnt = ButtonClass( 390, 212, 60, 25, "save" );

        playerName = "";

        isPaused = 0;
        deadTextDrawn = 0;
        levelWon = 0;

        isPlayerDead = 0;

        levelInd = 1;
        lives = 3;
        score = 0;

        player.construct( 6, 0, 1, "images/player.png" );

        npc[0].construct( 5, 7, 2, "images/ghost1.png" );
        npc[0].difficultMode = difficultMode;
        npc[1].construct( 5, 8, 3, "images/ghost2.png" );
        npc[1].difficultMode = difficultMode;
        npc[2].construct( 5, 9, 4, "images/ghost3.png" );
        npc[2].difficultMode = difficultMode;
        npc[3].construct( 5, 10, 5, "images/ghost4.png" );
        npc[3].difficultMode = difficultMode;

        myMap.construct( randomMap );

    }

    void loadLevel () {

        isPaused = 0;
        levelWon = 0;
        isPlayerDead = 0;

        player.construct( 6, 0, player.charId );

        npc[0].construct( 5, 7, npc[0].charId );
        npc[0].difficultMode = difficultMode;
        npc[1].construct( 5, 8, npc[1].charId  );
        npc[1].difficultMode = difficultMode;
        npc[2].construct( 5, 9, npc[2].charId  );
        npc[2].difficultMode = difficultMode;
        npc[3].construct( 5, 10, npc[3].charId  );
        npc[3].difficultMode = difficultMode;

    }

    void loadNextLevel () {
        loadLevel();
        levelInd ++;
        myMap.construct( randomMap );
    }

    void statusBarDraw () {

        text.setString("Score: ");
        text.setPosition( 10, 10 );
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        window.draw(text);

        text.setString( toString( score, 5 ) );
        text.setPosition( 90, 10 );
        window.draw(text);

        text.setString( "Lvl: " );
        text.setPosition( 230, 10 );
        window.draw(text);

        text.setString( toString( levelInd, 2 ) );
        text.setPosition( 280, 10 );
        window.draw(text);

        text.setString( "Lives: " );
        text.setPosition( 390, 10 );
        window.draw(text);

        for ( int i = 0 ; i < lives ; i ++ ) {
            //life.setString( "#" );
            lifeSprite.setPosition( 460 + i*20, 18 );
            window.draw(lifeSprite);
        }


    }

    void keyPressEvent( int keyCode) {
        if ( !isPaused ) {
            player.keyPressEvent( keyCode );
        }
    }

    void keyReleaseEvent ( int keyCode ) {

        if ( !isPlayerDead && keyCode == 15 ) {
            isPaused = !isPaused;
            pausedTextDrawn = 0;
        }
    }

    void textEnteredEvent ( int keyCode ) {
        if ( isPlayerDead ) {

            if ( keyCode < 128) {

                if ( keyCode == 8 ) {
                    if ( playerName.size() ) {
                        playerName.pop_back();
                    }
                }else if ( ( keyCode >= 'a' && keyCode <= 'z' ) ||
                          ( keyCode >= 'A' && keyCode <= 'Z' ) ||
                          ( keyCode >= '0' && keyCode <= '9' ) ) {
                    if ( playerName.size () < maxNameSize ) {
                        playerName += static_cast<char>(keyCode);
                    }
                }
            }

        }
    }

    void mouseMovedEvent( int mouseX, int mouseY ) {
        saveScoreBnt.mouseMoveEvent( mouseX, mouseY );
    }

    void saveScore () {

        vector < pair < int, string > > scores;
        ifstream fin;
        fin.open( "highscores/highscores.txt" );
        for ( pair < int, string > crr ; fin >> crr.second >> crr.first ; ) {
            scores.push_back( crr );
        }
        scores.push_back( make_pair( score, playerName ) );
        fin.close();

        sort( scores.begin(), scores.end() );
        reverse( scores.begin(), scores.end() );

        ofstream fout;
        fout.open( "highscores/highscores.txt" );
        for ( int i = 0 ; i < 10 && i < scores.size() ; i ++ ) {
            fout << scores[i].second << " " << scores[i].first << "\n";
        }
        fout.close();
        (*screenId) = 1;
    }

    void MouseButtonReleased( int mouseX, int mouseY ) {
        if ( isPlayerDead && saveScoreBnt.isClicked( mouseX, mouseY ) ) {
            if ( playerName.size() ) {
                saveScore();
            }
        }
    }

    void draw () {

        //text.setStyle(sf::Text::Bold | sf::Text::Underlined);
        window.clear();

        statusBarDraw();
        myMap.draw();
        player.draw();
        for ( int i = 0 ; i < 4 ;i ++ ) {
            npc[i].draw();
        }

        if ( isPaused ) {


            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(windowSizeX, windowSizeY));
            rectangle.setFillColor(sf::Color(0, 0, 0, 128)); // half transparent
            window.draw( rectangle );

            text.setString("Game Paused");
            text.setPosition( 110, 150 );
            text.setCharacterSize(50);
            text.setFillColor(sf::Color::White);
            window.draw(text);

        }else if ( isPlayerDead ) {

            sf::RectangleShape rectangle;
            rectangle.setSize( sf::Vector2f(windowSizeX, windowSizeY) );
            rectangle.setFillColor(sf::Color(0, 0, 0, 128)); // half transparent
            window.draw( rectangle );

            text.setString("Game Over");
            text.setPosition( 150, 105 );
            text.setCharacterSize(50);
            text.setFillColor(sf::Color::White);
            window.draw(text);

            text.setString("Total Score: ");
            text.setPosition( 150, 160 );
            text.setCharacterSize(30);
            text.setFillColor(sf::Color::White);
            window.draw(text);

            text.setString( toString( score, 5 ) );
            text.setPosition( 320, 160 );
            text.setFillColor( sf::Color::White );
            window.draw(text);

            rectangle.setSize( sf::Vector2f(210, 35) );
            rectangle.setPosition( 150, 210 );
            rectangle.setFillColor(sf::Color(128, 128, 128, 128)); // half transparent
            window.draw( rectangle );

            text.setString( playerName );
            text.setPosition( 150, 210 );
            text.setFillColor( sf::Color::White );
            window.draw(text);

            saveScoreBnt.draw();

        }



        window.display();

    }

    void playerDies () {
        isPlayerDead = 1;
    }

    void update() {

        if ( !isPaused && !isPlayerDead ) {
            if ( player.posX % tileSizeX == 0 && player.posY % tileSizeY == 0 ) {
                int crr = myMap.playerOn( player.posY/tileSizeY, player.posX/tileSizeX );
                score += crr;
                if ( crr == 50 ) {
                    player.withPowerPellet = 1;
                    player.timeWithPP = 500;

                    for ( int i = 0 ; i < 4 ; i ++ ) {
                        npc[i].withPowerPellet = 1;
                        npc[i].timeWithPP = 500;
                    }

                }


            }
            if ( myMap.dotsLeft == 0 ) {
                loadNextLevel();
                return;
            }

            player.update( myMap );
            myMap.bfs ( player.posX/tileSizeX, player.posY/tileSizeY );

            for ( int i = 0 ; i < 4 ;i ++ ) {
                npc[i].update( myMap );

                if ( npc[i].isDead ) { continue; }

                int dx = abs ( player.posX - npc[i].posX );
                int dy = abs ( player.posY - npc[i].posY );

                if ( dx + dy < 15 ) {
                    if ( npc[i].withPowerPellet ) {
                        npc[i].isDead = 1;
                        npc[i].timeDead = 400;
                    }else {
                        if ( lives == 0 ) {
                            playerDies ();
                        }else {
                            lives --;
                            loadLevel();
                        }
                    }
                }

            }

        }
    }

}lvl;

struct highscoreClass {

    int *screenId;
    ButtonClass backBtn;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    vector < pair < string, int > > scores;

    void load ( int *screenId_ ) {

        if (!backgroundTexture.loadFromFile("images/background.jpg") ) { cout << "Unable to load image\n"; }
        backgroundSprite.setTexture(backgroundTexture);

        screenId = screenId_;
        backBtn = ButtonClass( 450, 10, 50, 30, "Back" );

        ifstream fin;
        fin.open( "highscores/highscores.txt" );
        scores.clear();
        for ( pair < string, int > crr; fin >> crr.first >> crr.second ; ) {
            scores.push_back( crr );
        }
        fin.close();

    }

    void mouseMovedEvent ( int mouseX, int mouseY ) {
        backBtn.mouseMoveEvent( mouseX, mouseY );
    }

    void MouseButtonReleased ( int mouseX, int mouseY ) {
        if ( backBtn.isClicked( mouseX, mouseY ) ) {
            (*screenId) = 1;
        }
    }

    void draw () {
        window.clear();

        window.draw( backgroundSprite );
        backBtn.draw();

        for ( int i = 0 ; i < scores.size () ; i ++ ) {
            sf::RectangleShape rectangle;
            rectangle.setPosition(sf::Vector2f(50 , 10 + i*40 ));
            rectangle.setSize( sf::Vector2f(300, 30) );
            rectangle.setFillColor(sf::Color(255, 255, 255) );
            window.draw( rectangle );

            text.setString( toString( i+1 ) );
            text.setPosition( 55, 10 + i*40 );
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);
            window.draw(text);


            text.setString( scores[i].first );
            text.setPosition( 100, 10 + i*40 );
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);
            window.draw(text);

            text.setString( toString( scores[i].second, 5 ) );
            text.setPosition( 270, 10 + i*40 );
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);
            window.draw(text);

        }


        window.display();
    }

};

struct optionsClass {

    int *screenId;
    bool *difficultLvl, *randomMap;

    sf::Texture backgroundTexture, tickTexture;
    sf::Sprite backgroundSprite, tickSprite;

    ButtonClass backBtn, difficultyBtn, randomMapBtn;

    void load ( int *screenId_, bool *difficultLvl_, bool *randomMap_ ) {

        if (!backgroundTexture.loadFromFile("images/background.jpg") ) { cout << "Unable to load image\n"; }
        backgroundSprite.setTexture(backgroundTexture);

        if (!tickTexture.loadFromFile("images/tick.png") ) { cout << "Unable to load image\n"; }
        tickSprite.setTexture(tickTexture);

        screenId = screenId_;
        difficultLvl = difficultLvl_;
        randomMap = randomMap_;
        backBtn = ButtonClass( 450, 10, 50, 30, "Back" );

        difficultyBtn = ButtonClass( 10, 10, 30, 30, "" );
        randomMapBtn = ButtonClass( 10, 50, 30, 30, "" );

    }

    void mouseMovedEvent ( int mouseX, int mouseY ) {
        backBtn.mouseMoveEvent( mouseX, mouseY );
        difficultyBtn.mouseMoveEvent( mouseX, mouseY );
        randomMapBtn.mouseMoveEvent( mouseX, mouseY );
    }

    void MouseButtonReleased ( int mouseX, int mouseY ) {
        if ( backBtn.isClicked( mouseX, mouseY ) ) {
            (*screenId) = 1;
        }
        if ( difficultyBtn.isClicked( mouseX, mouseY ) ) {
            (*difficultLvl) = !(*difficultLvl);
        }
        if ( randomMapBtn.isClicked( mouseX, mouseY ) ) {
            (*randomMap) = !(*randomMap);
        }
    }

    void draw () {
        window.clear();
        window.draw( backgroundSprite );

        backBtn.draw();

        difficultyBtn.draw();
        if ( *difficultLvl ) {
            tickSprite.setPosition( difficultyBtn.posX, difficultyBtn.posY );
            window.draw( tickSprite );
        }
        randomMapBtn.draw();
        if ( *randomMap ) {
            tickSprite.setPosition( randomMapBtn.posX, randomMapBtn.posY );
            window.draw( tickSprite );
        }

        sf::RectangleShape rectangle;
        rectangle.setPosition(sf::Vector2f(50 , 10 ));
        rectangle.setSize( sf::Vector2f(200, 30) );
        rectangle.setFillColor(sf::Color(255, 255, 255) );
        window.draw( rectangle );

        text.setString( "Difficult Mode" );
        text.setPosition( 55, 10 );
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        window.draw(text);

        rectangle.setPosition(sf::Vector2f(50 , 50 ));
        rectangle.setSize( sf::Vector2f(200, 30) );
        rectangle.setFillColor(sf::Color(255, 255, 255) );
        window.draw( rectangle );

        text.setString( "Random Map" );
        text.setPosition( 55, 50 );
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        window.draw(text);




        window.display();
    }

};

struct gameClass {

    levelClass lvl;
    highscoreClass hs;
    optionsClass opt;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    ButtonClass startGameBtn, highscoresBtn, optionsBtn;
    bool isPlaying, difficultLvl, randomMap;
    int screenId;
    gameClass () {
        isPlaying = 0;

    }
    void load () {

        if (!backgroundTexture.loadFromFile("images/background.jpg") ) { cout << "Unable to load image\n"; }
        backgroundSprite.setTexture(backgroundTexture);

        screenId = 1;
        difficultLvl = 0;
        randomMap = 0;

        startGameBtn = ButtonClass( 370, 100, 120, 40, "Start" );
        highscoresBtn = ButtonClass( 370, 160, 120, 40, "Highscores" );
        optionsBtn = ButtonClass( 370, 220, 120, 40, "Options" );
    }
    void startGame () {
        isPlaying = 1;
        screenId = 2;
        lvl.startGame( &screenId, difficultLvl, randomMap );
    }

    void keyPressEvent( int keycode ) {
        if ( screenId == 2 ) {
            lvl.keyPressEvent( keycode );
        }
    }
    void keyReleaseEvent( int keycode ) {
        if ( screenId == 2 ) {
            lvl.keyReleaseEvent( keycode );
        }
    }
    void textEnteredEvent ( int keycode ) {
        if ( screenId == 2 ) {
            lvl.textEnteredEvent( keycode );
        }
    }
    void mouseMovedEvent ( int mouseX, int mouseY ) {
        if ( screenId == 1 ) {
            startGameBtn.mouseMoveEvent( mouseX, mouseY );
            highscoresBtn.mouseMoveEvent( mouseX, mouseY );
            optionsBtn.mouseMoveEvent( mouseX, mouseY );
        }else if ( screenId == 2 ) {
            lvl.mouseMovedEvent( mouseX, mouseY );
        }else if ( screenId == 3 ) {
            hs.mouseMovedEvent( mouseX, mouseY );
        }else if ( screenId == 4 ) {
            opt.mouseMovedEvent( mouseX, mouseY );
        }
    }

    void MouseButtonReleased ( int mouseX, int mouseY ) {
        if ( screenId == 1 ) {

            if ( startGameBtn.isClicked( mouseX, mouseY ) ) {
                startGameBtn.hovered = 0;
                startGame();
            }

            if ( highscoresBtn.isClicked( mouseX, mouseY ) ) {
                screenId = 3;
                highscoresBtn.hovered = 0;
                hs.load( &screenId );
            }

            if ( optionsBtn.isClicked( mouseX, mouseY ) ) {
                screenId = 4;
                optionsBtn.hovered = 0;
                opt.load( &screenId, &difficultLvl, &randomMap );
            }

        }else if ( screenId == 2 ) {
            lvl.MouseButtonReleased( mouseX, mouseY );
        }else if ( screenId == 3 ) {
            hs.MouseButtonReleased( mouseX, mouseY );
        }else if ( screenId == 4 ) {
            opt.MouseButtonReleased( mouseX, mouseY );
        }
    }

    void draw () {
        if ( screenId == 1 ) {
            window.clear();

            window.draw( backgroundSprite );

            startGameBtn.draw();
            highscoresBtn.draw();
            optionsBtn.draw();

            window.display();

        }else if ( screenId == 2 ) {
            lvl.draw();
        }else if ( screenId == 3 ) {
            hs.draw();
        }else if ( screenId == 4 ) {
            opt.draw();
        }
    }
    void update () {
        if ( screenId == 2 ) {
            lvl.update();
        }
    }
};

int main () {

    if ( !font.loadFromFile("font/arial.ttf") ) { cout << "Unable to load font\n"; }
    text.setFont( font );

    if (!buttonFrameTexture.loadFromFile("images/buttonFrame.png") ) { cout << "Unable to load image\n"; }
    buttonFrameSprite.setTexture(buttonFrameTexture);

    window.create(sf::VideoMode( windowSizeX, windowSizeY ), "Pacman");

    gameClass game;

 	sf::Clock clock;
    clock.restart ();

    game.load();

    while ( window.isOpen() )
    {
        sf::Event event;
        while (window.pollEvent(event) )
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    game.keyPressEvent( event.key.code );
                    break;

                case sf::Event::KeyReleased:
                    game.keyReleaseEvent( event.key.code );
                    break;

                case sf::Event::TextEntered:
                    game.textEnteredEvent( event.text.unicode );
                    break;

                case sf::Event::MouseMoved:
                    game.mouseMovedEvent( event.mouseMove.x, event.mouseMove.y );
                    break;

                case sf::Event::MouseButtonReleased:
                    game.MouseButtonReleased( event.mouseMove.x, event.mouseMove.y );
                    break;

                default:
                    break;
            }
        }

        if ( clock.getElapsedTime().asMilliseconds() > updateTime ) {
            game.update();
            clock.restart();
        }

        game.draw();
    }

    return 0;
}
