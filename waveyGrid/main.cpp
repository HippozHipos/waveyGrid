#define OLC_PGE_APPLICATION
//https ://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/olcPixelGameEngine.h
#include "pixelGameEngine.h"

struct Point
{
	float x = 0.0f;
	float y = 0.0f;
	std::vector<Point*> neighbours;
};

//Grid is defined as a bunch of points. 
//Each neighbour of a grid defines how
//the grid will be connected
struct Grid
{
	Grid(size_t detail)
	{
		points.reserve(detail * detail);
	}
	std::vector<std::unique_ptr<Point>> points;
};

class App : public olc::PixelGameEngine
{
private:
	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;

private:
	//grid stuff
	int detailLevel = 25;
	int gridSizex = 400;
	int gridSizey = 200;

	//wave stuff
	float frequency = 2.0f;
	float amplitude = 15.0f;

	//initial time point
	Time::time_point start = Time::now();

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		//User Controls
		if (GetKey(olc::A).bHeld)
			frequency -= 0.8f * elapsedTime;
		if (GetKey(olc::D).bHeld)
			frequency += 0.8f * elapsedTime;
		frequency = std::max(std::min(frequency, 10.0f), 0.0f);

		if (GetKey(olc::W).bHeld)
			amplitude += 10.1f * elapsedTime;
		if (GetKey(olc::S).bHeld)
			amplitude -= 10.1f * elapsedTime;
		amplitude = std::max(std::min(amplitude, 100.0f), 0.0f);

		//Need a new grid every frame because the "model" changes 
		//every frame.
		Grid grid(detailLevel);

		//Create the grid "model". Its 'z' value i.e. the "depth" increases with
		//y. So, z is constant with each x level and increses with y.
		for (int x = 1; x < detailLevel + 1; x++)
		{
			float z = 2.3f;
			float _x = (float)x / detailLevel;
			for (int y = 10; y < detailLevel + 10; y++)
			{
				float _y = (float)y / detailLevel;
				std::unique_ptr<Point> point = std::make_unique<Point>
					(Point{ ((_x * gridSizex) * (1 / z)), ((_y * gridSizey) * (1 / z)) });
				grid.points.push_back(std::move(point));
				z -= 0.05f;
			}
		}

		//define the neighbours. Each connection only needs to be made once,
		//since its only used for drawing lines.
		//Example, if point1's neighbour is set to point2, point2 does not
		//need to set point1 as its neighbour.
		for (size_t x = 0; x < detailLevel; x++)
		{
			for (size_t y = 0; y < detailLevel; y++)
			{
				if (y > 0)
				{
					grid.points[y * detailLevel + x].get()->neighbours.push_back
					(grid.points[(y - 1) * detailLevel + x].get());
				}

				if (x > 0)
				{
					grid.points[y * detailLevel + x].get()->neighbours.push_back
					(grid.points[y * detailLevel + (x - 1)].get());
				}

			}
		}

		//timer stuff for the sin wave
		auto end = Time::now();
		fsec fs = end - start;
		float t = std::fmod(fs.count() * 3.5f, 500.0f);

		//Create a sin wave by scrolling horizontally 
		//through each point and calculating its y value.
		//y is calculated based on z.
		for (size_t x = 0; x < detailLevel; x++)
		{
			float z = 2.0f;
			for (size_t y = 0; y < detailLevel; y++)
			{
				float _y = z / 2.0f * 2.0f * 3.14159f * frequency;
				grid.points[y * detailLevel + x].get()->y +=
					amplitude * std::sin(_y + t);
				z -= 0.05f;
			}
		}

		//same as above but for scrolling vertically.
	/*	for (size_t y = 0; y < detailLevel; y++)
		{
			float z = 2.0f;
			for (size_t x = 0; x < detailLevel; x++)
			{
				float _y = z / 2.0f * 2.0f * 3.14159f * frequency;
				grid.points[y * detailLevel + x].get()->y +=
					amplitude * std::sin(_y + t);
				z -= 0.05f;
			}
		}*/

		Clear(olc::BLACK);

		//Draw Lines between the points
		for (auto& point : grid.points)
		{
			for (auto& neighbour : point.get()->neighbours)
				DrawLine((int)point.get()->x, (int)point.get()->y,
					(int)neighbour->x, (int)neighbour->y,
					olc::Pixel(255,165,0));
		}

		DrawString(10, 10, "Frequency: " + std::to_string(frequency), olc::Pixel(255, 165, 0));
		DrawString(10, 280, "Amplitude: " + std::to_string(amplitude), olc::Pixel(255, 165, 0));

		//Mark the points with green pixel
	/*	for (auto& point : grid.points)
		{
			DrawCircle((int)point.get()->x, (int)point.get()->y, 3, olc::GREEN);
		}*/
		return true;
	}
};

int main()
{
	App flag;
	int ps = 2;
	int sw = 600;
	int sh = 600;
	if (flag.Construct(sw / ps, sh / ps, ps, ps))
		flag.Start();
}