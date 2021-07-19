											// _t C++ Implementation of Quad Tree
#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

											// #define SCREEN_W 800
											// #define SCREEN_H 600
#define LEVEL_MAX	6
#define CAPACITY	2

											// _t you can use THIS code, if needed
											//using namespace sf;
											//using namespace std;

class Point
{
public:
	float		x, y;
	bool		highlighted;

	Point()		:x(), y(), highlighted(false) {}

	Point(float _x, float _y, bool hlted = false) :
				x(_x), y(_y), highlighted(hlted) {}
};

											// _t create our area a rectangle
class Rectangle
{
public:
	float		x, y, w, h;

	Rectangle(float _x, float _y, float _w, float _h) :
		x(_x), y(_y), w(_w), h(_h) {}

											// _t if the point is in our rectangle, return the truth
	bool contains(const Point& p) const
	{
		return	p.x >= x - w && p.x <= x + w &&
				p.y >= y - h && p.y <= y + h;
	}

	bool intersects(const Rectangle& other) const
	{
		return !(x - w > other.x + other.w ||
				 x + w < other.x - other.w ||
				 y - h > other.y + other.h ||
				 y + h < other.y - other.h);
	}

											// _t draw our Rectangle
	void draw(sf::RenderTarget& target)
	{
		static sf::Vertex	vertices[5];
		vertices[0]			= sf::Vertex(sf::Vector2f(x - w, y - h), sf::Color::Magenta);
		vertices[1]			= sf::Vertex(sf::Vector2f(x + w, y - h), sf::Color::Magenta);
		vertices[2]			= sf::Vertex(sf::Vector2f(x + w, y + h), sf::Color::Magenta);
		vertices[3]			= sf::Vertex(sf::Vector2f(x - w, y + h), sf::Color::Magenta);
		vertices[4]			= sf::Vertex(sf::Vector2f(x - w, y - h), sf::Color::Magenta);

		target.draw(vertices, 5, sf::LineStrip);
	}
};

													// _t QuadTree
class QuadTree 
{
private:
													// _t QuadTree childe
	QuadTree*				topLeft;
	QuadTree*				topRight;
	QuadTree*				bottomLeft;
	QuadTree*				bottomRight;
	
	Rectangle				boundaries;				// _t tree size 
	bool					divided;				// _t tree have a child y/n
	size_t					capacity;				// _t how many points can be
	size_t					level;					// _t level tree
	std::vector<Point*>		children;

	void subdivide()
	{
		static sf::Vector2f halfSize;
		halfSize.x			= boundaries.w / 2.0f;	// _t half width
		halfSize.y			= boundaries.h / 2.0f;	// _t half hight
		topLeft				= new QuadTree(Rectangle(boundaries.x - halfSize.x,
													 boundaries.y - halfSize.y, 
													 halfSize.x, halfSize.y),
													 capacity, level + 1);
		topRight			= new QuadTree(Rectangle(boundaries.x + halfSize.x,
													 boundaries.y - halfSize.y,
													 halfSize.x, halfSize.y),
													 capacity, level + 1);
		bottomLeft			= new QuadTree(Rectangle(boundaries.x - halfSize.x,
													 boundaries.y + halfSize.y,
													 halfSize.x, halfSize.y),
													 capacity, level + 1);
		bottomRight			= new QuadTree(Rectangle(boundaries.x + halfSize.x,
													 boundaries.y + halfSize.y,
													 halfSize.x, halfSize.y),
													 capacity, level + 1);

		divided				= true;
	}

public:
	QuadTree(const Rectangle& _boundaries, size_t _capacity, size_t _level) :
				topLeft(NULL), topRight(NULL), bottomLeft(NULL), bottomRight(NULL),
				boundaries(_boundaries), divided(false), capacity(_capacity), level(_level)
	{
		if (level >= LEVEL_MAX)
		{
			capacity		= 0;
		}
	}

	~QuadTree()
	{
		if (divided)
		{
			delete			topLeft;
			delete			topRight;
			delete			bottomLeft;
			delete			bottomRight;
		}
	}

	bool insert(Point *p)
	{
		if (!boundaries.contains(*p)) 
		{
			return false;
		}
		if (!divided)
		{
			children.push_back(p);
			if (children.size() > capacity && capacity != 0)
			{
				subdivide();
				std::vector<Point*>::iterator it = children.begin();
				while (it != children.end())
				{
					if		(topLeft->insert(*it));
					else if	(topRight->insert(*it));
					else if	(bottomLeft->insert(*it));
					else if	(bottomRight->insert(*it));

					it		= children.erase(it);
				}	
			}
			return true;
		}
		else
		{
			if (topLeft->insert(p))
			{
				return true;
			}
			else if (topRight->insert(p))
			{
				return true;
			}
			else if (bottomLeft->insert(p))
			{
				return true;
			}
			else if (bottomRight->insert(p))
			{
				return true;
			}
			return false;
		}
	}

	void query(const Rectangle& area, std::vector<Point*>& found) const
	{
		if (!area.intersects(boundaries))									// _t if the rectangle doesn't intersect
		{
			return;
		}
		if (divided)
		{
			topLeft->query(area, found);
			topRight->query(area, found);
			bottomLeft->query(area, found);
			bottomRight->query(area, found);
		}
		else																// _t if  QuadTree don't have childe
		{
			for (size_t i = 0; i < children.size(); ++i)					// _t check all children
			{
				if (area.contains(*children[i]))
				{
					found.push_back(children[i]);							// _t if true, add in (found) point (children[i])
				}
			}
		}
	}

	void draw(sf::RenderTarget& t)
	{
		if (divided)
		{
			static sf::Vertex	vertices[4];
			vertices[0]			= sf::Vertex(sf::Vector2f(boundaries.x, boundaries.y - boundaries.h), 
														  sf::Color::White);
			vertices[1]			= sf::Vertex(sf::Vector2f(boundaries.x, boundaries.y + boundaries.h), 
														  sf::Color::White);
			vertices[2]			= sf::Vertex(sf::Vector2f(boundaries.x - boundaries.w, boundaries.y), 
														  sf::Color::White);
			vertices[3]			= sf::Vertex(sf::Vector2f(boundaries.x + boundaries.w, boundaries.y), 
														  sf::Color::White);
			t.draw(vertices, 4, sf::Lines);

			topLeft->draw(t);
			topRight->draw(t);
			bottomLeft->draw(t);
			bottomRight->draw(t);
		}
	}
};

char resizeWindow()
{
	while (true)
	{
		std::cout << "resize window: y/n ?" << std::endl;
		char				yn;
		std::cin >> yn;

		if (yn == 'y' || yn == 'Y' || yn == 'n' || yn == 'N')
			return yn; 
		else 
			std::cout << "Oops, that input is invalid.  Please try again.\n";
	}
}

int main()
{
	int						SCREEN_W = 800;
	int						SCREEN_H = 600;
	const char				retirnYN = resizeWindow();

	if (retirnYN == 'y' || retirnYN == 'Y')									// _t if you need resuze window
	{
		std::cout << "Enter WIDTH: " << std::endl;
		std::cin >>			SCREEN_W;
		std::cout << "Enter HIGHT: " << std::endl;
		std::cin >>			SCREEN_H;

		sf::RenderWindow	window(sf::VideoMode(SCREEN_W, SCREEN_H), "Quad Tree");

		QuadTree			tree(Rectangle(SCREEN_W / 2, SCREEN_H / 2,
										   SCREEN_W / 2, SCREEN_H / 2),
										   CAPACITY, 0);
	}

	sf::RenderWindow		window(sf::VideoMode(SCREEN_W, SCREEN_H), "Quad Tree");

	QuadTree				tree(Rectangle(SCREEN_W / 2, SCREEN_H / 2,
										   SCREEN_W / 2, SCREEN_H / 2),
										   CAPACITY, 0);

	Point*					po;												// _t pointer to Point
	std::vector<Point*>		points;											// _t pointer to all created Point
	std::vector<Point*>		found;

	sf::CircleShape			shape;
	shape.setRadius(3);
	shape.setOrigin(3,3);

	Rectangle				area(200, 200, 100, 100);
																			// _t Main application loop. Executed while the window IS OPEN
	while (window.isOpen())
	{
		sf::Event			e;												// _t We process the event queue in a loop
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)								// _t If the user clicked on the cross and wants to close the window?
			{
				window.close();												
			}
			else if (e.type == sf::Event::MouseButtonPressed && 
					e.mouseButton.button == sf::Mouse::Left)				// _t if the pressed key is only left
			{
				po			= new Point(sf::Mouse::getPosition(window).x,	// _t create a new pointer and write in po
										sf::Mouse::getPosition(window).y);	// _t mouse position
				points.push_back(po);										// _t add in vector points
				tree.insert(po);											// _t try to insert in tree
			}
			else if (e.type == sf::Event::MouseMoved)
			{
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))			// _t if the isButtonPressed, move the area
				{
					area.x	= sf::Mouse::getPosition(window).x;
					area.y	= sf::Mouse::getPosition(window).y;
				}
			}
		}

		for (size_t i = 0; i < points.size(); ++i)
		{
			points[i]->highlighted = false;									// _t set each point (highlighted=false)
		}
		found = std::vector<Point*>();
		tree.query(area, found);
		for (size_t i = 0; i < found.size(); ++i)
		{
			found[i]->highlighted = true;
		}

		window.clear();
																			// _t draw all point
		for (Point* p : points)
		{
			shape.setPosition(p->x, p->y);									// _t set position of the figure
			shape.setFillColor(p->highlighted ? sf::Color::Green : 
												sf::Color::White);			// _t set color if (highlighted = treu) set->Green, if (highlighted = false) set->White
			window.draw(shape);
		}
		tree.draw(window);
		area.draw(window);		
		window.display();													// _t Rendering window
	}

	for (int i = 0; i < points.size(); ++i)
	{
		delete points[i];
	}
	
    return 0;
}