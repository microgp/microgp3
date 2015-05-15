#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
using namespace std;

int main (int argc, char** argv)
{
	ofstream ofile("fitness.output");
	for(int i = 1; i < argc; i++)
	{
		ifstream file(argv[i]);
		
		string s;
		int steps = 0;
		int x = 0, y = 0;
		int dx = 1;
		int dy = 0;
		while(file.eof() == false)
		{
			
			file >> s;
			if(s == "move")
			{

				steps++;
// cout << "Step " << steps << ", move " << s << endl;
				x += dx;
				y += dy;

				if(x < 0 || x > 40 || y < 0 || y > 40)
				{
					x = -1;
					break;
				}
			}
			else if(s == "turn_left")
			{

				steps++;
// cout << "Step " << steps << ", move " << s << endl;
				if(dx == 1)
				{
					dx = 0;
					dy = 1;
				}
				else if(dx == -1)
				{
					dx = 0;
					dy = -1;
				}

				else if(dy == 1)
				{
					dx = -1;
					dy = 0;
				}
				else if(dy == -1)
				{
					dx = 1;
					dy = 0;
				}
			}
			else if(s == "turn_right")
			{

				steps++;
// cout << "Step " << steps << ", move " << s << endl;
				if(dx == 1)
				{
					dx = 0;
					dy = -1;
				}
				else if(dx == -1)
				{
					dx = 0;
					dy = 1;
				}

				else if(dy == 1)
				{
					dx = 1;
					dy = 0;
				}
				else if(dy == -1)
				{
					dx = -1;
					dy = 0;
				}
			}
		// erase s
		s = '\0';
		}

		if(x == -1)
		{
			ofile << 0 << " " << 0 << endl;
		}
		else 
		{
			float distance = sqrt((20.0 - x)*(20.0 - x) + (20.0 - y)*(20.0 - y));
			int stepDiff = abs(21 - steps);
		
// cout << "Steps: " << steps << ", distance: " << distance << endl;
// cout << "StepDiff: " << stepDiff << endl;

			ofile  << (distance == 0? 1000 : 1.0/ distance) << " " << (stepDiff == 0? 1000 : 1.0 / stepDiff) << endl;
		}
		
		file.close();
	}

ofile.close();
	return 0;
}
