struct Point
{
	float x;
	float y;
};

struct Distance
{
	float width;
	float height;
};

struct Square
{
	Point topLeft;
	Point bottomRight;
	Point center;
	float width;
	float height;
};

class Tile
{
public:
	Tile();
	~Tile();
};

class Character : public Tile
{
public:
	Character() : Tile() {}
	~Character();
};

class Player : public Character
{
public:
	Player() : Character() {}
	~Player();
};
